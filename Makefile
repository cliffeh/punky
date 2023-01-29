OBJ=parser.o scanner.o main.o expr.o print.o eval.o env.o punky.o

LEX=flex
CFLAGS=-g
LDFLAGS=-lm
YFLAGS=--defines=symbols.h

TESTOUT=$(wildcard test/*.out)
TESTIN=$(wildcard test/*.in)
TEST=$(patsubst %.in,%,$(TESTIN))

punky: $(OBJ) ## Build the punky executable (default)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

help: ## Show this help
	@echo "\nSpecify a command. The choices are:\n"
	@grep -E '^[0-9a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[0;36m%-12s\033[m %s\n", $$1, $$2}'
	@echo ""
.PHONY: help

scanner.c: scanner.l

scanner.o: symbols.h scanner.c

parser.o: punky.h parser.c

parser.c: parser.y

symbols.h: parser.c

expr.o: punky.h expr.c

eval.o: punky.h eval.c

env.o: punky.h env.c

print.o: punky.h print.c

main.o: punky.h main.c

test: .test ## Run tests
.PHONY:test

valgrind: .valgrind ## Run valgrind memory leak checks
.PHONY: valgrind

.test: punky $(TESTIN) $(TESTOUT)
	for f in $(TEST); do echo "\ntesting $$f.in\n"; ./punky -i $$f.in -o .test && diff $$f.out .test; done
	rm -f .test

.valgrind: punky $(TESTIN)
	for f in $(TESTIN); do echo "\nvalgrinding $$f\n"; valgrind --leak-check=full --log-file=.valgrind ./punky -i $$f -o /dev/null; tail -1 .valgrind; done
	rm -f .valgrind

test-clean: ## Clean up testing files
	rm -f .test .valgrind
.PHONY: test-clean

clean: test-clean ## Clean up generated source and object files
	rm -f *.o scanner.c parser.c symbols.h
.PHONY: clean

veryclean: clean
	rm -f punky punky.exe punky.exe.stackdump gmon.out perf.data
.PHONY: veryclean

realclean: veryclean ## Clean up everything
.PHONY: realclean
