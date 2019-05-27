minic: minic.l minic.y
	bison -d minic.y
	flex minic.l
	gcc -o $@ minic.tab.c lex.yy.c -ly -lfl
	rm -rf minic.tab.c minic.tab.h lex.yy.c

test_sample1: minic.l minic.y
	bison -vd minic.y
	flex minic.l
	gcc -o $@ minic.tab.c lex.yy.c -ly -lfl
	./$@ ./unit_test/sample1.c

.PHONY: clean 
clean:
	rm -rf *.tab.c *.tab.h *.yy.c *.output test_* *.vcg minic
