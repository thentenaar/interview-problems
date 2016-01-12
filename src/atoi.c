/**
 * Example of a simple atoi(3) implementation
 * Copyright (C) 2013 Tim Hentenaar.
 *
 * This code is licenced under the Simplified BSD License.
 * See the LICENSE file for details.
 *
 * Compiling: gcc -ansi -pedantic -Wall -W -O2 -o atoi atoi.c
 *
 * Running:
 *     tim@cid ~ $ ./atoi abcdef
 *     [RTL] The number was: 11259375 (hex: 0xabcdef)
 *     [LTR] The number was: 11259375 (hex: 0xabcdef)
 *     tim@cid ~ $ ./atoi 987654321
 *     [RTL] The number was: 987654321 (hex: 0x3ade68b1)
 *     [LTR] The number was: 987654321 (hex: 0x3ade68b1)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * A simple C implementation of atoi(3) using no standard
 * library functions.
 *
 * This can only handle base 10 (decimal), and base 16 (hexadecimal)
 * numbers, and will ignore any non-numeric characters on the right-hand
 * side of the string.
 *
 * The "bug" (read: feature) here is that if such a character occurs in
 * the middle of the string, the number returned here will represent
 * only the right-most part of the string. The caller may be expecting
 * either 0, or the left-most part of the string in this circumstance.
 *
 * The goal of this exercise is not to write a perfect clone of atoi,
 * and in fact, atoi performs no error checking and can only handle a
 * string of decimal digits. Here we also modify the original string,
 * converting upper-case hex digits to lowercase before the main
 * processing loop.
 *
 * It's further assumed that the numbers entered correspond to the
 * ASCII number characters (0x30 - 0x39).
 *
 */
int my_atoi_rtl(char *str)
{
	int n = 0, base = 10, exponent = 1; char *start;
	if (!str) return 0;

	/* Assume the number is decimal, but check for hex digits */
	for (start=str;*start;start++) {
		if (*start >= 'A' && *start <= 'F') *start += 0x20;
		if (*start >= 'a' && *start <= 'f') base = 16;
	}

	/* Move to the end of the string */
	start = str; while (*start) start++; start--;

	/* Do the actual conversion */
	for (;start>=str;start--) {
		if (*start >= '0' && *start <= '9')
			n += (*start - '0') * exponent;
		else if (*start >= 'a' && *start <= 'f' && base == 16)
			n += (0x0A + (*start - 'a')) * exponent;
		else if (exponent == 1) continue;
		else break;

		exponent *= base;
	}

	return n;
}

/**
 * This is similar to the above function, except that this one processes
 * the string left-to-right.
 *
 * This function has a similar "feature" to the above. The difference is
 * that if you have a non-numeric character occurs in the middle of the
 * string, only the left-most portion of the string is returned.
 *
 * If such a character occurs at the beginning of the string, 0 is
 * returned.
 */
int my_atoi_ltr(char *str)
{
	char *start;
	int n = 0, base = 10, exponent = 0;
	if (!str) return 0;

	/* Assume the number is decimal, but check for hex digits */
	for (start=str;*start;start++) {
		if (*start >= 'A' && *start <= 'F') *start += 0x20;
		if (*start >= 'a' && *start <= 'f') base = 16;
	}

	/* Find the exponent for the first position on the left */
	for (start=str;*start;start++) {
		if (!((*start >= '0' && *start <= '9') ||
		     (base == 16 && *start >= 'a' && *start <= 'f'))) break;
		else exponent = (!exponent ? 1 : exponent * base);
	}

	/* Do the actual conversion */
	start = str;
	while (*start && exponent >= 1) {
		if (*start >= '0' && *start <= '9')
			n += (*start - '0') * exponent;
		else if (*start >= 'a' && *start <= 'f' && base == 16)
			n += (0x0A + (*start - 'a')) * exponent;
		else break;

		start++;
		exponent /= base;
	}

	return n;
}

int main(int argc, char *argv[])
{
	int n;

	/* Check arguments */
	if (argc < 2 || !argv[1]) {
		printf("%s <number>\n", argv[0]);
		printf("Convert an number represented in ASCII to an integer\n");
		printf("\tnumber: A number to convert\n");
		exit(EXIT_FAILURE);
	}

	/* Do it */
	n = my_atoi_rtl(argv[1]);
	printf("[RTL] The number was: %d (hex: 0x%x)\n", n, n);
	n = my_atoi_ltr(argv[1]);
	printf("[LTR] The number was: %d (hex: 0x%x)\n", n, n);
	return 0;
}

