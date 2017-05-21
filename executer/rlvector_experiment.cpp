#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>
#include <sdsl/dac_vector.hpp>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/hyb_sd_vector.hpp>
#include <sdsl/rl_enc_vector.hpp>
#include <sdsl/rl_inc_vector.hpp>
#include <sdsl/wt_rlmn.hpp>
#include <sdsl/wt_huff.hpp>
#include <sdsl/csa_alphabet_strategy.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <chrono>
#include <climits>
#include <random>
#include <stack>

#include "sdsl_psi_vector.hpp"
#include "rlcsa_psi_vector.hpp"


#define x first
#define y second 
#define MILLI 1000 
#define MICRO 1000000

using namespace std;
using namespace sdsl;

using ll = long long;

using HighResClockTimepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

HighResClockTimepoint s, e;

string test_file, temp_dir, test_id;

const bool test = false;
size_t max_iteration_index = 10000;

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


void construct_phi(cache_config &test_config, string &test_file)
{

    {
        cout << "Load text..." << endl;
        int_vector<8> text;
        load_vector_from_file(text, test_file, 1);
        append_zero_symbol(text);

        cout << "Construct Suffix Array..." << endl;
        int_vector<> sa(text.size(), 0, bits::hi(text.size()) + 1);
        algorithm::calculate_sa((const unsigned char *)text.data(), text.size(), sa);

        cout << "Construct Inverse Suffix Array..." << endl;
        int_vector<> isa(text.size()+1, 0, bits::hi(text.size()) + 1);
        for (size_t i = 0; i < text.size(); ++i)
        {
            isa[sa[i]] = i;
        }
        isa[text.size()] = isa[0];

        cout << "Construct Psi Array" << endl;
        int_vector<> psi(text.size(), 0, bits::hi(text.size()) + 1);
        for (size_t i = 0; i < text.size(); ++i) {
            psi[i] = isa[sa[i] + 1];
        }
        util::bit_compress(psi);
        store_to_cache(psi, conf::KEY_PSI, test_config);
    }
}

void construct_bwt(cache_config &test_config, string &test_file)
{

    {
        cout << "Load text..." << endl;
        int_vector<8> text;
        load_vector_from_file(text, test_file, 1);
        append_zero_symbol(text);
        store_to_cache(text, conf::KEY_TEXT, test_config);

        cout << "Construct Suffix Array..." << endl;
        int_vector<> sa(text.size(), 0, bits::hi(text.size()) + 1);
        algorithm::calculate_sa((const unsigned char *)text.data(), text.size(), sa);
        store_to_cache(sa, conf::KEY_SA, test_config);

        cout << "Construct BWT..." << endl;
        construct_bwt<8>(test_config);
    }
}

inline void testPsiValue(size_t idx, uint64_t enc_val, int_vector<>& psi) {
    if(test) {
        if(enc_val != psi[idx]) {
            cout << "Failed to encrypt value!" << endl;
            cout << "Idx=" << idx << " - Encrypted Value=" << enc_val << " - Real Value=" << psi[idx] << endl;
            abort();
        }
    }
}

template<class t_vector>
class PsiVectorExperiment {
  public:
    PsiVectorExperiment(string &vector_type, int_vector<> &psi)
    {
        s = time();
        t_vector compressed_vec(psi);
        e = time();
        double construction_time = seconds();
        double space_bits_per_element = 8.0 * (static_cast<double>(size_in_bytes(compressed_vec)) / static_cast<double>(psi.size()));

        //std::cout << space_bits_per_element << std::endl;
        write_structure<HTML_FORMAT>(compressed_vec, "HTML/" + vector_type + "_" + test_id + ".html");

        std::mt19937_64 rng;
        std::uniform_int_distribution<uint64_t> distribution(0, psi.size());
        auto dice = bind(distribution, rng);

        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            size_t j = dice();
            volatile uint64_t psi_val = compressed_vec[j];
            testPsiValue(j, psi_val, psi);
        }
        e = time();
        double random_access_time_per_element = microseconds() / max_iteration_index;
        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            volatile uint64_t psi_val = compressed_vec[i];
            testPsiValue(i, psi_val, psi);
        }
        e = time();
        double sequential_acces_time_per_element = microseconds() / max_iteration_index;

        cout << "RESULT"
             << " Vector=" << vector_type
             << " Benchmark=" << test_id
             << " ConstructionTime=" << construction_time
             << " SpaceBitsPerElement=" << space_bits_per_element
             << " RandomAccessTimePerElement=" << random_access_time_per_element
             << " SequentialAccessTimePerElement=" << sequential_acces_time_per_element
             << endl;
  }
};

