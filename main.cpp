#include "cache.inl"
#include "perfect_cache.inl"
#include <random> 

#define CASH_SIZE 300
#define DATA_RANGE 2000
#define NUM_OF_ITER 1000000

Cache_elem get_page(Key_T cur_key){

    Cache_elem tmp(cur_key, cur_key, elem_with_data);

    return tmp;
}

page get_only_page(Key_T cur_key){

    return cur_key;
}

int main(){

    std::mt19937 mersenne(static_cast<unsigned int>(time(0)));

    LIRS_cache new_cache(CASH_SIZE);
    
    std::vector<Key_T> keys_vector;
    int counter = 0, cur_number = 0, perfect_counetr = 0;

    for (int i = 0; i < NUM_OF_ITER; i++){

        cur_number = mersenne();

        keys_vector.push_back(cur_number % DATA_RANGE);
        counter += new_cache.update(cur_number % DATA_RANGE, get_page);
    }

    Perfect_cache new_perfect_cache(keys_vector, CASH_SIZE);

    printf("number of hits: %i\n", counter);
    printf("number of perfect hits: %i\n", new_perfect_cache.get_hit_count(get_only_page));
    return 0;
}