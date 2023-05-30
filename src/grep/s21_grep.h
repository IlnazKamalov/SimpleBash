#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int pre_name;
  int regex_flags;
} flags;

int parseFlags(int argc, char *argv[], flags *options);

int parseFiles(int argc, char *argv[], flags *options, char *strSearch);

int openFiles(char *fileName, flags *options, char *strSearch);

void appendFileToPattern(char *strSearch, char *fileName);

int findMatchesInLine(char *strSearch, char *line, char *fileName,
                      flags *options, int count);

void addPatternPart(char *strSearch, char *argv);

#endif /* S21_GREP_H */