template <class t_wt>
class BWTExperiment
{

typedef typename wt_alphabet_trait<t_wt>::type alphabet_type;

  public:
    BWTExperiment(string &vector_type, cache_config &config)
    {
        s = time();
        int_vector_buffer<alphabet_type::int_width> bwt_buf(cache_file_name(key_trait<alphabet_type::int_width>::KEY_BWT, config));
        size_t n = bwt_buf.size();
        t_wt wt(bwt_buf,n);
        e = time();
        double construction_time = seconds();
        double space_bits_per_element = 8.0 * (static_cast<double>(size_in_bytes(wt)) / static_cast<double>(n));

        write_structure<HTML_FORMAT>(wt, "HTML/" + vector_type + "_" + test_id + ".html");

        std::mt19937_64 rng;
        std::uniform_int_distribution<uint64_t> distribution(0, bwt_buf.size());
        auto dice = bind(distribution, rng);

        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            size_t j = dice();
            volatile uint64_t bwt_val = wt[j];
        }
        e = time();
        double random_access_time_per_element = microseconds() / max_iteration_index;
        s = time();
        for (int i = 0; i < max_iteration_index; ++i)
        {
            volatile uint64_t bwt_val = wt[i];
        }
        e = time();
        double sequential_acces_time_per_element = microseconds() / max_iteration_index;

        cout << "RESULT"
             << " Vector=" << vector_type
             << " Benchmark=" << test_id
             << " ConstructionTime=" << construction_time
             << " SpaceBitsPerElement=" << space_bits_per_element
             << " RandomAccessTimePerElement=" << random_access_time_per_element
             << " SequentialAccessTimePerElement=" << sequential_acces_time_per_element
             << endl;
    }
};

int main(int argc, char *argv[])
{

    test_file = argv[1];
    temp_dir = argv[2];
    test_id = test_file.substr(test_file.find_last_of("/\\") + 1);

    cache_config test_config = cache_config(false, temp_dir, test_id);

    string psi_file = cache_file_name(conf::KEY_PSI, test_config);
    int_vector<> psi;
    if (!load_from_file(psi, psi_file))
    {
        construct_phi(test_config, test_file);
        load_from_file(psi, psi_file);
    }

    if (!cache_file_exists(conf::KEY_BWT, test_config))
    {
        construct_bwt(test_config, test_file);
    }


    {
        string algo = "rlcsa_vector";
        PsiVectorExperiment<rlcsa_psi_vector> experiment(algo, psi);
    }

    {
        string algo = "rl_inc_vector";
        PsiVectorExperiment<sdsl_psi_vector<rl_inc_vector<dac_vector_dp<>, sd_vector<>, 8>>> experiment(algo, psi);
    }

    {
        string algo = "wt_rlmn_bwt_vector";
        BWTExperiment<wt_rlmn<>> experiment(algo, test_config);
    }

    {
        string algo = "wt_rl_huff_bwt_vector";
        BWTExperiment<wt_huff<rl_bit_vector<>>> experiment(algo, test_config);
    }


    {
        string algo = "enc_vector";
        PsiVectorExperiment < sdsl_psi_vector<enc_vector<>>> experiment(algo, psi);
    }

}