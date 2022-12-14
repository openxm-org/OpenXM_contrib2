/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM$
*/
/* Implementation of NIST's Secure Hash Algorithm (FIPS 180)
* Lightly bummed for execution efficiency.
*
* Jim Gillogly 3 May 1993
*
* 27 Aug 93: imported LITTLE_ENDIAN mods from Peter Gutmann's implementation
* 5 Jul 94: Modified for NSA fix
*
* Compile: cc -O -o sha sha.c
*
* To remove the test wrapper and use just the nist_hash() routine,
*      compile with -DONT_WRAP
*
* To reverse byte order for little-endian machines, use -DLITTLE_ENDIAN
*
* To get the original SHA definition before the 1994 fix, use -DVERSION_0
*
* Usage: sha [-vt] [filename ...]
*
*      -v switch: output the filename as well
*      -t switch: suppress spaces between 32-bit blocks
*
*      If no input files are specified, process standard input.
*
* Output: 40-hex-digit digest of each file specified (160 bits)
*
* Synopsis of the function calls:
*
*   sha_file(char *filename, unsigned int *buffer)
*      Filename is a file to be opened and processed.
*      buffer is a user-supplied array of 5 or more ints.
*      The 5-word buffer is filled with 160 bits of non-terminated hash.
*      Returns 0 if successful, non-zero if bad file.
*
*   void sha_stream(FILE *stream, unsigned int *buffer)
*      Input is from already-opened stream, not file.
*
*   void sha_memory(char *mem, int length, unsigned int *buffer)
*      Input is a memory block "length" bytes int.
*
* Caveat:
*      Not tested for case that requires the high word of the length,
*      which would be files larger than 1/2 gig or so.
*
* Limitation:
*      sha_memory (the memory block function) will deal with blocks no inter
*      than 4 gigabytes; for inter samples, the stream version will
*      probably be most convenient (e.g. perl moby_data.pl | sha).
*
* Bugs:
*      The standard is defined for bit strings; I assume bytes.
*
* Copyright 1993, Dr. James J. Gillogly
* This code may be freely used in any application.
*/

#define ONT_WRAP    /* Using just the hash routine itself */

/* #define VERSION_0 */  /* Define this to get the original SHA definition */

#include <stdio.h>
#include <memory.h>

#define VERBOSE

#define TRUE  1
#define FALSE 0

#define SUCCESS 0
#define FAILURE -1

int sha_file(char* filename, unsigned int *buffer);
void sha_stream(FILE *stream, unsigned int *buffer);
void sha_memory(char *mem, unsigned int length, unsigned int *buffer);

static void nist_guts(int file_flag, /* Input from memory, or from stream? */
            FILE *stream,
            char *mem,
            unsigned int length,
            unsigned int *buf);

#ifndef ONT_WRAP    /* Using just the hash routine itself */

#define HASH_SIZE 5   /* Produces 160-bit digest of the message */

