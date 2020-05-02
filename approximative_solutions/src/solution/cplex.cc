/**
 * \file src/solution/cplex.cc
 * \brief Contains the \c Algorithm class methods.
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \authors Yuri Frota \<abitbol@gmail.com\>
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
  
    // nome das variaveis
    char var_name[100];

    // Estrutura do Cplex (ambiente, modelo e variaveis)
    struct CPLEX cplx(int n, int d, int m, int numvert, int numr, int numb); 
 
 /* **** RODIRGO **** n: numero de tarefas, d: numero de dados, m: numero de maquinas, numvert: numero de vertices do grafo de 
 conflito, numr: numero de  requerimentos de segurança, numb: numero de buckts */

/* **** RODRIGO **** t: tempo maximo */

    // variaveis de execucao
  // X_IJT => a tarefa I que esta na maquina J, comeca a executar no periodo T
  for(int i=0; i < n; i++)
    {
      cplx.x[i] =  IloArray<IloBoolVarArray>(cplx.env, m);
      for(int j=0; j < m; j++)
	    {
	       cplx.x[i][j] = IloBoolVarArray(cplx.env, t);
	       for(int k=0; k < t; k++)
	       {
	        sprintf (var_name, "x_%d_%d_%d", (int)i,(int)j, (int)k);              // nome da variavel
	        cplx.x[i][j][k] = IloBoolVar(cplx.env, var_name);                     // aloca variavel 
	        cplx.model.add(cplx.x[i][j][k]);                                      // adiciona variavel ao modelo
	       }
	    }
    } 

  
  IloCplex solver(cplx.model);                        // declara variável "solver" sobre o modelo a ser solucionado
  solver.exportModel("model.lp");                     // escreve modelo no arquivo no formato .lp


  
}  
