#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "prompt.h"

void replace_home_path(char *home, char *cwd)
{
  unsigned home_path_length = strlen(home);
  int is_home_path_present = strncmp(home, cwd, home_path_length);
  if (is_home_path_present != 0)
  {
    strcpy(home, cwd);
    return;
  }
  strcpy(home, "~");
  strcat(home, cwd + home_path_length);
}

char *cwd(char *none, ...)
{
  char *original = getenv("PWD");
  char *cwd = malloc(sizeof(char) * (strlen(original) + 1));
  strcpy(cwd, original);
  return cwd;
}

char *date(char *none, ...)
{
  char *date_str = malloc(sizeof(char) * (strlen(__DATE__) + 1));
  strcpy(date_str, __DATE__);
  return date_str;
}

char *time(char *none, ...)
{
  char *time_str = malloc(sizeof(char) * (strlen(__TIME__) + 1));
  strcpy(time_str, __TIME__);
  return time_str;
}

char *status(char *none, ...)
{
  va_list args;
  va_start(args, none);
  va_arg(args, operation_set **);
  int code = va_arg(args, int);
  va_end(args);
  char *str = malloc(sizeof(char) * 30);
  strcpy(str, code ? "‣" : "\033[0;31m‣\033[0m");
  str = realloc(str, sizeof(char) * (strlen(str) + 1));
  return str;
};

char *red(char *str, ...)
{
  va_list args;
  va_start(args, str);
  operation_set **operations = va_arg(args, operation_set **);
  int code = va_arg(args, int);
  char *prompt = malloc(sizeof(char) * 100);
  strcpy(prompt, "\033[0;31m");
  strcat(prompt, parse(str, operations, code));
  strcat(prompt, "\033[0m");
  prompt = realloc(prompt, (strlen(prompt) + 1) * sizeof(char));
  va_end(args);
  return prompt;
}

char *yellow(char *str, ...)
{
  va_list args;
  va_start(args, str);
  operation_set **operations = va_arg(args, operation_set **);
  int code = va_arg(args, int);
  char *prompt = malloc(sizeof(char) * 100);
  strcpy(prompt, "\033[0;33m");
  strcat(prompt, parse(str, operations, code));
  strcat(prompt, "\033[0m");
  prompt = realloc(prompt, (strlen(prompt) + 1) * sizeof(char));
  va_end(args);
  return prompt;
}

char *green(char *str, ...)
{
  va_list args;
  va_start(args, str);
  operation_set **operations = va_arg(args, operation_set **);
  int code = va_arg(args, int);
  char *prompt = malloc(sizeof(char) * 100);
  strcpy(prompt, "\033[0;32m");
  strcat(prompt, parse(str, operations, code));
  strcat(prompt, "\033[0m");
  prompt = realloc(prompt, (strlen(prompt) + 1) * sizeof(char));
  va_end(args);
  return prompt;
}

char *blue(char *str, ...)
{
  va_list args;
  va_start(args, str);
  operation_set **operations = va_arg(args, operation_set **);
  int code = va_arg(args, int);
  char *prompt = malloc(sizeof(char) * 100);
  strcpy(prompt, "\033[0;34m");
  strcat(prompt, parse(str, operations, code));
  strcat(prompt, "\033[0m");
  prompt = realloc(prompt, (strlen(prompt) + 1) * sizeof(char));
  va_end(args);
  return prompt;
}

char *magenta(char *str, ...)
{
  va_list args;
  va_start(args, str);
  operation_set **operations = va_arg(args, operation_set **);
  int code = va_arg(args, int);
  char *prompt = malloc(sizeof(char) * 100);
  strcpy(prompt, "\033[0;35m");
  strcat(prompt, parse(str, operations, code));
  strcat(prompt, "\033[0m");
  prompt = realloc(prompt, (strlen(prompt) + 1) * sizeof(char));
  va_end(args);
  return prompt;
}

char *cyan(char *str, ...)
{
  va_list args;
  va_start(args, str);
  operation_set **operations = va_arg(args, operation_set **);
  int code = va_arg(args, int);
  char *prompt = malloc(sizeof(char) * 100);
  strcpy(prompt, "\033[0;36m");
  strcat(prompt, parse(str, operations, code));
  strcat(prompt, "\033[0m");
  prompt = realloc(prompt, (strlen(prompt) + 1) * sizeof(char));
  va_end(args);
  return prompt;
}

