#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  // uint64_t pushed_count;
  // if (first_index == next_byte_index_){   // substring 是 buffered data 的下一个字节，直接写入缓冲流中
  //   pushed_count = std::min(data.size(),output_.writer().available_capacity());
  //   output_.writer().push(data);
  //   next_byte_index_ += pushed_count;
  //   first_unacceptable_index_ = next_byte_index_ + output_.writer().available_capacity();
  // }else if (first_index < next_byte_index_) 
  // {
  //   if (first_index+data.size() > next_byte_index_){ // substring 和 buffered data 部分重叠，重叠部分丢弃，未重叠部分写入
  //     pushed_count = std::min(first_index+data.size()-next_byte_index_, output_.writer().available_capacity());
  //     string pushed = data.substr(next_byte_index_-first_index, pushed_count);
  //     next_byte_index_ += pushed_count;
  //     first_unacceptable_index_ = next_byte_index_ + output_.writer().available_capacity();
  //   }else{
  //     if(first_index < first_unacceptable_index_){ // substring 部分在可容纳范围内，缓存可容纳范围内的，丢弃超出的部分
  //       //TODO 1.: storge
  //       string to_storged = data.substr(0,std::min(first_unacceptable_index_ - first_index - 1,data.size() - 1));
  //       storge_count_ += to_storged.size();
  //       buffer[first_index] = to_storged;
  //     }
  //   }
  // }
  // substring 已存在，丢弃
  if (buffer.find(first_index) != buffer.end()){
    return;
  }
  
  if (is_last_substring){ //标记字节流结束字节
    end_byte_index = first_index + data.size();
    is_eof = true;
  }
  // 在 Reassembler 可容纳范围内的 substring 保存
  string to_storged;
  if (first_index <= next_byte_index_){
    to_storged = data.substr(next_byte_index_ - first_index,std::min(data.size(),first_unacceptable_index_ - first_index));
    buffer[next_byte_index_] = to_storged;
  }else if (first_index < first_unacceptable_index_)
  {
    to_storged = data.substr(0,std::min(first_unacceptable_index_ - first_index,data.size()));
    buffer[first_index] = to_storged;
  }
  storge_count_ += to_storged.size();
  
  //把可写入的 substring 写入 ByteStream 
  while (true)
  {
    if( is_eof && next_byte_index_ >= end_byte_index){
      output_.writer().close();
    }
    auto search = buffer.find(next_byte_index_);
    if(search == buffer.end()){
      break;
    }
    uint64_t pushed_count = std::min(output_.writer().available_capacity(),buffer[next_byte_index_].size()); 
    output_.writer().push(buffer[next_byte_index_]);
    storge_count_ -= buffer[next_byte_index_].size();
    buffer.erase(next_byte_index_);
    next_byte_index_ +=  pushed_count;
    first_unacceptable_index_ = next_byte_index_ + output_.writer().available_capacity();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return storge_count_;
}
