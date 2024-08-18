#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return is_end_;
}

void Writer::push( string data )
{
  // Your code here.
  uint64_t to_add_data_size = std::min( data.size(), capacity_ - buffer.size() );

  for ( uint64_t i = 0; i < to_add_data_size; i++ ) {
    buffer.push_front( data[i] );
  }

  write_count_ += to_add_data_size;
  return;
}

void Writer::close()
{
  // Your code here.
  is_end_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - buffer.size();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return write_count_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return is_end_ && !buffer.size();
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return read_count_;
}

string_view Reader::peek() const
{
  // Your code here.
  if ( buffer.empty() ) {
    return std::string_view {};
  }
  return std::string_view( &buffer.back(), 1 );
}

void Reader::pop( uint64_t len )
{
  if ( buffer.size() >= len ) {
    read_count_ += len;
    buffer.erase( buffer.end() - len, buffer.end() );
  } else {
    throw exception();
  }
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffer.size();
}
