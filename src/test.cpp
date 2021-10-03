
#define BOOST_TEST_MODULE First_TestSuite
#include <boost/test/included/unit_test.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include "headers/cache.hpp"
#include "headers/perfect_cache.hpp"

enum context_errors{

    OK = 0,
    LIR_stack_size = 1,
    HIR_list_size = 2,
    LIR_hash_size = 3, 
    HIR_hash_size = 4,
    LIR_elem_key = 5,
    LIR_elem_type = 6,
    HIR_elem_key = 7,
    HIR_elem_type = 8
};

class Cache_for_test: public LIRS_cache<int, int>{

public:

    Cache_for_test():LIRS_cache<int, int>(5){

    }

    void init_context(std::ifstream& data_file);

    int check_context(std::ifstream& data_file);

};

void Cache_for_test::init_context(std::ifstream& data_file){

    int num_of_LIR_elems = 0, num_of_HIR_elems = 0, key = 0, type = 0;

    data_file >> num_of_LIR_elems >> num_of_HIR_elems;

    data_file >> LIR_size >> HIR_size;

    for (int i = 0; i < num_of_LIR_elems; i++){

        data_file >> key >> type;

        Cache_elem<int, int> tmp(key, key, type);
        LIR_stack.push_back(tmp);
        LIR_hash_t[key] = (--LIR_stack.end());
    }

    for (int i = 0; i < num_of_HIR_elems; i++){

        data_file >> key >> type;

        Cache_elem<int, int> tmp(key, key, type);
        HIR_list.push_back(tmp);
        HIR_hash_t[key] = (--HIR_list.end());
    }
}

int Cache_for_test::check_context(std::ifstream& data_file){

    int num_of_LIR_elems = 0, num_of_HIR_elems = 0, key = 0, type = 0;
    std::list<Cache_elem<int, int>>::iterator itter;

    data_file >> num_of_LIR_elems >> num_of_HIR_elems;

    data_file >> LIR_size >> HIR_size;

    if (num_of_LIR_elems != LIR_stack.size()){

        return LIR_stack_size;
    }

    if (num_of_HIR_elems != HIR_list.size()){

        return HIR_list_size;
    }

    if (num_of_LIR_elems != LIR_hash_t.size()){

        return LIR_hash_size;
    }

    if (num_of_HIR_elems != HIR_hash_t.size()){

        return HIR_hash_size;
    }

    itter = LIR_stack.begin();
    for (int i = 0; i < num_of_LIR_elems; i++){

        data_file >> key >> type;

        if (key != itter->get_key()){

            return LIR_elem_key;
        }

        if (type != itter->get_type()){

            return LIR_elem_type;
        }

        ++itter;
    }

    itter = HIR_list.begin();
    for (int i = 0; i < num_of_HIR_elems; i++){

        data_file >> key >> type;

        if (key != itter->get_key()){

            return HIR_elem_key;
        }

        if (type != itter->get_type()){

            return HIR_elem_type;
        }

        ++itter;
    }

    return OK;
}

int get_page(int key){

    return key;
}


BOOST_AUTO_TEST_SUITE(LIRS_cache_tests)

BOOST_AUTO_TEST_CASE(Module_hit_LIR_test){

    std::ifstream first_context("../bin/hit_LIR_1_test.txt");
    std::ifstream second_context("../bin/hit_LIR_2_test.txt");
    std::ifstream third_context("../bin/hit_LIR_3_test.txt");
    assert(first_context.is_open() == true);
    assert(second_context.is_open() == true);
    assert(third_context.is_open() == true);

    Cache_for_test new_cache;
    int sum_of_errors = 0;


    new_cache.init_context(first_context);
    new_cache.update(5, get_page);
    sum_of_errors += new_cache.check_context(second_context);
    new_cache.update(3, get_page);
    sum_of_errors += new_cache.check_context(third_context);

    BOOST_REQUIRE_EQUAL(sum_of_errors, OK);
}

BOOST_AUTO_TEST_CASE(Module_hit_HIR_test){

    std::ifstream first_context("../bin/hit_HIR_1_test.txt");
    std::ifstream second_context("../bin/hit_HIR_2_test.txt");
    std::ifstream third_context("../bin/hit_HIR_3_test.txt");
    assert(first_context.is_open() == true);
    assert(second_context.is_open() == true);
    assert(third_context.is_open() == true);

    Cache_for_test new_cache;
    int sum_of_errors = 0;


    new_cache.init_context(first_context);
    new_cache.update(3, get_page);
    sum_of_errors += new_cache.check_context(second_context);
    new_cache.update(6, get_page);
    sum_of_errors += new_cache.check_context(third_context);

    BOOST_REQUIRE_EQUAL(sum_of_errors, OK);
}

BOOST_AUTO_TEST_CASE(Module_miss_test){

    std::ifstream first_context("../bin/miss_1_test.txt");
    std::ifstream second_context("../bin/miss_2_test.txt");
    std::ifstream third_context("../bin/miss_3_test.txt");
    assert(first_context.is_open() == true);
    assert(second_context.is_open() == true);
    assert(third_context.is_open() == true);

    Cache_for_test new_cache;
    int sum_of_errors = 0;


    new_cache.init_context(first_context);
    new_cache.update(8, get_page);
    sum_of_errors += new_cache.check_context(second_context);
    new_cache.update(5, get_page);
    sum_of_errors += new_cache.check_context(third_context);

    BOOST_REQUIRE_EQUAL(sum_of_errors, OK);
}

BOOST_AUTO_TEST_CASE(Module_HIT_non_resident_test){

    std::ifstream first_context("../bin/hit_non_res_1_test.txt");
    std::ifstream second_context("../bin/hit_non_res_2_test.txt");
    std::ifstream third_context("../bin/hit_non_res_3_test.txt");
    assert(first_context.is_open() == true);
    assert(second_context.is_open() == true);
    assert(third_context.is_open() == true);

    Cache_for_test new_cache;
    int sum_of_errors = 0;


    new_cache.init_context(first_context);
    new_cache.update(4, get_page);
    sum_of_errors += new_cache.check_context(second_context);
    new_cache.update(9, get_page);
    sum_of_errors += new_cache.check_context(third_context);

    BOOST_REQUIRE_EQUAL(sum_of_errors, OK);
}

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

    BOOST_REQUIRE_EQUAL(hit_count, 4801903);
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

