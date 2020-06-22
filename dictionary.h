#ifndef __DICTIONARY_H_
#define __DICTIONARY_H_

typedef struct pair
{
  char *key;
  char *value;
  struct pair *next;
} Pair;

typedef struct
{
  Pair *start;
  Pair *end;
} Dictionary;

Dictionary *create_dictionary();
void add(Dictionary *dictionary, char *key, char *value);
char *get_value(Dictionary *dictionary, char *key);
void show(Dictionary *dictionary);

#endif