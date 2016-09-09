#include "base64.h"

namespace base {
namespace base64 {

namespace {
    const std::string kBase64Table =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/=";
} // !namespace anonymous

std::string Encode(const std::string & str)
{
    return Encode((const unsigned char*)str.c_str(), str.length());
}

std::string Encode(const unsigned char * str, size_t len)
{
    std::string base64String;
    if (nullptr == str)
        return base64String;

    unsigned char b;
    for (size_t i = 0; i < len; i += 3)
    {
        b = (str[i] & 0xFC) >> 2;
        base64String += kBase64Table[b];
        b = (str[i] & 0x03) << 4;
        if ((i + 1) < len)
        {
            b |= (str[i + 1] & 0xF0) >> 4;
            base64String += kBase64Table[b];
            b = (str[i + 1] & 0x0F) << 2;
            if ((i + 2) < len)
            {
                b |= (str[i + 2] & 0xC0) >> 6;
                base64String += kBase64Table[b];
                b = str[i + 2] & 0x3F;
                base64String += kBase64Table[b];
            }
            else
            {
                base64String += kBase64Table[b];
                base64String += "=";
            }
        }
        else
        {
            base64String += kBase64Table[b];
            base64String += "==";
        }
    }
    return base64String;
}
std::string Decode(const std::string & str)
{
    return Decode((const unsigned char*)str.c_str(), str.length());
}
std::string Decode(const unsigned char * str, size_t len)
{
    std::string decodedString;

    if (nullptr == str || len % 4 != 0)
        return decodedString;

    unsigned char b[4];
    for (size_t i = 0; i < len; i += 4)
    {
        b[0] = (unsigned char)kBase64Table.find_first_of(str[i]);
        b[1] = (unsigned char)kBase64Table.find_first_of(str[i + 1]);
        b[2] = (unsigned char)kBase64Table.find_first_of(str[i + 2]);
        b[3] = (unsigned char)kBase64Table.find_first_of(str[i + 3]);

        if (0xFF == b[0] || 0xFF == b[1] || 0xFF == b[2] || 0xFF == b[3])
            return "";

        decodedString += (b[0] << 2) | (b[1] >> 4);
        if (b[2] < 64)
        {
            decodedString += (b[1] << 4) | (b[2] >> 2);
            if (b[3] < 64)
            {
                decodedString += (b[2] << 6) | b[3];
            }
        }
    }
    return decodedString;
}

} // !namespace base64
} // !namespace base

