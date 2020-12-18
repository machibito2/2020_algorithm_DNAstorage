PROGS = gen enc syn seq dec eval
DATA = gendata encdata syndna seqdata decdata
CC = gcc
CFLAGS = -Wall

all: gen enc syn seq dec eval

.c:
	$(CC) $(CFLAGS) -lm -o $@ $<

clean:
	rm  $(PROGS) $(DATA)
