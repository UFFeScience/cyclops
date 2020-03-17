//
//  Predicates.h
//
//  Generic multi-dimensional Matrix
//
//  Follows design provided in Bjarne Stroustrup "The C++ Programming Language", Chapter 29
//
//  Created by Alex Ershov on 12/08/2018.
//

#ifndef Predicates_h
#define Predicates_h

#include <type_traits>

namespace Pred {

    template<bool B, typename T>
    using Enable_if = typename std::enable_if<B,T>::type;

    constexpr bool All() { return true; }

    template<typename... Args>
    constexpr bool All(bool b, Args... args)
    {
        return b && All(args...);
    }

    constexpr bool Some() { return true; }

    template<typename... Args>
    constexpr bool Some(bool b, Args... args)
    {
        return b || Some(args...);
    }

    template<typename A, typename B>
    using Convertible = typename std::is_convertible<A, B>::value;

    template< class T, class U >
    using Same = typename  std::is_same<T, U>::value;

    //template<class T>
    //using Matrix_type = typename std::is_same<T, Matrix>::value;

}

#endif /* Predicates_h */
