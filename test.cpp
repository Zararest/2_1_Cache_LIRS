
#define BOOST_TEST_MODULE First_TestSuite
#include <boost/test/included/unit_test.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include "cache.inl"
#include "perfect_cache.inl"


class Cache_for_test: public LIRS_cache<int, int>{

public:

    void init_test_1();
    bool check_test_1();
};

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

    std::ifstream file("../bin/big_data_test.txt");

    assert(file.is_open() == true);

    int cache_size = 5, hit_count = 0, elem = 0;

    LIRS_cache<int, int> new_cache(cache_size);
    
    while(!file.eof()){

        file >> elem;
        
        hit_count += new_cache.update(elem, get_page); 
    }

    BOOST_REQUIRE_EQUAL(hit_count, 3);
}

BOOST_AUTO_TEST_CASE(Equal_data_int){

    std::ifstream file("../bin/equal_data_test.txt");

    assert(file.is_open() == true);

    int cache_size = 5, hit_count = 0, elem = 0;

    LIRS_cache<int, int> new_cache(cache_size);
    
    while(!file.eof()){

        file >> elem;
        
        hit_count += new_cache.update(elem, get_page); 
    }

    BOOST_REQUIRE_EQUAL(hit_count, 19);
}

BOOST_AUTO_TEST_CASE(Result_of_prev_version_int){

    std::ifstream file("../bin/result_of_prev_version_test.txt");

    assert(file.is_open() == true);

    int cache_size = 100, hit_count = 0, elem = 0;

    LIRS_cache<int, int> new_cache(cache_size);
    
    while(!file.eof()){

        file >> elem;
        
        hit_count += new_cache.update(elem, get_page); 
    }

    BOOST_REQUIRE_EQUAL(hit_count, 3750493);
}

BOOST_AUTO_TEST_CASE(Time_test_int){

    std::ifstream file("../bin/time_test.txt");
    assert(file.is_open() == true);

    unsigned int start_time = 0, end_time = 0;
    int cache_size = 5, hit_count = 0, elem = 0;
    bool not_so_long = false;
    LIRS_cache<int, int> new_cache(cache_size);
    
    start_time = clock();
    while(!file.eof()){

        file >> elem;
        
        hit_count += new_cache.update(elem, get_page); 
    }
    end_time = clock();

    if ((end_time - start_time) / CLOCKS_PER_SEC <= 15){

        not_so_long = true;
    }

    BOOST_REQUIRE_EQUAL(not_so_long, true);
}


BOOST_AUTO_TEST_SUITE_END()

