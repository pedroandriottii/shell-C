all: main.c
	gcc main.c -o shell -lpthread
clean:
	rm shell