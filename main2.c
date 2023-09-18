#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// 2- Fazer Node para guardar os comandos.
// 3- Mais de um comando pode ser apresentado na linha de comando com o ; separando.
// 4- Fazer style sequencial e style parallel.

struct Node {
  char *command;
  struct Node *next;
} Node;

void sequentialShell();
void parallelShell();

int main(void){
  struct Node *head = NULL;
  struct Node *tail = NULL;


  while(1){
    char *line = NULL;
    char *args[40];
    char *command;
    char **commands = malloc(sizeof(char *) * 40);

    size_t len = 0;
    ssize_t read;
    int i = 0;

    printf("phab seq>");
    read = getline(&line, &len, stdin);

    command = strtok(line, " \n");
    printf(" ANTES DO WHILE -> %s\n", command);

    while(command!= NULL){
      args[i] = command;
      command = strtok(NULL, " \n");
      i++;
    }

    args[i] = NULL;

    if(strcmp(args[0], "exit") == 0){
      break;
    }

    sequentialShell();

  }
  return 0;
}

void sequentialShell(){

}

void parallelShell(){

}