
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "dir.h"

int dir_add(struct node *dirnode, struct direntry *entry, int replace, int *added) {
  struct direntry **dir = (struct direntry **) &dirnode->data;
  struct direntry *existing_entry;

  if(dir_find(dirnode, entry->name, strlen(entry->name), &existing_entry)) {
    if(replace) {
      *added = 0;
      existing_entry->node = entry->node;
      return 1;
    } else {
      errno = EEXIST;
      return 0;
    }
  }

  *added = 1;

  if(*dir == NULL) {
    *dir = entry;
    entry->next = NULL;
  } else {
    entry->next = *dir;
    *dir = entry;
  }

  // The entry is now linked in the directory
  entry->node->vstat.st_nlink++;

  // If the entry is a directory, .. is an implicit hardlink to the parent
  // directory.
  if(S_ISDIR(entry->node->vstat.st_mode)) {
    dirnode->vstat.st_nlink++;
  }

  return 1;
}

int dir_add_alloc(struct node *dirnode, const char *name, struct node *node, int replace) {
  struct direntry *entry = malloc(sizeof(struct direntry));
  int added;

  if(!entry) {
    errno = ENOMEM;
    return 0;
  }

  strcpy(entry->name, name);
  entry->node = node;

  if(!dir_add(dirnode, entry, replace, &added)) {
    free(entry);
    return 0;
  }

  if(!added) free(entry);

  return 1;
}

int dir_remove(struct node *dirnode, const char *name) {
  struct direntry **dir = (struct direntry **) &dirnode->data;

  struct direntry *ent = *dir;
  struct direntry **ptr = dir;

  while(ent != NULL) {
    if(strcmp(ent->name, name) == 0) {
      *ptr = ent->next;

      // See dir_add for details
      if(S_ISDIR(ent->node->vstat.st_mode)) {
        dirnode->vstat.st_nlink--;
      }

      free(ent);

      return 1;
    }

    ptr = &ent->next;
    ent = ent->next;
  }

  errno = ENOENT;

  return 0;
}

int dir_find(struct node *dirnode, const char *name, int namelen, struct direntry **entry) {
  struct direntry *ent = (struct direntry *) dirnode->data;

  while(ent != NULL) {
    if(strlen(ent->name) == namelen) {
      if(strncmp(ent->name, name, namelen) == 0) {
        if(entry != NULL) *entry = ent;
        return 1;
      }
    }
    ent = ent->next;
  }

  errno = ENOENT;

  return 0;
}

