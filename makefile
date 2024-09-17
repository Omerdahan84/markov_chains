.PHONY: tweets ,snake,clean
CC = gcc
CC_FLAGS = -Wall -Wextra -g -Wvla -std=c99

tweets:tweets_generator.o markov_chain.o linked_list.o
	$(CC) $(CC_FLAGS) $^ -o tweets_generator

snake:snakes_and_ladders.o markov_chain.o linked_list.o
	$(CC) $(CC_FLAGS) $^ -o snakes_and_ladders

snakes_and_ladders.o:snakes_and_ladders.c
	$(CC) $(CC_FLAGS) -c $^ -o $@

tweets_generator.o:tweets_generator.c
	$(CC) $(CC_FLAGS) -c $^ -o $@

linked_list.o:linked_list.c
	$(CC) $(CC_FLAGS) -c $^ -o $@

markov_chain.o:markov_chain.c
	$(CC) $(CC_FLAGS) -c $^ -o $@

clean:
	rm -rf  *.o