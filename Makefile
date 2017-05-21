CC=g++
CFLAGS=-std=c++11 -O3 
SDSL_PREFIX=-DNDEBUG -I ~/include -L ~/lib
SDSL_LIB=-lsdsl -ldivsufsort -ldivsufsort64
RLCSA_INCLUDE=-Irlcsa
RLCSA_LIB=rlcsa/bits/rlevector.cpp rlcsa/bits/bitvector.cpp rlcsa/bits/bitbuffer.cpp
SANITIZE=-g -fsanitize=address

all: experiments

bit_vector_test: executer/bit_vector_test

experiments: executer/rlvector_experiment executer/rlvector_sampling_experiment

executer/bit_vector_test: sdsl-lite/build/lib/libsdsl.a executer/bit_vector_test.cpp
						$(CC) $(CFLAGS) $(SDSL_PREFIX) executer/bit_vector_test.cpp $(SDSL_LIB) -o executer/bit_vector_test

executer/rlvector_experiment: executer/rlvector_experiment.cpp executer/rlcsa_psi_vector.hpp executer/sdsl_psi_vector.hpp sdsl-lite/build/lib/libsdsl.a
							  $(CC) $(CFLAGS) $(SDSL_PREFIX) $(RLCSA_INCLUDE) executer/rlvector_experiment.cpp $(SDSL_LIB) $(RLCSA_LIB) -o executer/rlvector_experiment  

executer/rlvector_sampling_experiment: executer/rlvector_sampling_experiment.cpp executer/rlcsa_psi_vector.hpp executer/sdsl_psi_vector.hpp sdsl-lite/build/lib/libsdsl.a
							  		   $(CC) $(CFLAGS) $(SDSL_PREFIX) $(RLCSA_INCLUDE) executer/rlvector_sampling_experiment.cpp $(SDSL_LIB) $(RLCSA_LIB) -o executer/rlvector_sampling_experiment  


sdsl-lite/build/lib/libsdsl.a: $(wildcard sdsl-lite/include/sdsl/*)
				    		   rm -f $@
				    		   bash -x sdsl-lite/build/build.sh