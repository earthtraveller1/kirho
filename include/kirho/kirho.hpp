#pragma once

#include <iostream>
#include <optional>

namespace kirho
{
    struct empty{};

    template<typename T>
    concept printable = requires(T a)
    {
        { std::cout << a };
    };

    // A basic way to implement errors as values.
    template<typename T, typename E>
    class result
    {
    private:
        union internal_union
        {
            T value;
            E error;
        };

    public:
        static auto success(T value) noexcept -> result<T, E>
        {
            return result<T, E>{ true, internal_union { .value = value } };
        }

        static auto error(E error) noexcept -> result<T, E>
        {
            return result<T, E>{ false, internal_union { .error = error } };
        }

        auto is_success(T& value) const noexcept -> bool
        {
            if (m_success)
            {
                value = m_union.value;
            }

            return m_success;
        }

        auto is_error(E& error) const noexcept -> bool
        {
            if (!m_success)
            {
                error = m_union.error;
            }

            return m_success;
        }

        auto to_optional() const noexcept -> std::optional<T>
        {
            if (m_success)
            {
                return std::optional<T>(m_union.value);
            }
            else
            {
                return std::optional<T>();
            }
        }

        template<printable... S>
        auto except(S... values) const noexcept -> T
        {
            if (!m_success)
            {
                (std::cerr << ... << values) << '\n';
                std::terminate();
            }

            return m_union.value;
        }

        auto unwrap() const noexcept -> T
        {
            if (!m_success)
            {
                std::cerr << "result::unwrap called on error value.\n";
                std::terminate();
            }

            return m_union.value;
        }

        result(const result&) = delete;
        result& operator=(const result&) = delete;

    private:
        result(bool p_success, internal_union p_union): m_success{p_success}, m_union{p_union} {}

    private:
        bool m_success;

        internal_union m_union;
    };
}
