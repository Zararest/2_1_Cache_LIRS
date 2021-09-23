
#define BOOST_TEST_MODULE First_TestSuite
#include <boost/test/included/unit_test.hpp>


#include <vector>
#include <iostream>
#include <fstream>
#include "cache.inl"
#include "perfect_cache.inl"

int get_page(int key){

    return key;
}

BOOST_AUTO_TEST_SUITE(LIRS_cache_tests)

BOOST_AUTO_TEST_CASE(Small_data_int){

    std::vector<int> data = {1, 2, 3, 4, 5, 6, 5, 7, 7, 8, 1, 1};
    int cache_size = 5, hit_count = 0;

    LIRS_cache<int, int> new_cache(cache_size);

    for (int i = 0; i < data.size(); i++){
        
        hit_count += new_cache.update(data[i], get_page);
    }

    BOOST_REQUIRE_EQUAL(hit_count, 3);
}

BOOST_AUTO_TEST_CASE(Big_data_int){

    std::ifstream file("/home/uwu/Desktop/Intel/Cache_LIRS/test_data.txt");

    assert(file.is_open() == true);

    int cache_size = 5, hit_count = 0, elem = 0;

    LIRS_cache<int, int> new_cache(cache_size);
    
    while(!file.eof()){

        file >> elem;
        
        hit_count += new_cache.update(elem, get_page); 
    }

    BOOST_REQUIRE_EQUAL(hit_count, 3);
}

BOOST_AUTO_TEST_SUITE_END()