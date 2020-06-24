#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "dictionary.h"
#include "prompt.h"
#include "command_line_parse.h"

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
    show(aliases);
  else if (args[2] == NULL)
    printf("%s: '%s'\n", args[1], get_value(aliases, args[1]));
  else
    add(aliases, args[1], args[2]);
}

Bool is_valid_varname(char *varname)
{
  return !(strstr(varname, "\n") || strstr(varname, " ") || strstr(varname, "\t"));
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
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, exit);
  Dictionary *aliases = create_dictionary();
  Dictionary *variables = create_dictionary();
  char *command_string = malloc(sizeof(char) * 100);
  int exit_code;
  int code = 1;
  operation_set **prompt_operations = get_operations();

  while (1)
  {
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
        signal(SIGINT, exit);
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