CC = gcc
CFLAGS = -Wall

all: supermarket team customer forkcustomers

supermarket: supermarket.c read_supermarket_config.c
	$(CC) $(CFLAGS) -o supermarket supermarket.c read_supermarket_config.c

team: team.c read_supermarket_config.c
	$(CC) $(CFLAGS) -o team team.c read_supermarket_config.c -lpthread

customer: customer.c read_supermarket_config.c
	$(CC) $(CFLAGS) -o customer customer.c read_supermarket_config.c
forkcustomers: forkcustomers.c
	$(CC) $(CFLAGS) -o forkcustomers forkcustomers.c 

.PHONY: clean

clean:
	rm -f supermarket team customer forkcustomers