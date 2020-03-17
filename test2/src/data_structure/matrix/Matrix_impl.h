//
//  Matrix_impl.h
//
//  Generic multi-dimensional Matrix
//
//  Follows design provided in Bjarne Stroustrup "The C++ Programming Language", Chapter 29
//
//  Created by Alex Ershov on 12/08/2018.
//

#ifndef MATRIX_IMPL_H_
#define MATRIX_IMPL_H_

#include "Predicates.h"

using std::array;
using std::vector;
using std::initializer_list;

using namespace Pred;

template<size_t N>
struct Matrix_slice;


namespace Matrix_impl
{

    struct slice {
        // slice() :start(-1), length(-1), stride(1) { }
        // explicit slice(size_t s) :start(s), length(-1), stride(1) { }
        slice() :start(0), length(0), stride(1) { }
        explicit slice(size_t s) :start(s), length(0), stride(1) { }
        slice(size_t s, size_t l, size_t n = 1) :start(s), length(l), stride(n) { }
        size_t operator()(size_t i) const { return start+i*stride; }
        static slice all;
        size_t start; // first index
        size_t length; // number of indices included (can be used for range checking)
        size_t stride; // distance between elements in sequence
    };

    template<typename T, size_t N>
    struct Matrix_init {
        using type = initializer_list<typename Matrix_init<T,N-1>::type>;
    };

    // Specialization
    template<typename T>
    struct Matrix_init<T,1> {
        using type = initializer_list<T>;
    };

    // Undefined specialization to exit recursive definition
    template<typename T>
    struct Matrix_init<T,0>; // undefined on purpose

    template<size_t N, typename I, typename List>
    Enable_if<(N>1),void> add_extents(I& first, const List& list)
    {
        assert(check_non_jagged(list));
        *first = list.size();
        add_extents<N-1>(++first,*list.begin());
    }

    template<size_t N, typename I, typename List>
    Enable_if<(N==1),void> add_extents(I& first, const List& list)
    {
        *first++ = list.size(); // we reached the deepest nesting
    }

    template<size_t N, typename List>
    array<size_t, N> derive_extents(const List& list)
    {
        array<size_t,N> a;
        auto f = a.begin();
        add_extents<N>(f,list); // put extents from list into f[]
        return a;
    }


    template<typename List>
    bool check_non_jagged(const List& list)
    {
        auto i = list.begin();
        for (auto j = i+1; j!=list.end(); ++j)
            if (i->size()!=j->size())
                return false;
        return true;
    }

    template<typename T, typename Vec>
    void insert_flat(initializer_list<T> list, Vec& vec)
    {
        add_list(list.begin(),list.end(),vec);
    }

    template<typename T, typename Vec> // nested initializer_lists
    void add_list(const initializer_list<T>* first, const initializer_list<T>* last, Vec& vec)
    {
        for (;first!=last;++first)
            add_list(first->begin(),first->end(),vec);
    }

    template<typename T, typename Vec>
    void add_list(const T* first, const T* last, Vec& vec)
    {
        vec.insert(vec.end(),first,last);
    }

    template<typename... Args>
    constexpr bool Requesting_element()
    {
        return All(Convertible<Args, size_t>()...);
    }

    template<typename... Args>
    constexpr bool Requesting_slice()
    {
        return All(  (Convertible<Args, size_t>() || Same<Args,slice>())...)
        && Some(Same<Args,slice>()...);
    }


    template<size_t N, typename... Dims>
    bool check_bounds(const Matrix_slice<N>& slice, Dims... dims)
    {
        size_t indexes[N] {size_t(dims)...};
        return equal(indexes, indexes+N, slice.extents, std::less<size_t> {});
    }



    template<size_t N, typename T, typename ... Args>
    size_t do_slice(const Matrix_slice<N>& os, Matrix_slice<N>& ns, const T& s, const Args&... args)
    {
        //size_t m = do_slice_dim<sizeof...(Args)+1>(os,ns,s);
        size_t m = 0;
        size_t n = do_slice(os,ns,args...);
        return m+n;
    }

}

#endif /* MATRIX_IMPL_H_ */
