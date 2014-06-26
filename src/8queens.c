/**
 * 8-Queens: 8-Queens Problem Solver (n <= 8 on a 8x8 board)
 * Copyright (C) 2014 Tim Hentenaar.
 *
 * This code is licenced under the Simplified BSD License.
 * See the LICENSE file for details.
 *
 * Compiling: gcc -ansi -pedantic -Wall -O2 -o 8queens 8queens.c
 *
 * Running:
 *     tim@cid ~ $ ./8queens 1
 *     1-queens: 64 solutions found
 *
 *     Solution 1:
 *     .......Q
 *     ........
 *     ........
 *     ........
 *     ........
 *     ........
 *     ........
 *     ........
 *
 *     [...]
 *
 *     Solution 64:
 *     ........
 *     ........
 *     ........
 *     ........
 *     ........
 *     ........
 *     ........
 *     Q.......
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *candidate;
char **solutions;
int  n_solutions;

/**
 * Add the candidate solution to the list of accepted solutions.
 */
void add_solution()
{
	if (!(solutions = realloc(solutions, (n_solutions + 2) * 8))) {
		fprintf(stderr, "ERROR: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	n_solutions++;
	if (!(solutions[n_solutions] = calloc(8, sizeof(char)))) {
		fprintf(stderr, "ERROR: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	memcpy(solutions[n_solutions], candidate, 8 * sizeof(char));
}

/**
 * Print a solution to STDOUT.
 */
void print_solution(int s)
{
	int i,j; char *out;

	if (!(out = calloc(8 + 1, sizeof(char)))) {
		fprintf(stderr, "ERROR: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	printf("Solution %d:\n", s + 1);
	for (i=0;i<8;i++) {
		for (j=0;j<8;j++) {
			out[7 - j] = (solutions[s][i] & (1 << j)) ? 'Q' : '.';
		}

		printf("%s\n", out);
	}

	printf("\n");
	free(out);
}

/**
 * Attempt to find a solution for placing n queens on
 * a 8x8 chessboard, such that no queen may attack
 * another.
 *
 * This was the simplest solution I could come up
 * with that serves to demonstrate using a Depth-
 * First-Search algorithm to solve this problem.
 *
 * The beauty of this algorithm is that we can take
 * advantage of the fact that a chessboard is 8x8,
 * and a char is 8 bits wide, to speed up our
 * comparisons by using bitwise ops.
 *
 * Here's an example representation of the board:
 *
 *  . = Empty cell
 *  Q = Queen
 *  x = Tiles attacked by a Queen
 *
 *   7 6 5 4 3 2 1 0
 * 0 x x x x x x x Q 0x01
 * 1 x x x x x Q x x 0x04
 * 2 . . . . x x x x 0x00
 * 3 . . . x x x . x 0x00
 * 4 . . x x . x . x 0x00
 * 5 . x x . . x . x 0x00
 * 6 x x . . . x . x 0x00
 * 7 x . . . . x . x 0x00
 *
 * State Variables:
 *     n:    Number of queens to place
 *     row:  Current row of the board
 *     cols: Columns containing a queen
 *     ldg:  Squares being attacked diagonally from the right
 *     rdg:  Squares being attacked diagonally from the left
 */
void solve(int n, int row, char cols, char ldg, char rdg)
{
	unsigned char pos, possible;

	/* Do we have a complete solution? */
	if (!n || row > 7) {
		add_solution();
		return;
	} else if ((cols | ldg | rdg) == 0xff) {
		/* Dead end. */
		return;
	}

	/* Get all possible positions for placing a queen */
	possible = ~(cols | ldg | rdg);

	while (possible) {
		/* Get the right-most possibility */
		pos = possible & -possible;

		/* Remove it from the list of possibilities */
		possible &= ~pos;

		/* Update the candidate row */
		candidate[row] = pos;

		/* Solve the next row */
		solve(n-1, row+1, cols | pos, (ldg | pos) << 1, (rdg | pos) >> 1);
	}
}

int main(int argc, char *argv[])
{
	int n, i;
	n_solutions = -1; solutions = NULL;

	/* Get 'n' */
	if (argc < 2) {
		printf("Usage: %s n\n", argv[0]);
		exit(EXIT_FAILURE);
	} else n = atoi(argv[1]);

	/* Allocate our candidate solution */
	if (!(candidate = calloc(8, sizeof(char)))) {
		fprintf(stderr, "ERROR: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	/* Solve for n queens, starting on each possible row. */
	for (i=0;i<8 && 8-i >= n;i++) {
		solve(n, i, 0, 0, 0);
		memset(candidate, 0, 8 * sizeof(char));
	}

	/* Print out our solutions */
	printf("%d-queens: %d solutions found\n\n", n, n_solutions + 1);
	for (i=0;i<=n_solutions;i++) {
		print_solution(i);
		free(solutions[i]);
	}

	if (solutions) free(solutions);
	free(candidate);
	return 0;
}

