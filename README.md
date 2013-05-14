interview-problems
==================

Example solutions for some basic programmer interview questions and/or semi-interesting problems,
posted here just for the hell of it.

Presentation Format
===================

The examples are written in 'ANSI' C (C89), and designed to be compiled with GCC. A Makefile is provided
for convenience. Simply invoke the almighty ``make`` to build all examples. For other compilers,
strrev.c should be compiled with ``-DUSE_C``.

Why C?
======

Because it's _the_ industry standard programming language (thank God it **ain't** Java!)
and thus, in this humble author's opinion, the ideal medium in which to publish these
sorts of solutions.

atoi.c
======

_Write an implementation of atoi(3) without using any standard library functions._

This is relatively easy as long as you understand that a number in any position is basically:
``digit * radix ^ position`` (e.g. 100 = 1 * 10^2).

Note that the functions presented here don't heed a sign prefixing a number, whereas the
glibc implementation of atoi will handle signs at least.

llmedian.c
==========

_Write a program to find the median node of a linked-list._

I present a simple O(n) solution to this problem. If you understand what a linked-list is,
and how it works, this is pretty painless.

rand.c
======

_Given an upper bound, a lower bound, and a sequence length, write a program which generates
a sequence of random numbers (without using any standard library functions.)_

I present two solutions. One which reads the string left-to-right, and one which reads it
right-to-left. Both solutions handle hexadecimal as well as decimal strings.

strrev.c
========

_Write a program to reverse a string, in place._

I provide a *pure-C* solution and slightly faster assembly solutions for 32-bit
and 64-bit x86 CPUs. My solutions run in O(n) time.

subarray.c
==========

_Given an array of integers, find the maximum contigious subarray._

I provide two solutions. A quickly-written O(n^2) solution, and a O(n) solution based on
Kadane's algorithm.
