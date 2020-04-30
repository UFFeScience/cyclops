/**
 * \file src/solution/cplex.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2020
 *
 * This source file contains the methods from the \c Cplex class that run the mode of the
 * exact solution.
 */

#include "src/solution/cplex.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <ilcplex/ilocplex.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <string>
// #include "data.h"

// #include <list>
// #include <vector>
// #include <limits>
// #include <cmath>
// #include <memory>
// #include <ctime>
// #include <utility>

#include "src/model/data.h"
// #include "src/model/storage.h"

DECLARE_string(cplex_input_file);

#define PRECISAO 0.00001

ILOSTLBEGIN
void Cplex::Run() {
    bool DEPU = false;

  // if (argc < 2) {
  //   cout << "Correct call: ./gcp filename" << endl;
  //   exit(1);
  // }

  // Data data(argc, argv[1]);
  Data data(FLAGS_cplex_input_file);
  data.readData(); // lê arquivo da instância

  int n = data.getDimension(); // dimensao do problema (data.cpp)

  // impressao da matriz
  if (DEPU) {
    for (int i = 0; i < n; i++) {
	    for (int j = 0; j < n; j++) {
	      cout << data.getEdgeValue(i, j) << " ";
	    }
	    cout << endl;
    }
  }

  clock_t start;
  start = clock();

  // create model

  IloEnv env;
  IloModel model(env);

  // x_i_j: if the color j is assigned to the vertex i
  IloArray<IloBoolVarArray> x(env, n);
  IloArray<IloBoolVar> w(env, n); // if color w_j was used;

  // adding x_i_j variables
  for (int i = 0; i < n; i++) {
    // allocate another vector of pointer to binary variables for each x_i_j
    x[i] = IloBoolVarArray(env, n);
    for (int j = 0; j < n; j++) {
      // variable allocation
      x[i][j] = IloBoolVar(env, string("x_" + to_string(i) + "_" + to_string(j)).c_str());
      model.add(x[i][j]); // add variable to model
    }
  }

  // adding w_j variables
  for (int j = 0; j < n; j++) {
    // variable allocation
    w[j] = IloBoolVar(env, string("w_" + to_string(j)).c_str());
    model.add(w[j]);
    // fo += w[j];
  }

  // funcao objetivo
  IloExpr fo(env); // expressao numerica do CPLEX

  for (int j = 0; j < n; j++) {
    fo += w[j]; // constroi função objetivo
  }

  model.add(IloMinimize(env, fo, "f")); // adiciona função objetivo ao modelo

  IloConstraintArray constraints(env);  // vetor de ponteiros para restrições do CPLEX

  // (2) constraints
  for (int i = 0; i < n; i++) {
    IloExpr constraint(env);            // expressao numerica do CPLEX
    for (int j = 0; j < n; j++) {
      constraint += x[i][j];
    }
    constraints.add(constraint == 1);   // adiciona restrição ao vetor de restrições
    constraint.end();                   // libera memoria
  }

  int **edgeMatrix = data.getEdegeMatrix(); // captura matriz custo de distancias (data.cpp)

  // (3) constraints
  for (int i = 0; i < n - 1; i++) {
    for (int k = i + 1; k < n; k++) {
      for (int j = 0; j < n; j++) {
        if (edgeMatrix[i][k] == 1) {
          IloExpr constraint(env);
          constraint = x[i][j] + x[k][j];
          constraints.add(constraint <= w[j]);  // adiciona restrição ao vetor de restrições
          constraint.end();                     // libera memoria
        }
      }
    }
  }

  // (4) constraints
  for (int j = 0; j < n - 1; j++) {
    constraints.add(w[j] >= w[j + 1]);  // adiciona restrição ao vetor de restrições
  }

  // (5) constraints
  for (int j = 0; j < n; j++) {
    IloExpr constraint(env);
    for (int i = 0; i < n; i++) {
      constraint += x[i][j];
    }
    constraints.add(w[j] <= constraint);  // adiciona restrição ao vetor de restrições
    constraint.end();                     // libera memoria
  }

  model.add(constraints);                 // adiciona restrições ao modelo

  IloCplex solver(model);                 // aloca solver do modelo
  solver.exportModel("gcpModel.lp");      // escreve modelo no arquivo no formato .lp

  // Parametros do CPLEX
  // solver.setParam(IloCplex::WorkMem, 1024 * 2);   // tamanho de RAM utilizada maxima
  // solver.setParam(IloCplex::NodeFileInd, 2);      // quando a RAM acaba, 1-guarda nos na memoria e
  //                                                 //  compactado 2-guarda os nos em disco 3-guarda nos
  //                                                 //  em disco e compactado
  // solver.setParam(IloCplex::TiLim, 3600);         // tempo limite
  // solver.setParam(IloCplex::Threads, 1);          // Numero de threads
  // solver.setParam(IloCplex::MIPInterval, 100);    // Log a cada N nos
  // solver.setOut(env.getNullStream());             // Desabilitando saida do cplex

  // Otimiza
  try {
    solver.solve();
  } catch(IloException& e) {
    cout << e;
  }

  // Saida
  bool   solved;
  int    nNodes;
  // double res, lb, ub, time_s;
  double res, lb, time_s;
  try {
    solved = (solver.getStatus() == IloAlgorithm::Optimal);

    res    = solver.getObjValue();                                 // solução
    lb     = solver.getBestObjValue();                             // limite dual (inferior)
    nNodes = solver.getNnodes();                                   // numero de nos analisados na arvore de busca
    time_s = ((double) clock() - (double) start) / CLOCKS_PER_SEC; // tempo de processamento

    cout << "----- ACABOU ----" << endl;
    cout << "SOLVED?: " << solved << endl;
    cout << "TEMPO  : " << time_s << endl;
    cout << "LB     : " << lb << endl;
    cout << "UB     : " << res << endl;
    cout << "NOS    : " << nNodes << endl;
    cout << "Pares (Vértice, Cor) da solucao otima:" << endl;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        float x_i_j = solver.getValue(x[i][j]);             // retorna o valor da variavel
        if (x_i_j >= (1 - PRECISAO)) {
          cout << "(" << i << ", " << j << "), ";
        }
      }
    }
    cout << endl;
  } catch (IloCplex::Exception &e) {
    cout << e;
  }
  env.end();
}  // end of GreedyRandomizedConstructiveHeuristic::run() method
