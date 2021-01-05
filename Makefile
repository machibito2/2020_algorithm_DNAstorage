PROGS = gen enc syn seq dec eval
DATA = gendata encdata syndna seqdata decdata
CC = gcc
CFLAGS = -Wall

all: gen enc syn seq dec eval

.c:
	$(CC) $(CFLAGS) -o $@ $< -lm

clean:
	rm  $(PROGS) $(DATA)
