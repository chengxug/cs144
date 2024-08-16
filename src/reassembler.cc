#include "reassembler.hh"

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

  if(first_index > first_unacceptable_index_ || first_index + data.size() < next_assembled_index_){ // data 不在接收窗口的范围内
  }
  
  // 获取在接收窗口内的数据
  uint64_t offset = 0;
  if (next_assembled_index_ > first_index){
    offset = next_assembled_index_ - first_index;
  }
  uint64_t start_index = first_index + offset;
  uint64_t count = std::min(first_unacceptable_index_ - start_index , data.size());
  string to_storge_data = data.substr(offset,count);

  uint64_t to_storge_data_end = start_index + to_storge_data.size();

  if (!buffer_.empty()){// buffer 不为空 需要处理可能重叠的情况，否则直接插入，
    auto last_iter = buffer_.lower_bound(to_storge_data_end); //找到一个块，这个块的首索引大于等于要插入的数据的尾部
    string insert_data;

    if (last_iter == buffer_.begin()){
      insert_data = to_storge_data;
    }else{
      std::map<uint64_t,std::string>::iterator pos = last_iter;
      std::map<uint64_t,std::string>::iterator first_iter;
      do{ // 从后向前步走，记录待擦除的元素，得到可以插入的数据
        pos--;
        if(pos->first >= start_index){ // 记录待擦除的元素, 更新 storge_count_
          first_iter = pos;
          storge_count_ -= pos->second.size();
        }
        uint64_t end = pos->first + pos->second.size();
        if( end <= start_index){ // 全部插入
          insert_data = to_storge_data;
        }else{ // 数据需截断, 更新start_index
          insert_data = to_storge_data.substr(end-start_index);
          start_index = end;
        }
        break;
      }while(pos != buffer_.begin());
      buffer_.erase(first_iter,last_iter);
    }
    buffer_[start_index] = insert_data;
  }else{
    buffer_[start_index] = to_storge_data;
    storge_count_ += to_storge_data.size();
  }

  while(buffer_.find(next_assembled_index_) != buffer_.end()){
    output_.writer().push(buffer_[next_assembled_index_]);
    storge_count_ -= buffer_[next_assembled_index_].size();
    next_assembled_index_ += buffer_[next_assembled_index_].size();
    buffer_.erase(next_assembled_index_);
    if(is_get_end_ && next_assembled_index_ >= first_unacceptable_index_){
      output_.writer().close();
    }
  }
  
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return storge_count_;
}
