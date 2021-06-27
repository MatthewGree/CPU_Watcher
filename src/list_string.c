#include <list_string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node_string node_string;

struct node_string {
  char *value;
  node_string *next;
};

struct list_string {
  node_string *root;
};

static node_string *node_string_create(char *value) {
  node_string *toReturn = malloc(sizeof(node_string));

  if (toReturn) {
    toReturn->value = value;
    toReturn->next = 0;
  } else {
    perror("Node error: ");
  }
  return toReturn;
}

static void node_string_destroy(node_string *ptr) { free(ptr); }

list_string *list_string_create() {
  list_string *toReturn = malloc(sizeof(list_string));
  if (toReturn) {
    toReturn->root = 0;
  } else {
    perror("List error: ");
  }
  return toReturn;
}

void list_string_destroy(list_string *ptr) {
  node_string *root = ptr->root;
  while (root != 0) {
    node_string *tmp = root->next;
    node_string_destroy(root);
    root = tmp;
  }
  free(ptr);
}

int list_string_popFirst(list_string *ptr, char **result) {
  if (ptr->root != 0) {
    *result = ptr->root->value;
    node_string *oldRoot = ptr->root;
    ptr->root = ptr->root->next;
    node_string_destroy(oldRoot);
    return 0;
  } else {
    return 1;
  }
}

void list_string_append(list_string *ptr, char *toAppend) {
  node_string *newNode = node_string_create(toAppend);
  if (newNode) {
    if (ptr->root != 0) {
      node_string *furthestNode = ptr->root;
      while (furthestNode->next != 0) {
        furthestNode = furthestNode->next;
      }
      furthestNode->next = newNode;
    } else {
      ptr->root = newNode;
    }
  }
}
