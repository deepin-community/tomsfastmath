/* TomsFastMath, a fast ISO C bignum library.
 *
 * This project is meant to fill in where LibTomMath
 * falls short.  That is speed ;-)
 *
 * This project is public domain and free for all purposes.
 *
 * Tom St Denis, tomstdenis@gmail.com
 */
#include <tfm_private.h>

int fp_isprime(fp_int *a)
{
  return fp_isprime_ex(a, 8);
}

/* $Source$ */
/* $Revision: 0.13.0 $ */
/* $Date: 2015-08-11 23:26:06 +0200 $ */
