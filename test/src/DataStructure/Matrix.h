
/*
    warning: this small multidimensional matrix library uses a few features
    not taught in ENGR112 and not explained in elementary textbooks

    (c) Bjarne Stroustrup, Texas A&M University.

    Use as you like as long as you acknowledge the source.
*/

#ifndef SRC_DATA_STRUCTURE_MATRIX_H
#define SRC_DATA_STRUCTURE_MATRIX_H

#include<string>
#include<algorithm>
#include<vector>
#include<array>
#include <iostream>
#include <type_traits>
#include <cassert>
//#include<iostream>

// template<bool B, typename T = void>
// struct std::enable_if {
//   typedef T type;
// };

// template<typename T>
// struct std::enable_if<false , T> {}; // no ::type if B==false

template<bool B, typename T = void>
using Enable_if = typename std::enable_if<B, T>::type;

// template<class T>
// using Matrix_type = typename std::is_same<T, Matrix>::value;

namespace Matrix_impl {

  // Implementation
  namespace traits
  {
    template <typename... Args> struct front_type;
    template <typename... Args> struct back_type;

    template <typename... Args> struct are_same;
  } // namespace traits

template <typename... Args>
  constexpr bool Same()
  {
    return traits::are_same<Args...>::value;
  }

template <typename T, typename U>
  constexpr bool Convertible() { return std::is_convertible<T, U>::value; }

//-----------------------------------------------------------------------------
// Implementing Enable_if
//-----------------------------------------------------------------------------

// template<bool B, typename T>
// using Enable_if = typename std::enable_if<B, T>::type;

//-----------------------------------------------------------------------------

constexpr bool All() { return true; }

template<typename... Args>
constexpr bool All(bool b, Args... args)
{
  return b && All(args...);
}

struct slice {
  slice() :start(18446744073709551615ul), length(18446744073709551615ul), stride(1) { }
  explicit slice(size_t s) :start(s), length(18446744073709551615ul), stride(1) { }
  slice(size_t s, size_t l, size_t n = 1) :start(s), length(l), stride(n) { }

  size_t operator()(size_t i) const { return start+i * stride; }

  static slice all;

  size_t start;   // first index
  size_t length;  // number of indices included (can be used for range checking)
  size_t stride;  // distance between elements in sequence
};

//-----------------------------------------------------------------------------
// Matrix_slices
//-----------------------------------------------------------------------------

template<size_t N>
struct Matrix_slice {
  Matrix_slice() = default;                               // an empty matrix: no elements

  Matrix_slice(size_t s, std::initializer_list<size_t> exts);  // extends
  Matrix_slice(size_t s, std::initializer_list<size_t> exts, std::initializer_list<size_t> strs);  // extents and strides

  template<typename... Dims>                              // N extents
    Matrix_slice(Dims... dims);

  template<typename... Dims,
           typename = Enable_if<All(Convertible<Dims, size_t>()...)>>
    size_t operator()(Dims... dims) const;                // calculate index from a set of subscripts

  size_t size;                     // total number of elements
  size_t start;                    // starting offset
  std::array<size_t, N> extents;   // number of elements in each dimension
  std::array<size_t, N> strides;   // offsets between elements in each dimension
};

template<size_t N>
  template<typename... Dims, class>
  size_t Matrix_slice<N>::operator()(Dims... dims) const
  {
    static_assert(sizeof...(Dims) == N, "");

    size_t args[N] { size_t(dims)... };     // Copy arguments into an array

    return inner_produt(args, args + N, strides.begin(), size_t(0));
  }

// template<size_t N>
//   template<typename... Dims>
//   size_t Matrix_slice<N>::operator()(Dims... dims) const
//   {
//     static_assert(sizeof...(Dims) == N, "");

//     size_t args[N] { size_t(dims)... };     // Copy arguments into an array

//     return inner_produt(args, args + N, strides.begin(), size_t(0));
//   };

//-----------------------------------------------------------------------------
// Matrix_ref
//-----------------------------------------------------------------------------

template<typename T, size_t N>
class Matrix_ref {
 public:
  Matrix_ref(const Matrix_slice<N>& s, T* p) :desc{s}, ptr{p} { }
  // ... mostly like Matrix ...
 private:
  Matrix_slice<N> desc;       // the shape of the matrix
  T* ptr;                     // the first element in the matrix
};

//-----------------------------------------------------------------------------
// Matrix_initializer
//-----------------------------------------------------------------------------

// The Matrix constructor that constructs from an initializer_list takes as its argument type the alias
// Matrix_initializer :

template<typename T, size_t N>
struct Matrix_init {
  using type = std::initializer_list<typename Matrix_init<T, N-1>::type>;
};

template<typename T, size_t N>
using Matrix_initializer = typename Matrix_impl::Matrix_init<T, N>::type;

// Matrix_init describes the structure of a nested initializer_list.
// Matrix_init<T,N> simply has Matrix_init<T,N−1> as its member type:

// The N==1 is special. That is where we get to the (most deeply nested) initializer_list<T> :

template<typename T>
struct Matrix_init<T, 1> {
  using type = std::initializer_list<T>;
};

// To avoid surprises, we define N=0 to be an error:

template<typename T>
struct Matrix_init<T, 0>;   // undefined on purpose

//-----------------------------------------------------------------------------

template<typename... Args>
constexpr bool Requesting_element()
{
  return All(Convertible<Args, size_t>()...);
}

template<typename... Args>
constexpr bool Requesting_slice()
{
  return All((Convertible<Args, size_t>() || Same<Args, slice>())...)
    && Some(Same<Args, slice>()...);
}

template<typename T, size_t N>
class Matrix {
 public:
  static constexpr size_t order = N;
  using value_type = T;
  using const_iterator = typename std::vector<T>::const_iterator;

