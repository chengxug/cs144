#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <iostream>

class ReassBuff{
private:
    std::map<uint64_t,std::string> buffer{};
    std::vector<uint64_t> block_index{};
    uint64_t storge_count_ {0};
    void vector_insert(uint64_t first, uint64_t end);
    void vector_erase(uint64_t first);
public:
    explicit ReassBuff(){};
    void insert(uint64_t first_index,std::string to_storged_data);
    uint64_t size() const{return storge_count_;}
    bool find(uint64_t index){return buffer.find(index) != buffer.end();}
    std::string pop(uint64_t index);
};
