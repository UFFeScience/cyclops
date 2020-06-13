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

  BEST(int n, int d, int m, int numr, int numb, int mb, int t, int max_num_intervals)
  {
    x = (int***) malloc((size_t) n * (size_t) d * (size_t) m * (size_t) sizeof(int));
    r = (int*****) malloc((size_t) n * (size_t) d * (size_t) m *  (size_t) mb * (size_t) t * (size_t) sizeof(int));
    w = (int*****) malloc((size_t) n * (size_t) d * (size_t) m *  (size_t) mb * (size_t) t * (size_t) sizeof(int));
    y = (int***) malloc((size_t) d * (size_t) mb * (size_t) t * (size_t) sizeof(int));
    yb = (int**) malloc((size_t) d * (size_t) mb * (size_t) sizeof(int));
    ws = (int**) malloc((size_t) d * (size_t) d * (size_t) sizeof(int));
    e = (double**) malloc((size_t) numr * (size_t) n * (size_t) sizeof(int));
    b = (int**) malloc((size_t) numb * (size_t) max_num_intervals * (size_t) sizeof(int));
    q = (double**) malloc((size_t) numb * (size_t) max_num_intervals * (size_t) sizeof(int));
    v = (int**) malloc((size_t) m * (size_t) t * (size_t) sizeof(int));
    z = (double*) malloc((size_t) m * (size_t) sizeof(int));
  }

  ~BEST()
  {
    free(x);
    free(r);
    free(w);
    free(y);
    free(yb);
    free(ws);
    free(e);
    free(b);
    free(z);
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

