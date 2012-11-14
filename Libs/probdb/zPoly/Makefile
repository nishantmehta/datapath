IDIR = -I ../../

all: zPoly

zPoly.o: zPoly.cc zPoly.h
	g++ -fPIC -I ~/DS/Headersdp $(IDIR) -c zPoly.cc -o zPoly.o -g -O3

DFT.o: DFT.h DFT.cc
	g++ -fPIC -I ~/DS/Headersdp $(IDIR) -c DFT.cc -o DFT.o -g -O3

zPoly: zPoly.o DFT.o
	gcc -shared -o libzPoly.so $(IDIR) zPoly.o DFT.o -g -lfftw3 -lfftw3_threads
	ln -sf $(shell readlink -f libzPoly.so) ../../libraries/

clean:
	rm *.o
	rm libzPoly.so