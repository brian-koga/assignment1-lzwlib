#include <sequence.h>
#include <bitStream.h>
#include <dict.h>
#include <lzw.h>
#include <stdlib.h>


//debug
#include <stdio.h>

bool lzwDecode(unsigned int bits, unsigned int maxBits,
               int  (*readFunc )(void* context),
               void (*writeFunc)(unsigned char c, void* context),
               void* context) {
    //max number of codes
    int tempBits = maxBits;
    int maxCodes = 1;
    while(tempBits > 0) {
      maxCodes*=2;
      tempBits--;
    }

    int HASHSIZE = 251;

//    Sequence* dict[maxCodes];
    Sequence **dict = malloc(sizeof(Sequence)*maxCodes);
    BitStream* inStream = openInputBitStream(readFunc, context);

    unsigned long long prevCode = 0;
    unsigned long long currCode = 0;

    //calculate bitCodeCount, which will track variable code length
    tempBits = bits;
    int bitCodeCount = 1;
    while(tempBits > 0) {
      bitCodeCount*=2;
      tempBits--;
    }


    //Build initial dictionary
    for(int i = 0; i < 256; i++) {
      Sequence*seqN = newSequence(i, HASHSIZE);
      dict[i] = seqN;
    }

    int nextCode = 256;

    char c;
    readInBits(inStream, bits, &prevCode);
    outputSequence(dict[prevCode], writeFunc, context);
    //acount for one code already being read
    if(nextCode >= bitCodeCount && bits < maxBits) {
      bitCodeCount*=2;
      bits++;
    }
    while(readInBits(inStream, bits, &currCode)) {
      if(currCode < nextCode) {
        c = dict[currCode]->bytes[0];
      } else {
        c = dict[prevCode]->bytes[0];
      }
        if(nextCode < maxCodes) {
          Sequence* seqW = copySequenceAppend(dict[prevCode], c, HASHSIZE);
          dict[nextCode] = seqW;

          nextCode++;

          //Check if bit length for codes need to be changed
          //and if so, ensure maxbits haven't been reached
          if(nextCode >= bitCodeCount && bits < maxBits) {
            bitCodeCount*=2;
            bits++;
          }
        }

        outputSequence(dict[currCode], writeFunc, context);

      prevCode = currCode;
      currCode = 0;
    }
    closeAndDeleteBitStream(inStream);
    //free dictionary
    for(int i = 0; i <nextCode; i++) {
      deleteSequence(dict[i]);
    }
    free(dict);
    return(true);
}
