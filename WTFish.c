#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#include "dictionary.h"
#include "prompt.h"
#include "command_line_parse.h"

Bool is_valid_key(char *key)
{
  return !(strstr(key, "\n") || strstr(key, " ") || strstr(key, "\t") || strstr(key, "\"") || strstr(key, "'"));
}

void handle_alias(Dictionary *aliases, char **args, int *code)
{
  if (args[1] == NULL)
    show(aliases);
  else if (args[2] == NULL)
    printf("%s: '%s'\n", args[1], get_value(aliases, args[1]));
  else if (!is_valid_key(args[1]))
  {
    printf("alias: Invalid key. ' ', '\\t', '\\n', ''' and '\"' are not allowed.\n");
    *code = 0;
  }
  else
    add(aliases, args[1], args[2]);
}

void show_prompt(operation_set **prompt_operations, char *prompt, int code)
{
  char *prompt_str = parse(prompt ? prompt : "#blue(WTFish)-> ", prompt_operations, code);
  // printf("\nWTFish (\033[0;36m%s\033[0m)%s‣\033[0m ", replace_home_path(home, cwd), code);
  printf("\n%s", prompt_str);
  free(prompt_str);
}

void handle_cd(char *path, int *code)
{
  if (chdir(path == NULL ? getenv("HOME") : path) < 0)
  {
    printf("'cd' command couldn't be executed. Please check the path or permission!!\n");
    *code = 0;
  }
}

void add_variable(Dictionary *variables, char **args, int *code)
{
  if (args[1] == NULL || args[2] == NULL)
  {
    printf("set: please provide correct format `set var value`\n");
    *code = 0;
  }
  else if (!is_valid_key(args[1]))
  {
    printf("set: variable name invalid. ' ', '\\t', '\\n', ''' and '\"' are not allowed.\n");
    *code = 0;
  }
  else
    add(variables, args[1], args[2]);
}

Bool execute_internal_commands(char **args, Dictionary *aliases, Dictionary *variables, int *code)
{
  if (strcmp(args[0], "cd") == 0)
    handle_cd(args[1], code);
  else if (strcmp(args[0], "alias") == 0)
    handle_alias(aliases, args, code);
  else if (strcmp(args[0], "unalias") == 0)
    remove_key(aliases, args[1]);
  else if (strcmp(args[0], "set") == 0)
    add_variable(variables, args, code);
  else if (strcmp(args[0], "unset") == 0)
    remove_key(variables, args[1]);
  else
    return false;

  return true;
}

void execute_external_command(char **args, int *code)
{
  signal(SIGINT, exit);
  execvp(args[0], args);
  printf("command '%s' was not found!!\n", args[0]);
  *code = 0;
}

void execute_command(char *command_string, Dictionary *aliases, Dictionary *variables, int *code)
{
  char **args = generate_args(command_string, variables);

  if (!execute_internal_commands(args, aliases, variables, code))
  {
    int pid = fork();
    if (pid == 0)
    {
      execute_external_command(args, code);
      exit(0);
    }
    else
      wait(&pid);
  }
  free_args(args);
}

int main(void)
{
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, exit);
  Dictionary *aliases = create_dictionary();
  Dictionary *variables = create_dictionary();
  operation_set **prompt_operations = get_operations();
  int *code = mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  *code = 1;

  while (1)
  {
    show_prompt(prompt_operations, get_value(variables, "PROMPT"), *code);
    *code = 1;
    char *command_string = get_command_string(aliases);
    execute_command(command_string, aliases, variables, code);
    free(command_string);
  }

  return 0;
}