#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <stdbool.h>

typedef struct _sequence {
    unsigned int length;        // number of bytes in the sequence
    unsigned int code;          // compression code assigned upon dict insertion
    unsigned long long hash;    // raw hash code for the sequence
    unsigned int bucket;        // hash code mod'ed to the hash table size
    struct _sequence* next;     // next sequence in a list of sequences (collisions)
    unsigned char bytes[1];     // buffer for the characters in the sequence
} Sequence;

/* create a new, one character Sequence using char firstByte as the character.
   hashSize is the size of the hashTable and if non-zero, a hash
   is computed for the new Sequence. */
Sequence* newSequence(unsigned char firstByte, unsigned int hashSize);

/* either frees sequence or places it on a list of idle sequences for possible reuse */
void deleteSequence(Sequence* sequence);


/* creates a new Sequence containing the old sequence bytes with newByte appended to the end,
   hashSize has the same meaning and function as in newSequence() above. */
Sequence* copySequenceAppend(Sequence* sequence, unsigned char newByte, unsigned long long hashSize);

/* write the data bytes of sequence to the FILE* fd, return the number of bytes written. */
unsigned int outputSequence(Sequence* sequence,
                            void (*writeFunc)(unsigned char c, void* context),
                            void* context);

/* returns true if a and b contain identical sequences of data bytes, false otherwise */
bool identicalSequences(Sequence* a, Sequence* b);


#endif
