CFLAGS = -Wall -g -std=c99 -Werror

friendme: friendme.o friends.o 
	gcc $(CFLAGS) -o friendme friendme.o friends.o

friendme.o: friendme.c friends.h
	gcc $(CFLAGS) -c friendme.c

friends.o: friends.c friends.h
	gcc $(CFLAGS) -c friends.c

clean: 
	rm friendme *.o