int main(int argc, char **argv)
{
  unsigned int hbuf[HASH_SIZE];
  char *s;
  int file_args = FALSE;  /* If no files, take it from stdin */
  int verbose = FALSE;
  int terse = FALSE;

#ifdef MEMTEST
  sha_memory("abc", 3l, hbuf);     /* NIST test value from appendix A */
  if (verbose) printf("Memory:");
  if (terse) printf("%08lx%08lx%08lx%08lx%08lx\n",
    hbuf[0], hbuf[1], hbuf[2], hbuf[3], hbuf[4]);
  else printf("%08lx %08lx %08lx %08lx %08lx\n",
    hbuf[0], hbuf[1], hbuf[2], hbuf[3], hbuf[4]);
#endif

  for (++argv; --argc; ++argv)       /* March down the arg list */
    {
      if (**argv == '-')         /* Process one or more flags */
        for (s = &(*argv)[1]; *s; s++) /* Obfuscated C contest entry */
          switch(*s)
            {
              case 'v': case 'V':
                verbose = TRUE;
                break;
              case 't': case 'T':
                terse = TRUE;
                break;
              default:
                fprintf(stderr, "Unrecognized flag: %c\n", *s);
                return FALSE;
            }
      else              /* Process a file */
        {
          if (verbose) printf("%s:", *argv);
          file_args = TRUE;       /* Whether or not we could read it */

          if (sha_file(*argv, hbuf) == FAILURE)
            printf("Can't open file %s.\n", *argv);
          else
            if (terse) printf("%08lx%08lx%08lx%08lx%08lx\n",
              hbuf[0], hbuf[1], hbuf[2], hbuf[3], hbuf[4]);
            else printf("%08lx %08lx %08lx %08lx %08lx\n",
              hbuf[0], hbuf[1], hbuf[2], hbuf[3], hbuf[4]);
        }
    }
  if (! file_args)  /* No file specified */
    {
      if (verbose) printf("%s:", *argv);
      sha_stream(stdin, hbuf);

      if (terse) printf("%08lx%08lx%08lx%08lx%08lx\n",
        hbuf[0], hbuf[1], hbuf[2], hbuf[3], hbuf[4]);
      else printf("%08lx %08lx %08lx %08lx %08lx\n",
        hbuf[0], hbuf[1], hbuf[2], hbuf[3], hbuf[4]);
    }
  return TRUE;
}

#endif

/* #ifdef LITTLE_ENDIAN */ /* Imported from Peter Gutmann's implementation */

/* When run on a little-endian CPU we need to perform byte reversal on an
  array of intwords.  It is possible to make the code endianness-
  independant by fiddling around with data at the byte level, but this
  makes for very slow code, so we rely on the user to sort out endianness
  at compile time */

static void byteReverse( unsigned int *buffer, int byteCount )
{
  unsigned int value;
  int count;
  extern int little_endian;

  if ( !little_endian )
  return;

  byteCount /= sizeof( unsigned int );
  for( count = 0; count < byteCount; count++ )
    {
      value = ( buffer[ count ] << 16 ) | ( buffer[ count ] >> 16 );
      buffer[ count ] = ( ( value & 0xFF00FF00L ) >> 8 ) | ( ( value & 0x00FF00FFL ) << 8 );
    }
}
/* #endif */ /* LITTLE_ENDIAN */

union intbyte
{
  unsigned int W[80];    /* Process 16 32-bit words at a time */
  char B[320];        /* But read them as bytes for counting */
};

int sha_file(char* filename, unsigned int *buffer)    /* Hash a file */
{
  FILE *infile;

  if ((infile = fopen(filename, "rb")) == NULL)
    {
      int i;

      for (i = 0; i < 5; i++)
        buffer[i] = 0xdeadbeef;
      return FAILURE;
    }
  (void) sha_stream(infile, buffer);
  fclose(infile);
  return SUCCESS;
}

void sha_memory(char *mem, unsigned int length, unsigned int *buffer)
{
  nist_guts(FALSE, (FILE *) NULL, mem, length, buffer);
}

void sha_stream(FILE *stream, unsigned int *buffer)
{
  nist_guts(TRUE, stream, (char *) NULL, 0l, buffer);
}

#define f0(x,y,z) (z ^ (x & (y ^ z)))       /* Magic functions */
#define f1(x,y,z) (x ^ y ^ z)
#define f2(x,y,z) ((x & y) | (z & (x | y)))
#define f3(x,y,z) (x ^ y ^ z)

#define K0 0x5a827999               /* Magic constants */
#define K1 0x6ed9eba1
#define K2 0x8f1bbcdc
#define K3 0xca62c1d6

#define S(n, X) ((X << n) | (X >> (32 - n)))  /* Barrel roll */

#define r0(f, K) \
  temp = S(5, A) + f(B, C, D) + E + *p0++ + K; \
  E = D;  \
  D = C;  \
  C = S(30, B); \
  B = A;  \
  A = temp

