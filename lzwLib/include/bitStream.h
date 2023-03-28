#ifndef BITSTREAM_H
#define BITSTREAM_H
#include <stdbool.h>

#define MAX_CODE_BITS   24
#define CHAR_BITS       8

/* BitStream: module to enable variable width integers/codes to be read */
/* or written from/to an open stdio FILE* */

/* module only supports codes with 8 to 24 bit width */

typedef struct _bitStream {
    int (*readFunc)(void* context);
    void (*writeFunc)(unsigned char c, void* context);
    void* context;
    int direction;              // input or output
    unsigned int extraCount;    // number of bits held in extraBits
    unsigned int extraBits;     // bits buffer
    unsigned int byteCount;     // # of bytes read/written from/to fd
} BitStream;

BitStream* openInputBitStream(int (*readFunc)(void* context), void* context);

BitStream* openOutputBitStream(void (*writeFunc)(unsigned char c, void* context), void* context);

/* free bs, flush any remaining bits if its an output direction */
void closeAndDeleteBitStream(BitStream* bs);

/* Write any remaining bits to to fileStream */
void flushOutBits(BitStream* bs);

/* Write out the least significant nBits of code to fileStream */
void outputBits(BitStream* bs, unsigned int nBits, unsigned int code);

/* Read an nBit code from fileStream, if EOF, return false,
   else write the code to the pointer argument and return true. */
bool readInBits(BitStream* bs, unsigned int nBits, unsigned long long* code);

#endif
