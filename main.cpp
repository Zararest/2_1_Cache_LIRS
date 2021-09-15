#include "cache.inl"

Cache_elem get_page(Key_T cur_key){

    Cache_elem tmp(cur_key, cur_key, elem_with_data);

    return tmp;
}

int main(){

    LIRS_cache new_cache(16);

    /*Cache_elem tmp_1(1, elem_with_data); 
    Cache_elem tmp_2(2, elem_with_data); 
    Cache_elem tmp_3(3, elem_with_data); 
    Cache_elem tmp_4(4, elem_with_data); 

    std::list<Cache_elem> fir_list = {tmp_1, tmp_2};
    std::list<Cache_elem> sec_list = {tmp_3, tmp_4};

    std::list<int> fir_list = {1, 2};
    std::list<int> sec_list = {3, 4};

    fir_list.splice(fir_list.begin(), sec_list, --sec_list.end());*/

    for (int i = 0; i < 20; i++){

        new_cache.update(i, get_page);
    }
    
    return 0;
}