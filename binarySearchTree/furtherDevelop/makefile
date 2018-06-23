CFLAGS = -Wall -Wextra -Werror -Wfloat-equal -pedantic -ansi -lm
INCS = mybst.h

all: hash hash_d thread thread_d prettier prettier_d



thread:  bst.c thread.c $(INCS)
	$(CC) bst.c thread.c -o thread -O3 $(CFLAGS)

thread_d:  bst.c thread.c $(INCS)
	$(CC) bst.c thread.c -o thread_d -g -O $(CFLAGS)


prettier:  bst.c prettier.c $(INCS)
	$(CC) bst.c prettier.c -o prettier -O3 $(CFLAGS)

prettier_d:  bst.c prettier.c $(INCS)
	$(CC) bst.c prettier.c -o prettier_d -g -O $(CFLAGS)


hash:  bst.c hash.c $(INCS)
	$(CC) bst.c hash.c -o hash -O3 $(CFLAGS)

hash_d:  bst.c thread.c $(INCS)
	$(CC) bst.c hash.c -o hash_d -g -O $(CFLAGS)

run: all
	./prettier
	./hash
	./thread
	
memchk: thread_d hash_d prettier_d
	valgrind --error-exitcode=1 --quiet --leak-check=full ./thread
	valgrind --error-exitcode=1 --quiet --leak-check=full ./hash
	valgrind --error-exitcode=1 --quiet --leak-check=full ./prettier
clean:
	rm -f hash hash_d thread thread_d prettier prettier_d

.PHONY: clean run memchk all
