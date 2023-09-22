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
void *executeParallel(void *arg);

int main(int argc, char *argv[]) {
    struct Node *head = NULL;
    struct Node *tail = NULL;

    FILE *file = NULL;
    int isParallel = 0;
    int fileMode = 0;

    if (argc > 1) {
        file = fopen(argv[1], "r");
        fileMode = 1;
    }

    while (1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        
        if(isParallel == 0 && fileMode == 0){
            printf("phab seq>");
        }

        if(isParallel == 1 && fileMode == 0){
            printf("phab par>");
        }

        if(fileMode == 0){
            read = getline(&line, &len, stdin);
        }
        if(fileMode == 1){
            read = getline(&line, &len, file);
        }

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

void parallelShell(char *line) {
    char *commands[40];
    int commandsqty = 0;
    int i = 0;

    char *token = strtok(line, ";");

    while (token != NULL && commandsqty < 40) {
        commands[commandsqty] = strdup(token);
        token = strtok(NULL, ";");
        commandsqty++;
    }

    pthread_t threads[40];

    for (int i = 0; i < commandsqty; i++) {
        pthread_create(&threads[i], NULL, executeParallel, (void *)commands[i]);
    }

    for (int i = 0; i < commandsqty; i++) {
        pthread_join(threads[i], NULL);
    }
}

void *executeParallel(void *arg) {
    char *cmd = (char *)arg;
    char buffer[128];
    FILE *fp = popen(cmd, "r");

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    pclose(fp);
    pthread_exit(NULL);
}