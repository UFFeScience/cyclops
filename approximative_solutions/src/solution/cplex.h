/**
 * \file src/solution/cplex.h
 * \brief Contains the \c Cplex class declaration.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This header file contains the \c Cplex class.
 */

#ifndef APPROXIMATIVE_SOLUTIONS_SRC_CPLEX_H_
#define APPROXIMATIVE_SOLUTIONS_SRC_CPLEX_H_

#include <ilcplex/ilocplex.h>
#include "src/solution/algorithm.h"


//------------ Estrutura do ambiente cplex (ambiente, modelo e variaveis) -------------------
struct CPLEX
{
  IloEnv   env;
  IloModel model;
  IloArray<IloArray<IloBoolVarArray>> x;
  IloArray<IloArray<IloArray<IloArray<IloBoolVarArray>>>> r;
  IloArray<IloArray<IloArray<IloArray<IloBoolVarArray>>>> w;
  IloArray<IloArray<IloBoolVarArray>> y;
  IloArray<IloBoolVarArray> yb;
  IloArray<IloBoolVarArray> ws;
  IloArray<IloNumVarArray> e;
  IloArray<IloBoolVarArray> b;
  IloArray<IloNumVarArray> q;
  IloArray<IloBoolVarArray> v;
  IloNumVarArray z;
  IloNumVarArray z_max;

  CPLEX(int n, int d, int m, int numr, int numb) :
    env(),
    model(env),
    x(env, n),
    r(env, n),
    w(env, n),
    y(env, d),
    yb(env, d),
    ws(env, d),
    e(env, numr),
    b(env, numb),
    q(env, numb),
    v(env, m),
    z(env, m),
    z_max(env, 1) {}

};

struct BEST
{
  int        n_;
  int        d_;
  int        m_;
  int        numr_;
  int        numb_;
  int        mb_;
  int        t_;
  int        max_num_intervals_;

  Algorithm* algorithm_;

  clock_t    start;
  double     PRECISAO = 1.0e-6;
  // double  b_sol    = 1.0e+10;
  double     b_sol    = std::numeric_limits<double>::max();

  // ---------- MELHOR SOLUCAO ------------------
  int ***    x;               // guarda a melhor solucao x
  int *****  r;               // guarda a melhor solucao r
  int *****  w;               // guarda a melhor solucao r
  int ***    y;               // guarda a melhor solucao y
  int **     yb;              // guarda a melhor solucao yb
  int **     ws;              // guarda a melhor solucao w
  double **  e;               // guarda a melhor solucao e
  int **     b;               // guarda a melhor solucao b
  double **  q;               // guarda a melhor solucao q
  int **     v;               // guarda a melhor solucao v
  double *   z;               // guarda a melhor solucao z
  double z_max;               // guarda a melhor solucao z_max

