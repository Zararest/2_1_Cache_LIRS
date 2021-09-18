#include <stdio.h>
#include <stdlib.h>
#include <cassert>

#include <functional>
#include <list>
#include <unordered_map>
#include <vector>

template <typename T_data, typename T_key>
class Perfect_cache_elem;

template <typename T_data, typename T_key>
class Perfect_cache;


template <typename T_data, typename T_key> class Perfect_cache_elem{

private:

    int number_in_all_data;

    T_data data;

    int dist_to_next_elem;

public:

    Perfect_cache_elem(T_data elem_data, int number_of_elem, int distance);    

    ~Perfect_cache_elem(){}

    void decrease_dist();

    friend bool operator ==(const Perfect_cache_elem<T_data, T_key>& left_elem, const Perfect_cache_elem<T_data, T_key>& right_elem){

        return left_elem.number_in_all_data == right_elem.number_in_all_data;
    }

    friend bool operator <(const Perfect_cache_elem<T_data, T_key>& left_elem, const Perfect_cache_elem<T_data, T_key>& right_elem){

        return left_elem.dist_to_next_elem < right_elem.dist_to_next_elem;
    }

    friend class Perfect_cache<T_data, T_key>;
};

template <typename T_data, typename T_key>
Perfect_cache_elem<T_data, T_key>::Perfect_cache_elem(T_data elem_data, int number_of_elem, int distance): data(elem_data){

    number_in_all_data = number_of_elem;
    dist_to_next_elem = distance;
}

template <typename T_data, typename T_key>
void Perfect_cache_elem<T_data, T_key>::decrease_dist(){

    dist_to_next_elem--;

    assert(dist_to_next_elem >= 0);
}


template <typename T_data, typename T_key> class Perfect_cache{

private: 

    int size;

    const std::vector<T_key>& all_data;

    std::list<Perfect_cache_elem<T_data, T_key>> cache_list;

    using List_iter = typename std::list<Perfect_cache_elem<T_data, T_key>>::iterator;
    std::unordered_map<T_key, List_iter> cache_hash_t;

public:

    Perfect_cache(const std::vector<T_key>& new_data, int new_size);

    ~Perfect_cache(){}

    int check_distance(int elem_number);

    void add_list_elem(T_data cur_page, int cur_page_number, int cur_dist);

    bool update(int page_number, T_data (*get_page)(T_key));

    int get_hit_count(T_data (*get_page)(T_key));

    void decrease_all_dist();
};

template <typename T_data, typename T_key>
Perfect_cache<T_data, T_key>::Perfect_cache(const std::vector<T_key>& new_data, int new_size): all_data(new_data){

    size = new_size;
}

template <typename T_data, typename T_key>
int Perfect_cache<T_data, T_key>::get_hit_count(T_data (*get_page)(T_key)){

    int hit_counter = 0;

    for (int i = 0; i < all_data.size(); i++){

        hit_counter += update(i, get_page);
    }

    return hit_counter;
}

template <typename T_data, typename T_key>
int Perfect_cache<T_data, T_key>::check_distance(int elem_number){

    T_key cur_key = all_data[elem_number];
    int counter = 1;
    
    while ((cur_key != all_data[elem_number + counter]) && (elem_number + counter < all_data.size())){

        counter++;
    }

    return counter;
}

template <typename T_data, typename T_key>
void Perfect_cache<T_data, T_key>::add_list_elem(T_data cur_elem_data, int cur_page_number, int cur_dist){//надо переделать get_page тк она должна возвращать только страницу

    Perfect_cache_elem<T_data, T_key> new_elem(cur_elem_data, cur_page_number, cur_dist);
    cache_list.push_back(new_elem);

    cache_hash_t[all_data[cur_page_number]] = --cache_list.end();

    cache_list.sort();

    decrease_all_dist();
}   

template <typename T_data, typename T_key>
void Perfect_cache<T_data, T_key>::decrease_all_dist(){

    typename std::list<Perfect_cache_elem<T_data, T_key>>::iterator list_itter = cache_list.begin();
    int i = 0;
    while (list_itter != cache_list.end()){

        list_itter->decrease_dist();
        ++list_itter;
        i++;
    }
}

template <typename T_data, typename T_key>
bool Perfect_cache<T_data, T_key>::update(int page_number, T_data (*get_page)(T_key)){

    auto cur_iter =  cache_hash_t.find(all_data[page_number]);

    if (cur_iter == cache_hash_t.end()){

        if (cache_list.size() == size){

            if (check_distance(page_number) < cache_list.front().dist_to_next_elem ){ //сравниваем с самым далеким элементом(тут надо с -1 подумать)

                T_key list_back_key = all_data[cache_list.front().number_in_all_data];
                cache_hash_t.erase(list_back_key);
                cache_list.pop_front();
                
                int cur_dist = check_distance(page_number);

                add_list_elem(get_page(all_data[page_number]), page_number, check_distance(page_number));
            }
        } else{

            add_list_elem(get_page(all_data[page_number]), page_number, check_distance(page_number));
        }

        return false;
    } else{
        
        cur_iter->second->dist_to_next_elem = check_distance(page_number) + 1;
    }

    decrease_all_dist();
    
    return true;
}

