CC=gcc
CPPFLAGS=`pkg-config --cflags glib-2.0`
CFLAGS=-std=gnu11 -Wall -Wextra -Wbad-function-cast -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -Werror=vla -g -pedantic
LDFLAGS=`pkg-config --libs glib-2.0`

test_builtin:
	$(CC) ./test_builtin.c $(CFLAGS) ../builtin.c ../command.c ../strextra.c  -o test_builtin.out $(CPPFLAGS) $(LDFLAGS) && ./test_builtin.out

test_command: 
	$(CC) ./test_command.c $(CFLAGS) ../command.c ../strextra.c -o test_command.out $(CPPFLAGS) $(LDFLAGS) && ./test_command.out
	
test_command_valgrind: $(test_command)
	valgrind ./test_command.out --leak-check=full -s

clean: 
	rm *.out
