CC=g++
CFLAGS=-std=c++11 -O3 
SDSL_PREFIX=-DNDEBUG -I ~/include -L ~/lib
SDSL_SUFFIX=-lsdsl -ldivsufsort -ldivsufsort64
SANITIZE=-g -fsanitize=address

all: experiments

experiments: executer/rlvector_experiment.o


executer/rlvector_experiment.o: executer/rlvector_experiment.cpp sdsl-lite/build/lib/libsdsl.a executer/sdsl_psi_vector.hpp
	                   			 $(CC) $(CFLAGS) $(SDSL_PREFIX) executer/rlvector_experiment.cpp -o executer/rlvector_experiment.o $(SDSL_SUFFIX)

sdsl-lite/build/lib/libsdsl.a: $(wildcard sdsl-lite/include/sdsl/*)
				    		   rm -f $@
				    		   bash -x sdsl-lite/build/build.sh