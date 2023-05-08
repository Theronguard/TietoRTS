OBJS	= main.o bot.o entities.o filesfunc.o maps.o orders.o rounds.o status.o
SOURCE	= main.c bot.c entities.c filesfunc.c maps.c orders.c rounds.c status.c
HEADER	= bot.h entities.h filesfunc.h maps.h orders.h rounds.h status.h
OUT	= rts
CC	 = gcc
FLAGS	 = -g -c
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c -std=c99

bot.o: bot.c
	$(CC) $(FLAGS) bot.c -std=c99

entities.o: entities.c
	$(CC) $(FLAGS) entities.c -std=c99

filesfunc.o: filesfunc.c
	$(CC) $(FLAGS) filesfunc.c -std=c99

maps.o: maps.c
	$(CC) $(FLAGS) maps.c -std=c99

orders.o: orders.c
	$(CC) $(FLAGS) orders.c -std=c99

rounds.o: rounds.c
	$(CC) $(FLAGS) rounds.c -std=c99

status.o: status.c
	$(CC) $(FLAGS) status.c -std=c99


clean:
	rm -f $(OBJS) $(OUT)
