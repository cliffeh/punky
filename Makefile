OBJ=parser.o scanner.o main.o expr.o print.o eval.o env.o punky.o

LEX=flex
CFLAGS=-g
YFLAGS=--defines=symbols.h

punky: $(OBJ)

scanner.c: scanner.l

scanner.o: symbols.h scanner.c

# parse.o: symbols.h punky.h parse.c

parser.o: punky.h parser.c

parser.c: parser.y

symbols.h: parser.c

expr.o: punky.h expr.c

eval.o: punky.h eval.c

env.o: punky.h env.c

print.o: punky.h print.c

main.o: punky.h main.c

test: .test

valgrind: .valgrind

.test: punky test/test.in test/test.out test/sort.in test/sort.out
	./punky -i test/test.in -o .test
	diff test/test.out .test
	./punky -i test/sort.in -o .sort.test
	diff test/sort.out .sort.test

.valgrind: punky test/test.in
	valgrind --leak-check=full --log-file=.valgrind ./punky -i test/test.in -o /dev/null
	cat .valgrind

test-clean:
	rm -f .test .sort.test .valgrind

clean: test-clean
	rm -f *.o scanner.c parser.c symbols.h

veryclean: clean
	rm -f punky punky.exe punky.exe.stackdump
