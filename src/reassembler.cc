#include "reassembler.hh"
#include <iostream>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{

  // Your code here.
  first_unacceptable_index_ = next_assembled_index_ + output_.writer().available_capacity();
  if(is_last_substring){
    end_byte_index_ = first_index + data.size();
    is_get_end_ = true;
  }
  if(is_get_end_ && next_assembled_index_ >= end_byte_index_){
    output_.writer().close();
    return;
  }

  if(first_index >= first_unacceptable_index_ || first_index + data.size() <= next_assembled_index_){ // data 不在接收窗口的范围内
    return;
  }
  
  // 获取在接收窗口内的数据
  uint64_t offset = 0;
  if (next_assembled_index_ > first_index){
    offset = next_assembled_index_ - first_index;
  }
  uint64_t start_index = first_index + offset;
  uint64_t count = std::min(first_unacceptable_index_ - start_index , data.size()-offset);//Done: data.size() - offset
  string to_storge_data = data.substr(offset,count);

  uint64_t to_storge_data_end = start_index + to_storge_data.size();

  auto last_iter = buffer_.lower_bound(to_storge_data_end); //找到一个块，这个块的首索引大于等于要插入的数据的尾部
  if(last_iter != buffer_.begin()){ // buffer 不为空, 对可能出现的重叠情况判断，处理
      std::map<uint64_t,std::string>::iterator pos = last_iter;
      std::map<uint64_t,std::string>::iterator first_iter = last_iter;
      do{ // 从后向前步走，记录待擦除的元素，得到可以插入的数据
        pos --;
        uint64_t end = pos->first + pos->second.size();
        if(pos->first >= start_index){
          if(end <= to_storge_data_end){ // 存储的数据块被包含，需要擦除
            first_iter = pos;
            storge_count_ -= pos->second.size();
            continue;
          }else{                      //  要插入的数据是前半段,有重叠可能，还需再比较  
            to_storge_data = to_storge_data.substr(0,pos->first - start_index);
            to_storge_data_end = start_index + to_storge_data.size();
            last_iter = pos; // 当前块在要插入的数据后，擦除范围需要更新
            first_iter = pos;
            continue;
          }
        }else{                       
          if(end <= to_storge_data_end){ // 要插入的数据是后半段
            uint64_t offs = 0;
            if(end > start_index){
              offs = end-start_index;
            }
            to_storge_data = to_storge_data.substr(offs);
            start_index += offs;
            break;
          }else{ // //要插入的数据被包含, 无需插入
          to_storge_data = "";
          break;
          }
        }
      }while(pos != buffer_.begin());
      buffer_.erase(first_iter,last_iter);
  }

  if (to_storge_data != ""){
    buffer_[start_index] = to_storge_data;
    storge_count_ += to_storge_data.size();
  }

  // 可写入的写入ByteStream
  while(buffer_.find(next_assembled_index_) != buffer_.end()){
    string temp = buffer_[next_assembled_index_];
    output_.writer().push(temp);

    storge_count_ -= temp.size();
    buffer_.erase(next_assembled_index_);
    next_assembled_index_ += temp.size();

    if(is_get_end_ && next_assembled_index_ >= end_byte_index_){
      output_.writer().close();
    }
  }

}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return storge_count_;
}
