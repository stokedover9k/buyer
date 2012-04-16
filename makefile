OBJS = parser.o agent.o ad-campaign.o model.o main.o
CC = g++
CFLAGS = -c
LFLAGS_DB = 

a.out: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o a.out

parser.o: parser.cpp parser.h input-params.h
	$(CC) $(CFLAGS) $<

agent.o: agent.cpp agent.h model-defs.h
	$(CC) $(CFLAGS) $<

ad-campaign.o: ad-campaign.cpp ad-campaign.h model-defs.h
	$(CC) $(CFLAGS) $<

model.o: model.cpp model.h agent.h ad-campaign.h model-defs.h
	$(CC) $(CFLAGS) $<

main.o: main.cpp parser.h agent.h model.h
	$(CC) $(CFLAGS) $<


clean: 
	rm -f $(OBJS) a.out *.gch *~
