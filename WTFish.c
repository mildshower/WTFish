#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

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
  Bool is_escaped = false;
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

    if (command_string[index] == ' ')
    {
      args[arg_num][string_pos++] = '\0';
      args[arg_num] = realloc(args[arg_num], sizeof(char) * string_pos);
      args[++arg_num] = malloc(sizeof(char) * strlen(command_string));
      string_pos = 0;
      continue;
    }

    args[arg_num][string_pos++] = command_string[index];
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

int main(void)
{
  char command_string[100];
  char cwd[100];
  char *home = getenv("HOME");
  int exit_code;
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, safe_exit);

  while (1)
  {
    getcwd(cwd, sizeof(cwd));
    printf("\nWTFish (\033[0;36m%s\033[0m)-> ", replace_home_path(home, cwd));
    fgets(command_string, 100, stdin);
    remove_new_line(command_string);
    char **args = parse_command_string(command_string);

    if (strcmp(args[0], "cd") == 0)
    {
      exit_code = chdir(args[1] == NULL ? home : args[1]);
      if (exit_code < 0)
        printf("'cd' command couldn't be executed. Please check the path or permission!!\n");
      continue;
    }

    int pid = fork();

    if (pid == 0)
    {
      signal(SIGINT, safe_exit);
      exit_code = execvp(args[0], args);
      if (exit_code == -1)
        printf("command '%s' was not found!!\n", command_string);
    }
    else
    {
      wait(&pid);
    }
  }

  return 0;
}