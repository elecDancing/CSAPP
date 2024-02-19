CC = gcc
CFLAGS = -Wall -Wextra 



# main: main.o csapp.o
# 	$(CC) $(CFLAGS) -o main main.o csapp.o -lpthread

test: test.o csapp.o
	$(CC) $(CFLAGS) -o test test.o csapp.o -lpthread

test.o: test.c csapp.h
	$(CC) $(CFLAGS) -c test.c -lpthread

# main.o: main.c csapp.h
# 	$(CC) $(CFLAGS) -c main.c -lpthread

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c -lpthread

clean:
	rm -f main *.o 
