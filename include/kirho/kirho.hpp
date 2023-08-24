/**
 * @file kirho.hpp
 * @brief Core Kirho library features.
 *
 * In other words, this file contains all of the features that are part of the
 * kirho library. There are, unfortunately, no more files in this library. I
 * suppose that you can say that this is not only a header only library, but
 * also a single-header library. Unsure of how long this is going to last,
 * however, as the library could become multifile in the future.
 */
#pragma once

#include <iostream>
#include <optional>
#include <variant>

namespace kirho
{
/**
 * @brief An empty type.
 *
 * A struct that contains absolutely nothing, and is therefore an empty type. It
 * can be useful when used with the @ref result_t type, such as , for example,
 * to indicate that a function returns nothing but still might fail.
 */
struct empty_t
{
};

/**
 * @brief Anything that can be printed with standard output.
 *
 * A concept that captures everything that can be printed using the standard
 * output objects, such as `std::cout`, `std::cin`, or `std::clog`.
 * This includes objects who have overrode the `>>` operator for standard
 * ostream.
 */
template <typename T>
concept printable_t = requires(T a) {
    {
        std::cout << a
    };
};

/**
 * @brief Anything that can be deferred.
 *
 * Basically, anything that can be called as if it's a function that takes in no
 * arguments.
 */
template <typename F>
concept deferable_t = requires(F a) {
    {
        a()
    };
};

/**
 * @brief A way to defer execution of something until the end of the current
 * scope.
 *
 * This is essentially a class that provides a way for you to defer execution of
 * something until the end of the current scope. How it works is that you create
 * an object of this type and pass into it's constructor the function or closure
 * that you would like to defer the execution of. Then, the destructor will
 * automatically call the function at the end of the current scope. This process
 * may sound tedious, but there is a macro provided that can a lot of this for
 * you.
 *
 * @sa #defer
 */
template <deferable_t F>
struct defer_t
{
    /**
     * @brief Creates a defer object with the specified function.
     *
     * The specified function will be called in the object's destructor. It has
     * to take in no arguments (as stated in the concept) in order to work. You
     * probably will not be using this constructor directly, but rather via the
     * @ref #defer macro.
     *
     * @sa kirho::deferable_t<F>
     * @sa #defer
     */
    defer_t(F f) noexcept : m_f(f)
    {
    }

    /**
     * @brief Calls the deferred function
     *
     * The deferred function needs to take in no arguments, or else this will
     * not work.
     */
    ~defer_t() noexcept
    {
        m_f();
    }

  private:
    F m_f;
};

/**
 * @brief A concept covers anything that can take in one argument of the
 * specified type.
 *
 * There really isn't much to say, except that now that I think about it, this
 * concept can be generalized into something more than just an error handler.
 * But, it's fine, don't worry about it. After all, I'm the library author,
 * right?
 */
template <typename Fc, typename E>
concept error_handler_t = requires(Fc f, E e) {
    {
        f(e)
    };
};

/**
 * @brief A basic implementation of error as values, inspired by the Rust
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
     * @brief Creates a success value.
     *
     * Creates a success value out of the value that you pass into it.
     *
     * @param value The value to wrap into a success result_t
     */
    static auto success(T value = T{}) noexcept -> result_t<T, E>
    {
        return result_t<T, E>{true, std::variant<T, E>{std::move(value)}};
    }

    /**
     * @brief Creates an error value.
     *
     * Creates an error value out of the value that you pass into it.
     *
     * @param error The error value to wrap.
     */
    static auto error(E error = E{}) noexcept -> result_t<T, E>
    {
        return result_t<T, E>{false, std::variant<T, E>{std::move(error)}};
    }

    /**
     * @brief Checks if the value is successful.
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
     * @brief Checks if the value is an error value.
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

        return !m_success;
    }

    /**
     * @brief Converts this result to a std::optional.
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
     * @brief Panic and prints the passed values if it is an error value.
     *
     * Panics and prints the values that are passed (not separated by spaces,
     * unlike the typical convention) if the result_t is an error value.
     * Otherwise, we return the success value back to the caller.
     *
     * @return The success value if this result is not an error value.
     */
    template <printable_t... S>
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
     * @brief Calls the passed lambda with the error value if this is indeed an
     * error type.
     *
     * Checks if we are an error type. If we are, then we simply call the
     * closure that was passed with the error value. This is a way for you to
     * handle certain error conditions, though it might be limited since it
     * requires that you pass in a closure.
     *
     * @param handler the closure or function that will be called with the
     * error.
     */
    template <error_handler_t<E> F>
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

/**
 * \def defer(name, statement)
 */

/**
 * @brief A helper macro that allows you to defer a statement.
 *
 * It will create an object named whatever the `name` is, with the term `_defer`
 * appended to the end of it. It should explain why you get errors when you try
 * to create objects of such names.
 *
 * @param name The name for the defer object.
 * @param statement The statement that you want to defer.
 */
#define defer(name, statement)                                                 \
    const auto name##_defer = kirho::defer_t{[&]() noexcept { statement; }};   \
    (void)name##_defer;
