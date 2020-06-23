#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "dictionary.h"
#include "prompt.h"

typedef enum
{
  false,
  true
} Bool;

void safe_exit(int signal)
{
  exit(1);
}

char **remove_unnecessay_arg(char **args)
{
  char **new_args = malloc(sizeof(char *) * 100);
  unsigned new_arg_index = 0;
  for (unsigned index = 0; args[index] != NULL; index++)
  {
    if (strcmp(args[index], "") != 0)
      new_args[new_arg_index++] = args[index];
  }
  return new_args;
}

char *get_key(char *str)
{
  char *key = malloc(sizeof(char) * 30);
  unsigned index;
  for (index = 0; str[index] != ']' && str[index] != '\0'; index++)
  {
    key[index] = str[index];
  }
  key[index] = '\0';
  return key;
}

char **parse_command_string(char *command_string, Dictionary *variables)
{
  char **args = malloc(sizeof(char *) * strlen(command_string));
  unsigned arg_num = 0, string_pos = 0;
  Bool is_escaped = false, double_quote = false, single_quote = false;
  args[0] = malloc(sizeof(char) * strlen(command_string));

  for (unsigned index = 0; command_string[index] != '\0'; index++)
  {
    if (is_escaped)
    {
      args[arg_num][string_pos++] = command_string[index];
      is_escaped = false;
      continue;
    }

    if (command_string[index] == '\\')
    {
      is_escaped = true;
      continue;
    }

    if (command_string[index] == '"' && !single_quote)
    {
      double_quote = !double_quote;
      continue;
    }

    if (command_string[index] == '\'' && !double_quote)
    {
      single_quote = !single_quote;
      continue;
    }

    if (command_string[index] == ' ' && !double_quote && !single_quote)
    {
      args[arg_num][string_pos] = '\0';
      args[arg_num] = realloc(args[arg_num], sizeof(char) * (string_pos + 1));
      args[++arg_num] = malloc(sizeof(char) * strlen(command_string));
      string_pos = 0;
      continue;
    }

    if (command_string[index] == '[' && !double_quote && !single_quote)
    {
      char *key = get_key(command_string + index + 1);
      char *value = get_value(variables, key);
      value == NULL && (value = "");
      for (unsigned i = 0; value[i] != '\0'; i++)
      {
        args[arg_num][string_pos++] = value[i];
      }
      index += strlen(key) + 1;
      command_string[index] == '\0' && (index--);
      continue;
    }

    args[arg_num][string_pos++] = command_string[index];
  }

  args[arg_num][string_pos] = '\0';
  args[arg_num] = realloc(args[arg_num], sizeof(char) * (string_pos + 1));

  if (double_quote || single_quote)
  {
    return NULL;
  }
  args = realloc(args, sizeof(char *) * (arg_num + 2));
  args[arg_num + 1] = NULL;

  return args;
}

void remove_new_line(char *string)
{
  for (unsigned index = 0; string[index] != '\0'; index++)
  {
    if (string[index] == '\n')
      string[index] = '\0';
  }
}

char **generate_args(char *command_string, Dictionary *variables)
{
  char total_command[1000] = "\0";
  strcpy(total_command, command_string);
  char **args = parse_command_string(command_string, variables);

  while (args == NULL)
  {
    printf("dquote> ");
    char new_line[100];
    fgets(new_line, 100, stdin);
    remove_new_line(new_line);
    strcat(total_command, "\n");
    strcat(total_command, new_line);
    args = parse_command_string(total_command, variables);
  }
  return remove_unnecessay_arg(args);
}

char *get_first_token(char *command_string)
{
  char *token = malloc(sizeof(char) * 30);
  unsigned index;
  for (index = 0; command_string[index] != ' ' && command_string[index] != '\0'; index++)
  {
    token[index] = command_string[index];
  }
  token[index] = '\0';
  return token;
}

char *prepend_alias_value(char *command_string, char *alias_value, unsigned first_token_length)
{
  char *total_command = malloc(sizeof(char) * 100);
  strcpy(total_command, alias_value);
  strcat(total_command, command_string + first_token_length);
  return total_command;
}

void handle_alias(Dictionary *aliases, char **args)
{
  if (args[1] == NULL)
  {
    show(aliases);
  }
  else if (args[2] == NULL)
  {
    char *value = get_value(aliases, args[1]);
    printf("%s: '%s'\n", args[1], value);
  }
  else
  {
    add(aliases, args[1], args[2]);
  }
}

Bool is_valid_varname(char *varname)
{
  for (unsigned index = 0; varname[index] != '\0'; index++)
  {
    if (varname[index] == ' ' || varname[index] == '\n' || varname[index] == '\t')
      return false;
  }
  return true;
}

void show_prompt(operation_set **prompt_operations, char *prompt, int code)
{
  char *prompt_str = parse(prompt ? prompt : "#blue(WTFish)-> ", prompt_operations, code);
  // printf("\nWTFish (\033[0;36m%s\033[0m)%s‣\033[0m ", replace_home_path(home, cwd), code);
  printf("\n%s", prompt_str);
  free(prompt_str);
}

int main(void)
{
  char *command_string = malloc(sizeof(char) * 100);
  char cwd[100];
  char *home = getenv("HOME");
  int exit_code;
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, safe_exit);
  int code = 1;
  Dictionary *aliases = create_dictionary();
  Dictionary *variables = create_dictionary();
  // add(variables, "PROMPT", "#red(this-#short_cwd())->");
  operation_set **prompt_operations = get_operations();

  while (1)
  {
    // getcwd(cwd, sizeof(cwd));
    show_prompt(prompt_operations, get_value(variables, "PROMPT"), code);
    code = 1;
    fgets(command_string, 100, stdin);
    remove_new_line(command_string);
    int pid = fork();
    if (pid == 0)
    {
      char *first_token = get_first_token(command_string);
      char *alias_value = get_value(aliases, first_token);
      if (alias_value != NULL)
      {
        command_string = prepend_alias_value(command_string, alias_value, strlen(first_token));
      }
      char **args = generate_args(command_string, variables);

      if (strcmp(args[0], "cd") == 0)
      {
        exit_code = chdir(args[1] == NULL ? getenv("HOME") : args[1]);
        if (exit_code < 0)
          printf("'cd' command couldn't be executed. Please check the path or permission!!\n");
        code = 0;
      }
      else if (strcmp(args[0], "alias") == 0)
      {
        handle_alias(aliases, args);
      }
      else if (strcmp(args[0], "unalias") == 0)
      {
        remove_key(aliases, args[1]);
      }
      else if (strcmp(args[0], "set") == 0)
      {
        if (args[1] == NULL || args[2] == NULL)
          printf("set: please provide correct format `set var value`\n");
        else if (!is_valid_varname(args[1]))
          printf("set: variable name invalid. <space>, <tab> and <new_line> are not allowed.\n");
        else
          add(variables, args[1], args[2]);
      }
      else if (strcmp(args[0], "unset") == 0)
      {
        remove_key(variables, args[1]);
      }
      else
      {
        signal(SIGINT, safe_exit);
        exit_code = execvp(args[0], args);
        if (exit_code == -1)
        {
          printf("command '%s' was not found!!\n", args[0]);
          code = 0;
        }
      }
    }
    else
    {
      wait(&pid);
    }
  }

  return 0;
}