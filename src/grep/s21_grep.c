#include "s21_grep.h"

int main(int argc, char *argv[]) {
  flags options = {0};
  char strSearch[1000] = "";
  parseFiles(argc, argv, &options, strSearch);
}

int parseFlags(int argc, char *argv[], flags *options) {
  static struct option opt_log[] = {
      {"ignore-case", 0, 0, 'i'},
      {"invert-match", 0, 0, 'v'},
      {"line-number", 0, 0, 'n'},
      {"count", 0, 0, 'c'},
      {"regexp", 0, 0, 'e'},
      {"file", 0, 0, 'f'},
      {"files-without-matches", 0, 0, 'l'},
      {0, 0, 0, 0},

  };
  int flag = 1, opt, optIndex;

  while ((opt = getopt_long(argc, argv, "eivclnhsfo", opt_log, &optIndex)) !=
         -1) {
    switch (opt) {
      case 'e': {
        options->e = 1;
        break;
      }
      case 'i': {
        options->i = 1;
        options->regex_flags |= REG_ICASE;
        break;
      }
      case 'v': {
        options->v = 1;
        options->o = 0;
        break;
      }
      case 'c': {
        options->c = 1;
        break;
      }
      case 'l': {
        options->l = 1;
        break;
      }
      case 'n': {
        options->n = 1;
        break;
      }
      case 'h': {
        options->h = 1;
        break;
      }
      case 's': {
        options->s = 1;
        break;
      }
      case 'f': {
        options->f = 1;
        break;
      }
      case 'o': {
        options->o = !options->v;
        break;
      }
      default: {
        flag = 0;
        if (!options->s) {
          fprintf(stderr, "This flag does not exist");
        }
      }
    }
  }

  return flag;
}

int parseFiles(int argc, char *argv[], flags *options, char *strSearch) {
  char *buf[100];
  int flag = 0, flagSpace = 0, i = 0;
  for (int j = 1; j < argc; j++) {
    if (argv[j - 1][0] == '-' && argv[j - 1][strlen(argv[j - 1]) - 1] == 'e') {
      options->e = 1;
    }
  }
  for (int j = 1; j < argc; j++) {
    if (argv[j - 1][0] == '-' && argv[j - 1][strlen(argv[j - 1]) - 1] == 'e') {
      flag = 1;
      addPatternPart(strSearch, argv[j]);
    } else if (argv[j - 1][0] == '-' &&
               argv[j - 1][strlen(argv[j - 1]) - 1] == 'f') {
      flag = 1;
      appendFileToPattern(strSearch, argv[j]);

    } else if (argv[j][0] != '-') {
      if (!flag && !options->e) {
        strSearch = argv[j];
        flag = 1;
      } else {
        buf[i] = argv[j];
        i++;
      }
    }
  }

  parseFlags(argc, argv, options);

  if (i > 1 && !options->h) {
    options->pre_name = 1;
  }
  if (options->e) {
    options->regex_flags |= REG_EBRACE;
  }
  for (int j = 0; j < i; j++) {
    if (openFiles(buf[j], options, strSearch)) {
      flagSpace = 1;
    }
  }
  if ((!flag || !flagSpace || !strSearch) && !options->s) {
    fprintf(stderr, "file does not exist");
  }
  return flag;
}

