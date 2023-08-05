#include <cassert>

#include <kirho/kirho.hpp>

auto main() -> int
{
    const auto error_value = kirho::result<int, const char*>::error("You failed, you bozo. :joy_cat:");
    auto is_error = false;
    error_value.handle_error([&is_error](const char*) {
            is_error = true;
    });

    assert(is_error);

    const auto success_value = kirho::result<int, const char*>::success(16);
    auto is_error2 = false;
    success_value.handle_error([&is_error2](const char*) {
            is_error2 = true;
    });

    assert(!is_error2);
}

