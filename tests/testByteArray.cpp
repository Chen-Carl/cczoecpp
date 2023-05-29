#include <iostream>
#include <string>
#include <vector>
#include "Net/ByteArray/bytearray.h"
#include "Log/Log.h"
#include "utils/macro.h"

using namespace cczoe;

net::ByteArray barray;
std::string res;

void testRWNum();
void testRWString();
void test();

int main()
{
    testRWNum();
    testRWString();
    test();
    return 0;
}

void testRWNum()
{
    std::cout << "test(1/5): test read/write numbers" << std::endl;
    std::cout << "input data: [(16)1024, (32)-1, (v32)-1]" << std::endl;
    barray.writeFint16((int16_t)1024);
    barray.writeFint32((int32_t)-1);
    barray.writeInt32((int32_t)-1);
    barray.setPosition(0);
    res = barray.toHexString();
    std::cout << res << std::endl;
    barray.clear();

    barray.writeFint16((int16_t)1024);
    barray.writeFint32((int32_t)-1);
    barray.writeInt32((int32_t)-1);
    barray.setPosition(0);
    int16_t rd16 = barray.readFint16();
    int32_t rd32 = barray.readFint32();
    int32_t rdv32 = barray.readInt32();
    std::cout << "output data: [";
    std::cout << rd16 << ", " << rd32 << ", " << rdv32 << "]" << std::endl;
    barray.clear();
}

void testRWString()
{
    std::cout << "test(2/5): test read/write string" << std::endl;
    barray.writeStringVint("cll loves zoe");
    barray.setPosition(0);
    res = barray.toHexString();
    std::cout << res << std::endl;
    res = barray.toString();
    std::cout << res << std::endl;
    barray.clear();
}

void test()
{
    std::cout << "test(3/5): test byte array int" << std::endl;
    std::vector<int16_t> vec;
    for (size_t i = 0; i < 100; i++)
    {
        vec.push_back(rand());
    }
    for (auto x : vec)
    {
        barray.writeFint16(x);
    }
    barray.setPosition(0);
    for (size_t i = 0; i < 100; i++)
    {
        int res = barray.readFint16();
        std::cout << "read data = " << res << "\traw data = " << vec[i];
        CCZOE_ASSERT(res == vec[i]);
        std::cout << "\tok" << std::endl;
    }
    barray.clear();

    std::cout << "test(4/5): test byte array varint" << std::endl;
    for (int32_t x : vec)
    {
        barray.writeInt32(x);
    }
    barray.setPosition(0);
    for (size_t i = 0; i < 100; i++)
    {
        int res = barray.readInt32();
        std::cout << "read data = " << res << "\traw data = " << vec[i];
        CCZOE_ASSERT(res == vec[i]);
        std::cout << "\tok" << std::endl;
    }
    barray.clear();
}