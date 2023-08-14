#include <cassert>

#include <kirho/kirho.hpp>

using kirho::result_t;

auto get_number(int number) -> result_t<int, int>
{
    if (number == 69)
    {
        return result_t<int, int>::success(420);
    }
    else
    {
        return result_t<int, int>::error(666);
    }
}

int main()
{
    const auto result = get_number(69).except("hello you suck bozo llll");
    assert(result == 420);

    return 0;
}
