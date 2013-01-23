IDIR = -I ../../


all: libmixture.so

mixture.o: mixture.cc mixture.h
	g++ -fPIC -I ~/DS/Headersdp $(IDIR) -c mixture.cc -o mixture.o -g -O3

libmixture.so: mixture.o
	gcc -shared -o libmixture.so $(IDIR) mixture.o -g $(shell pkg-config --libs gsl)
	ln -sf $(shell readlink -f libmixture.so) ../../libraries/

clean:
	rm *.o
	rm libmixture.*