  BEST(int n, int d, int m, int numr, int numb, int mb, int t, int max_num_intervals, Algorithm* algorithm)
      : n_(n), d_(d), m_(m), numr_(numr), numb_(numb), mb_(mb), t_(t),
        max_num_intervals_(max_num_intervals), algorithm_(algorithm)
  {
    start = clock();

    // x = (int***) malloc((size_t) n * (size_t) m * (size_t) t * (size_t) sizeof(int));
    x = new int**[n];
    for (int i = 0; i < n; ++i)
    {
      x[i] = new int*[m];
      for (int j = 0; j < m; ++j)
      {
        x[i][j] = new int[t];
      }
    }

    // r = (int*****) malloc((size_t) n * (size_t) d * (size_t) m *  (size_t) mb * (size_t) t * (size_t) sizeof(int));
    r = new int****[n];
    for (int i = 0; i < n; ++i)
    {
      r[i] = new int***[d];
      for (int j = 0; j < d; ++j)
      {
        r[i][j] = new int**[m];
        for (int k = 0; k < m; ++k)
        {
          r[i][j][k] = new int*[mb];
          for (int l = 0; l < mb; ++l)
          {
            r[i][j][k][l] = new int[t];
          }
        }
      }
    }

    // w = (int*****) malloc((size_t) n * (size_t) d * (size_t) m *  (size_t) mb * (size_t) t * (size_t) sizeof(int));
    w = new int****[n];
    for (int i = 0; i < n; ++i)
    {
      w[i] = new int***[d];
      for (int j = 0; j < d; ++j)
      {
        w[i][j] = new int**[m];
        for (int k = 0; k < m; ++k)
        {
          w[i][j][k] = new int*[mb];
          for (int l = 0; l < mb; ++l)
          {
            w[i][j][k][l] = new int[t];
          }
        }
      }
    }

    // y = (int***) malloc((size_t) d * (size_t) mb * (size_t) t * (size_t) sizeof(int));
    y = new int**[d];
    for (int i = 0; i < d; ++i)
    {
      y[i] = new int*[mb];
      for (int j = 0; j < mb; ++j)
      {
        y[i][j] = new int[t];
      }
    }

    // yb = (int**) malloc((size_t) d * (size_t) mb * (size_t) sizeof(int));
    yb = new int*[d];
    for (int i = 0; i < d; ++i)
    {
      yb[i] = new int[mb];
    }

    // ws = (int**) malloc((size_t) d * (size_t) d * (size_t) sizeof(int));
    ws = new int*[d];
    for (int i = 0; i < d; ++i)
    {
      ws[i] = new int[d];
    }

    // e = (double**) malloc((size_t) numr * (size_t) n * (size_t) sizeof(int));
    e = new double*[numr];
    for (int i = 0; i < numr; ++i)
    {
      e[i] = new double[n];
    }

    // b = (int**) malloc((size_t) numb * (size_t) max_num_intervals * (size_t) sizeof(int));
    b = new int*[numb];
    for (int i = 0; i < numb; ++i)
    {
      b[i] = new int[max_num_intervals];
    }

    // q = (double**) malloc((size_t) numb * (size_t) max_num_intervals * (size_t) sizeof(int));
    q = new double*[numb];
    for (int i = 0; i < numb; ++i)
    {
      q[i] = new double[max_num_intervals];
    }

    // v = (int**) malloc((size_t) m * (size_t) t * (size_t) sizeof(int));
    v = new int*[m];
    for (int i = 0; i < m; ++i)
    {
      v[i] = new int[t];
    }

    // z = (double*) malloc((size_t) m * (size_t) sizeof(int));
    z = new double[m];
  }

  ~BEST()
  {
    // free(x);
    for (int i = 0; i < n_; ++i)
    {
      for (int j = 0; j < m_; ++j)
      {
        delete [] x[i][j];
      }
      delete [] x[i];
    }
    delete [] x;

    // free(r);
    for (int i = 0; i < n_; ++i)
    {
      for (int j = 0; j < d_; ++j)
      {
        for (int k = 0; k < m_; ++k)
        {
          for (int l = 0; l < mb_; ++l)
          {
            delete [] r[i][j][k][l];
          }
          delete [] r[i][j][k];
        }
        delete [] r[i][j];
      }
      delete [] r[i];
    }
    delete [] r;

    // free(w);
    for (int i = 0; i < n_; ++i)
    {
      for (int j = 0; j < d_; ++j)
      {
        for (int k = 0; k < m_; ++k)
        {
          for (int l = 0; l < mb_; ++l)
          {
            delete [] w[i][j][k][l];
          }
          delete [] w[i][j][k];
        }
        delete [] w[i][j];
      }
      delete [] w[i];
    }
    delete [] w;

    // free(y);
    for (int i = 0; i < d_; ++i)
    {
      for (int j = 0; j < mb_; ++j)
      {
        delete [] y[i][j];
      }
      delete [] y[i];
    }
    delete [] y;

    // free(yb);
    for (int i = 0; i < d_; ++i)
    {
      delete [] yb[i];
    }
    delete [] yb;

    // free(ws);
    for (int i = 0; i < d_; ++i)
    {
      delete [] ws[i];
    }
    delete [] ws;

    // free(e);
    for (int i = 0; i < numr_; ++i)
    {
      delete [] e[i];
    }
    delete [] e;

    // free(b);
    for (int i = 0; i < numb_; ++i)
    {
      delete [] b[i];
    }
    delete [] b;

    // free(q);
    for (int i = 0; i < numb_; ++i)
    {
      delete [] q[i];
    }
    delete [] q;

    // free(v);
    for (int i = 0; i < m_; ++i)
    {
      delete [] v[i];
    }
    delete [] v;

    // free(z);
    delete [] z;
  }
};

class Cplex : public Algorithm {
 public:
  ///
  Cplex() = default;

  /// Default destructor
  ~Cplex() = default;

  /// Schedule the avail task, one-by-one
  void ScheduleAvailTasks(std::list<Task*> avail_tasks, Solution& solution);

  ///
  void Run(void);

 private:
};  // end of class GreedyAlgorithm

#endif  // APPROXIMATIVE_SOLUTIONS_SRC_CPLEX_H_

