/**
 * A simple string reversing example
 * Copyright (C) 2013 Tim Hentenaar.
 *
 * This code is licenced under the Simplified BSD License.
 * See the LICENSE file for details.
 *
 * Compiling: gcc -ansi -pedantic -Wall -W -O2 -o strrev strrev.c
 * Defines:
 *     USE_C: Use the C variant of strrev (default for non-x86 arches.)
 *
 * Running:
 *     tim@cid ~ $ ./strrev "deo vindice"
 *     Reversing: deo vindice
 *     Result:    ecidniv oed
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * There's no one standard with regard to how 'asm' and
 * 'volatile' are implemented by the compiler.
 */
#if !defined(asm) && defined(__asm__)
#define asm __asm__
#elif !defined(USE_C)
#define USE_C
#endif

#if !defined(volatile) && defined (__volatile__)
#define volatile __volatile__
#elif !defined(USE_C)
#define USE_C
#endif

/**
 * Reverse a string in-place
 *
 * Here, I present two ways for doing so. One written in
 * assembly (x86_64 and x86), the other in plain C. The
 * assembly version is much more optimal than the C version, simpler too,
 * despite the fact that the C version looks simpler.
 *
 * The C version of this function can be written a few different ways.
 * One is what I've written here, and another might very well be nearly
 * the same, but using a temporary variable to hold a byte while swapping.
 *
 * NOTE: the '.intel_syntax' directive requires binutils >= 2.10.
 *
 */
char *strrev(char *str)
{
	#if !defined(USE_C) && defined(__x86_64__)
	asm volatile(
		".intel_syntax noprefix\n\t"
		/* Load a byte from the beginning of the string */
		"mov al,[rsi]\n\t"
		/* Exchange this with a byte from the end of the string */
		"xchg al,[rdi]\n\t"
		/* Store the byte from the end at the beginning */
		"mov [rsi],al\n\t"
		/* Decrement rdi */
		"dec rdi\n\t"
		/* Increment rsi */
		"inc rsi\n\t"
		/* When rsi >= rdi, there are no bytes left to swap */
		"cmp rsi,rdi\n\t"
		"jcs $-15\n\t"
		".att_syntax\n\t"
		: /* No outputs */
		: "S"(str), "D"(str + strlen(str) - 1)
		: "flags"
	);
	#elif !defined(USE_C) && defined(__i386__)
	asm volatile(
		".intel_syntax noprefix\n\t"
		/* Load a byte from the beginning of the string */
		"mov al,[esi]\n\t"
		/* Exchange this with a byte from the end of the string */
		"xchg al,[edi]\n\t"
		/* Store the byte from the end at the beginning */
		"mov [esi],al\n\t"
		/* Decrement edi */
		"dec edi\n\t"
		/* Increment esi */
		"inc esi\n\t"
		/* When esi >= edi, there are no bytes left to swap */
		"cmp esi,edi\n\t"
		"jcs $-10\n\t"
		".att_syntax\n\t"
		: /* No outputs */
		: "S"(str), "D"(str + strlen(str) - 1)
		: "flags"
	);
	#else /* USE_C */
	char *start = str;
	char *end   = str + strlen(str) - 1;
	while (start < end) {
		*start ^= *end;
		*end   ^= *start;
		*start ^= *end;
		start++; end--;
	}
	#endif

	return str;
}

int main(int argc, char *argv[])
{
	/* Check arguments */
	if (argc < 2 || !argv[1]) {
		printf("%s <string>\n", argv[0]);
		printf("Reverse a string in place\n");
		printf("\tstring: string to reverse\n");
		exit(EXIT_FAILURE);
	}

	/* Do it */
	printf("Reversing: %s\n", argv[1]);
	printf("Result:    %s\n", strrev(argv[1]));
	return 0;
}

