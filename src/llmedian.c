/**
 * A program to find the median node in a linked list
 * Copyright (C) 2013 Tim Hentenaar.
 *
 * This code is licenced under the Simplified BSD License.
 * See the LICENSE file for details.
 *
 * Compiling: gcc -ansi -pedantic -Wall -W -O2 -o llmedian llmedian.c
 *
 * Running:
 *     tim@cid ~ $ ./llmedian 5 4
 *     The median node is at position 2
 *     { HEAD }  -> { IPKP }  -> M { DBUO }  -> { FEUK }  -> { FOHX }
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * Quick macro for printing error messages.
 */
#define ERROR(X) fprintf(stderr, (X))

#ifndef strndup
/**
 * A quick implementation of strndup(), since it isn't defined
 * in C89.
 */
char *strndup(const char *s, int len)
{
	char *r = NULL;
	if (!s || !(r = calloc(len + 1, sizeof(char))))
		return NULL;
	memcpy(r, s, len);
	return r;
}
#endif

/**
 * A structure representing a singly-linked list
 */
struct s_linked_list {
	char *data;
	struct s_linked_list *next;
};

/**
 * Allocate a single linked-list node.
 */
struct s_linked_list *alloc_node(char *data) 
{
	struct s_linked_list *node;

	if (!(node = calloc(1, sizeof(struct s_linked_list)))) {
		ERROR("alloc_node: Out of memory!\n");
		if (data) free(data);
		return NULL;
	}

	node->data = data;
	return node;
}

/**
 * Free a linked-list node, and all nodes connected to it.
 */
void free_node(struct s_linked_list *list)
{
	struct s_linked_list *tmp;

	while (list) {
		tmp = list->next;
		if (list->data) free(list->data);
		free(list);
		list = tmp;
	}
}

/**
 * Create a string of n random letters
 */
char *random_nchar_string(int n)
{
	char *s; int i;

	if (!(s = calloc(n + 1, sizeof(char)))) {
		ERROR("random_nchar_string: Out of memory!\n");
		return NULL;
	}

	for (i=0;i<n;i++) *(s + i) = (char)('A' + rand() % 25);
	return s;
}

/**
 * Build a singly-linked list of size nodes, with data_len
 * bytes of data.
 */
struct s_linked_list *build_linked_list(int size, int data_len)
{
	int i;
	struct s_linked_list *head,*tail;

	if (!(head = alloc_node(strndup("HEAD", 4)))) {
		ERROR("build_linked_list: Unable to allocate head!\n");
		return NULL;
	}

	for (tail=head,i=1;i<size&&tail;i++,tail=tail->next)
		tail->next = alloc_node(random_nchar_string(data_len));

	return head;
}

/**
 * Find the median node
 *
 * Since this makes one pass over the entire list, more or less,
 * this runs in roughly O(n) time.
 *
 * This could have been left inline in main(), but I wanted to put
 * it in a separate function just to give a bit more promenance to
 * the core part of the solution.
 */
struct s_linked_list *find_median(struct s_linked_list *head, int *pos)
{
	struct s_linked_list *n1, *n2;
	*pos = 0; n1 = n2 = head;

	while (n1 && n2 && n2->next) {
		*pos += 1;
		n1 = n1->next;
		n2 = (n2->next && n2->next->next) ? n2->next->next : NULL;
	}

	return n1;
}

/**
 * Print the list of nodes to STDOUT.
 */
void print_list(struct s_linked_list *list, struct s_linked_list *median)
{
	struct s_linked_list *x;

	for (x=list;x;x=x->next) {
		printf("%s %s } ", (x == median) ? "M {" : "{", x->data);
		if (x->next) printf(" -> ");
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	struct s_linked_list *list, *median;
	int pos = 0, len, data_len;

	if (argc < 2 || !argv[1]) {
		printf("%s list_size [data_len]\n", argv[0]);
		printf("\tlist_size: Number of nodes to generate.\n");
		printf("\tdata_len:  Length of the random data.\n");
		exit(EXIT_FAILURE);
	}

	/* Seed the PRNG */
	srand((unsigned int)time(NULL));

	/* Build the list */
	len = atoi(argv[1]);
	data_len = argv[2] ? atoi(argv[2]) : 8;
	if (!(list = build_linked_list(len, data_len)))
		exit(EXIT_FAILURE);

	/* Find the median */
	median = find_median(list, &pos);
	printf("The median node is at position %d\n", pos);

	/* Graph the list, if we have a small number of nodes */
	if (len <= 8) print_list(list, median);

	/* Cleanup */
	free_node(list);
	return 0;
}
