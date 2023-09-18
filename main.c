#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct Node {
  char *command;
  struct Node *next;
} Node;

void sequentialShell();
void parallelShell();
void repeat(struct Node *tail);
void enqueue(struct Node **tail, char *command);
void clean(struct Node *head);


int main(int argc, char *argv[]){
  struct Node *head = NULL;
  struct Node *tail = NULL;
  if(argc == 1){
    sequentialShell();
  }

  clean(head);
  return 0;
}

void sequentialShell(){
  char *args[40];

  while(1){
    printf("phab seq>");
    fflush(stdout);

    char *input;
    char *command;
    input = getline();
    input = trim(input);

    int i = 0;
    command = strtok(input, " \n");

    while(command!= NULL){
      args[i] = command;
      command = strtok(NULL, " \n");
      i++;
    }
    args[i] = NULL;

    if(i>0 && strcmp(args[0], "exit") == 0){
      break;
    }

    if(i>0 && strcmp(args[0], "style") == 0 && strcmp(args[1], "parallel") == 0){
      parallelShell();
    }

    pid_t pid = fork();

    if(pid < 0){
      fprintf(stderr, "Fork Failed");
    }

    else if(pid == 0){
      if(i>0 && strcmp(args[0], "!!") == 0){
      repeat(*tail);
    }
      execvp(args[0], args);
    }

    else{
      wait(NULL);
    }
  }
}

void parallelShell(){
  char *args[40];

  while(1){
    printf("phab par>");
    fflush(stdout);

    char input[80];
    fgets(input, 80, stdin);

    int i = 0;
    char *token = strtok(input, " \n");

    while(token != NULL){
      args[i] = token;
      token = strtok(NULL, " \n");
      i++;
    }
    args[i] = NULL;

    if(i>0 && strcmp(args[0], "exit") == 0){
      break;
    }
    
    if(i>0 && strcmp(args[0], "style") == 0 && strcmp(args[1], "sequential") == 0){
      sequentialShell();
    }

    pid_t pid = fork();

    if(pid < 0){
      fprintf(stderr, "Fork Failed");
    }

    else if(pid == 0){
      execvp(args[0], args);
    }

    else{
      wait(NULL);
    }
  }
}

void repeat(struct Node *tail){

}

void clean(struct Node *head){
  struct Node *current = head;
  while(current != NULL){
    struct Node *temp = current;
    current = current->next;
    free(temp->command);
    free(temp);
  }
}

void enqueue(struct Node **tail, char *command) {
    struct Node *newCommand = (struct Node *)malloc(sizeof(struct Node));
    newCommand->command = strdup(command);
    newCommand->next = NULL;

    if (*tail == NULL) {
        *tail = newCommand;
    } else {
        (*tail)->next = newCommand;
        *tail = newCommand;
    }
}
