#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>

#include <functional>
#include <list>
#include <unordered_map>

#define STACKS_SIZE_RATIO 2

template <typename T_data, typename T_key>
class Cache_elem;

template <typename T_data, typename T_key>
class My_cache_hash;

template <typename T_data, typename T_key>
class LIRS_cache;


enum elem_types{

    HIR_resident = 1,
    HIR_non_resident = 2,
    elem_with_data = 3
};

template <typename T_data, typename T_key> class Cache_elem{

private:

    T_key data_key;

    T_data data;

    int type;

public:

    Cache_elem(T_data elem_data, T_key elem_key, int elem_type);

    Cache_elem(T_key elem_key, int elem_type);

    void change_type(int new_type);

    T_key get_key() const{

        return data_key;
    }

    int get_type() const{

        return type;
    }

    friend bool operator ==(const Cache_elem<T_data, T_key>& left_elem, const Cache_elem<T_data, T_key>& right_elem){

        return left_elem.data_key == right_elem.data_key;
    }
};

template <typename T_data, typename T_key>
Cache_elem<T_data ,T_key>::Cache_elem(T_data elem_data, T_key elem_key, int elem_type): data(elem_data), data_key(elem_key){

    type = elem_type;
}

template <typename T_data, typename T_key>
Cache_elem<T_data, T_key>::Cache_elem(T_key elem_key, int elem_type): data_key(elem_key){

    type = elem_type;
}

void delete_data(const void* data_ptr, int data_size){

    for (int i = 0; i < data_size; i++){

        ((char*)data_ptr)[i] = 0;
    }
}

template <typename T_data, typename T_key>
void Cache_elem<T_data, T_key>::change_type(int new_type){

    type = new_type;

    if ((new_type == HIR_resident) || (new_type == HIR_non_resident)){
        
        delete_data(&data, sizeof(T_data));
    }                     
}


template <typename T_data, typename T_key> class LIRS_cache{

protected:

    int LIR_size;
    int HIR_size;

    std::list<Cache_elem<T_data, T_key>> LIR_stack;
    std::list<Cache_elem<T_data, T_key>> HIR_list;

    using List_iter = typename std::list<Cache_elem<T_data, T_key>>::iterator;
    std::unordered_map<T_key, List_iter> LIR_hash_t; 
    std::unordered_map<T_key, List_iter> HIR_hash_t;

    void handle_non_resident(T_key cur_key, T_data(*get_page)(T_key));

    void handle_resident(T_key cur_key);

    void handle_double_hit(T_key cur_key);
    
    void handle_miss(T_key cur_key, T_data(*get_page)(T_key));
    
    bool handle_if_cache_isnt_full(T_key cur_key, T_data(*get_page)(T_key));

    void move_from_LIR_to_HIR();

    bool check_LIR_bottom();

    bool check_HIR_bottom();

public:

    LIRS_cache(int size);

    bool update(T_key cur_key, T_data(*get_page)(T_key)); 
};

template <typename T_data, typename T_key>
LIRS_cache<T_data, T_key>::LIRS_cache(int size){

    if (size < 2){

        size = 2;
    }

    HIR_size = size / STACKS_SIZE_RATIO;
    LIR_size = size - HIR_size;
}

template <typename T_data, typename T_key>
bool LIRS_cache<T_data, T_key>::check_LIR_bottom(){

    if (LIR_stack.back().get_type() == elem_with_data){

        return true;
    }

    while ((LIR_stack.back().get_type() != elem_with_data) && (LIR_stack.size() != 0)){

        LIR_hash_t.erase(LIR_stack.back().get_key());
        LIR_stack.pop_back();
    }
    
    assert((LIR_stack.back().get_type() == elem_with_data) || (LIR_stack.size() == 0));
    return false;
}

template <typename T_data, typename T_key>
bool LIRS_cache<T_data, T_key>::check_HIR_bottom(){

    auto find_in_LIR = LIR_hash_t.find(HIR_list.back().get_key());

    if (find_in_LIR != LIR_hash_t.end()){

        find_in_LIR->second->change_type(HIR_non_resident);

        return true;
    }

    return false;
}

template <typename T_data, typename T_key>
void LIRS_cache<T_data, T_key>::move_from_LIR_to_HIR(){

    LIR_hash_t.erase(LIR_stack.back().get_key());
    HIR_list.splice(HIR_list.begin(), LIR_stack, --LIR_stack.end());

    HIR_hash_t[HIR_list.begin()->get_key()] = HIR_list.begin();

    check_LIR_bottom();
}

