//
//  Matrix.cpp
//
//  Generic multi-dimensional Matrix
//
//  Follows design provided in Bjarne Stroustrup "The C++ Programming Language", Chapter 29
//
//  Created by Alex Ershov on 12/08/2018.
//

#include "Predicates.h"
#include "Matrix.h"

using std::array;
using std::vector;
using std::initializer_list;

template<typename T, size_t N>
using Matrix_initializer = typename Matrix_impl::Matrix_init<T, N>::type;

/*----------------------------------*/
/* Matrix, Main class               */
/*----------------------------------*/
template<typename T, size_t N>
    template<typename... Exts>
    Matrix<T,N>::Matrix(Exts... exts)
        :desc{exts...},  // copy extents
        elems(desc.size) // allocate desc.size elements and default initialize them
{ }


template<typename T, size_t N>
Matrix<T, N>::Matrix(Matrix_initializer<T,N> init)
{
    Matrix_impl::derive_extents(init, desc.extents); // deduce extents from initializer list (§29.4.4)
    elems.reserve(desc.size);                       // make room for slices
    Matrix_impl::insert_flat(init,elems);           // initialize from initializer list (§29.4.4)
    assert(elems.size() == desc.size);
}

template<typename T, size_t N> // subscripting with slices
    template<typename... Args>
    Enable_if<Matrix_impl::Requesting_slice<Args...>(), Matrix_ref<T,N>>
Matrix<T,N>::operator()(const Args&... args)
{
    Matrix_slice<N> d;
    d.start = Matrix_impl::do_slice(desc,d,args...);
    return {d,data()};
}

template<typename T, size_t N> //subscripting with integers
    template<typename... Args>
    Enable_if<Matrix_impl::Requesting_element<Args...>(),T&>
Matrix<T,N>::operator()(Args... args)
{
    assert(Matrix_impl::check_bounds(desc, args...));
    return *(data() + desc(args...));
}

template<typename T, size_t N>
    template<typename U>
    Matrix<T,N>::Matrix(const Matrix_ref<U,N>& x)
        :desc{x.desc}, elems{x.begin(),x.end()}     // copy desc and elements
{
    static_assert(Convertible<U,T>(),"Matrix constructor: incompatible element types");
}

template<typename T, size_t N>
    template<typename U>
    Matrix<T,N>& Matrix<T,N>::operator=(const Matrix_ref<U,N>& x)
{
    static_assert(Convertible<U,T>(),"Matrix =: incompatible element types");

    desc = x.desc;
    elems.assign(x.begin(),x.end());
    return *this;
}

template<typename T, size_t N>
Matrix_ref<T,N-1> Matrix<T,N>::row(size_t n)
{
    //assert(n<rows());
    Matrix_slice<N-1> row;
    Matrix_impl::slice_dim<0>(n,desc,row);
    return {row,data()};
}

template<typename T>
T& Matrix<T,1>::row(size_t i)
{
    return &elems[i];
}

template<typename T>
T& Matrix<T,0>::row(size_t n) = delete;

template<typename T, size_t N>
Matrix_ref<T,N−1> Matrix<T,N>::column(size_t n)
{
    assert(n<cols());
    Matrix_slice<N−1> col;
    Matrix_impl::slice_dim<1>(n,desc,col);
    return {col,data()};
}

template<typename T>
T& Matrix<T,1>::col(size_t i)
{
    return &elems[i];
}

template<typename T>
T& Matrix<T,0>::col(size_t n) = delete;


/*----------------------------------*/
/* Matrix Slice                     */
/*----------------------------------*/
template<size_t N>
    template<typename... Dims>
    size_t Matrix_slice<N>::operator()(Dims... dims) const
{
    static_assert(sizeof...(Dims) == N, "");
    size_t args[N] { size_t(dims)... }; // Copy arguments into an array
    return std::inner_product(args,args+N,strides.begin(),size_t(0));
}



/*----------------------------------*/
/* Mathematical operations          */
/*----------------------------------*/

template<typename T, size_t N>
    template<typename F>
    Matrix<T,N>& Matrix<T,N>::apply(F f)
{
    for (auto& x : elems) f(x); // this loop uses stride iterators
    return *this;
}

template<typename T, size_t N>
    template<typename M, typename F>
    Enable_if<Matrix_type<M>(),Matrix<T,N>&> Matrix<T,N>::apply(M& m, F f)
{
    assert(same_extents(desc,m.descriptor())); // make sure sizes match

    for (auto i = begin(), j = m.begin(); i!=end(); ++i, ++j)
        f(∗i,∗j);

    return ∗this;
}

template<typename T, size_t N>
Matrix<T,N>& Matrix<T,N>::operator+=(const T& val)
{
    return apply([&](T& a) { a+=val; } ); // using a lambda (§11.4)
}

template<typename T, size_t N>
    template<typename M>
    Enable_if<Matrix_type<M>(),Matrix<T,N>&> Matrix<T,N>::operator+=(const M& m)
{
    static_assert(m.order()==N,"+=: mismatched Matrix dimensions");

    assert(same_extents(desc,m.descriptor())); // make sure sizes match
    return apply(m, [](T& a,Value_type<M>&b) { a+=b; });
}

