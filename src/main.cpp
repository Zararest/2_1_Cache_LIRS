#include "headers/cache.hpp"
#include "headers/perfect_cache.hpp"
#include <random> 
#include <fstream>
#include <ctime>

#define COMPARE 0
#define FROM_FILE 0

typedef long key;
typedef int page;

page get_page(key cur_key){

    return cur_key;
}

int main(){

    long size_of_cache = 0;

    std::cin >> size_of_cache;

    LIRS_cache<page, key>  new_cache(size_of_cache);
    long number_of_elems = 0, counter = 0;
    page elem;

    if (FROM_FILE == 1){

        std::ifstream data_file("../bin/015.dat");
        assert(data_file.is_open());

        while (!data_file.eof()){

            data_file >> elem;

            counter += new_cache.update(elem, get_page); 
        }

        std::cout << counter << '\n';
    } else{

        std::cin >> number_of_elems;
        
        for (long i = 0; i < number_of_elems; i++){
            
            std::cin >> elem;
            counter += new_cache.update(elem, get_page);
        }

        std::cout << counter << '\n';
    }
}