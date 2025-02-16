#include "common.h"

#include <assert.h>

#if defined(_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

int change_directory(const char *dir) {
  assert(dir);
#if defined(_WIN32)
  return _chdir(dir);
#else
  return chdir(dir);
#endif
}

int get_cwd(char *cwd, int cwd_length) {
  assert(cwd);
  assert(cwd_length > 0);

#if defined(_WIN32)
  return _getcwd(cwd, cwd_length) ? 0 : -1;
#else
  return getcwd(cwd, cwd_length) ? 0 : -1;
#endif
}
