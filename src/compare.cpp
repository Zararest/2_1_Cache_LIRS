#include "headers/cache.hpp"
#include "headers/perfect_cache.hpp"
#include <random> 
#include <fstream>
#include <ctime>

#define DATA_RANGE 200
#define NUM_OF_ITER 1000000

typedef long key;
typedef int page;


page get_page(key cur_key){

    return cur_key;
}

int main(){

    std::mt19937 mersenne(static_cast<unsigned int>(time(0))); 

    std::vector<key> keys_vector;
    int counter = 0, cur_number = 0, perfect_counetr = 0;
    long size_of_cache = 0;

    std::cin >> size_of_cache;

    LIRS_cache<page, key>  new_cache(size_of_cache);

    for (int i = 0; i < NUM_OF_ITER; i++){

        cur_number = mersenne();

        keys_vector.push_back(cur_number % DATA_RANGE);
        counter += new_cache.update(cur_number % DATA_RANGE, get_page);
    }

    Perfect_cache<page, key> new_perfect_cache(keys_vector, size_of_cache);

    std::cout << "number of hits: " << counter << '\n';
    std::cout << "number of perfect hits: " << new_perfect_cache.get_hit_count(get_page) << '\n';
}