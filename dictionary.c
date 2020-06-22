#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

Dictionary *create_dictionary()
{
  Dictionary *dictionary = malloc(sizeof(Dictionary));
  dictionary->start = NULL;
  dictionary->end = NULL;
  return dictionary;
}

Pair *create_pair(char *key, char *value)
{
  Pair *new_pair = malloc(sizeof(Pair));
  new_pair->key = malloc(sizeof(char) * strlen(key) + 1);
  new_pair->value = malloc(sizeof(char) * strlen(value) + 1);
  strcpy(new_pair->key, key);
  strcpy(new_pair->value, value);
  new_pair->next = NULL;
  return new_pair;
}

void add(Dictionary *dictionary, char *key, char *value)
{
  Pair *current_pair = dictionary->start;

  while (current_pair != NULL)
  {
    if (strcmp(current_pair->key, key) == 0)
    {
      char *value_str = malloc(sizeof(char) * strlen(value) + 1);
      current_pair->value = realloc(current_pair->value, sizeof(char) * (strlen(value) + 1));
      strcpy(current_pair->value, value);
      return;
    }
    current_pair = current_pair->next;
  }

  Pair *new_pair = create_pair(key, value);

  if (dictionary->start == NULL)
  {
    dictionary->start = new_pair;
  }
  else
  {
    dictionary->end->next = new_pair;
  }
  dictionary->end = new_pair;
}

char *get_value(Dictionary *dictionary, char *key)
{
  Pair *current_pair = dictionary->start;

  while (current_pair != NULL)
  {
    if (strcmp(current_pair->key, key) == 0)
    {
      return current_pair->value;
    }
    current_pair = current_pair->next;
  }

  return NULL;
}

void show(Dictionary *dictionary)
{
  Pair *current_pair = dictionary->start;

  while (current_pair != NULL)
  {
    printf("%s: '%s'\n", current_pair->key, current_pair->value);
    current_pair = current_pair->next;
  }
}