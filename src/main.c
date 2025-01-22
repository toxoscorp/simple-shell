#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// constant list of possible path
/* constexpr const char *PATHS[] = {"/usr/local/sbin", "/usr/local/bin", */
const char *PATHS[] = {"/usr/local/sbin", "/usr/local/bin", "/usr/sbin",
                       "/usr/bin",        "/sbin",          "/bin"};

/* bool isInPath(const char *program, const char *path) { */
int isInPath(const char *program, const char *path) {
  // combine program and path in 1 char*
  char *fullPath = (char *)malloc(strlen(program) + strlen(path) + 2);
  strcpy(fullPath, path);
  strcat(fullPath, "/");
  strcat(fullPath, program);
  int returner = access(fullPath, F_OK) != -1;
  free(fullPath);
  return returner;
}

char *getProgramPath(const char *program) {
  /* for (const char *path : PATHS) { */
  for (int i = 0; i < 6; i++) {
    /* if (isInPath(program, path)) { */
    if (isInPath(program, PATHS[i])) {
      /* char *fullPath = (char *)malloc(strlen(program) + strlen(path) + 2); */
      char *fullPath = malloc(strlen(program) + strlen(PATHS[i]) + 2);
      /* strcpy(fullPath, path); */
      strcpy(fullPath, PATHS[i]);
      strcat(fullPath, "/");
      strcat(fullPath, program);
      return fullPath;
    }
  }
  /* return nullptr; */
  return NULL;
}

char **splitCommand(char *command) {
  char **returner = (char **)malloc(255);
  int i = 0;
  char *token = strtok(command, " ");
  /* while (token != nullptr) { */
  while (token != NULL) {
    returner[i] = token;
    /* token = strtok(nullptr, " "); */
    token = strtok(NULL, " ");
    i++;
  }
  /* returner[i] = nullptr; */
  returner[i] = NULL;
  return returner;
}

int main(int argc, char *argv[]) {
  char command[255];
  int running = 1;
  while (running) {
    write(1, "-> ", 3);
    int cmdLength = read(0, command, 255);
    command[cmdLength - 1] = '\0';
    char **args = splitCommand(command);
    if (strcmp(args[0], "exit") == 0) {
      write(1, "Exiting shell\n", 14);
      running = 0;
      break;
    }
    pid_t pid = fork();
    if (pid == 0) {
      char *programPath = getProgramPath(command);
      /* if (programPath != nullptr) { */
      if (programPath != NULL) {
        execv(programPath, args);
        free(programPath);
      } else {
        write(1, "Command not found\n", 18);
      }
      exit(0);
    } else {
      siginfo_t status;
      waitid(P_ALL, 0, &status, WEXITED);
    }
  }
  return 0;
}
