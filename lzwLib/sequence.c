#include <stdlib.h>
#include <sequence.h>
#include <stdio.h>
#include <string.h>
#define BYTE 8;

Sequence* newSequence(unsigned char firstByte, unsigned int hashSize) {
  Sequence* seq = (Sequence*) malloc(sizeof(Sequence));
  seq->length = 1;
  seq->bytes[0] = firstByte;
  seq->hash = firstByte;
  seq->bucket = firstByte % hashSize;
  seq->next = NULL;
  return seq;
}

void deleteSequence(Sequence* sequence) {
  free(sequence);
}

Sequence* copySequenceAppend(Sequence* sequence, unsigned char addByte, unsigned long long hashSize) {
  // allocate sequence's size + bytes equal to length +1
  Sequence* seq = (Sequence*) malloc(sizeof(Sequence) + sequence->length + 1);
  seq->length = sequence->length + 1;
  for(int i = 0; i < seq->length-1; i++) {
    seq->bytes[i] = sequence->bytes[i];
  }

  seq->bytes[seq->length - 1] = addByte;

  //set hash
  seq->hash = sequence->hash*65003;
  seq->hash = seq->hash + addByte;

  seq->bucket = seq->hash % ((int)hashSize); //assign bucket
  seq->next = NULL;
  return seq;
}

unsigned int outputSequence(Sequence* sequence,
                    void (*writeFunc)(unsigned char c, void* context), void* context) {


  for(int i = 0; i < sequence->length; i++) {
    writeFunc(sequence->bytes[i], context);
  }
  return sequence->length;
}

bool identicalSequences(Sequence* a, Sequence* b) {
  return a->hash == b->hash;
}
