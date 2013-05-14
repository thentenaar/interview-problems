/**
 * An example solution to the classic "maximum subarray" problem
 * Copyright (C) 2013 Tim Hentenaar.
 *
 * This code is licenced under the Simplified BSD License.
 * See the LICENSE file for details.
 *
 * Compiling: gcc -ansi -pedantic -Wall -O2 -o subarray subarray.c
 *
 * Running:
 *     O(n^2) algorithm:
 *
 *     tim@cid ~ $ ./subarray 0 -1 2 5 -1 3 -2 1
 *     The maximum sub-array is: [ 2, 5, -1, 3 ] with sum: 9
 *
 *     O(n) algorithm:
 *
 *     tim@cid ~ $ ./subarray 1 -1 2 5 -1 3 -2 1
 *     The maximum sub-array is: [ 2, 5, -1, 3 ] with sum: 9
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * A struct to keep track of the current max. subarray.
 */
struct sub_array {
	unsigned int start;
	unsigned int end;
	long sum;
};

/**
 * Compute the sum of a subarray.
 *
 * array: Pointer to the start of the subarray
 * length: Length of the subarray
 */
long sum_array(long *array, unsigned int length)
{
	long sum = 0; int i;
	for (i=0;i<length;i++) sum += array[i];
	return sum;
}

/**
 * Find the maximum subarray by linearly searching over the array,
 * from each end, for each possible subarray length.
 *
 * For me, this was the solution which came the easiest. It's also
 * quite naieve.
 *
 * This is runs in O(n^2) time, and O(1) space.
 */
struct sub_array *find_max_subarray(long *array, unsigned int size)
{
	struct sub_array *max_subarray;
	unsigned int sa_len;

	if (!(max_subarray = calloc(1, sizeof(struct sub_array)))) {
		fprintf(stderr, "find_max_subarray: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	for (sa_len=2;sa_len<size;sa_len++) {
		unsigned int start = 0, end = size - 1;

		do {
			long sum1 = sum_array(array + start, sa_len);
			long sum2 = sum_array(array + end - sa_len, sa_len);

			if (sum1 > max_subarray->sum) {
				max_subarray->start = start;
				max_subarray->end   = start + sa_len;
				max_subarray->sum   = sum1;
			}

			if (sum2 > max_subarray->sum) {
				max_subarray->start = end - sa_len;
				max_subarray->end   = end;
				max_subarray->sum   = sum2;
			}

			start++; end--;
		} while (start <= (size >> 1)  && end > (size >> 1) &&
		         start < size - sa_len && end > sa_len);
	}

	return max_subarray;
}

/**
 * An implementation of Kadane's Algorithm for solving this problem.
 *
 * This runs in O(n) time, and O(1) space.
 */
struct sub_array *max_subarray_kadane(long *array, unsigned int size)
{
	unsigned int i,s = 0; long current_max = array[0];
	struct sub_array *max_subarray;

	if (!(max_subarray = calloc(1, sizeof(struct sub_array)))) {
		fprintf(stderr, "max_subarray_kadane: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	/**
	 * Iterate over the array, figuring the maximum subarray as a
	 * running sum.
	 */
	for (i=1;i<size;i++) {
		if (current_max < 0) {
			current_max = array[i];
			s = i;
		} else current_max += array[i];

		if (current_max > max_subarray->sum) {
			max_subarray->start = s;
			max_subarray->end   = i+1;
			max_subarray->sum   = current_max;
		}
	}

	return max_subarray;
}

void usage(char *arg0)
{
	printf("Usage: %s variant n1 n2 ...\n",arg0);
	printf("\tvariant: 0 for our O(n^2) algorithm\n");
	printf("\t         1 for Kadane's O(n) algorithm\n");
	printf("\tn: Array of integers separated by spaces\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	unsigned int i = 2; long *array = NULL;
	struct sub_array *max_subarray = NULL;
	if (argc < 3) usage(argv[0]);

	/* Allocate our array */
	if (!(array = calloc(argc - 1, sizeof(long)))) {
		fprintf(stderr, "Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	/* Read the array from the command line arguments */
	while (i < argc) {
		array[i-1] = atol((const char *)argv[i]);
		i++;
	}

	/* Do it */
	if (atoi(argv[1])) max_subarray = max_subarray_kadane(array, i-1);
	else               max_subarray = find_max_subarray(array, i-1);

	/* Print the result */
	printf("The maximum sub-array is: [ ");
	for (i=max_subarray->start;i<max_subarray->end;i++) {
		printf("%ld",array[i]);
		if (i < max_subarray->end - 1) printf(", ");
	}
	printf(" ] with sum: %ld\n", max_subarray->sum);

	free(max_subarray);
	free(array);
	return 0;
}

