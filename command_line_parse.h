#ifndef __COMMAND_LINE_PARSE_H_
#define __COMMAND_LINE_PARSE_H_

typedef enum
{
  false,
  true
} Bool;

void remove_new_line(char *string);
char **generate_args(char *command_string, Dictionary *variables);
char *get_first_token(char *command_string);
char *get_command_string();
void free_args(char **args);

#endif