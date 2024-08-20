#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  // (void)n;
  // (void)zero_point;
  // 借助类型转换做到: seqno = abso_sea + zero_point % uint32_t max
  Wrap32 seqno = zero_point + n;
  return seqno;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  // (void)zero_point;
  // (void)checkpoint;
  (void)zero_point;
  // uint32_t uint32_max = ( ( 1UL << 32 ) -1 );
  // uint32_t uint32_num = checkpoint / uint32_max; // num of uint32 
  // uint32_t map_pos = checkpoint % (static_cast<uint64_t>(uint32_max)+1);
  // uint32_t distance = this->raw_value_ - map_pos;

  // uint64_t abso_seqno = static_cast<uint64_t>(distance) +  static_cast<uint32_t>(uint32_num)* static_cast<uint64_t>(uint32_max); 
  // return abso_seqno;

  // uint64_t uint32_MAX = ( ( 1UL << 32 ) - 1 );
  // uint64_t num = checkpoint / uint32_MAX;
  // uint32_t map_pos = checkpoint % ( uint32_MAX + 1 );
  // uint64_t distance;
  // uint64_t abs_seqno;
  // distance = this->raw_value_ - map_pos;
  // abs_seqno = num * uint32_MAX + distance;

  // return abs_seqno;

  uint64_t int32 = 1UL << 32; // 2^32
  uint64_t abs_seqno;
  uint64_t map_pos = Wrap32::wrap(checkpoint,zero_point).raw_value_;
  uint32_t distance = this->raw_value_ - map_pos;
  if(distance <= (int32 >> 1) || checkpoint + distance < int32){
    abs_seqno = checkpoint + distance;
  }else{
    abs_seqno = checkpoint + distance - int32;
  }
  return abs_seqno;
}
