OBJS = parser.o agent.o ad-campaign.o model.o main.o
CC = g++
CFLAGS = -c
LFLAGS_DB = 
EXECUTABLE = BuyerModel
CONVERTER = convert

$(EXECUTABLE): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(EXECUTABLE)

$(CONVERTER): parser.o convert.cpp
	$(CC) convert.cpp parser.o -o $(CONVERTER)

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

all: $(EXECUTABLE) $(CONVERTER)

clean: 
	rm -f $(OBJS) $(EXECUTABLE) *.gch *~
