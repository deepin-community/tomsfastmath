/* makes a bignum test harness with NUM tests per operation
 *
 * the output is made in the following format [one parameter per line]

operation
operand1
operand2
[... operandN]
result1
result2
[... resultN]

So for example "a * b mod n" would be

mulmod
a
b
n
a*b mod n

e.g. if a=3, b=4 n=11 then

mulmod
3
4
11
1

 */

#ifdef MP_8BIT
#define THE_MASK 127
#else
#define THE_MASK 32767
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tommath.h>
#define CRYPT
#undef DIGIT_BIT
#include "../src/headers/tfm.h"

FILE *rng;

/* 1-2048 bit numbers */
void rand_num(mp_int *a)
{
   int size;
   unsigned char buf[(FP_MAX_SIZE/16 - DIGIT_BIT/2) + 1];

   size = 1 + ((fgetc(rng)<<8) + fgetc(rng)) % (FP_MAX_SIZE/16 - DIGIT_BIT/2);
   buf[0] = (fgetc(rng)&1)?1:0;
   fread(buf+1, 1, size, rng);
   while (buf[1] == 0) buf[1] = fgetc(rng);
   mp_read_raw(a, buf, 1+size);
}

/* 1-256 bit numbers (to test things like exptmod) */
void rand_num2(mp_int *a)
{
   int size;
   unsigned char buf[(FP_MAX_SIZE/16 - DIGIT_BIT/2) + 1];

   size = 1 + ((fgetc(rng)<<8) + fgetc(rng)) % (FP_MAX_SIZE/16 - DIGIT_BIT/2);
   buf[0] = (fgetc(rng)&1)?1:0;
   fread(buf+1, 1, size, rng);
   while (buf[1] == 0) buf[1] = fgetc(rng);
   mp_read_raw(a, buf, 1+size);
}

#define mp_to64(a, b) mp_toradix_n(a, b, 64, sizeof(b))