  Matrix() = default;
  Matrix(Matrix&&) = default;                     // move
  Matrix& operator=(Matrix&&) = default;
  Matrix(Matrix const&) = default;                // copy
  Matrix& operator=(Matrix const&) = default;
  ~Matrix() = default;

  template<typename U>
    Matrix(const Matrix_ref<U, N>&);                          // construct from Matrix_ref
  template<typename U>
    Matrix& operator=(const Matrix_ref<U, N>&);               // assign from Matrix_ref

  template<typename... Exts>                                  // specify the extents
    explicit Matrix(Exts... exts);

  Matrix(Matrix_initializer<T, N>);                           // initialize from list
  Matrix& operator=(Matrix_initializer<T, N>);                // assign from list

  template<typename U>
    Matrix(std::initializer_list<U>) = delete;                     // don't user {} except for elements
  template<typename U>
    Matrix& operator=(std::initializer_list<U>) = delete;

  // static constexpr size_t order() { return N; }               // number of dimensions
  size_t extent(size_t n) const { return desc.extents[n]; }   // #elements in the nth dimension
  size_t size() const { return elems.size(); }                // total number of elements
  const Matrix_slice<N>& descriptor() const { return desc; }  // the slice defining subscripting

  T* data() { return elems.data(); }                          // "flat" element access
  const T* data() const { return elems.data(); }

  // ...

  template<typename... Args>                             // m(i,j,k) subscripting with integers
    Enable_if<Matrix_impl::Requesting_element<Args...>(), T&>
    operator()(Args... args);
  template<typename... Args>
    Enable_if<Matrix_impl::Requesting_element<Args...>(), const T&>
    operator()(Args... args) const;
  template<typename... Args>                             // m(s1,s2,s3) subscripting with slices
    Enable_if<Matrix_impl::Requesting_slice<Args...>(), Matrix_ref<const T, N>>
    operator()(const Args&... args);
  template<typename... Args>
    Enable_if<Matrix_impl::Requesting_slice<Args...>(), Matrix_ref<const T, N>>
    operator()(const Args&... args) const;

  Matrix_ref<T, N-1> operator[](size_t i) { return row(i); }      // m[i] row access
  Matrix_ref<const T, N-1> operator[](size_t i) const { return row(i); }

  Matrix_ref<T, N-1> row(size_t n);                               // row access
  Matrix_ref<const T, N-1> row(size_t n) const;

  Matrix_ref<T, N-1> col(size_t n);                               // column access
  Matrix_ref<const T, N-1> col(size_t n) const;

  // ...

  // The mathematical operations are defined like this:

  template<typename F>
    Matrix& apply(F f);                           // f(x) for every element x

  template<typename M, typename F>
    Matrix& apply(const M& m, F f);               // f(x,mx) for corresponding elements

  Matrix& operator=(const T& value);              // assignment with scalar

  Matrix& operator+=(const T& value);             // scalar addition
  Matrix& operator-=(const T& value);             // scalar subtraction
  Matrix& operator*=(const T& value);             // scalar multiplication
  Matrix& operator/=(const T& value);             // scalar division
  Matrix& operator%=(const T& value);             // scalar modulo

  template<typename M>                            // matrix addition
    Matrix& operator+=(const M& x);
  template<typename M>                            // matrix subtraction
    Matrix& operator-=(const M& x);

  // ...

