#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  // (void)message;
  if (RST_){
    return;
  }

  if ( message.RST ){
    reassembler_.reader().set_error();
    RST_ = reassembler_.reader().has_error();
    return;
  }

  if (message.SYN){ // LISTEN -> SYN_RECV
    ISN_ = message.seqno;
    is_get_ISN_ = true;
  }

  if(is_get_ISN_){ // SYN_RECV 状态
    check_point_ = reassembler_.writer().bytes_pushed(); // byte pushed == next_assembled_index
    uint64_t abso_seqno = message.seqno.unwrap(ISN_,check_point_);

    //SYN 携带的PayLoad 无法处理
    // if (abso_seqno != 0){ 
    //   uint64_t stream_index = abso_seqno -1;
    //   reassembler_.insert(stream_index,message.payload,message.FIN);
    // }

    uint64_t stream_index = abso_seqno + message.SYN - 1; // SYN 携带的 Payload 也可以处理
    reassembler_.insert(stream_index,message.payload,message.FIN);

    // SYN_RECV: abso_ackno = first_assembled_index + 1;  FIN_RECV: abso_ackno = first_assembled_indx + 2;
    // 这里的实现本来是 first_assembled + 1 + message.FIN，但是这样对FIN到到达，但FIN前的数据还未到达的情况不能正确处理，开始的修改思路是再记录一下FIN到达时的数据的索引，链接老哥的思路点醒了我，这个已经在reassembler实现了，当插入FIN后，流就关闭了，可以用流关闭的状态替代这个1
    uint64_t abso_ackno = reassembler_.writer().bytes_pushed() + 1 + reassembler_.writer().is_closed(); 
    ackno_ = Wrap32::wrap(abso_ackno,ISN_);
  }
  
  return;
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage message;
  if (is_get_ISN_){ // 没有收到SYN时，ackno 无效
    message.ackno = ackno_;
  }
  message.RST = reassembler_.reader().has_error();
  if (reassembler_.writer().available_capacity() > 65535){
    message.window_size = 65535;
  }else{
    message.window_size = reassembler_.writer().available_capacity();
  }
  return message;
}
