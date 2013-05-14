/**
 * An example of generating a sequence of pseudo-random numbers
 * Copyright (C) 2013 Tim Hentenaar.
 *
 * This code is licenced under the Simplified BSD License.
 * See the LICENSE file for details.
 *
 * Compiling: gcc -ansi -pedantic -Wall -O2 -o rand rand.c
 * Running:
 *     tim@cid ~ $ ./rand 0 32 6
 *     Generated: 15,6,4,27,12,22
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Generate a pseudo-random number
 *
 * This is a really basic example of a Linear Congruential Generator (LCG.)
 *
 * Given: a (multiplier), c (increment), modulus (m), and a seed (X[0])
 * we generate pseudo-random sequence X by the following recurrence
 * relation:
 *
 * X[n+1] = (aX[n] + c) % m
 *
 * where our m comes into play with a bitwise and + right shift before
 * the bounds are applied.
 *
 * This isn't particularly 'random', but for demonstration purposes
 * it's enough.
 */
unsigned int my_rand(unsigned long *seed,
                     unsigned int lbound,
                     unsigned int ubound)
{
	ubound = (!ubound ? 1 : ubound);
	*seed = (unsigned long)(*seed * 0x6fe5) + 0x7ab9;
	return (unsigned int)(lbound + ((*seed & 0x0ff0) >> 4) % ubound);
}

int main(int argc, char *argv[])
{
	int i,seq_len = 5;
	unsigned long seed = (unsigned long)time(NULL);

	if (argc < 3) {
		printf("%s <lower_bound> <upper_bound> [<seq_len>]\n", argv[0]);
		printf("\tGenerate a sequence of random numbers\n");
		printf("\tlower_bound:  Lower bound\n");
		printf("\tupper_bound:  Upper bound\n");
		printf("\tseq_len: Length of the sequence to generate\n");
		exit(EXIT_FAILURE);
	} else if (argc == 4) seq_len = atoi(argv[3]);

	/* Generate the sequence */
	printf("Generated: ");
	for (i=0;i<seq_len;i++) {
		printf((i < seq_len - 1) ? "%u," : "%u",
		       my_rand(&seed, atoi(argv[1]), atoi(argv[2])));
	}
	printf("\n");
	return 0;
}

