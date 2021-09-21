#include "cache.inl"
#include "perfect_cache.inl"
#include <random> 


#define COMPARE 1
#define DATA_RANGE 200
#define NUM_OF_ITER 500

typedef long key;
typedef int page;


page get_page(key cur_key){

    return cur_key;
}

void compare_to_perfect(int cache_size){

    std::mt19937 mersenne(static_cast<unsigned int>(time(0))); 

    LIRS_cache<page, key>  new_cache(cache_size);
    
    std::vector<key> keys_vector;
    int counter = 0, cur_number = 0, perfect_counetr = 0;

    for (int i = 0; i < NUM_OF_ITER; i++){

        cur_number = mersenne();

        keys_vector.push_back(cur_number % DATA_RANGE);
        counter += new_cache.update(cur_number % DATA_RANGE, get_page);
    }

    Perfect_cache<page, key> new_perfect_cache(keys_vector, cache_size);

    std::cout << "number of hits: " << counter << '\n';
    std::cout << "number of perfect hits: " << new_perfect_cache.get_hit_count(get_page) << '\n';
}

int main(){

    long size_of_cache = 0;

    std::cin >> size_of_cache;

    if (COMPARE == 1){

        compare_to_perfect(size_of_cache);
    } else{

        LIRS_cache<page, key>  new_cache(size_of_cache);

        long number_of_elems = 0, counter = 0;
        page elem;

        std::cin >> number_of_elems;

        for (long i = 0; i < number_of_elems; i++){

            std::cin >> elem;
            counter += new_cache.update(elem, get_page);
        }

        std::cout << counter << '\n';
    }
    return 0;
}