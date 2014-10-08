#pragma once

template <typename Type>
constexpr Type Pow(const Type base, const Type exponent)
{   
    return(exponent != 0 )? base * Pow(base, exponent -1) : 1;
} 

template <template<typename> class Trait, typename Head, typename ...Tail> 
struct check_all
{
      enum { value = Trait<Head>::value && check_all<Trait, Tail...>::value };
};

template <template<typename> class Trait, typename Head>
struct check_all<Trait, Head>
{
      enum { value = Trait<Head>::value };
};
