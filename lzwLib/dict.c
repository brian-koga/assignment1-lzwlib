#include <stdlib.h>
#include <dict.h>

//used for debugging
#include <stdio.h>

Dict* newDict(unsigned int hashSize) {
  Dict* dictionary;
  dictionary = (Dict*) malloc(sizeof(Dict));
  dictionary->hashSize = hashSize;
  dictionary->table = malloc(hashSize*sizeof(Sequence));
    return dictionary;
}

void deleteDictDeep(Dict* dict) {
  Sequence* temp1;
  Sequence* temp2;
  for(int i = 0; i < dict->hashSize; i++) {
    temp1 = dict->table[i];
    while(temp1 != NULL) {
      temp2 = temp1->next;
      free(temp1);
      temp1 = temp2;
    }
  }
  free(dict->table);
  free(dict);
}

bool searchDict(Dict* dict, Sequence* key, unsigned int* code) {
  Sequence* temp = dict->table[key->bucket];
  while(temp != NULL) {
    if(identicalSequences(temp, key)) {
      *code = temp->code;
      return true;
    }
    temp = temp->next;
  }
    return false;
}

void insertDict(Dict* dict, Sequence* key, unsigned int code) {
  key->code = code;
  if(key->hash != key->bucket) {
    Sequence*pt = dict->table[key->bucket];
    while(pt->next != NULL) {
      pt = pt->next;
    }
    pt->next = key;
  } else {
    dict->table[key->hash] = key;
  }


}
