#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void remove_new_line(char *string)
{
  for (unsigned index = 0; string[index] != '\0'; index++)
  {
    if (string[index] == '\n')
      string[index] = '\0';
  }
}

int main(void)
{
  char command_string[50];

  while (1)
  {
    printf("\nWTFish-> ");
    fgets(command_string, 50, stdin);
    remove_new_line(command_string);
    int pid = fork();

    if (pid == 0)
    {
      char *args[50] = {command_string, NULL};
      // args[0] = command_string;
      // args[1] = NULL;
      execvp(command_string, args);
    }
    else
    {
      wait(&pid);
    }
  }

  return 0;
}