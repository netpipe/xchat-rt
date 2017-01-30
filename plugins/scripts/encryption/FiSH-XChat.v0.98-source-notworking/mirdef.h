/*
 *   MIRACL compiler/hardware definitions - mirdef.h
 *   Copyright (c) 1988-2002 Shamus Software Ltd.
 */

#define MR_LITTLE_ENDIAN
#define MIRACL 32
#define mr_utype int               /* the underlying type is usually int *
                                    * but see mrmuldv.any                */

#define MR_IBITS      32           /* bits in int  */
#define MR_LBITS      32           /* bits in long */

#define mr_unsign32 unsigned int   /* 32 bit unsigned type               */

#define mr_dltype __int64
#define mr_unsign64 unsigned __int64
#define MR_STRIPPED_DOWN
#define MR_NO_STANDARD_IO
#define MR_NO_FILE_IO
#define MR_ALWAYS_BINARY
#define MAXBASE ((mr_small)1<<(MIRACL-1))
#define MR_BITSINCHAR 8
