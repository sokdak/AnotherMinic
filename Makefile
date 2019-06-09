minic: minic.l minic.y
	bison -d minic.y
	flex minic.l
	gcc -o $@ main.c node.c symbol.c function.c log.c eval.c minic.tab.c lex.yy.c -ly -lfl
	rm -rf minic.tab.c minic.tab.h lex.yy.c

sample%: minic.l minic.y
	bison -vd minic.y
	flex minic.l
	gcc -o $@ main.c node.c symbol.c function.c log.c eval.c minic.tab.c lex.yy.c  -ly -lfl
	./$@ ./unit_test/$@.mc

bison_verbose: minic.y
	bison -dyv minic.y

.PHONY: clean 
clean:
	rm -rf *.tab.c *.tab.h *.yy.c *.output test_* sample* *.vcg minic
