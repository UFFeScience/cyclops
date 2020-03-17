
#ifndef SRC_DATA_STRUCTURE_MATRIX_H
#define SRC_DATA_STRUCTURE_MATRIX_H

#include <new>
#include <iostream>

// #ifdef DEBUG_ALL
// #define DEBUG_Matrix

// extern int naloc;
// extern int nfree;
// #endif

// extern void mserro(const char *clas,const char *func,const char * ms,const int nerr);

template<typename T>
class Matrix
{
public:
  Matrix() : pType(NULL), n(0), m(0) { }
  Matrix(const int nin, const int min, const T val = 0);
  Matrix(const Matrix &rhs);
  ~Matrix();
  T& operator()(const int i, const int j);
  const T& operator()(const int i, const int j) const;
  Matrix& operator=(const Matrix&);
  void redefine(const int novo_n, const int novo_m, const T val = 0); // aloca a Matrix e os elementos ficam igual a val
  void aloca(const int novo_n, const int novo_m);  // aloca a Matrix
  void set(const T val = 0);  // toda a Matrix fica igual a val
  void set(const int i, const T val);
  void get_size(int &nout, int &mout) { nout = n; mout = m; }
  void desaloca() { del(); n = m = 0; pType = NULL; }
  void imprime();
  int getNumLinhas() const { return n; };
  int getNumColunas() const { return m; };

  template<typename U>
  friend std::ostream& operator<<(std::ostream& os, const Matrix<U>& m);

private:
  std::ostream& write(std::ostream& os) const {
    os << "--- Matrix ---" << std::endl;
    for (int i = 0; i < n; ++i) {
      os << i << ") ";
      for (int j = 0; j < m; ++j) {
        os << pType[i][j] << ", ";
      }
      os << std::endl;
    }
    return os << '}';
  }
  void del();
  T** pType;
  int n; // numero de linhas
  int m; // nomero de colunas
};

template<typename T>
Matrix<T>::Matrix(const Matrix &rhs)
{
  pType = NULL;
  aloca(rhs.getNumLinhas(),rhs.getNumColunas());
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      pType[i][j] = rhs(i,j);
    }
  }
}

template<typename T>
Matrix<T>::Matrix(const int nin, const int min, const T val)
{
  pType = NULL; n = m = 0;
  redefine(nin, min, val);
}

template<typename T>
Matrix<T>::~Matrix()
{
  del();
  pType = NULL;
  n = m = 0;
}

template<typename T>
T& Matrix<T>::operator()(const int i, const int j)
{
  return pType[i][j];
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix &rhs)
{
  pType = NULL;
  aloca(rhs.getNumLinhas(),rhs.getNumColunas());
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<m;j++)
    {

      pType[i][j] = rhs(i,j);
    }

  }
  return *this;
}

template<typename T>
const T& Matrix<T>::operator()(const int i, const int j) const
{
  return pType[i][j];
}

template<typename T>
void Matrix<T>::aloca(const int novo_n, const int novo_m)
{
  del();
  if (novo_n + novo_m != 0) {
    pType = new T*[novo_n];
    // if (pType == NULL)
	  //   mserro("Matrix", "aloca", "Falta de memoria 1", 0);
    for (int i = 0; i < novo_n; i++) {
      pType[i] = new T[novo_m];
      // if (pType[i] == NULL)
	    //   mserro("Matrix", "aloca", "Falta de memoria 2", 0);
    }
  } else
    pType = NULL;
  n = novo_n;
  m = novo_m;
}

template<typename T>
void Matrix<T>::redefine(const int novo_n, const int novo_m, const T val)
{
  aloca(novo_n, novo_m);
  set(val);
}

template<typename T>
void Matrix<T>::set(const T val)
{
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j)
      pType[i][j] = val;
}

template<typename T>
void Matrix<T>::set(const int i, const T val)
{
  for (int j = 0; j < m; ++j)
    pType[i][j] = val;
}

template<typename T>
void Matrix<T>::del()
{
  if (pType != NULL) {
    for (int i = 0; i < n; ++i) {
      delete [] pType[i];
    }
    delete [] pType;
  }
}

template<typename T>
void Matrix<T>::imprime()
{
  std::cout << "--- Matrix ---" << std::endl;
  for (int i = 0; i < n; ++i) {
    std::cout << i << ") ";
    for (int j = 0; j < m; ++j) {
	    std::cout<<pType[i][j]<<", ";
	  }
    std::cout<<std::endl;
  }
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m)
{
  return m.write(os);
  // os << '{';
  // for (siz e_t i = 0; i != rows(m); ++i) {
  //     os << m[i];
  //     if (i+1 != rows(m)) os << ',';
  // }
  // return os << '}';
}

#endif  // SRC_DATA_STRUCTURE_MATRIX_H