int main(void)
{
   int n, tmp;
   mp_int a, b, c, d, e;
#ifdef MTEST_NO_FULLSPEED
   clock_t t1;
#endif
   char buf[4096];

   mp_init(&a);
   mp_init(&b);
   mp_init(&c);
   mp_init(&d);
   mp_init(&e);


   /* initial (2^n - 1)^2 testing, makes sure the comba multiplier works [it has the new carry code] */
/*
   mp_set(&a, 1);
   for (n = 1; n < ((FP_MAX_SIZE-(8*DIGIT_BIT))/2); n++) {
       mp_mul(&a, &a, &c);
       printf("mul\n");
       mp_to64(&a, buf);
       printf("%s\n%s\n", buf, buf);
       mp_to64(&c, buf);
       printf("%s\n", buf);

       mp_add_d(&a, 1, &a);
       mp_mul_2(&a, &a);
       mp_sub_d(&a, 1, &a);
   }
*/

   rng = fopen("/dev/urandom", "rb");
   if (rng == NULL) {
      rng = fopen("/dev/random", "rb");
      if (rng == NULL) {
         fprintf(stderr, "\nWarning:  stdin used as random source\n\n");
         rng = stdin;
      }
   }

#ifdef MTEST_NO_FULLSPEED
   t1 = clock();
#endif
   for (;;) {
#ifdef MTEST_NO_FULLSPEED
      if (clock() - t1 > CLOCKS_PER_SEC) {
         sleep(2);
         t1 = clock();
      }
#endif
       n = fgetc(rng) % 16;
   if (n == 0) {
       /* add tests */
       rand_num(&a);
       rand_num(&b);
       mp_add(&a, &b, &c);
       printf("add\n");
       mp_to64(&a, buf);
       printf("%s\n", buf);
       mp_to64(&b, buf);
       printf("%s\n", buf);
       mp_to64(&c, buf);
       printf("%s\n", buf);
   } else if (n == 1) {
      /* sub tests */
       rand_num(&a);
       rand_num(&b);
       mp_sub(&a, &b, &c);
       printf("sub\n");
       mp_to64(&a, buf);
       printf("%s\n", buf);
       mp_to64(&b, buf);
       printf("%s\n", buf);
       mp_to64(&c, buf);
       printf("%s\n", buf);
   } else if (n == 2) {
       /* mul tests */
       rand_num(&a);
       rand_num(&b);
       mp_mul(&a, &b, &c);
       printf("mul\n");
       mp_to64(&a, buf);
       printf("%s\n", buf);
       mp_to64(&b, buf);
       printf("%s\n", buf);
       mp_to64(&c, buf);
       printf("%s\n", buf);
   } else if (n == 3) {
      /* div tests */
       rand_num(&a);
       rand_num(&b);
       mp_div(&a, &b, &c, &d);
       printf("div\n");
       mp_to64(&a, buf);
       printf("%s\n", buf);
       mp_to64(&b, buf);
       printf("%s\n", buf);
       mp_to64(&c, buf);
       printf("%s\n", buf);
       mp_to64(&d, buf);
       printf("%s\n", buf);
   } else if (n == 4) {
      /* sqr tests */
       rand_num(&a);
       mp_sqr(&a, &b);
       printf("sqr\n");
       mp_to64(&a, buf);
       printf("%s\n", buf);
       mp_to64(&b, buf);
       printf("%s\n", buf);
   } else if (n == 5) {
      /* mul_2d test */
      rand_num(&a);
      mp_copy(&a, &b);
      n = fgetc(rng) & 63;
      mp_mul_2d(&b, n, &b);
      mp_to64(&a, buf);
      printf("mul2d\n");
      printf("%s\n", buf);
      printf("%d\n", n);
      mp_to64(&b, buf);
      printf("%s\n", buf);
   } else if (n == 6) {
      /* div_2d test */
      rand_num(&a);
      mp_copy(&a, &b);
      n = fgetc(rng) & 63;
      mp_div_2d(&b, n, &b, NULL);
      mp_to64(&a, buf);
      printf("div2d\n");
      printf("%s\n", buf);
      printf("%d\n", n);
      mp_to64(&b, buf);
      printf("%s\n", buf);
   } else if (n == 7) {
      /* gcd test */
      rand_num(&a);
      rand_num(&b);
      a.sign = MP_ZPOS;
      b.sign = MP_ZPOS;
      mp_gcd(&a, &b, &c);
      printf("gcd\n");
      mp_to64(&a, buf);
      printf("%s\n", buf);
      mp_to64(&b, buf);
      printf("%s\n", buf);
      mp_to64(&c, buf);
      printf("%s\n", buf);
   } else if (n == 8) {
      /* lcm test */
      rand_num(&a);
      rand_num(&b);
      a.sign = MP_ZPOS;
      b.sign = MP_ZPOS;
      mp_lcm(&a, &b, &c);
      printf("lcm\n");
      mp_to64(&a, buf);
      printf("%s\n", buf);
      mp_to64(&b, buf);
      printf("%s\n", buf);
      mp_to64(&c, buf);
      printf("%s\n", buf);
   } else if (n == 9) {
      /* exptmod test */
      rand_num2(&a);
      rand_num2(&b);
      rand_num2(&c);
      a.sign = b.sign = c.sign = 0;
      c.dp[0] |= 1;
//      if (c.used <= 4) continue;
//      if (mp_cmp(&a, &c) != MP_LT) continue;
//      if (mp_cmp(&b, &c) != MP_LT) continue;
      mp_exptmod(&a, &b, &c, &d);
      printf("expt\n");
      mp_to64(&a, buf);
      printf("%s\n", buf);
      mp_to64(&b, buf);
      printf("%s\n", buf);
      mp_to64(&c, buf);
      printf("%s\n", buf);
      mp_to64(&d, buf);
      printf("%s\n", buf);
   } else if (n == 10) {
      /* invmod test */
      rand_num2(&a);
      rand_num2(&b);
      b.dp[0] |= 1;
      b.sign = MP_ZPOS;
      a.sign = MP_ZPOS;
      mp_gcd(&a, &b, &c);
      if (mp_cmp_d(&c, 1) != 0) continue;
      if (mp_cmp_d(&b, 1) == 0) continue;
      mp_invmod(&a, &b, &c);
      printf("invmod\n");
      mp_to64(&a, buf);
      printf("%s\n", buf);
      mp_to64(&b, buf);
      printf("%s\n", buf);
      mp_to64(&c, buf);
      printf("%s\n", buf);
   } else if (n == 11) {
      rand_num(&a);
      mp_mul_2(&a, &a);
      mp_div_2(&a, &b);
      printf("div2\n");
      mp_to64(&a, buf);
      printf("%s\n", buf);
      mp_to64(&b, buf);
      printf("%s\n", buf);
   } else if (n == 12) {
      rand_num(&a);
      mp_mul_2(&a, &b);
      printf("mul2\n");
      mp_to64(&a, buf);
      printf("%s\n", buf);
      mp_to64(&b, buf);
      printf("%s\n", buf);
   } else if (n == 13) {
      rand_num(&a);
      tmp = abs(rand()) & THE_MASK;
      mp_add_d(&a, tmp, &b);
      printf("add_d\n");
      mp_to64(&a, buf);
      printf("%s\n%d\n", buf, tmp);
      mp_to64(&b, buf);
      printf("%s\n", buf);
   } else if (n == 14) {
      rand_num(&a);
      tmp = abs(rand()) & THE_MASK;
      mp_sub_d(&a, tmp, &b);
      printf("sub_d\n");
      mp_to64(&a, buf);
      printf("%s\n%d\n", buf, tmp);
      mp_to64(&b, buf);
      printf("%s\n", buf);
   } else if (n == 15) {
      rand_num(&a);
      tmp = abs(rand()) & THE_MASK;
      mp_mul_d(&a, tmp, &b);
      printf("mul_d\n");
      mp_to64(&a, buf);
      printf("%s\n%d\n", buf, tmp);
      mp_to64(&b, buf);
      printf("%s\n", buf);
   }
   }
   fclose(rng);
   return 0;
}

/* $Source$ */
/* $Revision: 0.13.0 $ */
/* $Date: 2014-08-14 21:11:31 +0200 $ */
