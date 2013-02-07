IDIR = -I ../../

all: libzPoly.so

test.o: test.cc zPoly.h
	g++ -o test.o -c test.cc -march=native -g $(IDIR) -I ~/DS/Headersdp -std=gnu++11

test: libzPoly.so test.o
	g++ -o test test.o -L . -lzPoly

zPoly.o: zPoly.cc zPoly.h
	g++ -fPIC -I ~/DS/Headersdp $(IDIR) -c zPoly.cc -o zPoly.o -g -O3

DFT.o: DFT.h DFT.cc
	g++ -fPIC -I ~/DS/Headersdp $(IDIR) -c DFT.cc -o DFT.o -g -O3

libzPoly.so: zPoly.o DFT.o
	gcc -shared -o libzPoly.so $(IDIR) zPoly.o DFT.o -g -lfftw3 -lfftw3_threads

clean:
	rm *.o
	rm libzPoly.so