 private:
  Matrix_slice<N> desc;  // slice defining extents in the N dimensions
  std::vector<T> elems;  // the elements
};

// Binary +, -, * are provided as nonmember functions



//-----------------------------------------------------------------------------
// We can now complete the Matrix constructor that takes a Matrix_initializer :
//-----------------------------------------------------------------------------

template<typename T, typename Vec>
void add_list(const T* first, const T* last, Vec& vec)
{
  vec.insert(vec.end(), first, last);
}

// I use vec.insert(vec.end(),first,last) because there is no push_back() that takes a sequence argument.

template<typename T, typename Vec>    // nested initializer_lists
void add_list(const std::initializer_list<T>* first, const std::initializer_list<T>* last, Vec& vec)
{
  for (; first != last; ++first)
    add_list(first->begin(), first->end(), vec);
}

template<typename T, typename Vec>
void insert_flat(std::initializer_list<T> list, Vec& vec)
{
  add_list(list.begin(), list.end(), vec);
}

template<typename List>
bool check_non_jagged(const List& list)
{
  auto i = list.begin();
  for (auto j = i+1; j != list.end(); ++j)
    if (i->size() != j->size())
      return false;
  return true;
}

template<size_t N, typename I, typename List>
Enable_if<(N>1), void> add_extents(I& first, const List& list)
{
  assert(check_non_jagged(list));
  *first = list.size();
  add_extents<N-1>(++first, *list.begin());
}

template<size_t N, typename I, typename List>
Enable_if<(N==1), void> add_extents(I& first, const List& list)
{
  *first++ = list.size();   // we reached the deepest nesting
}

template<size_t N, typename List>
std::array<size_t, N> derive_extents(const List& list)
{
  std::array<size_t, N> a;
  auto f = a.begin();
  add_extents<N>(f, list);    // put extents from into f[]
  return a;
}

template<typename T, size_t N>
Matrix<T, N>::Matrix(Matrix_initializer<T, N> init)
{
  Matrix_impl::derive_extents(init, desc.extents);    // deduce extents from initializer list (§29.4.4)
  elems.reserve(desc.size);                           // make room for slices
  Matrix_impl::insert_flat(init, elems);              // initialize from initializer list (§29.4.4)
  assert(elems.size() == desc.size);
}

//-----------------------------------------------------------------------------
// Matrix Access
//-----------------------------------------------------------------------------

// The row of a Matrix<T,N> is a Matrix_ref<T,N−1> as long as 1<N :

template<typename T, size_t N>
Matrix_ref<T, N-1> Matrix<T, N>::row(size_t n)
{
  // assert(n < rows());
  Matrix_slice<N-1> row;
  // Matrix_impl::slice_dim<0>(n, desc, row);
  return { row, data() };
}

// We need specializations for N==1 and N==0 :

// template<typename T>
// T& Matrix<T, 1>::row(size_t i)
// {
//   return &elems[i];
// }

// template<typename T>
// T& Matrix<T, 0>::row(size_t n) = delete;

// Selecting a column() is essentially the same as selecting a row(). The difference is simply in the
// construction of the Matrix_slice :

// template<typename T, size_t N>
// Matrix_ref<T, N-1> Matrix<T, N>::column(size_t n)
// {
//   assert(n < cols());
//   Matrix_slice<N-1> col;
//   Matrix_impl::slice_dim<1>(n, desc, col);
//   return { col, data() };
// }

// The const versions are equivalent.

template<size_t N, typename... Dims>
bool check_bounds(const Matrix_slice<N>& slice, Dims... dims)
{
  size_t indexes[N] { size_t(dims)... };
  return equal(indexes, indexes+N, slice.extents, std::less<size_t> { });
}

template<typename T, size_t N>        // subscripting with integers
  template<typename... Args>
  Enable_if<Matrix_impl::Requesting_element<Args...>(), T&>
  Matrix<T, N>::operator()(Args... args)
  {
    assert(Matrix_impl::check_bounds(desc, args...));
    return *(data() + desc(args...));
  }

template<size_t N, typename T, typename... Args>
size_t do_slice(const Matrix_slice<N>& os, Matrix_slice<N>& ns, const T& s, const Args&... args)
{
  // size_t m = do_slice_dim<sizeof...(Args)+1>(os, ns, s);
  size_t m = 0;
  size_t n = do_slice(os, ns, args...);
  return m + n;
}

// The do_slice_dim() is a tricky bit of computation (to get the slice values right) but illustrates no new
// programming techniques.

template<size_t N>
size_t do_slice(const Matrix_slice<N>& os, Matrix_slice<N>& ns)
{
  return 0;
}

template<typename T, size_t N>    // subscripting with slices
  template<typename... Args>
    Enable_if<Matrix_impl::Requesting_slice<Args...>(), Matrix_ref<const T, N>>
  Matrix<T, N>::operator()(const Args&... args)
  {
    Matrix_slice<N> d;
    d.start = Matrix_impl::do_slice(desc, d, args...);
    return { d, data() };
  }

// template<typename T, size_t N>    // subscripting with slices
//   template<typename... Args>
//     Enable_if<Matrix_impl::Requesting_slice<Args...>(), Matrix_ref<const T, N>>
//   Matrix<T, N>::operator()(const Args&... args)
//   {
//     matrix_slice<N> d;
//     d.start = matrix_impl::do_slice(desc, d.args...);
//     return { d, data() };
//   }

//-----------------------------------------------------------------------------
// Zero-Dimensional Matrix
//-----------------------------------------------------------------------------

template<typename T>
class Matrix<T, 0> {
 public:
  static constexpr size_t order = 0;
  using value_type = T;

