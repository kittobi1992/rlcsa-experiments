#include <sdsl/bit_vectors.hpp>
#include <sdsl/rl_bit_vector.hpp>

#include <iostream>

using namespace sdsl;
using namespace std;


std::mt19937_64 rng;

#define MILLI 1000 
#define MICRO 1000000
using HighResClockTimepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

HighResClockTimepoint s, e;
size_t max_iteration_index = 100000;
size_t one_count = 0;

inline HighResClockTimepoint time()
{
    return std::chrono::high_resolution_clock::now();
}

double seconds()
{
    std::chrono::duration<double> elapsed_seconds = e - s;
    return elapsed_seconds.count();
}

double milliseconds()
{
    std::chrono::duration<double> elapsed_seconds = e - s;
    return elapsed_seconds.count()*MILLI;
}

double microseconds()
{
    std::chrono::duration<double> elapsed_seconds = e - s;
    return elapsed_seconds.count() * MICRO;
}

template<class t_bit_vector>
class BitVectorExperiment {

  typedef typename t_bit_vector::rank_0_type rank_0;
  typedef typename t_bit_vector::rank_1_type rank_1;
  typedef typename t_bit_vector::select_0_type select_0;
  typedef typename t_bit_vector::select_1_type select_1;

  public:
    BitVectorExperiment(string &vector_type, bit_vector &vec)
    {
        s = time();
        t_bit_vector bit_vec(vec);
        e = time();
        double construction_time = seconds();
        double space_bits_per_element = 8.0 * (static_cast<double>(size_in_bytes(bit_vec)) / static_cast<double>(vec.size()));

        std::mt19937_64 rng;
        std::uniform_int_distribution<uint64_t> distribution(0, vec.size());
        auto dice = bind(distribution, rng);

        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            size_t j = dice();
            volatile uint64_t val = bit_vec[j];
        }
        e = time();
        double random_access_time_per_element = microseconds() / max_iteration_index;
        
        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            volatile uint64_t val = bit_vec[i];
        }
        e = time();
        double sequential_acces_time_per_element = microseconds() / max_iteration_index;

        rank_0 rank0(&bit_vec);
        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            size_t j = dice();
            volatile uint64_t val = rank0(j);
        }
        e = time();
        double rank0_time_per_element = microseconds() / max_iteration_index;

        rank_1 rank1(&bit_vec);
        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            size_t j = dice();
            volatile uint64_t val = rank1(j);
        }
        e = time();
        double rank1_time_per_element = microseconds() / max_iteration_index;

        std::uniform_int_distribution<uint64_t> distribution_select_0(1, vec.size()-one_count-1);
        auto dice1 = bind(distribution_select_0, rng);
        select_0 select0(&bit_vec);
        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            size_t j = dice1();
            volatile uint64_t val = select0(j);
        }
        e = time();
        double select0_time_per_element = microseconds() / max_iteration_index;

        std::uniform_int_distribution<uint64_t> distribution_select_1(1, one_count-1);
        auto dice2 = bind(distribution_select_1, rng);
        select_1 select1(&bit_vec);
        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            size_t j = dice2();
            volatile uint64_t val = select1(j);
        }
        e = time();
        double select1_time_per_element = microseconds() / max_iteration_index;


        cout << "\nVector=" << vector_type
             << "\nConstructionTime=" << construction_time
             << "\nSpaceBitsPerElement=" << space_bits_per_element
             << "\nRandomAccessTimePerElement=" << random_access_time_per_element
             << "\nSequentialAccessTimePerElement=" << sequential_acces_time_per_element
             << "\nRank0TimePerElement=" << rank0_time_per_element
             << "\nRank1TimePerElement=" << rank1_time_per_element
             << "\nSelect0TimePerElement=" << select0_time_per_element
             << "\nSelect1TimePerElement=" << select1_time_per_element
             << endl;
  }
};

int main(int argc, char *argv[])
{

  size_t N = atoi(argv[1]);
  size_t R = atoi(argv[2]);
  bit_vector b(N,0);

  std::uniform_int_distribution<uint64_t> distribution(N/(4*R), N/(2*R));
  auto dice = bind(distribution, rng);
  size_t idx = 0;
  for(size_t i = 0; i < R; ++i) {
      uint64_t run_length = dice();
      for(size_t j = idx; j < std::min(idx + run_length, N); ++j) {
          b[j] = 1;
          one_count++;
      }
      idx += run_length + dice();
      if(idx >= N) break;
  }

  {
      string algo = "rl_bit_vector";
      BitVectorExperiment<rl_bit_vector<>> experiment(algo, b);
  }

  {
      string algo = "bit_vector";
      BitVectorExperiment<bit_vector> experiment(algo, b);
  }

  {
      string algo = "sd_vector";
      BitVectorExperiment<sd_vector<>> experiment(algo, b);
  }

  {
      string algo = "rrr_vector";
      BitVectorExperiment<rrr_vector<>> experiment(algo, b);
  }
  

}