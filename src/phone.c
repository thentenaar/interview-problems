/**
 * 'Phone Words' Example
 * Copyright (C) 2013 Tim Hentenaar.
 *
 * This code is licenced under the Simplified BSD License.
 * See the LICENSE file for details.
 *
 * Compiling: gcc -ansi -pedantic -Wall -W -O2 -o phone phone.c
 *
 * Running:
 *     tim@cid ~ $ ./phone 8675309
 *     #1: TMPJD0W
 *     ...
 *     #729: VORLF0Y
 *     Done!
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Quick error macros */
#define ERROR(X)      fprintf(stderr, (X))
#define ERROR_1(X, Y) fprintf(stderr, (X), (Y))

/* Location of the overflow bit in the counter */
#define OVERFLOW (1 << 14)

/**
 * Our state structure
 *
 * counter:
 *     Each key on an American dialpad has 3 or 4 letters. We can
 *     represent this in 2 bits, with the values 0, 1, 2, and 3. Thus,
 *     for 7 positions, we need 14 bits.
 *
 *     The closest power of 2 is 16, so it'll be a 16-bit integer.
 *     We'll use the 15th bit for the overflow flag.
 *
 * current_perm:
 *    This is the pattern of letters that our current state maps to.
 *
 * perm_count:
 *     Number of permutations thus far.
 *
 */
struct phone_state {
	unsigned short counter;
	char          *current_perm;
	unsigned short perm_count;
};

/**
 * Map of dialpad digit (2 - 9), to its starting offset in the alphabet
 *
 * '2' -> 'A', '3' -> 'D', etc.
 */
char digit_to_alpha[8] = {
	0, 3, 6, 9, 12, 15, 19, 22
};

/**
 * Perform a single permutation of the given phone number,
 * and print the result to STDOUT.
 *
 * Background:
 *     Most renditions of this particular problem exclude 'Q' and 'Z'
 *     from consideration, however, I've added the ability to include
 *     'Q' and 'Z', since it doesn't change the algorithm. It simply
 *     requires testing a couple of corner cases.
 *
 *     Usually 1 and 0 are treated as passthrough values.
 *
 * Solution:
 *     We have a state value which holds counters for every possible
 *     position in the phone number. On each iteration of this function,
 *     we permute the state by adding 1 to the right-most letter's
 *     counter. Then, we propagate any overflow to the left.
 *
 *     It's basically long-hand base 3 (or 4 - with 'Q' and 'Z') addition.
 *     Thus, we can signal the caller that the final permutation was done
 *     by checking whether or not the counter has been reset to 0, and we
 *     have an overflow.
 *
 *     This function makes only one pass over the number, and the state
 *     structure is provided, thus each iteration runs in O(n) time and
 *     O(1) space. So, the whole thing will run in:
 *
 *     Q & Z Excluded: O(3^n) time
 *     Q & Z Included: O(4^n) time
 *
 * Returns:
 *     -EINVAL if invalid arguments are passed to this function
 *     1       if the sequence is finished
 *     0       otherwise
 *
 */
int permute_num(struct phone_state *state,
                const char *number,
                int num_len,
                int qz)
{
	int i, current;
	char cur_num;

	/* Validate our arguments */
	if (!state || !number || !num_len || !*number)
		return -EINVAL;

	/* Render the state, and propagate overflow */
	for (i=0;i<num_len;i++) {
		int cur_idx = num_len - i - 1;
		cur_num = number[cur_idx];

		/* '0' and '1' should be passed through */
		if (cur_num == '0' || cur_num == '1') {
			state->current_perm[cur_idx] = cur_num;
			continue;
		}

		/* Get and clear the counter for the current position */
		current = ((state->counter >> (2 * i)) & 3);
		state->counter &= ~(3 << (2 * i));

		/* Propagate prior overflow */
		if (state->counter & OVERFLOW) {
			state->counter &= ~OVERFLOW;
			current++;
		}

		/* Check for overflow and update the state */
		if (current == ((qz && (cur_num == '7' ||
		                        cur_num == '9')) ? 4 : 3)) {
			current         = 0;
			state->counter |= OVERFLOW;
		} else {
			state->counter &= ~OVERFLOW;
			state->counter |= current << (2*i);
		}

		/* Update the current letter */
		cur_num = digit_to_alpha[cur_num - '2'] + current;
		state->current_perm[cur_idx] = (char)('A' + cur_num);
	}

	/* Check for wrap-around (signifies the end of the sequence) */
	if (state->perm_count > 1 && state->counter == OVERFLOW)
		return 1;

	/* Increment the right-most counter (this drives the algorithm) */
	cur_num        = number[num_len - 1];
	current        = (state->counter & 3) + 1;
	state->counter = (state->counter & ~3) | (current & 3);

	/* If we're not using Q, skip it */
	if (!qz && cur_num == '7' && current >= 1) current++;

	/* Check for overflow (in case of Q and Z) */
	if (qz && current == 4 && (cur_num == '7' || cur_num == '9'))
		state->counter |= (OVERFLOW | 3);

	/* Print our state */
	printf("#%d: %s\n", ++state->perm_count, state->current_perm);
	return 0;
}

/**
 * Note: The maximum possible permutations will be in the range:
 * 1 .. 3^7 (2187) or 4^7 (16384).
 *
 * For example, if the number contains:
 *     0's and 1's only: 1 permutation.
 *     7's and 9's only: 3^7 permutations, or
 *                       4^7 permutations (if enable_qz is set.)
 *     no 0's or 1's:    3^7 permutations (if enable_qz is not set.)
 */
int main(int argc, char *argv[])
{
	int qz=0, num_len, i;
	struct phone_state *state;

	/* Handle arguments */
	if (argc < 2 || !argv[1]) {
		printf("%s phone_number [enable_qz]\n",argv[0]);
		printf("\tphone_number: Phone number (e.g. 8675309)\n");
		printf("\tenable_qz:    1: Enable use of 'Q' and 'Z'\n");
		printf("\t              0: Disable (default)\n");
		exit(EXIT_FAILURE);
	} else if (argc == 3) {
		qz = (argv[2] && atoi(argv[2]) == 1) ? 1 : 0;
	}

	/* Get the length of the number */
	num_len = strlen(argv[1]);
	if (num_len > 7) {
		ERROR("The number can't be longer than 7 digits.\n");
		exit(EXIT_FAILURE);
	}

	/* Allocate/Initialize our state structure */
	if (!(state = calloc(1, sizeof(struct phone_state)))) {
		ERROR("Unable to allocate memory for state structure!\n");
		exit(EXIT_FAILURE);
	}

	if (!(state->current_perm = calloc(num_len + 1, sizeof(char)))) {
		ERROR("Unable to allocate memory for the perm. buffer!\n");
		free(state);
		exit(EXIT_FAILURE);
	} else memset(state->current_perm, ' ', num_len);

	/* Run the sequence */
	i = 0;
	while (!i) {
		i = permute_num(state, (const char *)argv[1], num_len, qz);
	}

	/* Finish up */
	if (i < 0) ERROR_1("%s\n", strerror(-1 * i));
	else       printf("Done!\n");
	free(state->current_perm);
	free(state);
	return 0;
}

