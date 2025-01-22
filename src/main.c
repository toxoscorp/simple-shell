#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const char *PATHS[] = {"/usr/local/sbin", "/usr/local/bin", "/usr/sbin",
                       "/usr/bin",        "/sbin",          "/bin"};

int isInPath(const char *program, const char *path) {
  char *fullPath = malloc(strlen(program) + strlen(path) + 2);
  strcpy(fullPath, path);
  strcat(fullPath, "/");
  strcat(fullPath, program);
  int returner = access(fullPath, F_OK) != -1;
  free(fullPath);
  return returner;
}

char *getProgramPath(const char *program) {
  for (int i = 0; i < 6; i++) {
    if (isInPath(program, PATHS[i])) {
      char *fullPath = malloc(strlen(program) + strlen(PATHS[i]) + 2);
      strcpy(fullPath, PATHS[i]);
      strcat(fullPath, "/");
      strcat(fullPath, program);
      return fullPath;
    }
  }
  return NULL;
}

char **splitCommand(char *command) {
  char **returner = malloc(255);
  int i = 0;
  char *token = strtok(command, " ");
  while (token != NULL) {
    returner[i] = token;
    token = strtok(NULL, " ");
    i++;
  }
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
