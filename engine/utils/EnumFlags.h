#pragma once
#include <type_traits>

/// <summary>
/// enumをフラグ化
/// </summary>
/// <typeparam name="E">フラグ化するenum</typeparam>
template <typename E>
struct EnumFlags : std::false_type
{
};

template <typename E>
constexpr std::enable_if_t<EnumFlags<E>::value, E> operator|( E a, E b )
{
    using T = std::underlying_type_t<E>;
    return static_cast<E>( static_cast<T>( a ) | static_cast<T>( b ) );
}

template <typename E>
constexpr std::enable_if_t<EnumFlags<E>::value, E> operator^( E a, E b )
{
    using T = std::underlying_type_t<E>;
    return static_cast<E>( static_cast<T>( a ) ^ static_cast<T>( b ) );
}

template <typename E>
constexpr std::enable_if_t<EnumFlags<E>::value, E> operator&( E a, E b )
{
    using T = std::underlying_type_t<E>;
    return static_cast<E>( static_cast<T>( a ) & static_cast<T>( b ) );
}

template <typename E>
constexpr std::enable_if_t<EnumFlags<E>::value, E> operator~( E a )
{
    using T = std::underlying_type_t<E>;
    return static_cast<E>( ~static_cast<T>( a ) );
}

template <typename E>
constexpr std::enable_if_t<EnumFlags<E>::value, E&> operator|=( E& a, E b )
{
    a = a | b;
    return a;
}

template <typename E>
constexpr std::enable_if_t<EnumFlags<E>::value, E&> operator^=( E& a, E b )
{
    a = a ^ b;
    return a;
}

template <typename E>
constexpr std::enable_if_t<EnumFlags<E>::value, E&> operator&=( E& a, E b )
{
    a = a & b;
    return a;
}

// enumをフラグ化
#define ENABLE_ENUM_FLAGS( E )           \
    template <>                          \
    struct EnumFlags<E> : std::true_type \
    {                                    \
    };
