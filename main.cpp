#include "cache.inl"
#include "perfect_cache.inl"
#include <random> 

#define CASH_SIZE 300
#define DATA_RANGE 2000
#define NUM_OF_ITER 1000000

typedef long key;
typedef int page;

page get_page(key cur_key){

    return cur_key;
}

int main(){

    std::mt19937 mersenne(static_cast<unsigned int>(time(0))); 

    LIRS_cache<page, key>  new_cache(CASH_SIZE);
    
    std::vector<key> keys_vector;
    int counter = 0, cur_number = 0, perfect_counetr = 0; //на вход: размер кэша, количество элементов, элементы

    for (int i = 0; i < NUM_OF_ITER; i++){

        cur_number = mersenne();

        keys_vector.push_back(cur_number % DATA_RANGE);
        counter += new_cache.update(cur_number % DATA_RANGE, get_page);
    }

    Perfect_cache<page, key> new_perfect_cache(keys_vector, CASH_SIZE);

    printf("number of hits: %i\n", counter);
    printf("number of perfect hits: %i\n", new_perfect_cache.get_hit_count(get_page));//количество попаданий
    return 0;
}