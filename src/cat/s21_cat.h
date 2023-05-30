#ifndef S21_CAT_H
#define S21_CAT_H

#include <getopt.h>
#include <stdio.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} flags;

int flagsOptions(int argc, char *argv[], flags *options);
int readFile(int argc, char *argv[], flags *options);
void cat(char *file, flags *options);

#endif /* S21_CAT_H */