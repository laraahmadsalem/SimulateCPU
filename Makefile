CC = gcc
CFLAGS = -Wall -pedantic -ansi  
OBJ1 = queue.o scheduler.o 
EXEC1 = scheduler

$(EXEC1): $(OBJ1)
	$(CC) $(OBJ1) -o $(EXEC1) -lpthread

scheduler.o: scheduler.c scheduler.h queue.h 
	$(CC) -c scheduler.c $(CFLAGS) -lpthread

queue.o: queue.c queue.h
	$(CC) -c queue.c $(CFLAGS)

clean:
		rm -f $(EXCE1) $(OBJ1)
