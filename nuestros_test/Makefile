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

test_parser:
	$(CC) ./test_parser.c $(CFLAGS) ../objects-x86_64/parser.o ../objects-x86_64/lexer.o  -o test_parser.out $(CPPFLAGS) $(LDFLAGS) && ./test_parser.out

test_parser_valgrind: $(test_parser)
	valgrind ./test_parser.out --leak-check=full -s

test_parsing:
	$(CC) ./test_parsing.c $(CFLAGS) ../parsing.c ../objects-x86_64/parser.o ../objects-x86_64/lexer.o ../command.c ../strextra.c  -o test_parsing.out $(CPPFLAGS) $(LDFLAGS) && ./test_parsing.out

test_parsing_valgrind:
	valgrind ./test_parsing.out --leak-check=full -s


clean: 
	rm *.out
