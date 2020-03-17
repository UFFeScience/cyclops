//
//  Matrix.h
//
//  Generic multi-dimensional Matrix
//
//  Follows design provided in Bjarne Stroustrup "The C++ Programming Language", Chapter 29
//
//  Created by Alex Ershov on 12/08/2018.
//

#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdio.h>
#include <vector>
#include <array>
#include <numeric>
#include <ostream>

#include "Matrix_impl.h"
#include "Predicates.h"

using std::array;
using std::vector;
using std::initializer_list;

using namespace Pred;

// Shortcuts
template<typename T, size_t N>
using Matrix_initializer = typename Matrix_impl::Matrix_init<T, N>::type;

template<typename T, size_t N>
class Matrix_ref;

template<size_t N>
struct Matrix_slice;

/*----------------------------------*/
/* Matrix, Main class               */
/*----------------------------------*/
template<typename T, size_t N>
class Matrix {

public:
    static constexpr size_t order = N;
    using value_type = T;
    using iterator = typename vector<T>::iterator;
    using const_iterator = typename vector<T>::const_iterator;

    Matrix() = default;
    Matrix(Matrix&&) = default; // move
    Matrix& operator=(Matrix&&) = default;
    Matrix(Matrix const&) = default; // copy
    Matrix& operator=(Matrix const&) = default;

    ~Matrix() = default;

    /*----------------------------------*/
    /* Matrix construction              */
    /*----------------------------------*/
    template<typename U>
        Matrix(const Matrix_ref<U,N>&);                 // construct from Matrix_ref
    template<typename U>
        Matrix& operator=(const Matrix_ref<U,N>&);      // assign from Matrix_ref

    template<typename...Exts>                           // specify the extents
        explicit Matrix(Exts... exts);

    Matrix(Matrix_initializer<T,N>);                    // initialize from list
    Matrix& operator=(Matrix_initializer<T,N>);         // assign from list

    template<typename U>
    Matrix(initializer_list<U>) = delete;               // don’t use {} except for elements

    template<typename U>
    Matrix& operator=(initializer_list<U>) = delete;

    //static constexpr size_t order() { return N; } /    / number of dimensions
    size_t extent(size_t n) const { return desc.extents[n]; } // #elements in the nth dimension
    size_t size() const { return elems.size(); }        // total number of elements
    const Matrix_slice<N>& descriptor() const { return desc; } // the slice defining subscripting

    T* data() { return elems.data(); }                  // ‘‘flat’’ element access
    const T* data() const { return elems.data(); }

    /*----------------------------------*/
    /* Subscription operators           */
    /*----------------------------------*/
    template<typename... Args>                                      //m(i,j,k) subscripting with integers
        Enable_if<Matrix_impl::Requesting_element<Args...>(), T&>
        operator()(Args... args);

    template<typename... Args>
        Enable_if<Matrix_impl::Requesting_element<Args...>(), const T&>
        operator()(Args... args) const;

    template<typename... Args>                                     //m(s1,s2,s3) subscripting with slices
        Enable_if<Matrix_impl::Requesting_slice<Args...>(), Matrix_ref<T, N>>
        operator()(const Args&... args);

    template<typename... Args>
        Enable_if<Matrix_impl::Requesting_slice<Args...>(), Matrix_ref<const T,N>>
        operator()(const Args&... args) const;

    Matrix_ref<T,N-1> operator[](size_t i) { return row(i); } //   m[i] row access
    Matrix_ref<const T,N-1> operator[](size_t i) const { return row(i); }
    Matrix_ref<T,N-1> row(size_t n);                // row access
    Matrix_ref<const T,N-1> row(size_t n) const;    // row access
    Matrix_ref<T,N-1> col(size_t n);                // column access
    Matrix_ref<const T,N-1> col(size_t n) const;    // column access


    /*----------------------------------*/
    /* Mathematical operations          */
    /*----------------------------------*/
    template<typename F>
        Matrix& apply(F f);             // f(x) for every element x

