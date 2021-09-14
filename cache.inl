#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <unordered_map>

typedef int Key_T;
typedef long page;

enum elem_types{

    HRI_resident = 1,
    HRI_non_resident = 2,
    LRI_resident = 3
};


class My_cache_hash{

public:

    Key_T operator()(const Cache_elem& elem) const{

        return elem.data_key;
    }
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

    friend class My_cache_hash;
    friend class LIRS_cache;
};


class LIRS_cache{

private:

    int LIR_size;
    int HIR_size;

    std::list<Cache_elem> LIR_stack;
    std::list<Cache_elem> HIR_list;

    std::unordered_map<Key_T, Cache_elem, My_cache_hash> LIR_hash_t;
    std::unordered_map<Key_T, Cache_elem, My_cache_hash> HIR_hash_t;

public:

    LIRS_cache(int size);

    ~LIRS_cache();

    bool update(Key_T cur_key, Cache_elem (*get_page(Key_T)));

    void handle_non_resident(Key_T cur_key, Cache_elem (*get_page(Key_T)));

    void move_from_LIR_to_HIR();
};


void LIRS_cache::move_from_LIR_to_HIR(){

    auto find_in_LIR = LIR_hash_t.find(HIR_list.back().data_key);

    if (find_in_LIR != LIR_hash_t.end()){

        find_in_LIR.second.change_type(HRI_non_resident);
    }

    HIR_hash_t.erase(HIR_list.back());
    HIR_list.pop_back();

    LIR_hash_t.erase(LIR_stack.back());
    HIR_list.splice(HIR_list.front(), LIR_stack, LIR_stack.back());

    HIR_hash_t[HIR_list.front().data_key] = HIR_list.front();
}

void LIRS_cache::handle_non_resident(Key_T cur_key, Cache_elem (*get_page(Key_T))){

    move_from_LIR_to_HIR();
    
    LIR_stack.push_front(get_page(cur_key));
    LIR_hash_t[cur_key] = LIR_stack.begin();
}


bool LIRS_cache::update(Key_T cur_key, Cache_elem (*get_page(Key_T))){

    auto find_in_LIR = LIR_hash_t.find(cur_key);
    auto find_in_HIR = HIR_hash_t.find(cur_key);

    if ((find_in_LIR != LIR_hash_t.end()) && (find_in_HIR == HIR_hash_t.end())){ //попадание по LIR но не в HIR

        if (find_in_LIR->second.type == HRI_non_resident){

            LIR_stack.remove(find_in_LIR->second);

            handle_non_resident(cur_key, get_page);
            return false;
        } else{

            Cache_elem tmp(find_in_LIR->second);

            LIR_stack.remove(find_in_LIR->second);
            LIR_stack.push_front(tmp);
            return true;
        }
    }

    if ((find_in_LIR == LIR_hash_t.end()) && (find_in_HIR != HIR_hash_t.end())){ //попадание по HIR но не в LIR

        return true;
    }

    if ((find_in_LIR == LIR_hash_t.end()) && (find_in_HIR == HIR_hash_t.end())){ //попадание по LIR и в HIR

        return true;
    }


    return false;
}
