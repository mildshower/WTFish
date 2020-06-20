#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

typedef enum
{
  false,
  true
} Bool;

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
  char command_string[50];
  char cwd[50];
  char *home = getenv("HOME");

  while (1)
  {
    getcwd(cwd, sizeof(cwd));
    printf("\nWTFish (\033[0;36m%s\033[0m)-> ", replace_home_path(home, cwd));
    fgets(command_string, 50, stdin);
    remove_new_line(command_string);

    if (strcmp(command_string, "cd") == 0)
    {
      chdir("/home/sudpta/");
      continue;
    }

    int pid = fork();

    if (pid == 0)
    {
      char *args[50] = {command_string, NULL};
      int exit_code = execvp(command_string, args);
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