    template<typename M, typename F>
        Matrix& apply(const M& m, F f); // f(x,mx) for corresponding elements

    Matrix& operator=(const T& value);  // assignment with scalar
    Matrix& operator+=(const T& value); // scalar addition
    //Matrix& operator−=(const T& value); // scalar subtraction
    //Matrix& operator∗=(const T& value); // scalar multiplication
    //Matrix& operator/=(const T& value); // scalar division
    //Matrix& operator%=(const T& value); // scalar modulo

    template<typename M>                // matrix addition
        Matrix& operator+=(const M& x);

    template<typename M>                // matrix subtraction
        Matrix& operator-=(const M& x);


private:
    Matrix_slice<N> desc; // slice defining extents in the N dimensions
    vector<T> elems; // the elements
};


/*----------------------------------*/
/* Matrix Slice                     */
/*----------------------------------*/
template<size_t N>
struct Matrix_slice {
    Matrix_slice() = default; // an empty matrix: no elements

    Matrix_slice(size_t s, initializer_list<size_t> exts); // extents
    Matrix_slice(size_t s, initializer_list<size_t> exts, initializer_list<size_t> strs);// extents and strides

    template<typename...Dims> //N extents
        Matrix_slice(Dims... dims);

    template<typename... Dims>
        Enable_if< All(Convertible<Dims, size_t>()...), size_t>
            operator()(Dims... dims) const; // calculate index from a set of subscripts

    size_t size; // total number of elements
    size_t start; // star ting offset

    array<size_t,N> extents; // number of elements in each dimension
    array<size_t,N> strides; // offsets between elements in each dimension
};

template<typename T>
class Matrix<T,0> {
public:
    static constexpr size_t order = 0;
    using value_type = T;

    Matrix(const T& x) : elem(x) { }
    Matrix& operator=(const T& value) { elem = value; return *this; }

    T& operator()() { return elem; }
    const T& operator()() const { return elem; }
    operator T&() { return elem; }
    operator const T&() { return elem; }
private:
    T elem;
};

/*----------------------------------*/
/* Matrix Ref                       */
/*----------------------------------*/
template<typename T, size_t N>
class Matrix_ref {
public:
    Matrix_ref(const Matrix_slice<N>& s, T* p) :desc{s}, ptr{p} {}
    // ... mostly like Matrix ...

private:
    Matrix_slice<N> desc; // the shape of the matrix
    T* ptr; //the first element in the matrix
};


/*----------------------------------*/
/* Matrix Init                       */
/*----------------------------------*/
template<typename T, size_t N>
struct Matrix_init {
    using type = initializer_list<typename Matrix_init<T,N-1>::type>;
};

template<typename T>
struct Matrix_init<T,1> {
    using type = initializer_list<T>;
};

template<typename T>
struct Matrix_init<T,0>; // undefined on purpose

/*----------------------------------*/
/* Mathematical operations          */
/*----------------------------------*/
template<typename T, size_t N>
Matrix<T,N> operator+(const Matrix<T,N>& m, const T& val)
{
    Matrix<T,N> res = m;
    res+=val;
    return res;
}

template<typename T, size_t N>
Matrix<T,N> operator+(const Matrix<T,N>& a, const Matrix<T,N>& b)
{
    Matrix<T,N> res = a;
    res+=b;
    return res;
}

template<typename T, size_t N>
Matrix<T,N> operator+(const Matrix_ref<T,N>& x, const T& n)
{
    Matrix<T,N> res = x;
    res+=n;
    return res;
}


//template<typename M>
//    Enable_if<Matrix_type<M>(),ostream&>
//operator<<(ostream& os, const M& m)
//{
//    os << '{';
//    for (siz e_t i = 0; i!=rows(m); ++i) {
//        os << m[i];
//        if (i+1!=rows(m)) os << ',';
//    }
//    return os << '}';
//}




#endif /* MATRIX_H_ */
