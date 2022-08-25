CC = gcc
CFLAGS = -O2 -Wall -I .

LIB = -lpthread

olive: olive.c app.o
	$(CC) $(CFLAGS) -o olive olive.c app.o $(LIB)

app.o: app.c
	$(CC) $(CFLAGS) -c app.c


clean:
	rm -f *.o olive *~
	(make clean)

