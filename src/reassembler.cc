#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  first_unaccpetable_index = next_byte_index + output_.writer().available_capacity();
  if(first_index+data.size() < next_byte_index || first_index > first_unaccpetable_index){ //超出了 unreassembler 的范围
    return;
  }
  is_get_end = is_last_substring;
  buffer_.insert(first_index,data);
  while(true){
    if( !buffer_.find(next_byte_index)){
      break;
    }
    output_.writer().push(buffer_.pop(next_byte_index));
  }
  if(is_get_end && next_byte_index >= end_byte_index){
    output_.writer().close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return buffer_.size();
}
