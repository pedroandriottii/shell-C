#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

// Estrutura para fazer uma fila de comandos.
struct Node {
    char *command;
    struct Node *next;
};

// Estrutura para guardar os comandos em background
struct Background {
    char *command;
    int background;
};

void enqueue(struct Node **head, struct Node **tail, char *command);
void printList(struct Node *head);
void freeList(struct Node *head);
void sequencialShell(char *line);
void parallelShell(char *line);
void *executeParallel(void *arg);
void execPipe(char *command1, char *command2);

int main(int argc, char *argv[]) {
    struct Node *head = NULL;
    struct Node *tail = NULL;

    FILE *file = NULL;

    // Modo de execução do shell (0 = sequencial, 1 = paralelo)
    int isParallel = 0;

    // Modo de execução do input por arquivo (0 = false, 1 = true)
    int fileMode = 0;

    // Verifica se o arquivo foi passado como argumento
    if (argc > 1) {
        file = fopen(argv[1], "r");
        fileMode = 1;
    }

    while (1) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        
        // Verifica se o shell está em modo sequencial
        if(isParallel == 0 && fileMode == 0){
            printf("phab seq>");
        }

        // Verifica se o shell está em modo paralelo
        if(isParallel == 1 && fileMode == 0){
            printf("phab par>");
        }

        // Verifica se há um arquivo para ser lido (Caso 0, o input é feito pelo terminal)
        if(fileMode == 0){
            read = getline(&line, &len, stdin);
        }

        // Verifica se há um arquivo para ser lido (Caso 1, o input é feito pelo arquivo)
        if(fileMode == 1){
            read = getline(&line, &len, file);
        }

        line[strcspn(line, "\n")] = '\0';

        // Se o comando for "exit", encerra o shell
        if (strcmp(line, "exit") == 0) {
            free(line);
            break;
        }
        
        // Altera o modo de execução do shell para paralelo
        if(strcmp(line, "style parallel") == 0){
            free(line);
            isParallel = 1;
            continue;        
        }

        // Altera o modo de execução do shell para sequencial
        if(strcmp(line, "style sequencial") == 0){
            free(line);
            isParallel = 0;
            continue;        
        }

        // Verifica se o comando é para executar o último comando
        if(strcmp(line, "!!") == 0){
            free(line);

            // Verifica se há comandos na fila
            if (head == NULL) {
                printf("No commands.\n");
                continue;
            }

            // Executa o último comando da fila
            line = strdup(tail->command);
            printf("%s\n", line);
        }

        // Verifica se os comandos é para executar em pipe
        if(strcmp(line, "|") == 0){
            execPipe(head->command, tail->command);
        }

        // Executa o comando em paralelo
        if(isParallel){
            enqueue(&head, &tail, line);
            printList(head);

            parallelShell(line);
        }
        
        // Executa o comando em sequencial
        else{
            enqueue(&head, &tail, line);
            printList(head);

            sequencialShell(line);
        }
    }

    // Libera a memória alocada
    freeList(head);
    return 0;
}

// Funcao para adicionar um comando na fila
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

// Funcao para imprimir a fila de comandos
void printList(struct Node *head) {
    struct Node *current = head;

    printf("Comandos -> ");

    while (current != NULL) {
        printf("%s -> ", current->command);
        current = current->next;
    }
    printf("\n");
}

// Funcao para liberar a memoria alocada pela fila de comandos
void freeList(struct Node *head) {
    struct Node *current = head;

    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free(temp->command);
        free(temp);
    }
}

// Funcao para executar os comandos em sequencial
void sequencialShell(char *line) {
    char *commands[40];
    int i = 0;
    int fromFile = 0; // Variável para indicar se estamos lendo comandos de um arquivo

    char *token = strtok(line, ";");

    while (token != NULL) {
        commands[i] = strdup(token);
        token = strtok(NULL, ";");
        i++;
    }

    for (int j = 0; j < i; j++) {
        if (strlen(commands[j]) > 0) {
            char *redirect_output = strchr(commands[j], '>');

            // Verifica se o operador '<' está presente no comando
            char *redirect_input = strchr(commands[j], '<');

            if (redirect_output != NULL) {
                *redirect_output = '\0';
                char *output_file = redirect_output + 1; 
                
                int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

                dup2(output_fd, STDOUT_FILENO);
                close(output_fd); 

                execlp("sh", "sh", "-c", commands[j], NULL);
            } else if (redirect_input != NULL) {
                *redirect_input = '\0';
                char *input_file = redirect_input + 1;
                
                // Abra o arquivo de entrada em modo de leitura
                int input_fd = open(input_file, O_RDONLY);

                dup2(input_fd, STDIN_FILENO);
                close(input_fd);

                // Execute o comando com entrada redirecionada do arquivo
                execlp("sh", "sh", "-c", commands[j], NULL);
            } else {
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
        }
        free(commands[j]);
    }
}

// Funcao para criar threads e mandar para uma funcao que executa os comandos em paralelo
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

// Funcao para executar os comandos em paralelo
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

// Funcao para executar os comandos em pipe
void execPipe(char *command1, char *command2) {
    int pipe_fd[2];

    pid_t pid = fork();

    if (pid == 0) {
        close(pipe_fd[0]);

        dup2(pipe_fd[1], STDOUT_FILENO);

        close(pipe_fd[1]);

        execlp("sh", "sh", "-c", command1, NULL);
    } else {
        close(pipe_fd[1]);

        dup2(pipe_fd[0], STDIN_FILENO);

        close(pipe_fd[0]);

        execlp("sh", "sh", "-c", command2, NULL);
    }
}
