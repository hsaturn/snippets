#include <iostream>
#include <map>
#include <vector>
#include <iomanip>
#include "crc.h"
#include <stdlib.h>

using namespace std;

int main(int argc, const char* argv[])
{
    string hello="Hello world";

    Crc32   crc;
    Crc16   crc16;
    crc << hello;
    crc16 << hello;

    cout << "crc=" << hex << showbase << crc << endl;
    cout << "crc16=" << hex << showbase << crc16 << endl;
    cout << "-------------------" << endl;

    unsigned long size=10000000;
    char* dummy=(char*)malloc(size);
    for(unsigned long z=0; z<size; z++)
    {
        dummy[z]=z;
    }
    Crc16 check;
    check.add(dummy, size);
    cout << "crc16=" << hex << showbase << check << endl;
    cout << "malloc=" << (long)dummy << endl;

	return 0;
}


