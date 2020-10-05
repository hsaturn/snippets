#pragma once
#include <cstdint>
#include <string>


class Crc32
{
    public:

        // (iSCI) POLY 0x82f63b78 (Zip, Ethernet=0xedb88320)
        Crc32(uint32_t poly_=0xedb88320) : poly(poly_), crc(0){};

        friend Crc32& operator << (Crc32& crc32, char c)
        {
            uint32_t& crc=crc32.crc;
            crc = ~crc;
            crc ^= c;
            for(int k=0; k<8; k++)
                crc = (crc >> 1) ^ (crc32.poly & (0 - (crc & 1)));
            crc = ~crc;
            return crc32;
        }

        friend Crc32& operator << (Crc32& crc32, const std::string s)
        {
            uint32_t& crc=crc32.crc;
            crc = ~crc;
            for(const auto& c: s)
            {
                crc ^= c;
                for(int k=0; k<8; k++)
                    crc = (crc >> 1) ^ (crc32.poly & (0 - (crc & 1)));
            }
            crc = ~crc;
            return crc32;
        }

        operator uint32_t() const { return crc; }
        
    private:
        uint32_t poly;
        uint32_t crc;
};


class Crc16
{
    public:

        friend Crc16& operator<< (Crc16& crc16, char c)
        {
            uint16_t& crc=crc16.crc;
            uint16_t x=((crc>>8)^c)&0xff;
            crc = crc<<8 ^ (x<<12) ^ (x<<5) ^ x;
            return crc16;
        }
        friend Crc16& operator << (Crc16& crc16, const std::string& s)
        {
            for(const auto& c: s) crc16 << c;
            return crc16;
        }
        uint16_t add(const char* buff, uint32_t len)
        {
            while(len--)
                (*this)<<*buff++;
            return crc;
        }
        operator uint16_t() const { return crc; }

   private:
        uint16_t crc=0;
};
