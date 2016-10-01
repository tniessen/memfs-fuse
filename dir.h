
#include <linux/limits.h>
#include "node.h"

#ifndef _DIR_H
#define _DIR_H

struct direntry {
  char             name[PATH_MAX];
  struct node     *node;
  struct direntry *next;
};

int dir_add(struct node *dir, struct direntry *entry, int replace, int *added);

int dir_add_alloc(struct node *dir, const char *name, struct node *node, int replace);

int dir_remove(struct node *dir, const char *name);

int dir_find(struct node *dir, const char *name, int namelen, struct direntry **entry);

#endif

