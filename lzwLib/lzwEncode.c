#include <sequence.h>
#include <bitStream.h>
#include <dict.h>
#include <lzw.h>

//debug
#include <stdio.h>
#define DEBUG 0


bool lzwEncode(unsigned int bits, unsigned int maxBits,
               int (*readFunc)(void* context),
               void (*writeFunc)(unsigned char c, void* context),
               void* context) {

  //max number of codes
  int maxCodes = 1;
  while(maxBits > 0) {
    maxCodes*=2;
    maxBits--;
  }

  //calculate bitCodeCount, which will track variable code length
  int tempBits = bits;
  int bitCodeCount = 1;
  while(tempBits > 0) {
    bitCodeCount*=2;
    tempBits--;
  }

  int HASHSIZE = 251;

  Dict* dict = newDict(HASHSIZE);
  BitStream* out = openOutputBitStream(writeFunc, context);
  int nextCode = 256;
  unsigned int codeToOutput = 0;


  //Build initial dictionary
  for(int i = 0; i < 256; i++) {
    Sequence*seqN = newSequence(i, HASHSIZE);
    insertDict(dict, seqN, i);
  }

  char c = readFunc(context);

      if(DEBUG) {
        printf("read char %c\n", c);
      }

  Sequence* seqW = newSequence(c, HASHSIZE);
  while(c != -1) {
    c = readFunc(context);

        if(DEBUG) {
            printf("read char '%c' ", c);

            printf("word: '");
            outputSequence(seqW, writeFunc, context);
            printf("' ");
        }


    Sequence* seqX = copySequenceAppend(seqW, c, HASHSIZE);
        if(DEBUG) {
          printf("new word:'");
          outputSequence(seqX, writeFunc, context);
        }


    if(searchDict(dict, seqX, &codeToOutput)) {
      //seqX is in dict

          if(DEBUG) {
            printf("' -> found.\n");
          }
      //free seqW
      deleteSequence(seqW);

      seqW = seqX;

    } else {
      //seqX is not in dictionary
          if(DEBUG) {
            printf("' -> Not found.\n");
          }
      //find code assigned to seqW
      searchDict(dict, seqW, &codeToOutput);

          if(DEBUG) {
            printf("bits=%d\n", bits);
          }

      //output code assigned to seqW
      outputBits(out, bits, codeToOutput);

      //free seqW
      deleteSequence(seqW);

      //check if more codes can be added
      if(nextCode < maxCodes) {
        insertDict(dict, seqX, nextCode);
        //Set number of bits to print
        if(nextCode >= bitCodeCount) {
          bits++;
          bitCodeCount*=2;
        }
        nextCode++;
      } else {
        //delete seqX
        deleteSequence(seqX);
      }
      seqW = newSequence(c, HASHSIZE);
    }
  }
  searchDict(dict, seqW, &codeToOutput);

  deleteSequence(seqW);
  closeAndDeleteBitStream(out);
  deleteDictDeep(dict);
  return true;
}
