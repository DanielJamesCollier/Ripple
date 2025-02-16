#include "del.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#if defined(__APPLE__)
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
static void del_windows(const char *path) {
  WIN32_FIND_DATA find_data;
  char search_path[MAX_PATH];
  snprintf(search_path, sizeof(search_path), "%s\\*", path);

  HANDLE hFind = FindFirstFile(search_path, &find_data);
  if (hFind == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Error: Could not open directory %s\n\n", path);
    return;
  }

  do {
    if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) continue;

    char full_path[MAX_PATH];
    snprintf(full_path, sizeof(full_path), "%s\\%s", path, find_data.cFileName);

    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      // Recursive call for directories
      del_windows(full_path);
    } else {
      // Delete file
      if (DeleteFile(full_path) == 0) {
        fprintf(stderr, "Error: Could not delete file %s\n\n", full_path);
      } else {
        printf("Deleted %s\n", full_path);
      }
    }
  } while (FindNextFile(hFind, &find_data));

  FindClose(hFind);

  // Remove empty directory
  if (RemoveDirectory(path) == 0) {
    fprintf(stderr, "Error: Could not remove directory %s\n\n", path);
    return;
  } else {
    printf("Deleted %s\n", path);
  }
  return;
}
#endif

#if defined(__APPLE__)
static void del_apple(const char *path) {
  struct stat path_stat;
  
  // Check if path exists
  if (stat(path, &path_stat) != 0) {
    fprintf(stderr, "Error: Cannot access %s\n\n", path);
    return;
  }

  if (S_ISREG(path_stat.st_mode)) { // If it's a file, delete it
    if (remove(path) != 0) {
      fprintf(stderr, "Error: Could not delete file %s\n\n", path);
      return;
    } else {
      printf("Deleted %s\n", path);
    }
    return;
  }

  if (S_ISDIR(path_stat.st_mode)) { // If it's a directory, delete recursively
    DIR *dir = opendir(path);
    if (!dir) {
      fprintf(stderr, "Error: Could not open directory %s\n\n", path);
      return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
      char full_path[1024];
      snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
      del_apple(full_path);
    }
    closedir(dir);

    // Remove empty directory
    if (rmdir(path) != 0) {
      fprintf(stderr, "Error: Could not remove directory %s\n\n", path);
      return; 
    } else {
      printf("Deleted %s\n", path);
    }
  }
  return;
}
#endif

void del(const char *path) {
  assert(path);

  while (*path == ' ') {
    ++path;
  }

  if (*path == '\0') {
    printf("del requires a path.\n\n");
    return;
  }

#if defined(_WIN32)
  del_windows(path);
#else
  del_apple(path);
#endif
  putchar('\n');
}
