/**
 * Calc: Simple Arithmetic Expression Solver
 * Copyright (C) 2013 Tim Hentenaar.
 *
 * This code is licenced under the Simplified BSD License.
 * See the LICENSE file for details.
 *
 * Compiling: gcc -ansi -pedantic -Wall -W -O2 -o calc calc.c
 *
 * Running:
 *     tim@cid ~ $ ./calc "1 + 2"
 *     Result: 3
 *     tim@cid ~ $ ./calc "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3 / 1"
 *     Result: 3
 *     tim@cid ~ $ ./calc "8 ^ 2"
 *     Result: 64
 *     tim@cid ~ $ ./calc "-2 + 1"
 *     Result: -1
 *     tim@cid ~ $ ./calc "3 * 2 + 2 + 2 / 1"
 *     Result: 10
 *     tim@cid ~ $ ./calc "(1 + 2 * (4+5) / (6/2*(9/3)) + 1 + (4+3))"
 *     Result: 11
 *     tim@cid ~ $ ./calc "3 * 2 + 2 + 2 / 1)"
 *     ERROR: handle_ops: Unmatched ')'.
 *     tim@cid ~ $ ./calc "3 * 2 + 2 + (2 / 1"
 *     ERROR: infix_to_postfix: Unmatched '('.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Quick error macros */
#define ERROR(X)      fprintf(stderr, (X))
#define ERROR_1(X, Y) fprintf(stderr, (X), (Y))

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
 * These are our stack sizes.
 *
 * We will have around half as many operators as operands
 * in general, so these are good initial values. Besides,
 * I can't imagine anyone typing in a longer expression on
 * the command line, except to test that the stack functions
 * properly handle overflow.
 */
#define OPERATOR_STACK_SIZE 32
#define POSTFIX_STACK_SIZE  64

/**
 * We handle the simple operators here, making sure to
 * check for division by 0.
 *
 * 'p' and 'n' are unary '+' and '-'.
 *
 * This runs in O(1) time.
 */
long eval_simple_op(char op, long a, long b)
{
	switch (op) {
		case 'p': return (a < 0) ? -1 * a : a;
		case 'n': return (a > 0) ? -1 * a : a;
		case '+': return a + b;
		case '-': return a - b;
		case '*': return a * b;
		case '<': return a << b;
		case '>': return a >> b;
	}

	if (!b) {
		ERROR("eval_simple_op: Division by 0.\n");
		exit(EXIT_FAILURE);
	}

	if (op == '/') return a / b;
	return a % b;
}

/**
 * Exponents
 *
 * Here we take advantage of recursion in calculating the exponental
 * value. We could simply use pow(3) / powf(3) if we wanted to include
 * the math library (math.h / libm). But, then we'd have to use the
 * double type.
 *
 * This runs in O(m) time and space, where m is the exponent in the
 * variable 'b'.
 */
long eval_exponent(char op, long a, long b)
{
	if (b < 0)  return 0;
	if (b == 0) return 1;
	if (b == 1) return a;
	return a * eval_exponent(op, a, b - 1);
}

/**
 * Op flags: Associativity / Unary flags and Precendence
 */
#define OP_UNARY           (1 << 7)
#define OP_ASSOC_RIGHT     (1 << 6)
#define OP_ASSOC_LEFT      (1 << 5)
#define OP_PRECEDENCE_MASK 0x0f
#define OP_PRECEDENCE(X)   ((X)->flags & OP_PRECEDENCE_MASK)

/**
 * Boolean macro which evaluates to true if OP1
 * has precedence over OP2.
 */
#define OP_HAS_PRECEDENCE(OP1, OP2) (\
	(OP_PRECEDENCE(OP1)  < OP_PRECEDENCE(OP2)) ||   \
	(                                               \
	    OP_PRECEDENCE(OP1) == OP_PRECEDENCE(OP2) && \
	    (OP1)->flags & OP_ASSOC_LEFT                \
	)                                               \
)

/**
 * Our table of operators, their flags, and functions
 * to evaluate them.
 *
 * Note: In C just as we've reproduced here, the
 * bitwise-shift operators have a lower prescedence than
 * addition or subtraction.
 */
struct op {
	char op;
	unsigned int flags;
	long (*eval)(char op, long a, long b);
};

#define N_OPERATORS 12
struct op operators[N_OPERATORS] = {
	{ 'p', 4 | OP_ASSOC_RIGHT | OP_UNARY, eval_simple_op },
	{ 'n', 4 | OP_ASSOC_RIGHT | OP_UNARY, eval_simple_op },
	{ '^', 3 | OP_ASSOC_RIGHT,            eval_exponent  },
	{ '*', 2 | OP_ASSOC_LEFT,             eval_simple_op },
	{ '/', 2 | OP_ASSOC_LEFT,             eval_simple_op },
	{ '%', 2 | OP_ASSOC_LEFT,             eval_simple_op },
	{ '+', 1 | OP_ASSOC_LEFT,             eval_simple_op },
	{ '-', 1 | OP_ASSOC_LEFT,             eval_simple_op },
	{ '<', 0 | OP_ASSOC_LEFT,             eval_simple_op },
	{ '>', 0 | OP_ASSOC_LEFT,             eval_simple_op },
	{ '(', 0,                             NULL           },
	{ ')', 0,                             NULL           }
};

