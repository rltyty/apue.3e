#include <stdlib.h>
#include <string.h>
#include "rltapue.h"

int add_path(char *path)
{
  if (path == NULL) return -1;
  char *paths = getenv("PATH");
  int len = strlen(paths) + strlen(path) + 2; // ":" and "\0"
  char *newpaths = (char*) malloc(len);
  if (newpaths == NULL) {
    my_perror("error: malloc");
    return 1;
  }
  strcpy(newpaths, paths);
  strcat(newpaths, ":");
  strcat(newpaths, path);

  if (setenv("PATH", newpaths, 1) != 0) {
    my_perror("error: setenv %s", "PATH");
    free(newpaths);
    return 1;
  }
  return 0;
}
