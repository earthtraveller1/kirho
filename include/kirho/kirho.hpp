#pragma once

#include <iostream>
#include <optional>
#include <variant>

namespace kirho
{
struct empty
{
};

template <typename T>
concept printable = requires(T a) {
    {
        std::cout << a
    };
};

template <typename F>
concept deferable = requires(F a) {
    {
        a()
    };
};

template <deferable F>
struct defer
{
    defer(F f) noexcept : m_f(f)
    {
    }
    ~defer() noexcept
    {
        m_f();
    }
    F m_f;
};

template <typename Fc, typename E>
concept error_handler = requires(Fc f, E e) {
    {
        f(e)
    };
};

/**
 * A basic implementation of error as values, inspired by the Rust
 * Result type.
 *
 * This is a fairly basic implementation of an error as value type, and was
 * heavily inspired by Rust's Result type. This is the reason why it's named
 * so. It is using std::variant, and as a result, suffers from similiar
 * limitations. For instance, you cannot have T and E be the same type.
 */
template <typename T, typename E>
class result_t
{
  public:
    /**
     * Creates a success value.
     *
     * Creates a success value out of the value that you pass into it.
     *
     * @param value The value to wrap into a success result_t
     */
    static auto success(T value) noexcept -> result_t<T, E>
    {
        return result_t<T, E>{true, std::variant<T, E>{std::move(value)}};
    }

    /**
     * Creates an error value.
     *
     * Creates an error value out of the value that you pass into it.
     *
     * @param error The error value to wrap.
     */
    static auto error(E error) noexcept -> result_t<T, E>
    {
        return result_t<T, E>{false, std::variant<T, E>{std::move(error)}};
    }

    /**
     * Checks if the value is successful.
     *
     * Checks if the value is a success value, and returns it through the
     * reference if it is.
     *
     * @param value The variable in which to store the returned value.
     *
     * @return A boolean that indicates whether it's successful.
     */
    auto is_success(T& value) const noexcept -> bool
    {
        if (m_success)
        {
            value = std::get<T>(m_union);
        }

        return m_success;
    }

    /**
     * Checks if the value is an error value.
     *
     * Checks if the value is an error value, and returns it rhough the
     * reference if it is.
     *
     * @param error The variable in which to store the returned error.
     *
     * @return A boolean that indicates whether it's an error.
     */
    auto is_error(E& error) const noexcept -> bool
    {
        if (!m_success)
        {
            error = std::get<E>(m_union);
        }

        return m_success;
    }

    /**
     * Converts this result to a std::optional.
     *
     * Converts this result into a std::optional. When converted, the optional
     * type will not contain any information regarding the error.
     *
     * @return The resulting optional value.
     */
    auto to_optional() const noexcept -> std::optional<T>
    {
        if (m_success)
        {
            return std::optional<T>(std::get<T>(m_union));
        }
        else
        {
            return std::optional<T>();
        }
    }

    /**
     * Panic and prints the passed values if it is an error value.
     *
     * Panics and prints the values that are passed (not separated by spaces,
     * unlike the typical convention) if the result_t is an error value.
     * Otherwise, we return the success value back to the caller.
     *
     * @return The success value if this result is not an error value.
     */
    template <printable... S>
    auto except(S... values) const noexcept -> T
    {
        if (!m_success)
        {
            (std::cerr << ... << values) << '\n';
            std::terminate();
        }

        return std::get<T>(m_union);
    }

    /**
     * Panics if the result is an error value, otherwise return the success
     * value.
     *
     * This function will check if the value is successful or not. If it is an
     * error, it will call std::terminate and stop the program. Otherwise, we
     * will simply return the successful value.
     *
     * @return The success value if this result is not an error value.
     */
    auto unwrap() const noexcept -> T
    {
        if (!m_success)
        {
            std::cerr << "result_t::unwrap called on error value.\n";
            std::terminate();
        }

        return std::get<T>(m_union);
    }

    result_t(const result_t&) = delete;
    result_t& operator=(const result_t&) = delete;

    /**
     * Calls the passed lambda with the error value if this is indeed an error
     * type.
     *
     * Checks if we are an error type. If we are, then we simply call the
     * closure that was passed with the error value. This is a way for you to
     * handle certain error conditions, though it might be limited since it
     * requires that you pass in a closure.
     *
     * @param handler the closure or function that will be called with the
     * error.
     */
    template <error_handler<E> F>
    auto handle_error(F handler) const -> void
    {
        if (!m_success)
        {
            handler(std::get<E>(m_union));
        }
    }

  private:
    result_t(bool p_success, std::variant<T, E> p_union)
        : m_success{p_success}, m_union{p_union}
    {
    }

  private:
    bool m_success;

    std::variant<T, E> m_union;
};
} // namespace kirho

#define defer(name, statement)                                                 \
    const auto name##_defer = kirho::defer{[&]() noexcept { statement; }};     \
    (void)name##_defer;
