#include "reassembler.hh"

void ReassBuff::vector_insert(uint64_t first, uint64_t end){
    if(block_index.empty()){
        block_index[0] = first;
        block_index[1] = end;
        return;
    }
    bool flag = true;
    uint64_t i;
    for(i = 0; i < block_index.size(); i+=2){
        if(first < block_index[i]){
            flag = false;
            //Done: 后面的元素向后移动两个位置，插入
            for(uint64_t j = block_index.size() - 1; j >= i; j --){
                block_index[j+2] = j;
            }
            block_index[i] = first;
            block_index[i+1] = end;
            break;
        }
    }
    if (flag){
        //Done: 在末尾插入
        block_index[i] = first;
        block_index[i+1] = end;
    }
    // Done:从后往前走， 检查、合并块
    for(uint64_t j = block_index.size() - 3; j < block_index.size(); j -= 2){ //j<0,溢出导致 j >= block_index.size()
        if(block_index[j] == block_index[j+1]){
            for(uint64_t k = j; k <= block_index.size() - 2; k++ ){
                block_index[k] = block_index[k+2];
            }
            block_index.resize(block_index.size()-2);
        }
    }
}

void ReassBuff::vector_erase(uint64_t first){
    bool flag = true;
    for(uint64_t i = 0; i < block_index.size()-2; i+=2){
        flag = false;
        if(block_index[i] == first){
            for(uint64_t j = i; j < block_index.size()-2; j++){
                block_index[j] = block_index[j+1];
            }
            break;
        }
    }
    if(flag){
        std::cerr << "Exeception in ReassBuff::vector_erase, don't find index " << std::endl;
    }
}

void ReassBuff::insert(uint64_t first_index, std::string to_storged_data){
    if (buffer.empty()){
        buffer[first_index] = to_storged_data;
        //Done 更新块索引
        vector_insert(first_index, first_index + to_storged_data.size());
        storge_count_ += to_storged_data.size();
    }

    bool flag = true;
    for(uint64_t i = 1; i < block_index.size(); i+=2){
        if (block_index[i] > first_index){ //定位到 块的尾索引 > to_storged_data的首索引  的位置
            flag = false;
            //Done: 得到Front_data,back_data,back_data再和后块比较，插入
            // unsigned -> signed
            int64_t front_bytes = block_index[i-1] - first_index; 
            int64_t back_bytes = first_index + to_storged_data.size() -block_index[i];

            if(front_bytes > 0){
                std::string front_data = to_storged_data.substr(0,front_bytes);
                buffer[first_index] = front_data;
                vector_insert(first_index,first_index + front_data.size());
                storge_count_ += front_data.size();
            }
            if (back_bytes >0){
                std::string back_data = to_storged_data.substr(to_storged_data.size() - back_bytes);
                uint64_t start_index = block_index[i]+1;
                // uint64_t end_index = start_index + back_data.size();
                if(i+1 < block_index.size()){
                    //usigned -> signed
                    int64_t valid_bytes = block_index[i+1] - start_index;
                    if(valid_bytes > 0){
                        std::string valid_data = back_data.substr(0,valid_bytes);
                        vector_insert(start_index,start_index+valid_data.size());
                        storge_count_ += valid_data.size();
                    }
                }
            }
        }
    }

    if (flag){
        buffer[first_index] = to_storged_data;
        storge_count_ += to_storged_data.size();
        vector_insert(first_index,first_index+to_storged_data.size());
    }
}

std::string ReassBuff::pop(uint64_t index){
    std::string data = buffer[index];
    //TODO:弹出元素 更新block_index, 更新storge_count_
    buffer.erase(index);
    storge_count_ -= data.size();
    vector_erase(index);
    return data;
}