/**
 * Determine if a given character corresponds to an operator, and if so,
 * return that operator. Return NULL otherwise.
 *
 * A hit, or miss, both require a lookup in O(n) time. Using a hash
 * table you could reduce that to O(1), but I want to keep this simple.
 */
struct op *get_operator(char c)
{
	unsigned int i;
	for (i=0;i<N_OPERATORS;i++)
		if (c == operators[i].op) return &operators[i];
	return NULL;
}

/**
 * A generic stack structure.
 *
 * size:
 *     Upper bound of possible elements in the stack.
 *
 * pos:
 *     Our current position in the stack
 *
 * data:
 *     The area of memory which holds our stack.
 */
struct stack {
	unsigned int size;
	unsigned int pos;
	long *data;
};

/**
 * Allocate a new stack structure representing a
 * stack of the given size.
 */
struct stack *stack_init(unsigned int size)
{
	struct stack *stack;

	if (!(stack = calloc(1, sizeof(struct stack)))) {
		ERROR("stack_init: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	if (!(stack->data = calloc(size, sizeof(long)))) {
		ERROR("stack_init: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	stack->size = size;
	return stack;
}

/**
 * Push an operator, or operand onto the stack.
 *
 * This is an O(1) operation.
 */
void stack_push(struct stack *stack, long data)
{
	if (!stack) {
		ERROR("stack_push: Stack is NULL!\n");
		exit(EXIT_FAILURE);
	}

	/* Check for overflow */
	if (stack->pos == stack->size) {
		ERROR("stack_push: Stack overflow.\n");
		exit(EXIT_FAILURE);
	}

	stack->data[stack->pos++] = data;
}

/**
 * Pop an operator, or operand from the stack.
 *
 * This is an O(1) operation.
 */
long stack_pop(struct stack *stack)
{
	if (!stack) {
		ERROR("stack_pop: Stack is NULL!\n");
		exit(EXIT_FAILURE);
	}

	/* Check for underflow */
	if (stack->pos == 0) {
		ERROR("stack_pop: Stack underflow.\n");
		exit(EXIT_FAILURE);
	}

	return stack->data[--stack->pos];
}

/**
 * Free a stack structure.
 */
void stack_free(struct stack *stack)
{
	if (!stack) return;
	if (stack->data) free(stack->data);
	free(stack);
}

/**
 * Handle operators in the infix expression.
 *
 * ps:
 *     Postfix stack
 * os:
 *     Operator stack
 * op:
 *     Current operator
 * last_token_op:
 *     1 if the last token was an operator
 *     0 otherwise
 */
void handle_ops(struct stack *ps,
                struct stack *os,
                struct op **op,
                int last_token_op)
{
	struct op *top_op = NULL;

	if ((*op)->op == '(') {
		stack_push(os, (long)(*op)->op);
		return;
	}

	/**
	 * When we encounter a ')', pop into the postfix
	 * stack until '(' is found.
	 *
	 * We could also reduce the expression here.
	 */
	if ((*op)->op == ')') {
		do {
			top_op = get_operator((char)stack_pop(os));
			if (!top_op || top_op->op == '(') break;
			stack_push(ps, (long)top_op->op);
		} while (os->pos > 0);

		if (!top_op || top_op->op != '(') {
			ERROR("handle_ops: Unmatched ')'.\n");
			exit(EXIT_FAILURE);
		}

		return;
	}

	/**
	 * Check for a unary operator.
	 *
	 * A binary +/- will never follow another operator,
	 * so we can use that as a quick litmus test to see
	 * whether the operator is binary or unary.
	 */
	if ((ps->pos == 0 || last_token_op) &&
	    ((*op)->op == '-' || (*op)->op == '+')) {
		*op = get_operator((char)((*op)->op == '-' ? 'n' : 'p'));
	}

	/**
	 * If we have more than one op on the stack,
	 * check for precedence.
	 */
	if (os->pos > 0) {
		top_op = get_operator((char)os->data[os->pos - 1]);
		while (OP_HAS_PRECEDENCE(*op, top_op)) {
			stack_push(ps, stack_pop(os));
			if (!os->pos) break;
			top_op = get_operator((char)os->data[os->pos - 1]);
		}
	}

	stack_push(os, (long)((*op)->op));
}

/**
 * Convert a given expression in infix notation (e.g. 2 + 2 / 1 * 4) to
 * a stack in postfix notation (e.g. 2 2 1 / 4 * +)
 *
 * This function returns a newly allocated stack structure  containing
 * the expression in postfix notation.
 *
 * NOTE: This modifies 'expression' inline, so you can't simply pass it
 * argv[1].
 */
struct stack *infix_to_postfix(char *expression)
{
	struct op *op;
	struct stack *pf_stack, *op_stack;
	char *num_start=NULL, *expr, *expr_start, tmp;
	int last_token_op = 0;

	if (!expression || !*expression) {
		ERROR("infix_to_postfix: No expression to evaluate.\n");
		exit(EXIT_FAILURE);
	}

	/**
	 * Allocate our stacks.
	 */
	pf_stack = stack_init(POSTFIX_STACK_SIZE);
	op_stack = stack_init(OPERATOR_STACK_SIZE);
	expr     = expression;

	while (*expr) {
		/**
		 * Here, we lexically analyze the string, converting the
		 * expression into postfix, or 'Reverse-Polish' notation
		 * a la Dijkstra's Shunting-yard Algorithm.
		 *
		 * First, skip any whitespace.
		 */
		while (isspace(*expr)) expr++;
		expr_start = expr;

		/**
		 * Look for numbers, and push them onto the
		 * postfix stack.
		 */
		while (isdigit(*expr)) {
			if (!num_start) num_start = expr;
			if (!isdigit(*(expr + 1))) {
				tmp = *(expr + 1); *(expr + 1) = 0;
				stack_push(pf_stack, atol(num_start));
				*(expr + 1) = tmp;
				num_start = NULL;
				last_token_op = 0;
			}
			expr++;
		}

		/**
		 * Now, handle operators.
		 */
		if ((op = get_operator(*expr))) {
			handle_ops(pf_stack, op_stack, &op, last_token_op);
			if (op->op != '(' && op->op != ')')
				last_token_op = 1;
			expr++;
		}

		/* If we didn't get a number/op, we got an unknown token. */
		if (expr == expr_start) {
			ERROR_1("infix_to_postfix: Unknown token at %d.\n",
			        (int)(expr - expression));
			exit(EXIT_FAILURE);
		}
	}

	/* Pop the remainder of the operators into the postfix stack. */
	while (op_stack->pos > 0) {
		op = get_operator((char)stack_pop(op_stack));
		if (op->op == '(') {
			ERROR("infix_to_postfix: Unmatched '('.\n");
			exit(EXIT_FAILURE);
		}
		stack_push(pf_stack, (long)op->op);
	}

	/* Return our stack */
	stack_free(op_stack);
	return pf_stack;
}

/**
 * Apply an operator to the preceding one/two operands, and
 * place the result back on the stack in the first operand's
 * position. Then, move the rest of the stack down.
 *
 * i is updated to reflect the position of the current
 * item on the stack, minus one.
 */
void reduce_op(int *i, struct stack *pfs, struct op *op)
{
	if (!i || !pfs || !op || !op->eval) return;

	if (op->flags & OP_UNARY) {
		pfs->data[*i - 1] = op->eval(op->op, pfs->data[*i - 1], 0L);
		memmove(&pfs->data[*i],
		        &pfs->data[*i + 1],
		        sizeof(long) * (pfs->pos - *i - 1));
		pfs->pos--; (*i)--;
	} else {
		pfs->data[*i - 2] = op->eval(op->op,
		                             pfs->data[*i - 2],
		                             pfs->data[*i - 1]);
		memmove(&pfs->data[*i - 1],
		        &pfs->data[*i + 1],
		        sizeof(long) * (pfs->pos - *i));
		pfs->pos -= 2; *i -= 3;
	}
}

/**
 * Solve an expression from a stack in postfix notation.
 *
 * As you can see, this is extremely simple to solve
 * programatically.
 *
 */
long solve_postfix(struct stack *pf_stack)
{
	struct op *op; int i; long result;

	if (!pf_stack) {
		ERROR("solve_postfix: pf_stack is NULL\n");
		exit(EXIT_FAILURE);
	}

	/* Reduce all operators. */
	for (i=0;i<(int)pf_stack->pos;i++) {
		if (!(op = get_operator((char)(pf_stack->data[i]))))
			continue;
		reduce_op(&i, pf_stack, op);
	}

	/**
	 * At this point, the only thing on the stack should be our
	 * result. If we have anything left on the stack, this
	 * expression isn't properly balanced.
	 */
	result = stack_pop(pf_stack);

	if (pf_stack->pos > 0) {
		ERROR_1("solve_postfix: %d unsolved items remain.\n",
		        pf_stack->pos);
	}

	/* Free our stack */
	stack_free(pf_stack);
	return result;
}

int main(int argc, char *argv[])
{
	char *expression;
	long result = 0;

	if (argc < 2) {
		printf("Usage: %s expression\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (!(expression = strndup(argv[1], strlen(argv[1])))) {
		ERROR("main: Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	result = solve_postfix(infix_to_postfix(expression));
	printf("Result: %ld\n", result);
	free(expression);
	return 0;
}

