OBJS = main.o parser.o
CC = g++
CFLAGS = -c
LFLAGS_DB = 

a.out: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o a.out

parser.o: parser.cpp parser.h input-params.h
	$(CC) $(CFLAGS) $<

main.o: main.cpp parser.o
	$(CC) $(CFLAGS) $<


clean: 
	rm -f $(OBJS) a.out *.gch *~
