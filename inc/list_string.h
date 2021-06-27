#ifndef LIST_STRING_H
#define LIST_STRING_H

typedef struct list_string list_string;

int list_string_popFirst(list_string *ptr, char **result);
void list_string_append(list_string *ptr, char *toAppend);
list_string *list_string_create(void);
void list_string_destroy(list_string *ptr);

#endif
