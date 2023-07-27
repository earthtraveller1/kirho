#include <iostream>
#include <kirho/kirho.hpp>

using kirho::result;

auto get_number(int number) -> result<int, int>
{
    if (number == 69)
    {
        return result<int, int>::success(420);
    }
    else
    {
        return result<int, int>::error(666);
    }
}

int main()
{
    int bogo;
    std::cout << get_number(18).except("hello you suck bozo llll") << std::endl;

    return 0;
}
