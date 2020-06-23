#ifndef __PROMPT_H_
#define __PROMPT_H_

typedef char *(*Operator)(char *, ...);

typedef struct
{
  char *key;
  Operator operation;
} operation_set;

char *parse(char *str, operation_set **, int code);
operation_set **get_operations();

//dir, sDir, color, time, date, status

#endif