char *short_cwd(char *none, ...)
{
  char *short_cwd = malloc(sizeof(char) * 100);
  strcpy(short_cwd, getenv("HOME"));
  char *cwd = getenv("PWD");
  replace_home_path(short_cwd, cwd);
  short_cwd = realloc(short_cwd, sizeof(char) * (strlen(short_cwd) + 1));
  return short_cwd;
}

operation_set **get_operations()
{
  operation_set **operations = malloc(sizeof(operation_set *) * 12);
  operations[0] = malloc(sizeof(operation_set));
  operations[0]->key = malloc(sizeof(char) * 4);
  operations[0]->key = "cwd";
  operations[0]->operation = cwd;

  operations[1] = malloc(sizeof(operation_set));
  operations[1]->key = malloc(sizeof(char) * 10);
  operations[1]->key = "short_cwd";
  operations[1]->operation = short_cwd;

  operations[2] = malloc(sizeof(operation_set));
  operations[2]->key = malloc(sizeof(char) * 5);
  operations[2]->key = "date";
  operations[2]->operation = date;

  operations[3] = malloc(sizeof(operation_set));
  operations[3]->key = malloc(sizeof(char) * 5);
  operations[3]->key = "time";
  operations[3]->operation = time;

  operations[4] = malloc(sizeof(operation_set));
  operations[4]->key = malloc(sizeof(char) * 4);
  operations[4]->key = "red";
  operations[4]->operation = red;

  operations[5] = malloc(sizeof(operation_set));
  operations[5]->key = malloc(sizeof(char) * 5);
  operations[5]->key = "blue";
  operations[5]->operation = blue;

  operations[6] = malloc(sizeof(operation_set));
  operations[6]->key = malloc(sizeof(char) * 7);
  operations[6]->key = "yellow";
  operations[6]->operation = yellow;

  operations[7] = malloc(sizeof(operation_set));
  operations[7]->key = malloc(sizeof(char) * 8);
  operations[7]->key = "magenta";
  operations[7]->operation = magenta;

  operations[8] = malloc(sizeof(operation_set));
  operations[8]->key = malloc(sizeof(char) * 5);
  operations[8]->key = "cyan";
  operations[8]->operation = cyan;

  operations[9] = malloc(sizeof(operation_set));
  operations[9]->key = malloc(sizeof(char) * 6);
  operations[9]->key = "green";
  operations[9]->operation = green;

  operations[10] = malloc(sizeof(operation_set));
  operations[10]->key = malloc(sizeof(char) * 7);
  operations[10]->key = "status";
  operations[10]->operation = status;

  operations[11] = NULL;
  return operations;
}

Operator get_operator(operation_set **operations, char *op)
{
  for (unsigned index = 0; operations[index] != NULL; index++)
  {
    if (strcmp((operations[index])->key, op) == 0)
      return operations[index]->operation;
  }
}

Op_P *get_operator_and_param(char *str)
{
  unsigned index;
  int inner_func_num = 0;
  Op_P *result = malloc(sizeof(Op_P));
  result->op = malloc(sizeof(char) * 50);
  result->param = malloc(sizeof(char) * 50);
  for (index = 0; str[index] != '('; index++)
    result->op[index] = str[index];
  result->op[index++] = '\0';
  result->op = realloc(result->op, sizeof(char) * index);
  unsigned i = 0;
  for (; str[index] != ')' || inner_func_num != 0; index++)
  {
    (str[index] == '#') && (inner_func_num++);
    str[index] == ')' && (inner_func_num--);
    result->param[i++] = str[index];
  }
  result->param[i++] = '\0';
  result->param = realloc(result->param, sizeof(char) * i);
  return result;
}

char *parse(char *str, operation_set **operations, int code)
{
  char *prompt = malloc(sizeof(char) * 100);
  unsigned prompt_pos = 0;
  for (unsigned index = 0; str[index] != '\0'; index++)
  {
    if (str[index] == '#')
    {
      Op_P *oAndP = get_operator_and_param(str + index + 1);
      Operator func = get_operator(operations, oAndP->op);
      char *token = func(oAndP->param, operations, code);
      for (unsigned i = 0; token[i] != '\0'; i++)
        prompt[prompt_pos++] = token[i];
      index += strlen(oAndP->op) + strlen(oAndP->param) + 2;
      free(oAndP->op);
      free(oAndP->param);
      free(oAndP);
      free(token);
      continue;
    }

    prompt[prompt_pos++] = str[index];
  }
  prompt[prompt_pos++] = '\0';
  prompt = realloc(prompt, prompt_pos * sizeof(char));
  return prompt;
}