int openFiles(char *fileName, flags *options, char *strSearch) {
  char line[MAX_LINE_LENGTH];
  int space = 0, count = 0, lastWasEpmty = 0;
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    if (!options->s) {
      fprintf(stderr, "grep: %s: No such file or directory", fileName);
    }
  } else {
    while (fgets(line, MAX_LINE_LENGTH, file)) {
      regex_t regex;
      if (!regcomp(&regex, strSearch, options->regex_flags)) {
        count++;
        if (!regexec(&regex, line, 0, NULL, 0) - options->v) {
          lastWasEpmty = 0;
          space++;
          if (!options->c && !options->l && !options->n && !options->o) {
            if (options->pre_name) {
              printf("%s:%s", fileName, line);
            } else {
              printf("%s", line);
            }
          } else if (options->n && !options->o && !options->l) {
            if (options->pre_name) {
              printf("%s:%d:%s", fileName, count, line);
            } else {
              printf("%d:%s", count, line);
            }
          } else if (options->o) {
            findMatchesInLine(strSearch, line, fileName, options, count);
          }
          if (line[strlen(line) - 1] != '\n' && !lastWasEpmty && !options->c &&
              !options->l) {
            lastWasEpmty = 1;
            printf("\n");
            regfree(&regex);
          }
        }

      } else {
        if (!options->s) {
          return 0;
        }
      }
      regfree(&regex);
    }
    if (options->c) {
      if (options->pre_name) {
        printf("%s:", fileName);
      }
      printf("%d", space);
      printf("\n");
    }
    if (options->l && space) {
      if (options->c) {
        printf("\n");
      }
      printf("%s\n", fileName);
    }
    fclose(file);
  }

  return space;
}

void appendFileToPattern(char *strSearch, char *fileName) {
  FILE *fp = fopen(fileName, "r");
  char *buff;
  long fileSize;
  if (!fp) {
    fprintf(stderr, "NO FILE!");
    return;
  }
  fseek(fp, 0, SEEK_END);
  fileSize = ftell(fp);
  rewind(fp);
  buff = (char *)malloc(fileSize + 1);
  fread(buff, fileSize, 1, fp);
  buff[fileSize] = '\0';
  char *p = buff;
  while (*p != '\0') {
    if (*p == '\n') {
      *p = '|';
    }
    p++;
  }
  if (!strSearch[0]) {
    strcat(strSearch, buff);
  } else {
    strcat(strSearch, "|");
    strcat(strSearch, buff);
  }
  free(buff);
  fclose(fp);
}

int findMatchesInLine(char *strSearch, char *line, char *fileName,
                      flags *options, int count) {
  char *ptr = line;
  int i = 0, result = 0;
  regex_t regexO;

  if (regcomp(&regexO, strSearch, options->regex_flags | REG_EXTENDED)) {
    printf("ERROR");
    return 1;
  }

  int found = 0;  // флаг, указывающий, было ли уже найдено вхождение шаблона

  while (!found && strSearch[0] != '\0') {
    regmatch_t regmatch[1024];
    result = regexec(&regexO, ptr + i, 1024, regmatch, 0);

    if (result == REG_NOMATCH) {
      break;
    } else if (result != 0) {
      return 0;
    } else {
      int start = (int)regmatch[0].rm_so;
      int end = (int)regmatch[0].rm_eo;
      int len = end - start;

      if (options->pre_name) printf("%s:", fileName);
      if (options->n) printf("%d:", count);

      printf("%.*s", len, ptr + i + start);
      printf("\n");

      found = 1;  // установка флага, что было найдено первое вхождение
      i += end;
    }
  }

  while (found && strSearch[0] != '\0' && ptr != NULL) {
    ptr = strstr(ptr, strSearch);  // поиск следующего вхождения
    if (ptr != NULL) {
      regmatch_t regmatch[1024];
      result = regexec(&regexO, ptr, 1024, regmatch, 0);

      if (result == REG_NOMATCH) {
        break;
      } else if (result != 0) {
        return 0;
      } else {
        int start = (int)regmatch[0].rm_so;
        int end = (int)regmatch[0].rm_eo;
        int len = end - start;

        if (options->pre_name) printf("%s:", fileName);
        if (options->n) printf("%d:", count);

        printf("%.*s", len, ptr + start);
        printf("\n");

        i = (int)(ptr + end -
                  line);  // продолжение поиска со следующего символа
      }
    }
  }

  printf("\b");
  regfree(&regexO);
  return 1;
}

void addPatternPart(char *strSearch, char *argv) {
  if (!strSearch[0]) {
    strcat(strSearch, argv);
  } else {
    strcat(strSearch, "|");
    strcat(strSearch, argv);
  }
}