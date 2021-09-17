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
    elem_with_data = 3
};

class Cache_elem{

private:

    Key_T data_key;

    page data;

    int type;

public:

    Cache_elem(page elem_data, Key_T elem_key, int elem_type);

    Cache_elem(Key_T elem_key, int elem_type); //с данными в другом месте (HIR)

    ~Cache_elem(){};

    void change_type(int new_type);

    friend bool operator ==(const Cache_elem& left_elem, const Cache_elem& right_elem);
    friend class LIRS_cache;
};

Cache_elem::Cache_elem(page elem_data, Key_T elem_key, int elem_type): data(elem_data), data_key(elem_key){

    type = elem_type;
}

Cache_elem::Cache_elem(Key_T elem_key, int elem_type): data_key(elem_key){

    type = elem_type;
}

void Cache_elem::change_type(int new_type){

    type = new_type;

    if ((new_type == HRI_resident) || (new_type == HRI_non_resident)){

        data.~page(); //вызываем деструктор для данных которые хранятся в HIR и есть информация в LIR
    }                 //эта функция вызывается для резидентов/не_резидентов HIR в стеке LIR    
}

bool operator ==(const Cache_elem& left_elem, const Cache_elem& right_elem){

    return left_elem.data_key == right_elem.data_key;
}


class LIRS_cache{

private:

    int LIR_size;
    int HIR_size;

    std::list<Cache_elem> LIR_stack;
    std::list<Cache_elem> HIR_list;

    using List_iter = typename std::list<Cache_elem>::iterator;
    std::unordered_map<Key_T, List_iter> LIR_hash_t; //надо хранить итераторы в хэш таблице 
    std::unordered_map<Key_T, List_iter> HIR_hash_t;

public:

    LIRS_cache(int size);

    ~LIRS_cache(){};

    bool update(Key_T cur_key, Cache_elem (*get_page)(Key_T));

    void handle_non_resident(Key_T cur_key, Cache_elem (*get_page)(Key_T));

    void handle_resident(Key_T cur_key);

    void handle_double_hit(Key_T cur_key);

    void move_from_LIR_to_HIR();

    bool check_LIR_bottom();

    void check_HIR_bottom();
};

LIRS_cache::LIRS_cache(int size){

    HIR_size = size / 4;
    LIR_size = size - HIR_size;
}

bool LIRS_cache::check_LIR_bottom(){

    if (LIR_stack.back().type == elem_with_data){

        return true;
    }

    while ((LIR_stack.back().type != elem_with_data) && (LIR_size != 0)){

        LIR_hash_t.erase(LIR_stack.back().data_key);
        LIR_stack.pop_back();
    }

    return false;
}

void LIRS_cache::check_HIR_bottom(){

    auto find_in_LIR = LIR_hash_t.find(HIR_list.back().data_key);

    if (find_in_LIR != LIR_hash_t.end()){

        find_in_LIR->second->change_type(HRI_non_resident);
    }
}

void LIRS_cache::move_from_LIR_to_HIR(){

    check_HIR_bottom();

    HIR_hash_t.erase(HIR_list.back().data_key);
    HIR_list.pop_back();

    LIR_hash_t.erase(LIR_stack.back().data_key);
    HIR_list.splice(HIR_list.begin(), LIR_stack, --LIR_stack.end());   //перемещение элемента 

    HIR_hash_t[HIR_list.begin()->data_key] = HIR_list.begin();

    check_LIR_bottom();
}

void LIRS_cache::handle_non_resident(Key_T cur_key, Cache_elem (*get_page)(Key_T)){

    move_from_LIR_to_HIR();
    
    LIR_stack.push_front( get_page(cur_key) );
    LIR_hash_t[cur_key] = LIR_stack.begin();
}

void LIRS_cache::handle_double_hit(Key_T cur_key){

    auto find_in_HIR = HIR_hash_t.find(cur_key);   //переместить найденный элемент из HIR в LIR

    HIR_hash_t.erase(find_in_HIR->second->data_key);
    LIR_stack.splice(LIR_stack.begin(), HIR_list, find_in_HIR->second); //поскольку эта функция вроед не меняет положение элемента хэш таблицу LIR не надо менять

    LIR_stack.begin()->change_type(elem_with_data);

    move_from_LIR_to_HIR();
}

void LIRS_cache::handle_resident(Key_T cur_key){

    auto HIR_res = HIR_hash_t.find(cur_key);

    Cache_elem tmp(*(HIR_res->second));

    HIR_list.remove(*(HIR_res->second));
    HIR_list.push_front(tmp); //можно попробовать с помощью splice 

    Cache_elem new_HIR_resident(cur_key, HRI_resident);

    LIR_stack.push_front(new_HIR_resident);
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

            Cache_elem tmp(*(find_in_LIR->second)); //можно попробовать с помощью splice 

            LIR_stack.remove(*(find_in_LIR->second));
            LIR_stack.push_front(tmp);
            return true;
        }
    }

    if ((find_in_LIR == LIR_hash_t.end()) && (find_in_HIR != HIR_hash_t.end())){ //попадание по HIR но не в LIR

        handle_resident(cur_key);
        return true;
    }

    if ((find_in_LIR != LIR_hash_t.end()) && (find_in_HIR != HIR_hash_t.end())){ //попадание по LIR и в HIR

        handle_double_hit(cur_key);
        return true;
    }


    if (HIR_list.size() < HIR_size){

        if (LIR_stack.size() < LIR_size){

            LIR_stack.push_front(get_page(cur_key));
            LIR_hash_t[cur_key] = LIR_stack.begin();
        } else{

            LIR_hash_t.erase(LIR_stack.back().data_key);
            HIR_list.splice(HIR_list.begin(), LIR_stack, --LIR_stack.end());   //перемещение элемента 

            HIR_hash_t[HIR_list.begin()->data_key] = HIR_list.begin();
        }
    } else{

        Cache_elem new_HIR_elem(get_page(cur_key));

        check_HIR_bottom();
        
        HIR_hash_t.erase(HIR_list.back().data_key);
        HIR_list.pop_back();

        HIR_list.push_front(new_HIR_elem);
        HIR_hash_t[cur_key] = HIR_list.begin();

        Cache_elem new_HIR_resident(cur_key, HRI_resident);

        LIR_stack.push_front(new_HIR_resident);
        LIR_hash_t[cur_key] = LIR_stack.begin();
    }

    return false;
}
