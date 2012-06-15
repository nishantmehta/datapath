a.out: T.g main.c 
	#export LD_LIBRARY_PATH=/usr/local/lib/ Just remember this
	antlr3.sh T.g -o output
	g++ output/TParser.c output/TLexer.c -lantlr3c main.c -I./output
