#include <iostream>
#include <boost/cstdint.hpp>

int main()
{
    const char* a = "晕";
    unsigned wchar_t c = L'晕';
    const int8_t* b = reinterpret_cast<const int8_t*>(a);
    std::cout << a << std::endl
        << c << std::endl
        << (b[0] * 256 + b[1]) << std::endl
        << static_cast<int>(c) << std::endl
        << static_cast<wchar_t>(26197) << std::endl;
    std::wcout << c << std::endl
        << sizeof(wchar_t) << std::endl;

    std::cout << static_cast<wchar_t>(a[0]) << std::endl;
}

