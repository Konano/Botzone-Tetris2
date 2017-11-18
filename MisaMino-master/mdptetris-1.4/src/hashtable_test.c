/**
 * This program is a test for the hashtable module.
 * The test is performed when running 'make check'.
 */

#include "hashtable.h"
#include "macros.h"
#include <string.h>

static int nb_a;
static void count_a(const char *key, void *data);
static int starts_with_a(const char *key, void *data);

/**
 * Main function.
 */
int main(int argc, char **argv) {

  char *string;

  /* make a hashtable of strings */
  Hashtable *h = hashtable_new(5, NULL);

  /* check the functions for an empty hashtable */
  ASSERT(hashtable_get_length(h) == 0);
  ASSERT(!hashtable_contains(h, "key1"));
  ASSERT(hashtable_get(h, "key1") == NULL);
  hashtable_clear(h);
  hashtable_prune(h, starts_with_a);
  
  /* check hashtable_foreach() for an empty hashtable */
  nb_a = 0;
  hashtable_foreach(h, count_a);
  ASSERT(nb_a == 0);

  /* fill the hashtable */
  hashtable_add(h, "key1", "abbbb");
  hashtable_add(h, "key2", "a2222");
  hashtable_add(h, "key3", "adata3");
  hashtable_add(h, "key4", "a4");
  hashtable_add(h, "key5", "data5");
  hashtable_add(h, "key6", "abcde");
  hashtable_add(h, "key7", "data7");
  hashtable_add(h, "key8", "data8");
  hashtable_add(h, "key9", "aaaa");
  hashtable_add(h, "key10", "data10");

/*   hashtable_print(h); */

  /* accessing the elements */
  ASSERT(hashtable_get_length(h) == 10);
  ASSERT(!hashtable_contains(h, "key0"));
  ASSERT(hashtable_get(h, "key0") == NULL);
  ASSERT(hashtable_contains(h, "key1"));
  ASSERT(!strcmp(hashtable_get(h, "key1"), "abbbb"));

  /* check hashtable_foreach() */
  nb_a = 0;
  hashtable_foreach(h, count_a);
  ASSERT(nb_a == 6);

  /* remove elements */
  hashtable_remove(h, "key5");
  ASSERT(!hashtable_contains(h, "key5"));
  ASSERT(hashtable_get_length(h) == 9);

  hashtable_prune(h, starts_with_a);
  ASSERT2(hashtable_get_length(h) == 3, "length is %d instead of 3", hashtable_get_length(h));
  ASSERT(hashtable_contains(h, "key7"));
  ASSERT(hashtable_contains(h, "key8"));
  ASSERT(hashtable_contains(h, "key10"));
  ASSERT(!hashtable_contains(h, "key9"));

  nb_a = 0;
  hashtable_foreach(h, count_a);
  ASSERT(nb_a == 0);

  hashtable_clear(h);
  ASSERT(hashtable_get_length(h) == 0);

  hashtable_free(h);

  /* make a hashtable of mallocated elements */
  MALLOCN(string, char, 6);
  strcpy(string, "12345");
  
  h = hashtable_new(100, free);
  hashtable_add(h, "key1", string);
  ASSERT(hashtable_contains(h, "key1"));
  hashtable_remove(h, "key1");
  ASSERT(hashtable_get_length(h) == 0);

  hashtable_free(h);

  return 0;
}

/**
 * This function is designed to be used with
 * hashtable_foreach(). It increases nb_a for
 * each element whose first character is 'a'.
 */
static void count_a(const char *key, void *data) {

  char *string = (char*) data;

  if (string[0] == 'a') {
    nb_a++;
  }
}

/**
 * This function is designed to be used with
 * hashtable_prune(). It returns whether each
 * element starts with character 'a'.
 */
static int starts_with_a(const char *key, void *data) {
  
  char *string = (char*) data;
  return (string[0] == 'a');
}
