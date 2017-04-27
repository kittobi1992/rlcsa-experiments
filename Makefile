CC=g++
CFLAGS=-std=c++11 -O3 
SDSL_PREFIX=-DNDEBUG -I ~/include -L ~/lib
SDSL_SUFFIX=-lsdsl -ldivsufsort -ldivsufsort64
RLCSA_INCLUDE=-I rlcsa
RLCSA_SUFFIX=rlcsa/bits/rlevector.cpp rlcsa/bits/bitvector.cpp rlcsa/bits/bitbuffer.cpp
SANITIZE=-g -fsanitize=address

all: experiments

experiments: executer/rlvector_experiment


executer/rlvector_experiment: executer/rlvector_experiment.cpp executer/rlcsa_psi_vector.hpp executer/sdsl_psi_vector.hpp sdsl-lite/build/lib/libsdsl.a
							  $(CC) $(CFLAGS) -static executer/rlvector_experiment.cpp $(SDSL_PREFIX) $(SDSL_SUFFIX) $(RLCSA_INCLUDE) $(RLCSA_SUFFIX) -o executer/rlvector_experiment  

sdsl-lite/build/lib/libsdsl.a: $(wildcard sdsl-lite/include/sdsl/*)
				    		   rm -f $@
				    		   bash -x sdsl-lite/build/build.sh