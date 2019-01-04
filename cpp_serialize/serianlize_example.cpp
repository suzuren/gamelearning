
#include "serialize.hpp"
#include "buffer.hpp"
#include "buffer_view.hpp"
#include "buffer_writer.hpp"

struct MyMessage2
{
    int a;
    int b;
    bool operator ==(const MyMessage2& other)
    {
        return a == other.a && b == other.b;
    }
};
DECLAR_META(MyMessage2, 2);
struct MyMessage
{
    int a;
    int b;
    std::vector<std::string> c;
    MyMessage2 d;
    bool operator ==(const MyMessage& other)
    {
        return a == other.a && b == other.b && c == other.c && d == other.d;
    }
};
DECLAR_META(MyMessage, 4);


int main(int argc, char*argv[])
{
    moon::serialize ser;
    moon::buffer buf;
    MyMessage mm = { 101,102,{"hello","world","who","are","you"},{201,202} };
    moon::buffer_writer bw{ buf };
    ser.encode(bw, mm);
    moon::buffer_view bv(buf.data(), buf.size());
    auto msg = ser.decode< MyMessage>(bv);
    assert(msg == mm);
    return 0;
}