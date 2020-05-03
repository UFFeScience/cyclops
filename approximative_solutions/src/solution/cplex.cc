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
  // bool DEPU = false;

  // nome das variaveis
  char var_name[100];

  // Estrutura do Cplex (ambiente, modelo e variaveis)
  int n = static_cast<int>(GetTaskSize() - 2);  // less source and target
  int d = static_cast<int>(GetFileSize());
  int m = static_cast<int>(GetVirtualMachineSize());
  int numr = static_cast<int>(GetRequirementsSize());
  int numb = static_cast<int>(get_bucket_size());

  struct CPLEX cplx(n, d, m, numr, numb);

  // variaveis de execucao
  // X_IJT => a tarefa I que esta na maquina J, comeca a executar no periodo T
  for (int i = 0; i < n; i++) {
    cplx.x[i] =  IloArray<IloBoolVarArray>(cplx.env, m);
    for (int j = 0; j < m; j++) {
      cplx.x[i][j] = IloBoolVarArray(cplx.env, makespan_max_);
      for (int k = 0; k < makespan_max_; k++) {
      sprintf (var_name, "x_%d_%d_%d", (int)i,(int)j, (int)k);              // nome da variavel
      cplx.x[i][j][k] = IloBoolVar(cplx.env, var_name);                     // aloca variavel
      cplx.model.add(cplx.x[i][j][k]);                                      // adiciona variavel ao modelo
      }
    }
  }

  // variaveis de leitura
  // R_IDJPT => a tarefa I que esta na maquina J, comeca a ler o seu D-esimo dado de entrada
  // (note que nao eh o dado de indice D) a partir da maquina P no periodo T
  for(int i=0; i < n; i++)
    {
      cplx.r[i] =  IloArray<IloArray<IloArray<IloBoolVarArray>>>(cplx.env, data.d_in_tam[i]);
      for(int j=0; j < data.d_in_tam[i]; j++)
	    {
	      cplx.r[i][j] = IloArray<IloArray<IloBoolVarArray>>(cplx.env, data.m);
	      for(int k=0; k < data.m; k++)
	      {
	        cplx.r[i][j][k] = IloArray<IloBoolVarArray>(cplx.env, data.m);
	        for(int l=0; l < data.m; l++)
		      {
		        cplx.r[i][j][k][l] = IloBoolVarArray(cplx.env, data.t);
		        for(int m=0; m < data.t; m++)
		        {
		          sprintf (var_name, "r_%d_%d_%d_%d_%d", (int)i,(int)j, (int)k, (int)l, (int)m);            // nome da variavel
		          cplx.r[i][j][k][l][m] = IloBoolVar(cplx.env, var_name);                                   // aloca variavel 
		          cplx.model.add(cplx.r[i][j][k][l][m]);                                                    // adiciona variavel ao modelo
		        }
		      }
	      }
	    }
    }

  IloCplex solver(cplx.model);                        // declara variável "solver" sobre o modelo a ser solucionado
  solver.exportModel("model.lp");                     // escreve modelo no arquivo no formato .lp



}
