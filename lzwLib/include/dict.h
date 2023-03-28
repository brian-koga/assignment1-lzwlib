#ifndef DICT_H
#define DICT_H
#include <stdbool.h>

#include "sequence.h"

/* this module implements a dictionary of Sequences.  Sequences
   are paired with integer codes when inserted and when
   a Sequence is successfully searched for, its code is
   produced. */

typedef struct _dict {
    unsigned int hashSize;  // size of the hash table array
    Sequence** table;       // hash table array (of buckets)
} Dict;

/* create a dictionary, with hashSize hash table. */
Dict* newDict(unsigned int hashSize);

/* free a dictionary and all the sequences in it. */
void deleteDictDeep(Dict* dict);

/* search dict for key, if found, return true and
   set code, otherwise return false */
bool searchDict(Dict* dict, Sequence* key, unsigned int* code);

/* insert key into dict and associate code with key.
   it is assumed that key does not already exist in dict */
void insertDict(Dict* dict, Sequence* key, unsigned int code);


#endif
