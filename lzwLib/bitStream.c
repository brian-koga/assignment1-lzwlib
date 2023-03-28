#include <stdlib.h>
#include <bitStream.h>
#define BYTE 8
#define DEBUG 0

//debug
#include <stdio.h>


BitStream* openInputBitStream(int (*readFunc)(void* context), void* context) {
    BitStream* inputStream = (BitStream*) malloc(sizeof(BitStream));
    inputStream->readFunc = readFunc;
    inputStream->context = context;
    inputStream->direction = 1;
    inputStream->extraCount = 0;
    inputStream->extraBits = 0;
    inputStream->byteCount = 0;
    return inputStream;
}

BitStream* openOutputBitStream(void (*writeFunc)(unsigned char c,void* context),void* context) {
  BitStream* outputStream = (BitStream*) malloc(sizeof(BitStream));
  outputStream->writeFunc = writeFunc;
  outputStream->context = context;
  outputStream->direction = 0;
  outputStream->extraCount = 0;
  outputStream->extraBits = 0;
  outputStream->byteCount = 0;
  return outputStream;
}

void closeAndDeleteBitStream(BitStream* bs) {
  if(bs->direction == 0) {
    flushOutBits(bs);
  }
  free(bs);
}

void flushOutBits(BitStream* bs) {
  if(bs->extraCount > 0) {
    bs->extraBits = bs->extraBits << (BYTE - bs->extraCount);
    bs->writeFunc(bs->extraBits, bs->context);
    bs->extraCount = 0;
    bs->extraBits = 0;
  }
}

void outputBits(BitStream* bs, unsigned int nBits, unsigned int code) {


  char c;
  if(DEBUG) {
    printf("code=%x\n", code);
  }

  // add extra bits to the left of all bits in code
  // and clear extraBits and extraCount
  if(bs->extraCount > 0) {
    bs->extraBits = bs->extraBits << nBits;
    code = code | bs->extraBits;
    //adjust nBits so its the number of bits in code
    nBits = nBits + bs->extraCount;
    //clear these
    bs->extraCount = 0;
    bs->extraBits = 0;
  }

  int temp = code;

    while(nBits >= BYTE) {

      //set temp so it has only 8 bits
      temp = temp >> (nBits - BYTE);
      c = temp;
      bs->writeFunc(c, bs->context);
      bs->byteCount++;
      nBits = nBits - BYTE;
      //construct mask
      unsigned int mask = 0;
      if(nBits > 0) {
        mask = mask + 1;
        for(int i = 0; i < nBits-1; i++) {
          mask = mask << 1;
          mask = mask + 1;
        }
      }
      //and code with mask to 'delete' the 8 leftmost bits written
      temp = code & mask;
    }

    //Store the remaining bits in extraBits
    bs->extraBits = temp;
    bs->extraCount = nBits;
}

bool readInBits(BitStream* bs, unsigned int nBits, unsigned long long* code) {
  // Check if there are any bits in the stream
  if(bs->extraCount > 0) {
    //Since the minimum bits are 8, can safely take all bits in extraBits
    *code = bs->extraBits;
    nBits = nBits - bs->extraCount;
    bs->extraCount = 0;
  }

  //Since minimum bits are 8, at least one more byte must be read
  while(nBits > 0) {
    int c = bs->readFunc(bs->context);
    unsigned char currentByte = c;
    bs->byteCount = bs->byteCount + 1;
    if(c == EOF) {
      return false;
    }
    if(nBits >= BYTE) {
      *code = *code << BYTE;
      *code = *code | currentByte;
      nBits = nBits - BYTE;
    } else {
      // else the code needs fewer than BYTE bits
      *code = *code << nBits;
      // number of bits that will be left
      int toBeLeftCount = BYTE - nBits;
      bs->extraCount = toBeLeftCount;
      char toBeLeft = 0x01;
      toBeLeftCount = toBeLeftCount - 1;
      //create bit mask
      while(toBeLeftCount > 0) {
        toBeLeft = toBeLeft << 1; //shift bits left by 1
        toBeLeft = toBeLeft + 1; // add a 1 at rightmost bit
        toBeLeftCount = toBeLeftCount - 1;
      }
      // and the mask with currentByte
      bs->extraBits = currentByte & toBeLeft;
      //create second bit mask
      char mask = 0x01;
      nBits = nBits - 1;
      while(nBits > 0) {
        mask = mask << 1; //shift bits left by 1
        mask = mask + 1; // add a 1 at rightmost bit
        nBits = nBits - 1;
      }
      currentByte = currentByte >> bs->extraCount;
      currentByte = currentByte & mask;
      *code = *code | currentByte;
    }
  }
  return true;
}