template <typename T_data, typename T_key> 
void LIRS_cache<T_data, T_key>::handle_non_resident(T_key cur_key, T_data (*get_page)(T_key)){

    move_from_LIR_to_HIR();

    assert(HIR_list.size() > 0);

    check_HIR_bottom();
    HIR_hash_t.erase(HIR_list.back().get_key());
    HIR_list.pop_back();
    
    Cache_elem<T_data, T_key> tmp(get_page(cur_key), cur_key, elem_with_data);
    LIR_stack.push_front(tmp);
    LIR_hash_t[cur_key] = LIR_stack.begin();
}

template <typename T_data, typename T_key>
void LIRS_cache<T_data, T_key>::handle_double_hit(T_key cur_key){

    auto find_in_HIR = HIR_hash_t.find(cur_key); 
    auto find_in_LIR = LIR_hash_t.find(cur_key);
    assert(find_in_HIR->second != HIR_list.end());
    assert(find_in_LIR->second->get_type() == HIR_resident);

    HIR_hash_t.erase(find_in_HIR->second->get_key()); 
    LIR_hash_t.erase(find_in_LIR->second->get_key());
    LIR_stack.erase(find_in_LIR->second);

    LIR_stack.splice(LIR_stack.begin(), HIR_list, find_in_HIR->second);
    LIR_hash_t[cur_key] = LIR_stack.begin();

    move_from_LIR_to_HIR();
}

template <typename T_data, typename T_key>
void LIRS_cache<T_data, T_key>::handle_resident(T_key cur_key){

    auto HIR_res = HIR_hash_t.find(cur_key);

    if (HIR_res->second != HIR_list.begin()){

        HIR_list.splice(HIR_list.begin(), HIR_list, HIR_res->second);
    }

    Cache_elem<T_data, T_key> new_HIR_resident(cur_key, HIR_resident);

    LIR_stack.push_front(new_HIR_resident);
    LIR_hash_t[LIR_stack.begin()->get_key()] = LIR_stack.begin();
}

template <typename T_data, typename T_key>
void LIRS_cache<T_data, T_key>::handle_miss(T_key cur_key, T_data(*get_page)(T_key)){

    Cache_elem<T_data, T_key> new_HIR_elem(get_page(cur_key), cur_key, elem_with_data);

    check_HIR_bottom();

    HIR_list.push_front(new_HIR_elem);
    HIR_hash_t[cur_key] = HIR_list.begin();

    HIR_hash_t.erase(HIR_list.back().get_key());
    HIR_list.pop_back();

    Cache_elem<T_data, T_key> new_HIR_resident(cur_key, HIR_resident);

    LIR_stack.push_front(new_HIR_resident);
    LIR_hash_t[cur_key] = LIR_stack.begin();
}

template <typename T_data, typename T_key>
bool LIRS_cache<T_data, T_key>::handle_if_cache_isnt_full(T_key cur_key, T_data(*get_page)(T_key)){

    if ((HIR_list.size() < HIR_size) || (LIR_stack.size() < LIR_size)){

        if (LIR_stack.size() < LIR_size){

            Cache_elem<T_data, T_key> tmp(get_page(cur_key), cur_key, elem_with_data);
            LIR_stack.push_front(tmp);
            LIR_hash_t[cur_key] = LIR_stack.begin();
        } else{

            LIR_hash_t.erase(LIR_stack.back().get_key());
            HIR_list.splice(HIR_list.begin(), LIR_stack, --LIR_stack.end()); 

            HIR_hash_t[HIR_list.begin()->get_key()] = HIR_list.begin();
        }
    } else{

        return false;
    }

    return true;
}

template <typename T_data, typename T_key>
bool LIRS_cache<T_data, T_key>::update(T_key cur_key, T_data (*get_page)(T_key)){

    auto find_in_LIR = LIR_hash_t.find(cur_key);
    auto find_in_HIR = HIR_hash_t.find(cur_key);
    
    if ((find_in_LIR != LIR_hash_t.end()) && (find_in_HIR == HIR_hash_t.end())){

        if (find_in_LIR->second->get_type() == HIR_non_resident){

            LIR_hash_t.erase(cur_key);
            LIR_stack.erase(find_in_LIR->second);

            handle_non_resident(cur_key, get_page);

            return false;
        } else{

            if (find_in_LIR->second != LIR_stack.begin()){

                LIR_stack.splice(LIR_stack.begin(), LIR_stack, find_in_LIR->second);

                check_LIR_bottom();
            }
            return true;
        }
    }

    if ((find_in_LIR == LIR_hash_t.end()) && (find_in_HIR != HIR_hash_t.end())){ 

        handle_resident(cur_key);
        return true;
    }

    if ((find_in_LIR != LIR_hash_t.end()) && (find_in_HIR != HIR_hash_t.end())){

        handle_double_hit(cur_key);
        return true;
    }

    if (handle_if_cache_isnt_full(cur_key, get_page) == false){
        
        handle_miss(cur_key, get_page);
    }

    return false;
}
