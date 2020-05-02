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
  IloArray<IloBoolVarArray> v;
  IloNumVarArray z;
  IloNumVarArray z_max;
  
  CPLEX(int n, int dd, int d, int m) :
    env(),
    model(env),
    x(env, n),
    r(env, n),
    w(env, n),
    y(env, d),
    yb(env, d),
    v(env, m),
    z(env, m),
    z_max(env, 1) {}
 
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