#ifdef VERSION_0
#define r1(f, K) \
  temp = S(5, A) + f(B, C, D) + E + \
      (*p0++ = *p1++ ^ *p2++ ^ *p3++ ^ *p4++) + K; \
  E = D;  \
  D = C;  \
  C = S(30, B); \
  B = A;  \
  A = temp
#else           /* Version 1: Summer '94 update */
#define r1(f, K) \
  temp = *p1++ ^ *p2++ ^ *p3++ ^ *p4++; \
  temp = S(5, A) + f(B, C, D) + E + (*p0++ = S(1,temp)) + K; \
  E = D;  \
  D = C;  \
  C = S(30, B); \
  B = A;  \
  A = temp
#endif

static void nist_guts(int file_flag, /* Input from memory, or from stream? */
            FILE *stream,
            char *mem,
            unsigned int length,
            unsigned int *buf)
{
  int i, nread, nbits;
  union intbyte d;
  unsigned int hi_length, lo_length;
  int padded;
  char *s;

  register unsigned int *p0, *p1, *p2, *p3, *p4;
  unsigned int A, B, C, D, E, temp;

  unsigned int h0, h1, h2, h3, h4;

  h0 = 0x67452301;              /* Accumulators */
  h1 = 0xefcdab89;
  h2 = 0x98badcfe;
  h3 = 0x10325476;
  h4 = 0xc3d2e1f0;

  padded = FALSE;
  s = mem;
  for (hi_length = lo_length = 0; ;)  /* Process 16 ints at a time */
    {
      if (file_flag)
        {
          nread = fread(d.B, 1, 64, stream);  /* Read as 64 bytes */
        }
      else
        {
          if (length < 64) nread = length;
          else       nread = 64;
          length -= nread;
          memcpy(d.B, s, nread);
          s += nread;
        }
      if (nread < 64)   /* Partial block? */
        {
          nbits = nread << 3;         /* Length: bits */
          if ((int)(lo_length += nbits) < nbits)
            hi_length++;        /* 64-bit integer */

          if (nread < 64 && ! padded)  /* Append a single bit */
            {
              d.B[nread++] = (char)0x80; /* Using up next byte */
              padded = TRUE;     /* Single bit once */
            }
          for (i = nread; i < 64; i++) /* Pad with nulls */
            d.B[i] = 0;
          if (nread <= 56)   /* Room for length in this block */
            {
              d.W[14] = hi_length;
              d.W[15] = lo_length;
/* #ifdef LITTLE_ENDIAN */
              byteReverse(d.W, 56 );
/* #endif */ /* LITTLE_ENDIAN */
            }
/* #ifdef LITTLE_ENDIAN */
          else
            byteReverse(d.W, 64 );
/* #endif */ /* LITTLE_ENDIAN */
        }
      else  /* Full block -- get efficient */
        {
          if ((lo_length += 512) < 512)
            hi_length++;  /* 64-bit integer */
/* #ifdef LITTLE_ENDIAN */
          byteReverse(d.W, 64 );
/* #endif */ /* LITTLE_ENDIAN */
        }

      p0 = d.W;
      A = h0; B = h1; C = h2; D = h3; E = h4;

      r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0);
      r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0);
      r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0);
      r0(f0,K0);

      p1 = &d.W[13]; p2 = &d.W[8]; p3 = &d.W[2]; p4 = &d.W[0];

      r1(f0,K0); r1(f0,K0); r1(f0,K0); r1(f0,K0);
      r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1);
      r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1);
      r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1);
      r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1);
      r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2);
      r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2);
      r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2);
      r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2);
      r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3);
      r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3);
      r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3);
      r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3);

      h0 += A; h1 += B; h2 += C; h3 += D; h4 += E;

      if (nread <= 56) break; /* If it's greater, length in next block */
    }
  buf[0] = h0; buf[1] = h1; buf[2] = h2; buf[3] = h3; buf[4] = h4;
}
