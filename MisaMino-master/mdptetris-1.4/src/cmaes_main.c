#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <values.h>
#include <sys/time.h>
#include <unistd.h>


#include "random.h"
#include "feature_functions.h"
#include "feature_policy.h"
#include "games_statistics.h"
#include "game.h"
#include "cmaes_interface.h"
#include "common_parameters.h"

#define _FREE(x) if (x!=NULL) { free(x); x=NULL;} \
else {fprintf(stderr, "Problem freeing memmory\n"); exit(1);}
#define _ALLOC(p,nb,typ) if ((p=(typ*)calloc(nb,sizeof(typ)))==NULL) \
{fprintf(stderr, "Memory full\n"); exit(1);}


/* Evaluation function */
double eval(double *x, int n, int nb_games, FeaturePolicy *fp, Game *game,
	    GamesStatistics *stats){

    int i;

    /* copy the vector in a feature struct */
    for(i=0; i <n; i++){
        fp->features[i].weight = x[i];
    }

    /* play nb_games and do not print scores */
    return feature_policy_play_games(fp, nb_games, game, stats, 0); 
}



int main(int argc, char *argv[]){
    int            i=0,j=0;
   
    /* CMA-ES Variables */
    char          *cma_parameter_file="incmaes.par"; /* cma parameter files */
    cmaes_t        evo;                     /* CMA struct contains everything*/
    double        *rgFunVal = NULL;         /* pointer to population fitness */
    double *const *pop;                     /* pointer to sample population */
    double        *xinit=NULL;              /* initial feature vector */
    double        *xbest=NULL;              /* Best feature vector so far */
    double         fbest=0;                 /* Fitness of the best vector */
    double        *mean=NULL;               /* Mean of all feature vectors */
    double         fmean=0;                 /* fitness of the mean vector */
    
    /* MDP Tetris Variables */
    int              board_width;
    int              board_height;
    char            *feature_file_name;    /* Initial feature file name */
    char            *statistics_file_name; /* MDP Tetris stat file name */
    FeaturePolicy    feature_policy;
    Game            *game;
    GamesStatistics *stats;

    int   nb_games=10;     /* Number of games to evaluate each vector */
    int   nb_games_mean=0; /* Number of games to evaluate the mean vector */
    int   gen;             /* Maximum number of generations for CMA ES */

    /* stats */
    int   print = 1; /* print stats or not, argument 7 in command line*/
    char *outf;      /* the file name to print to if "print" is true */
    FILE *fp=NULL;   
  
    /* parse the command line */
    if (argc != 10 ) {
	fprintf(stderr, "Usage: PROG feature_file nb_games board_width board_height statistics_file gens  print nb_games_mean outfile\n");
	exit(1);
    }
    
    feature_file_name    = argv[1];
    nb_games             = atoi(argv[2]);
    board_width          = atoi(argv[3]);
    board_height         = atoi(argv[4]);
    statistics_file_name = argv[5];
    gen                  = atoi(argv[6]);
    print                = atoi(argv[7]);
    nb_games_mean        = atoi(argv[8]);
    outf                 = argv[9];

    /* open outfile*/
    if(print){
	fp = fopen(outf, "w");
	if(!fp){
	    fprintf(stderr, "could not create out file %s\n", outf);
	    exit(1);
	}
    }
		
    /* initialize mdptetris */
    initialize_random_generator(time(NULL));
    load_feature_policy(feature_file_name, &feature_policy);
    features_initialize(&feature_policy);
    game = new_game(0, board_width, board_height, 0, "pieces4.dat", NULL);
    stats = games_statistics_new(statistics_file_name, nb_games, NULL);
   
    /* copy  initial vector */
    _ALLOC(xinit, feature_policy.nb_features, double);
    for(i=0; i <feature_policy.nb_features; i++){
	xinit[i] = feature_policy.features[i].weight;
    }
  
    /* init CMA */
    rgFunVal = cmaes_init(&evo, 
			  feature_policy.nb_features, 
			  xinit , 
			  NULL, 0, 0, cma_parameter_file);

    /* print column titles of statistics */
    if(print){
	fprintf(fp,"#%s\n", cmaes_SayHello(&evo));
	fprintf(fp,"# (1)gen   (2)best  (3)bestever (4)eigenRatio (5)stddevRatio (6)sigma (7-%d)axisLength [mean] [bestever]\n", feature_policy.nb_features+6);
    }
	
    /* gen loop */
    for(i=1; i<=gen; i++) {
	char meanfeature_fname[128];

	/* sample lambda vectors according to the current distribution */ 
	pop = cmaes_SamplePopulation(&evo);
	    
	/* evaluate the sample */
	for (j=0; j < cmaes_Get(&evo, "lambda"); ++j)
	    rgFunVal[j] = -eval(pop[j], (int) cmaes_Get(&evo,"dim"), nb_games,
				&feature_policy, game, stats);

	/* update the distribution */
	cmaes_UpdateDistribution(&evo, rgFunVal);  

	/* eval and copy mean of the distribution */
	mean  = cmaes_GetInto(&evo, "xmean", mean);
	fmean = eval(mean, (int) cmaes_Get(&evo, "dim"), nb_games_mean,
				&feature_policy, game, stats);
	
	/* eval and copy best vector of this gen */
	xbest = cmaes_GetInto(&evo, "xbest", xbest);
	fbest = eval(xbest, (int) cmaes_Get(&evo, "dim"), nb_games_mean,
				&feature_policy, game, stats);
		
	/* save the mean in a feature file */
	for(j=0; j <feature_policy.nb_features; j++){
	    feature_policy.features[j].weight = mean[j];
	}
	sprintf(meanfeature_fname, "mean-%d.dat", i);
	save_feature_policy(meanfeature_fname, &feature_policy); 

	/* print the best score */
	printf("%.3e ", fmean); 
	
	/* print scores */
	if(print){
	    fprintf(fp, "%7d %.5e %.5e %.5e %.5e %.5e %.5e ", 
		    i,                              /* gen */ 
		    -cmaes_Get(&evo, "fctvalue"),   /* best his gen */
		    fbest,                          /* best evalnb_games_mean*/
		    fmean,                          /* mean */
		    cmaes_Get(&evo, "maxaxislen") / /* ratio of eigen axis*/
		    cmaes_Get(&evo, "minaxislen"),
		    cmaes_Get(&evo, "maxstddev") /  /* ratio stddev */ 
		    cmaes_Get(&evo, "minstddev"),
		    cmaes_Get(&evo, "sigma"));      /* sigma */

	    /* principal axis lengths */
	    for(j=0; j <(int) cmaes_Get(&evo, "dim"); j++)
		fprintf(fp, "%.5e ", sqrt(evo.rgD[j]));

	    /* standar dev of all axis */
	    for(j=0; j <(int) cmaes_Get(&evo, "dim"); j++)
		fprintf(fp, "%.5e ", sqrt(evo.rgD[j])*cmaes_Get(&evo,"sigma"));

	    /* mean */ 
	    fprintf(fp,"[(%f) ", fmean);
	    for (j=0; j < (int) cmaes_Get(&evo, "dim"); j++)
		fprintf(fp,"%.3e ", mean[j]);
	    fprintf(fp,"] ");
	    
	    /* xbest */
	    fprintf(fp,"[(%f) ", fbest);
	    for (j=0; j < (int) cmaes_Get(&evo, "dim"); j++)
		fprintf(fp,"%.3e ", xbest[j]);
	    fprintf(fp,"]");
	    fprintf(fp,"\n"); 
	    fflush(fp);
	}
    } /* gens loop*/

    /* free and close everything */ 
    games_statistics_end_episode(stats, NULL);
    games_statistics_free(stats);
    exit_random_generator();
    free_game(game);
    _FREE(xinit);
    _FREE(mean);
    _FREE(xbest);
   
    if(print)
	fclose(fp);

    cmaes_exit(&evo);
    exit(0);
}

