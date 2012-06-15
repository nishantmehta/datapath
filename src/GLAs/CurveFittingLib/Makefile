CC=g++

LD=ar

CCFLAGS=-g -march=native -O3 --fast-math -fPIC

# CCFLAGS+=-DDEBUG

LIBRARIES=-lgsl -lgslcblas

%.o: %.cpp
	$(CC) -c -o $@ $< $(CCFLAGS) $(LIBRARIES)

libcfit.a: MLE.o gsl_opt.o DistributionFitting.o
	ar rvs $@ $^ 

clean:
	rm -f *.o
	rm -f libcfit.a
