# Shell em C

Uma interface de linha de comando personalizada projetada para operar em modos sequencial e paralelo, aprimorando o controle e a flexibilidade na execução de comandos. Suporta encadeamento simples de comandos, redirecionamento e execução de arquivos.
Esse projeto foi realizado a fim de estudar mais a fundo o funcionamento de Threads e Processos.

# Principais Funcionalidades
- Modo Sequencial: Executa comandos um após o outro, garantindo ordem e dependência.
- Modo Paralelo: Executa comandos simultaneamente para utilizar recursos do sistema e reduzir tempos de espera.
- Histórico de Comandos: Armazena e reexecuta comandos do histórico.
- Execução de Arquivos: Executa comandos a partir de um arquivo, permitindo processamento em lote.
- Redirecionamento: Redireciona entradas e saídas para e de arquivos, simplificando a gestão de dados.
- Execução de Pipe: Conecta dois comandos onde a saída de um se torna a entrada do outro.

## Requisitos

- Linux ou macOS
- GCC ou Clang para compilação

## Como Executar

- Clone o Repositório
- Rode o comando **gcc -o phabshell main.c -lpthread** ou apenas **make**
- Execute o shell

## Modo de uso

Para alternar entre os modos sequencial e paralelo, utilize os comandos style sequencial e style parallel. Comandos podem ser encadeados usando ;, redirecionados com > ou <, e pipes podem ser criados com |.

## Autor

* *Pedro Andriotti* 

## Projeto em Funcionamento

GIF em Breve