  Matrix(const T& x) : elem(x) { }
  Matrix& operator=(const T& value) { elem = value; return *this; }

  T& operator()() { return elem; }
  operator const T&() { return elem; }
 private:
  T elem;
};

// Matrix<T, 0> is not really a matrix. It stores a single element of type T and can only be converted to a
// reference to that type.

//-----------------------------------------------------------------------------

// The constructor that takes extents (numbers of elements in dimensions) is a fairly trivial exam-
// ple of a variadic template (§28.6):

template<typename T, size_t N>
  template<typename... Exts>
  Matrix<T, N>::Matrix(Exts... exts)
    :desc{exts...},    // copy extents
     elems(desc.size)  // allocate desc.size elements and default initialize them
  {}

// The constructor that takes an initializer list requires a bit of work:

// template<typename T, size_t N>
// Matrix<T, N>::Matrix(Matrix_initializer<T, N> init)
// {
//   Matrix_impl::derive_extents(init, desc.extents);  // deduce extents from initializer list (§29.4.4)
//   elems.reserve(desc.size);                         // make room for slices
//   Matrix_impl::insert_flat(init, elems);            // initialize from initializer list (§29.4.4)
//   assert(elems.size() == desc.size);
// }

// Finally, we hav e to be able to construct from a Matrix_ref , that is, from a reference to a Matrix or
// a part of a Matrix (a submatrix):

template<typename T, size_t N>
  template<typename U>
  Matrix<T, N>::Matrix(const Matrix_ref<U, N>& x)
    :desc{x.desc}, elems{x.begin(), x.end()}  // copy desc and elements
  {
    static_assert(Convertible<U, T>(), "Matrix constructor: incompatible element types");
  }

//-----------------------------------------------------------------------------

// The use of a template allows us to construct from a Matrix with a compatible element type.
// As usual, the assignments resemble the constructors. For example:

template<typename T, size_t N>
  template<typename U>
  Matrix<T, N>& Matrix<T, N>::operator=(const Matrix_ref<U, N>& x)
  {
    static_assert(Convertible<U, T>(), "Matrix =: incompatible element types");

    desc = x.desc;
    elems.assign(x.begin(), x.end());
    return *this;
  }

//-----------------------------------------------------------------------------

// C-style subscripting is done by m[i] selecting and returning the i th row:
// Matrix_ref<T, N-1> operator[](size_t i) { return row(i); }      // m[i] row access
// template<typename T, size_t N>
// Matrix_ref<T, N−1> Matrix<T, N>::operator[](size_t i)
// {
//   return row(i); // §29.4.5
// }

//-----------------------------------------------------------------------------

// A scalar arithmetic operation simply applies its operation and right-hand operand to each element.
// For example:

template<typename T, size_t N>
Matrix<T, N>& Matrix<T, N>::operator+=(const T& val)
{
  return apply([&](T& a) { a+=val; } ); // using a lambda (§11.4)
}

// This apply() applies a function (or a function object) to each element of its Matrix :

template<typename T, size_t N>
  template<typename F>
  Matrix<T, N>& Matrix<T, N>::apply(F f)
  {
    for (auto& x : elems) f(x);  // this loop uses stride iterators
    return *this;
  }

// As usual, returning ∗this enables chaining. For example:
// m.apply(abs).apply(sqr t);  // m[i] = sqrt(abs(m[i])) for all i

// As usual (§3.2.1.1, §18.3), we can define the ‘‘plain operators,’’ such as + , outside the class using
// the assignment operators, such as += . For example:

template<typename T, size_t N>
Matrix<T, N> operator+(const Matrix<T, N>& m, const T& val)
{
  Matrix<T, N> res = m;
  res += val;
  return res;
}

// Without the move constructor, this return type would be a bad performance bug.

// template<typename T, size_t N>
//   template<typename M>
//   Enable_if<Matrix_type<M>(), Matrix<T, N>&> Matrix<T, N>::operator+=(const M& m)
//   {
//     static_assert(m.order()==N, "+=: mismatched Matrix dimensions");
//     assert(same_extents(desc, m.descriptor()));  // make sure sizes match

//     return apply(m, [](T& a, Value_type<M>&b) { a += b; });
//   }

// Matrix::apply(m,f) is the two-argument version of Matrix::apply(f). It applies its f to its two
// Matrixes (m and ∗this ):

// template<typename T, size_t N>
//   template<typename M, typename F>
//   Enable_if<Matrix_type<M>(), Matrix<T, N>&> Matrix<T, N>::apply(M& m, F f)
//   {
//     assert(same_extents(desc, m.descriptor()));  // make sure sizes match
//     for (auto i = begin(), j = m.begin(); i != end(); ++i, ++j)
//       f(*i, *j);
//     return *this;
//   }

template<typename T, size_t N>
Matrix<T, N> operator+(const Matrix<T, N>& a, const Matrix<T, N>& b)
{
  Matrix<T, N> res = a;
  res += b;
  return res;
}

// This defines + for two Matrix es of the same type yielding a result of that type. We could generalize:

// template<typename T, typename T2, size_t N,
//   typename RT = Matrix<Common_type<Value_type<T>, Value_type<T2>>, N>
// Matrix<RT, N> operator+(const Matrix<T, N>& a, const Matrix<T2, N>& b)
// {
//   Matrix<RT, N> res = a;
//   res += b;
//   return res;
// }

// If, as is common, T and T2 are the same type, Common_type is that type. The Common_type type
// function is derived from std::common_type (§35.4.2). For built-in types it, like ?: , giv es a type that
// best preserves values of arithmetic operations. If Common_type is not defined for a pair of types we
// want to use in combination, we can define it. For example:
// template<>
// struct common_type<Quad,long double> {
//   using type = Quad;
// };
// Now Common_type<Quad,long double> is Quad .

template<typename T, size_t N>
Matrix<T, N> operator+(const Matrix_ref<T, N>& x, const T& n)
{
  Matrix<T, N> res = x;
  res += n;
  return res;
}

// Subtraction, multiplication, etc., by scalars and the handling of Matrix_ref s are just repetition of
// the techniques used for addition.

//-----------------------------------------------------------------------------
// Multiplication
//-----------------------------------------------------------------------------

// Treating one Matrix<T,1> as an N-by-1 matrix and another as a 1-by-M matrix, we get:

template<typename T>
Matrix<T, 2> operator*(const Matrix<T, 1>& u, const Matrix<T, 1>& v)
{
  const size_t n = u.extent(0);
  const size_t m = v.extent(0);
  Matrix<T, 2> res(n, m);         // an n-by-m matrix
  for (size_t i = 0; i != n; ++i)
    for (size_t j = 0; j != m; ++j)
      res(i, j) = u[i] * v[j];
  return res;
}

// Next, we can multiply an N-by-M matrix with a vector seen as an M-by-1 matrix. The result is
// an N-by-1 matrix:

template<typename T>
Matrix<T, 1> operator*(const Matrix<T, 2>& m, const Matrix<T, 1>& v)
{
  assert(m.extent(1) == v.extent(0));

  const size_t n = m.extend(0);
  Matrix<T, 1> res(n);
  for (size_t i = 0; i != 0; ++i)
    for (size_t j = 0; j != n; ++j)
      res(i) += m(i, j) * v(j);
  return res;
}

// The N-by-M matrix times M-by-P matrix is handled similarly:

template<typename T>
Matrix<T, 2> operator*(const Matrix<T, 2>& m1, const Matrix<T, 2>& m2)
{
  const size_t n = m1.extent(0);
  const size_t m = m1.extent(1);
  assert(m == m2.extent(0));      // columns must match rows

  const size_t p = m2.extent(1);
  Matrix<T, 2> res(n, p);
  for (size_t i = 0; i != n; ++i)
    for (size_t j = 0; j != m; ++j)
      for (size_t k = 0; k != p; ++k)
        res(i, j) = m1(i, k) * m2(k, j);
}

//-----------------------------------------------------------------------------

}  // namespace Numeric_lib {

#endif  // SRC_DATA_STRUCTURE_MATRIX_H
