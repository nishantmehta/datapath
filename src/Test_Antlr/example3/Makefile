a.out: T.g main.c Eval.g
	antlr3.sh T.g Eval.g -o output
	g++ -g output/TParser.c output/TLexer.c output/Eval.c -lantlr3c main.c -I./output
