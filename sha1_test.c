/*
SHA-1 in C
By Steve Reid <sreid@sea-to-sky.net>
100% Public Domain

-----------------
Modified 7/98
By James H. Brown <jbrown@burgoyne.com>
Still 100% Public Domain

Corrected a problem which generated improper hash values on 16 bit machines
Routine SHA1Update changed from
	void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned int
len)
to
	void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned
long len)

The 'len' parameter was declared an int which works fine on 32 bit machines.
However, on 16 bit machines an int is too small for the shifts being done
against
it.  This caused the hash function to generate incorrect values if len was
greater than 8191 (8K - 1) due to the 'len << 3' on line 3 of SHA1Update().

Since the file IO in main() reads 16K at a time, any file 8K or larger would
be guaranteed to generate the wrong hash (e.g. Test Vector #3, a million
"a"s).

I also changed the declaration of variables i & j in SHA1Update to
unsigned long from unsigned int for the same reason.

These changes should make no difference to any 32 bit implementations since
an
int and a long are the same size in those environments.

--
I also corrected a few compiler warnings generated by Borland C.
1. Added #include <process.h> for exit() prototype
2. Removed unused variable 'j' in SHA1Final
3. Changed exit(0) to return(0) at end of main.

ALL changes I made can be located by searching for comments containing 'JHB'
-----------------
Modified 8/98
By Steve Reid <sreid@sea-to-sky.net>
Still 100% public domain

1- Removed #include <process.h> and used return() instead of exit()
2- Fixed overwriting of finalcount in SHA1Final() (discovered by Chris Hall)
3- Changed email address from steve@edmweb.com to sreid@sea-to-sky.net

-----------------
Modified 4/01
By Saul Kravitz <Saul.Kravitz@celera.com>
Still 100% PD
Modified to run on Compaq Alpha hardware.

-----------------
Modified 07/2002
By Ralph Giles <giles@ghostscript.com>
Still 100% public domain
modified for use with stdint types, autoconf
code cleanup, removed attribution comments
switched SHA1Final() argument order for consistency
use SHA1_ prefix for public api
move public api to sha1.h
*/

/*
Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/* #define SHA1HANDSOFF  */

#include "common.h"
#include "pd_sha1.h"

static char *test_data[] = {
    "abc",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "A million repetitions of 'a'"};
static char *test_results[] = {
    "a9993e364706816aba3e25717850c26c9cd0d89d",
    "84983e441c3bd26ebaae4aa1f95129e5e54670f1",
    "34aa973cd4c4daa4f61eeb2bdbad27316534016f"};


int sha1_test()
{
    int k;
    PD_SHA1_CTX context;
    uint8_t digest[20];
    char output[80];

    for (k = 0; k < 2; k++){
        pd_sha1_init(&context);
        pd_sha1_update(&context, (uint8_t*)test_data[k], strlen(test_data[k]));
        pd_sha1_final(&context, digest);
	digest_to_hex(digest, output);

        if (strcmp(output, test_results[k])) {
            fprintf(stdout, "FAIL\n");
            fprintf(stderr,"* hash of \"%s\" incorrect:\n", test_data[k]);
            fprintf(stderr,"\t%s returned\n", output);
            fprintf(stderr,"\t%s is correct\n", test_results[k]);
            return (1);
        }
    }
    /* million 'a' vector we feed separately */
    pd_sha1_init(&context);
    for (k = 0; k < 1000000; k++)
        pd_sha1_update(&context, (uint8_t*)"a", 1);
    pd_sha1_final(&context, digest);
    digest_to_hex(digest, output);
    if (strcmp(output, test_results[2])) {
        fprintf(stdout, "FAIL\n");
        fprintf(stderr,"* hash of \"%s\" incorrect:\n", test_data[2]);
        fprintf(stderr,"\t%s returned\n", output);
        fprintf(stderr,"\t%s is correct\n", test_results[2]);
        return (1);
    }

    /* success */
    return 1;
}
