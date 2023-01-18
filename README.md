# Polyphase merge sort
Algorithm implemented in C. It can be used to sort large files containing specified records. It can also generate files with the number of records chosen by the user.

## Records
Record consists of three variables: P(A): float, P(B): float, P(AB): float and the key for sorting is the conditional probability P(A/B). All this can be changed in record.h and record.c files.

## Block operations
All the files operations(read/write) are done in blocks, where each block contains a fixed number of records. It is a simulation of how data is extracted from hard disks.
