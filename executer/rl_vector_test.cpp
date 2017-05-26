#include <sdsl/rl_vector.hpp>
#include <sdsl/int_vector.hpp>

#include <iostream>

using namespace sdsl;
using namespace std;

int main(int argc, char *argv[])
{

    int_vector<64> vec(16,0);
    vec[0] = 3; vec[1] = 4; vec[2] = 5; vec[3] = 6;
    vec[4] = 9; vec[5] = 10; vec[6] = 12; vec[7] = 13;
    vec[8] = 15; vec[9] = 17; vec[10] = 18; vec[11] = 19;
    vec[12] = 22; vec[13] = 23; vec[14] = 25; vec[15] = 27;

    for(size_t i = 0; i < 16; ++i) {
        std::cout << vec[i] << " ";
    } 
    std::cout << std::endl;

    rl_vector<> rl_vec(vec);

    for(size_t i = 0; i < 16; ++i) {
        uint64_t val = rl_vec[i];
        std::cout << val << " ";
    } 
    std::cout << std::endl;

}