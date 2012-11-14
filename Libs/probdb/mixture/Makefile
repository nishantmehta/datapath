IDIR = -I ../../

all: mixture

mixture.o: mixture.cc mixture.h
	g++ -fPIC -I ~/DS/Headersdp $(IDIR) -c mixture.cc -o mixture.o -g -O3

mixture: mixture.o
	gcc -shared -o libmixture.so $(IDIR) mixture.o -g -lfftw3 -lfftw3_threads
	ln -sf $(shell readlink -f libmixture.so) ../../libraries/

clean:
	rm *.o
	rm libmixture.so