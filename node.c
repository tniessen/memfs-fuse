
#include <errno.h>

#include "node.h"
#include "dir.h"

int getnoderelativeto(const char *path, struct node *root, struct node **node) {
  if(!S_ISDIR(root->vstat.st_mode)) {
    errno = ENOTDIR;
    return 0;
  }

  if(path[0] != '/') {
    errno = EINVAL;
    return 0;
  }

  // Root directory
  // This also handles paths with trailing slashes (as long as it is a
  // directory) due to recursion.
  if(path[1] == '\0') {
    *node = root;
    return 1;
  }

  // Extract name from path
  const char *name = path + 1;
  int namelen = 0;
  const char *name_end = name;
  while(*name_end != '\0' && *name_end != '/') {
    name_end++;
    namelen++;
  }

  // Search directory
  struct direntry *dirent;
  if(!dir_find(root, name, namelen, &dirent)) {
    errno = ENOENT;
    return 0;
  }

  if(*name_end == '\0') {
    // Last node in path
    *node = dirent->node;
    return 1;
  } else {
    // Not the last node in path (or a trailing slash)
    return getnoderelativeto(name_end, dirent->node, node);
  }
}

