#ifndef UCT_TREE_H
#define UCT_TREE_H

#include "types.h"

UctNode *uct_node_new(Game *game, double (*heuristic)(Game *game));
void uct_node_free(void *node);
Action think(UctNode *root, const Game *game, double time,
	     double (*value_estimator)(Game *game), double (*heuristic)(Game *game));

#endif
