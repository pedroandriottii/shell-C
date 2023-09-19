#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct Node {
    char *command;
    struct Node *next;
};

void enqueue(struct Node **head, struct Node **tail, char *command);
void printList(struct Node *head);
void freeList(struct Node *head);
void execute(char *command);

int main(void) {
    struct Node *head = NULL;
    struct Node *tail = NULL;

    while (1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        printf("phab seq>");
        read = getline(&line, &len, stdin);

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "exit") == 0) {
            break;
        }

        enqueue(&head, &tail, line);
        printList(head);

        execute(line);
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

    printf("Lista de Comandos:\n");

    while (current != NULL) {
        printf("%s\n", current->command);
        current = current->next;
    }
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

void execute(char *command) {
    char *args[40];
    int i = 0;

    char *token = strtok(command, " \n");

    while (token != NULL) {
        args[i] = token;
        token = strtok(NULL, " \n");
        i++;
    }

    args[i] = NULL;

    if (strcmp(args[0], "style") == 0 && strcmp(args[1], "parallel") == 0) {
    }
    if (strcmp(args[0], "exit") == 0){
        exit(0);
    } else {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork Failed\n");
        } else if (pid == 0) {
            execvp(args[0], args);
            exit(1);
        } else {
            wait(NULL);
        }
    }
}
