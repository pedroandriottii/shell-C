#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

struct Node {
    char *command;
    struct Node *next;
};

void enqueue(struct Node **head, struct Node **tail, char *command);
void printList(struct Node *head);
void freeList(struct Node *head);
void sequencialShell(char *line);
void parallelShell(char *line);

int main(void) {
    struct Node *head = NULL;
    struct Node *tail = NULL;

    int isParallel = 0;

    while (1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        
        if(isParallel == 0){
            printf("phab seq>");
        }

        if(isParallel == 1){
            printf("phab par>");
        }

        read = getline(&line, &len, stdin);

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "exit") == 0) {
            free(line);
            break;
        }

        if(strcmp(line, "style parallel") == 0){
            free(line);
            isParallel = 1;
            continue;        
        }

        if(strcmp(line, "style sequencial") == 0){
            free(line);
            isParallel = 0;
            continue;        
        }

        if(strcmp(line, "!!") == 0){
            free(line);
            if (head == NULL) {
                printf("No commands.\n");
                continue;
            }
            line = strdup(tail->command);
            printf("%s\n", line);
        }

        if(isParallel){
            enqueue(&head, &tail, line);
            printList(head);

            parallelShell(line);
        }else{
            enqueue(&head, &tail, line);
            printList(head);

            sequencialShell(line);
        }
    }

    freeList(head);
    return 0;
}

void enqueue(struct Node **head, struct Node **tail, char *command) {
    struct Node *newNode = malloc(sizeof(struct Node));

    newNode->command = strdup(command);
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
        *tail = newNode;
    } else {
        (*tail)->next = newNode;
        *tail = newNode;
    }
}

void printList(struct Node *head) {
    struct Node *current = head;

    printf("Comandos -> ");

    while (current != NULL) {
        printf("%s -> ", current->command);
        current = current->next;
    }
    printf("\n");
}

void freeList(struct Node *head) {
    struct Node *current = head;

    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free(temp->command);
        free(temp);
    }
}

void sequencialShell(char *line) {
    char *commands[40];
    int i = 0;

    char *token = strtok(line, ";");

    while (token != NULL) {
        commands[i] = strdup(token);
        token = strtok(NULL, ";");
        i++;
    }

    for (int j = 0; j < i; j++) {
        if (strlen(commands[j]) > 0) {
            pid_t pid = fork();

            if (pid < 0) {
                printf("Fork Failed\n");
            } else if (pid == 0) {
                execlp("sh", "sh", "-c", commands[j], NULL);
                exit(1);
            } else {
                wait(NULL);
            }
        }
        free(commands[j]);
    }
}

void parallelShell(char *line)  {
    char *commands[40];
    int commandsqty = 1;
    int i = 0;

    char *token = strtok(line, ";");

    while(token != NULL){
        commands[commandsqty] = strdup(token);
        token = strtok(NULL, ";");
        commandsqty++;
    }

    for (int i = 0; i < commandsqty; i++) {
        pid_t pid = fork();
        if (pid == 0) { 
        char *cmd = commands[i];
        system(cmd);
        exit(EXIT_SUCCESS);
        }
    }
    
    for (int i = 0; i < commandsqty; i++) {
    int status;
    wait(&status);
    }

}