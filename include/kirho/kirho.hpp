#pragma once

#include <iostream>
#include <optional>
#include <variant>

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
    class result_t
    {
    public:
        static auto success(T value) noexcept -> result_t<T, E>
        {
            return result_t<T, E>{ true, std::variant<T, E> { std::move(value) } };
        }

        static auto error(E error) noexcept -> result_t<T, E>
        {
            return result_t<T, E>{ false, std::variant<T, E> { std::move(error) } };
        }

        auto is_success(T& value) const noexcept -> bool
        {
            if (m_success)
            {
                value = std::get<T>(m_union);
            }

            return m_success;
        }

        auto is_error(E& error) const noexcept -> bool
        {
            if (!m_success)
            {
                error = std::get<E>(m_union);
            }

            return m_success;
        }

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

        template<printable... S>
        auto except(S... values) const noexcept -> T
        {
            if (!m_success)
            {
                (std::cerr << ... << values) << '\n';
                std::terminate();
            }

            return std::get<T>(m_union);
        }

        auto unwrap() const noexcept -> T
        {
            if (!m_success)
            {
                std::cerr << "result::unwrap called on error value.\n";
                std::terminate();
            }

            return std::get<T>(m_union);
        }

        result_t(const result_t&) = delete;
        result_t& operator=(const result_t&) = delete;

    private:
        result_t(bool p_success, std::variant<T, E> p_union): m_success{p_success}, m_union{p_union} {}

    private:
        bool m_success;

        std::variant<T, E> m_union;
    };
}
