#include "s21_cat.h"

int main(int argc, char *argv[]) {
  flags options = {0};
  flagsOptions(argc, argv, &options);
  readFile(argc, argv, &options);
}

int flagsOptions(int argc, char *argv[], flags *options) {
  int flag = 1, opt, optIndex;
  static struct option opt_log[] = {
      {"number", 0, 0, 'n'},
      {"number-nonblank", 0, 0, 'b'},
      {"squeeze-blank", 0, 0, 's'},
      {0, 0, 0, 0},

  };
  while ((opt = getopt_long(argc, argv, "benstvET", opt_log, &optIndex)) !=
         -1) {
    switch (opt) {
      case 'b': {
        options->b = 1;
        options->n = 0;
        break;
      }
      case 'e': {
        options->e = 1;
        options->v = 1;
        break;
      }
      case 'n': {
        options->n = options->b ? 0 : 1;
        break;
      }
      case 's': {
        options->s = 1;
        break;
      }
      case 't': {
        options->t = 1;
        options->v = 1;
        break;
      }
      case 'v': {
        options->v = 1;
        break;
      }
      case 'E': {
        options->e = 1;
        break;
      }
      case 'T': {
        options->t = 1;
        break;
      }
      default: {
        flag = 0;
        fprintf(stderr, "Usage: %s [OPTION]... [FILE]...\n", argv[0]);
      }
    }
  }

  return flag;
}

int readFile(int argc, char *argv[], flags *options) {
  int flag = 0;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      flag = 1;
      cat(argv[i], options);
    }
  }
  if (!flag) {
    fprintf(stderr, "THIS FILE DOES NOT EXIST");
  }
  return flag;
}

void cat(char *fileName, flags *options) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    fprintf(stderr, "THIS FILE DOES NOT EXIST: %s !", fileName);
  } else {
    int currentSymbol;
    int lineCurrent = 1, count = 1, lastLineEmpty = 1, currentLineEmpty = 0;
    currentSymbol = fgetc(file);
    while (!feof(file)) {
      if (options->s && lineCurrent && currentSymbol == '\n') {
        if (currentLineEmpty) {
          currentLineEmpty = -1;
        }
      }
      if (options->n && lineCurrent) {
        if (currentLineEmpty != -1) {
          printf("%6d\t", count++);
        }
      }
      if (options->b && lineCurrent && currentSymbol != '\n') {
        printf("%6d\t", count++);
      }
      if (options->e && currentSymbol == '\n') {
        if (currentLineEmpty != -1) {
          printf("$");
        }
      }
      if (options->v) {
        int ch = (int)currentSymbol;
        if (currentSymbol < 0) {
          currentSymbol &= 127;
          ch = (int)currentSymbol;
          ch += 128;
        }

        if (ch != 9 && ch != 10 && ch < 32) {
          printf("^");
          currentSymbol += 64;
        } else if (ch == 127) {
          printf("^");
          currentSymbol = '?';
        } else if (ch > 160 && ch <= 255) {
          currentSymbol -= 128;
        }
      }
      if (options->t && currentSymbol == '\t') {
        printf("^I");
        currentLineEmpty = -1;
      }
      if (currentLineEmpty != -1) {
        printf("%c", currentSymbol);
      }
      lineCurrent = (currentSymbol == '\n') ? 1 : 0;
      currentLineEmpty = (lastLineEmpty && currentSymbol == '\n') ? 1 : 0;
      lastLineEmpty = (lineCurrent && currentSymbol == '\n') ? 1 : 0;
      currentSymbol = fgetc(file);
    }
    fclose(file);
  }
}