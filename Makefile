minic: minic.l minic.y
	bison -d minic.y
	flex minic.l
	gcc -o $@ main.c minic.tab.c lex.yy.c util.c -ly -lfl
	rm -rf minic.tab.c minic.tab.h lex.yy.c

test_sample1: minic.l minic.y
	bison -vd minic.y
	flex minic.l
	gcc -o $@ main.c minic.tab.c lex.yy.c util.c  -ly -lfl
	./$@ ./unit_test/sample1.mc

test_sample2: minic.l minic.y
	bison -vd minic.y
	flex minic.l
	gcc -o $@ main.c minic.tab.c lex.yy.c util.c  -ly -lfl
	./$@ ./unit_test/sample2.mc

bison_verbose: minic.y
	bison -dyv minic.y

.PHONY: clean 
clean:
	rm -rf *.tab.c *.tab.h *.yy.c *.output test_* *.vcg minic
