#include <stdio.h>
#include <stdlib.h>

#include <functional>
#include <list>
#include <unordered_map>

class Cache_elem;
class My_cache_hash;
class LIRS_cache;

typedef int Key_T;
typedef long page;

enum elem_types{

    HRI_resident = 1,
    HRI_non_resident = 2,
    LRI_resident = 3
};

class Cache_elem{

private:

    Key_T data_key;

    page data;

    int type;

public:

    Cache_elem(page elem_data, int elem_type);

    ~Cache_elem(){};

    void change_type(int new_type);

    int get_type();

    friend class LIRS_cache;
};


class LIRS_cache{

private:

    int LIR_size;
    int HIR_size;

    std::list<Cache_elem> LIR_stack;
    std::list<Cache_elem> HIR_list;

    using List_itter = typename std::list<Cache_elem>::iterator;
    std::unordered_map<Key_T, List_itter> LIR_hash_t; //надо хранить итераторы в хэш таблице 
    std::unordered_map<Key_T, List_itter> HIR_hash_t;

public:

    LIRS_cache(int size);

    ~LIRS_cache();

    bool update(Key_T cur_key, Cache_elem (*get_page)(Key_T));

    void handle_non_resident(Key_T cur_key, Cache_elem (*get_page)(Key_T));

    void handle_resident(Key_T cur_key);

    void move_from_LIR_to_HIR();

    bool check_LIR_bottom();
};


bool LIRS_cache::check_LIR_bottom(){

    if (LIR_stack.back().type == LRI_resident){

        return true;
    }

    while ((LIR_stack.back().type != LRI_resident) && (LIR_size != 0)){

        LIR_hash_t.erase(LIR_stack.back().data_key);
        LIR_stack.pop_back();
    }

    return false;
}

void LIRS_cache::move_from_LIR_to_HIR(){

    auto find_in_LIR = LIR_hash_t.find(HIR_list.back().data_key);

    if (find_in_LIR != LIR_hash_t.end()){

        find_in_LIR->second->change_type(HRI_non_resident);
    }

    HIR_hash_t.erase(HIR_list.back().data_key);
    HIR_list.pop_back();

    LIR_hash_t.erase(LIR_stack.back().data_key);
    HIR_list.splice(HIR_list.begin(), LIR_stack, LIR_stack.end());   //перемещение элемента 


    HIR_hash_t[HIR_list.begin()->data_key] = HIR_list.begin();

    check_LIR_bottom();
}

void LIRS_cache::handle_non_resident(Key_T cur_key, Cache_elem (*get_page)(Key_T)){

    move_from_LIR_to_HIR();
    
    LIR_stack.push_front( get_page(cur_key) );
    LIR_hash_t[cur_key] = LIR_stack.begin();
}


void LIRS_cache::handle_resident(Key_T cur_key){

    
}

bool LIRS_cache::update(Key_T cur_key, Cache_elem (*get_page)(Key_T)){

    auto find_in_LIR = LIR_hash_t.find(cur_key);
    auto find_in_HIR = HIR_hash_t.find(cur_key);

    if ((find_in_LIR != LIR_hash_t.end()) && (find_in_HIR == HIR_hash_t.end())){ //попадание по LIR но не в HIR

        if (find_in_LIR->second->type == HRI_non_resident){

            LIR_stack.remove(*(find_in_LIR->second));

            handle_non_resident(cur_key, get_page);
            return false;
        } else{

            Cache_elem tmp(*(find_in_LIR->second));

            LIR_stack.remove(*(find_in_LIR->second));
            LIR_stack.push_front(tmp);
            return true;
        }
    }

    if ((find_in_LIR == LIR_hash_t.end()) && (find_in_HIR != HIR_hash_t.end())){ //попадание по HIR но не в LIR

        handle_resident(cur_key);
        return true;
    }

    if ((find_in_LIR == LIR_hash_t.end()) && (find_in_HIR == HIR_hash_t.end())){ //попадание по LIR и в HIR

        return true;
    }


    return false;
}
