#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "dictionary.h"

typedef enum
{
  false,
  true
} Bool;

void safe_exit(int signal)
{
  exit(1);
}

char **parse_command_string(char *command_string)
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
      args[arg_num][string_pos++] = '\0';
      args[arg_num] = realloc(args[arg_num], sizeof(char) * string_pos);
      args[++arg_num] = malloc(sizeof(char) * strlen(command_string));
      string_pos = 0;
      continue;
    }

    args[arg_num][string_pos++] = command_string[index];
  }

  if (double_quote || single_quote)
  {
    return NULL;
  }

  args = realloc(args, sizeof(char *) * arg_num + 2);
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

char *replace_home_path(char *home, char *cwd)
{
  unsigned home_path_length = strlen(home);
  Bool is_home_path_present = strncmp(home, cwd, home_path_length) == 0;
  if (!is_home_path_present)
    return cwd;

  cwd[home_path_length - 1] = '~';
  return cwd + home_path_length - 1;
}

char **generate_args(char *command_string)
{
  char total_command[1000] = "\0";
  strcpy(total_command, command_string);
  char **args = parse_command_string(command_string);

  while (args == NULL)
  {
    printf("dquote> ");
    char new_line[100];
    fgets(new_line, 100, stdin);
    remove_new_line(new_line);
    strcat(total_command, "\n");
    strcat(total_command, new_line);
    args = parse_command_string(total_command);
  }
  return args;
}

char *get_first_token(char *command_string)
{
  char *token = malloc(sizeof(char *) * 30);
  unsigned index;
  for (index = 0; command_string[index] != ' '; index++)
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
    printf("%s: '%s'\n", args[1], value == NULL ? "(alias not found)" : value);
  }
  else
  {
    add(aliases, args[1], args[2]);
  }
}

int main(void)
{
  char *command_string = malloc(sizeof(char) * 100);
  char cwd[100];
  char *home = getenv("HOME");
  int exit_code;
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, safe_exit);
  char code[20] = "\033[0m";
  Dictionary *aliases = create_dictionary();

  while (1)
  {
    getcwd(cwd, sizeof(cwd));
    printf("\nWTFish (\033[0;36m%s\033[0m)%s‣\033[0m ", replace_home_path(home, cwd), code);
    strcpy(code, "\033[0m");
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
      char **args = generate_args(command_string);

      if (strcmp(args[0], "cd") == 0)
      {
        exit_code = chdir(args[1] == NULL ? home : args[1]);
        if (exit_code < 0)
          printf("'cd' command couldn't be executed. Please check the path or permission!!\n");
      }
      else if (strcmp(args[0], "alias") == 0)
      {
        handle_alias(aliases, args);
      }
      else
      {
        signal(SIGINT, safe_exit);
        exit_code = execvp(args[0], args);
        if (exit_code == -1)
        {
          printf("command '%s' was not found!!\n", command_string);
          strcpy(code, "\033[0;31m");
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