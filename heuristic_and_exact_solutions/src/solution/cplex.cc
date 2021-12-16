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

#include "src/model/data.h"
#include "src/model/dynamic_file.h"
#include "src/model/static_file.h"

DECLARE_string(cplex_output_file);

// #define PRECISAO 0.00001

template<typename T= VirtualMachine>
T dynamic_pointer_cast(Storage);

/**
 * If file reside in the same vm, then the transfer time is 0.0.
 * Otherwise, calculate the transfer time between them using the smallest bandwidth.
 *
 *   \f[
 *      time = \lceil{\min_{storage1.bandwidth(), storage2.bandwidth()} bandwidth}\rceil
 *   \f]
 *
 * Where:
 * \f$ time \f$ is the transfer time between the \c storage1 and \c storage2
 * \f$ storage1.bandwidth() \f$ is the transfer rate of the \c storage1
 * \f$ storage2.bandwidth() \f$ is the transfer rate of the \c storage2
 * \f$ bandwidth \f$ is the minimal transfer rate between the storage1.bandwidth() and storage2.bandwidth()
 *
 * \param[in]  file          File to be transfered
 * \param[in]  storage1      Storage origin/destination
 * \param[in]  storage2      Storage origin/destination
 * \retval     time          The time to transfer \c file from \c file_vm to \c vm with possible
 *                           applied penalts
 */
int ComputeFileTransferTime(const std::shared_ptr<File>& file,
                            const std::shared_ptr<Storage>& storage1,
                            const std::shared_ptr<Storage>& storage2) {
  int time = 0;

  DLOG(INFO) << "Compute the transfer time of File[" << file->get_id() << "] to/from VM["
      << storage1->get_id() << "] to Storage[" << storage2->get_id() << "]";

  // Calculate time
  if (storage1->get_id() != storage2->get_id()) {
    // get the smallest link
    double link = std::min(storage1->get_bandwidth_GBps(), storage2->get_bandwidth_GBps());
    time = static_cast<int>(std::ceil(file->get_size_in_GB() / link));
    // std::cout << "storage1->get_bandwidth_GBps(): " << storage1->get_bandwidth_GBps() << "\n"
    //           << "storage2->get_bandwidth_GBps(): " << storage2->get_bandwidth_GBps() << "\n"
    //           << "std::ceil(file->get_size_in_GB(): " << std::ceil(file->get_size_in_GB()) << "\n"
    //           << "link: " << link << "\n"
    //           << "time: " << time << std::endl;
    // time = static_cast<int>(std::ceil(file->get_size() / link));
    // time = file->get_size() / link;
  }
  else
  {
    time = 1;
  }


  DLOG(INFO) << "tranfer_time: " << time;
  return time;
}  // double Solution::FileTransferTime(File file, Storage vm1, Storage vm2) {

// ---- VARIAVEIS PARA IMPRESSAO DA SOLUCAO ---
// int * tempo;
// int ** maq_dado;

// tempo    = new int [m];
// maq_dado = new int * [m];
// for (int i=0; i < m; i++)
// {
//   maq_dado[i] = new int [d];
// }

// delete [] tempo;
// for (int i=0; i < m; i++)
// {
//   delete [] maq_dado[i];
// }
// delete [] maq_dado;

void print_sol(struct BEST* data)
{
  Algorithm* algorithm = data->algorithm_;
  // ---- VARIAVEIS PARA IMPRESSAO DA SOLUCAO ---
  // int * tempo;
  // int ** maq_dado;
  int* tempo = new int[static_cast<size_t>(data->m_)];
  int** maq_dado = new int*[static_cast<size_t>(data->mb_)];

  for (int i = 0; i < data->mb_; i++)
  {
    maq_dado[i] = new int [static_cast<size_t>(data->d_)];
  }

  // inicializacao
  for (int i = 0; i < data->m_; i++)
  {
    tempo[i] = 0;
  }  // Vetor para TESTE que guarda o tempo em que a maquina passa realizando a sua acao (exec., leit., escrit.) corrente

  for (int i = 0; i < data->m_; i++)
  {
    for (int j = 0; j < data->d_; j++)
    {
      maq_dado[i][j] = -1;
    }  // Guarda os arquivos por maquina
  }

  std::cout << "---------------- Linha do Tempo --------------" << std::endl;
  std::cout << "t)\t";
  // for (int j = 0; j < data->m_; j++)
  for (int j = 0; j < data->mb_; j++)
  {
    std::cout << "(M" << j << ")\t" << "(Y)\t" << "(V)\t";
  }
  std::cout << std::endl;
  std::cout << std::endl;

  // std::cout << data->t_ << std::endl;

  // TEMPO
  for (size_t checa = 0, t1 = 0; t1 < static_cast<size_t>(data->t_) and checa <= 1UL; t1++)
  {
    std::cout << "t" << t1 << ")\t";

    // // MAQUINA
    // DEVICES
    for (int j = 0; j < data->mb_ and checa <= 1; j++)
    {
      std::shared_ptr<Storage> device = algorithm->GetStoragePerId(static_cast<size_t>(j));
      // if (Bucket* bucket = dynamic_cast<Bucket*>(storage))
      //{
      //dynamic_cast<Bucket*>(
      if (auto virtual_machine = dynamic_pointer_cast<VirtualMachine>(device))
      {
        // checa se inicia mais de uma acao no mesmo tempo na mesma maquina
        checa = 0;

        // durante uma acao (exec, leitura, escrita)
        tempo[j] = tempo[j] - 1;

        if (tempo[j] > 0)
        {
          checa += 1;
          std::cout << "(" << tempo[j] << ")" << "\t";
        }

        // execucao
        for (size_t passou = 0, i = 0; i < static_cast<size_t>(data->n_) and checa <= 1UL; i++)
        {
          std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(1UL + i));
          std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

          auto x_ijt = static_cast<float>(data->x[i][j][t1]);             // retorna o valor da variavel

          if (x_ijt >= (1 - data->PRECISAO))
          {
            checa += 1;
            std::cout << "x" << i << "\t";

            if (tempo[j] > 0)
            {
              std::cout << "*** EXECUTOU EM CIMA DE ALGO ***";
              checa += 1;
              break;
            }

            // tempo[j] = t_ij(i, j);  // tempo de execucao?
            tempo[j] = std::ceil(task->get_time() * virtual_machine->get_slowdown());

            // checa se as leituras foram feitas pela maquina antes da execucao
            // for (int d1 = 0; d1 < d_in_tam[i] and checa <= 1; d1++)
            for (int d1 = 0; d1 < static_cast<int>(input_files.size()) and checa <= 1; d1++)
            {
              passou = 0;

              // for (int k = 0; k < data->m_ and checa <= 1; k++)
              for (int k = 0; k < data->mb_ and checa <= 1; k++)
              {
                for (int q = 0; q < data->t_ and checa <= 1; q++)
                {
                  auto r_idjpt = static_cast<float>(data->r[i][d1][j][k][q]);  // retorna o valor da variavel

                  if (r_idjpt >= (1 - data->PRECISAO))
                  {
                    passou = 1;
                  }
                }
              }

              if (passou == 0)
              {
                std::cout << "*** EXECUTOU A TAREFA SEM TER LIDO TODAS AS ENTRADAS *** faltou dado="
                          // << d_in[i][d1];
                          << input_files[static_cast<size_t>(d1)]->get_id()
                          << "\n" << input_files[static_cast<size_t>(d1)];
                checa += 1;
                break;
              }
            }
            // ----------------------------------------------------------------
          }

          if (checa > 1)
          {
            std::cout << "*** CONFLITO COM EXECUCAO ***";
            break;
          }
        }  // for (int passou = 0, i = 0; i < data->n_ and checa <= 1; i++)
                // leitura
        for (size_t i = 0; i < static_cast<size_t>(data->n_) and checa <= 1; i++)
        {
          std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(1UL + i));
          std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

          // for (int d1 = 0; d1 < d__in_tam[i] and checa <= 1; d1++)
          for (int d1 = 0; d1 < static_cast<int>(input_files.size()) and checa <= 1; d1++)
          {
            std::shared_ptr<File> file = input_files[static_cast<size_t>(d1)];

            for (size_t k = 0UL; k < static_cast<size_t>(data->mb_) and checa <= 1UL; k++)
            {
              std::shared_ptr<Storage> storage = algorithm->GetStoragePerId(static_cast<size_t>(k));

              auto r_idjpt = static_cast<float>(data->r[i][d1][j][k][t1]);  // retorna o valor da variavel

              if (r_idjpt >= (1 - data->PRECISAO))
              {
                int tempo_leitura = ComputeFileTransferTime(file, virtual_machine, storage);

                checa += 1;
                // std::cout << "r" << i << "(" << d_in[i][d1] << ")<" << k << "\t";
                std::cout << "r" << i << "(" << input_files[static_cast<size_t>(d1)]->get_id() << ")<" << k << "\t";

                if (tempo[j] > 0)
                {
                  std::cout << "*** LEU EM CIMA DE ALGO ***";
                  checa += 1;
                  break;
                }

                // tempo[j] = t_djp(d_in[i][d1],j,k);
                tempo[j] = tempo_leitura;

                // checa existencia de arquivo
                // float y_djt = data->y[d_in[i][d1]][k][t1];             // retorna o valor da variavel
                auto y_djt = static_cast<float>(data->y[static_cast<int>(file->get_id())][k][t1]);  // retorna o valor da variavel

                if (y_djt < (1 - data->PRECISAO))
                {
                  std::cout << "*** LEU ARQUIVO INEXISTENTE NA MAQUINA ***";
                  checa += 1;
                  break;
                }
                // ---------------------------

              }

              if (checa > 1)
              {
                std::cout << "*** CONFLITO COM LEITURA ***";
                break;
              }
            }
          }
        }

        // escrita
        for (size_t passou = 0UL, i = 0UL; i < static_cast<size_t>(data->n_) and checa <= 1; i++)
        {
          std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(1UL + i));
          std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

          // for (int d1 = 0; d1 < d_out_tam[i] and checa <= 1; d1++)
          for (int d1 = 0; d1 < static_cast<int>(output_files.size()) and checa <= 1; d1++)
          {
            std::shared_ptr<File> file = output_files[static_cast<size_t>(d1)];

            for (int k = 0; k < data->mb_ and checa <= 1; k++)
            {
              std::shared_ptr<Storage> storage = algorithm->GetStoragePerId(static_cast<size_t>(k));

              auto w_idjpt = static_cast<float>(data->w[i][d1][j][k][t1]);  // retorna o valor da variavel

              if (w_idjpt >= (1 - data->PRECISAO))
              {
                int tempo_escrita = ComputeFileTransferTime(file, virtual_machine, storage);

                checa += 1;
                // std::cout << "w" << i << "(" << d_out[i][d1] << ")>" << k << "\t";
                std::cout << "w" << i << "(" << output_files[static_cast<size_t>(d1)]->get_id() << ")<" << k << "\t";

                if (tempo[j] > 0)
                {
                  std::cout << "*** ESCREVEU EM CIMA DE ALGO ***";
                  checa += 1;
                  break;
                }

                // tempo[j] = t_djp(d_out[i][d1],j,k);
                tempo[j] = tempo_escrita;

                // checa se a tarefa foi executada na maquina antes
                passou = 0;
                for (int q = 0; q < data->t_ and checa <= 1; q++)
                {
                  auto x_ijt = static_cast<float>(data->x[i][j][q]);

                  if (x_ijt >= (1 - data->PRECISAO))
                  {
                    passou = 1;
                  }
                }

                if (passou == 0)
                {
                  std::cout << "*** ESCREVEU DADO SEM TER EXECUTADO A TAREFA NA MAQUINA ***";
                  checa += 1;
                  break;
                }

                // -----------------------------------------------

              }

              if (checa > 1)
              {
                std::cout << "*** CONFLITO COM ESCRITA ***";
                break;
              }
            }
          }
        }

        // maquina ociosa neste tempo
        if (checa == 0)
        {
          std::cout << "-\t";
        }

        // dados
        for (int d1 = 0; d1 < data->d_ and checa <= 1; d1++)
        {
          auto y_djt = static_cast<float>(data->y[d1][j][t1]);  // retorna o valor da variavel

          if (y_djt >= (1 - data->PRECISAO))
          {
            maq_dado[j][d1] = 1;
          }

          if (maq_dado[j][d1] == 1)
          {
            std::cout << d1 << ",";
          }
        }

        std::cout << "\t";

        // contratacao
        auto v_jt = static_cast<float>(data->v[j][t1]);             // retorna o valor da variavel

        if (v_jt >= (1 - data->PRECISAO))
        {
          std::cout << "*\t";
        }
        else
        {
          std::cout << "\t";
        }
      }
      else
      {
        // maquina ociosa neste tempo
        // if (checa == 0)
        // {
          std::cout << "N\\A\t";
        // }

        // // dados
        for (int d1 = 0; d1 < data->d_ and checa <= 1; d1++)
        {
          auto y_djt = static_cast<float>(data->y[d1][j][t1]);  // retorna o valor da variavel

          if (y_djt >= (1 - data->PRECISAO))
          {
            maq_dado[j][d1] = 1;
          }

          if (maq_dado[j][d1] == 1)
          {
            std::cout << d1 << ",";
          }
        }

        std::cout << "\t";

        // // contratacao
        // float v_jt = data->v[j][t1];             // retorna o valor da variavel

        // if (v_jt >= (1 - data->PRECISAO))
        // {
          std::cout << "N\\A\t";
        // }
        // else
        // {
          // std::cout << "\t";
        // }
      }

    }  // for(int j = 0; j < m and checa <= 1; j++)

    std::cout << std::endl;
  }  // for(int checa = 0, t = 0; t < t and checa <= 1; t++)

  std::cout << "---------------------------------------------" << std::endl;

  // Liberando recursos

  delete [] tempo;
  for (int i = 0; i < data->m_; i++)
  {
    delete [] maq_dado[i];
  }
  delete [] maq_dado;
}







void ImprimeBest(struct BEST* data)
{
  Algorithm* algorithm = data->algorithm_;

  // std::cout << "y[" << 1 << "]["
  //                           << 2 << "]["
  //                           << 18 << "] = " << data->y[1][2][18] << std::endl;

  // std::cout << "y[" << 1 << "]["
  //                           << 2 << "]["
  //                           << 19 << "] = " << data->y[1][2][19] << std::endl;

  // // w_0_0_2_2_18
  // std::cout << "w[" << 0 << "]["
  //                           << 0 << "]["
  //                           << 2 << "]["
  //                           << 2 << "]["
  //                           << 18 << "] = " << data->w[0][0][2][2][18] << std::endl;
  // -------- x ----------
  for (int i = 0; i < data->n_; ++i)
  {
    for (int j = 0; j < data->m_; ++j)
    {
      for (int t = 0; t < data->t_; ++t)
      {
        if (data->x[i][j][t] > data->PRECISAO)
        {
          std::cout << "x[" << i << "][" << j << "][" << t << "] = " << data->x[i][j][t] << std::endl;
        }
      }
    }
  }

  // -------- r ----------
  for (size_t i = 0UL; i < static_cast<size_t>(data->n_); ++i)
  {
    std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(i + 1));
    std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

    // for (int j = 0; j < data->d_; ++j)
    for (int j = 0; j < static_cast<int>(input_files.size()); ++j)
    {
      for (int k = 0; k < data->m_; ++k)
      {
        for (int l = 0; l < data->mb_; ++l)
        {
          for (int m = 0; m < data->t_; ++m)
          {
            if (data->r[i][j][k][l][m] > data->PRECISAO)
            {
              std::cout << "r[" << i << "]["
                                << j << "]["
                                << k << "]["
                                << l << "]["
                                << m << "] = " << data->r[i][j][k][l][m] << std::endl;
            }
          }
        }
      }
    }
  }

  // -------- w ----------
  for (size_t i = 0UL; i < static_cast<size_t>(data->n_); ++i)
  {
    std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(i + 1));
    std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

    // for (int j = 0; j < data->d_; ++j)
    for (int j = 0; j < static_cast<int>(output_files.size()); ++j)
    {
      for (int k = 0; k < data->m_; ++k)
      {
        for (int l = 0; l < data->mb_; ++l)
        {
          for (int m = 0; m < data->t_; ++m)
          {
            if (data->w[i][j][k][l][m] > data->PRECISAO)
            {
              std::cout << "w[" << i << "]["
                                << j << "]["
                                << k << "]["
                                << l << "]["
                                << m << "] = " << data->w[i][j][k][l][m] << std::endl;
            }
          }
        }
      }
    }
  }

  // -------- y ----------
  for (int i = 0; i < data->d_; ++i)
  {
    for (int j = 0; j < data->mb_; ++j)
    {
      for (int k = 0; k < data->t_; ++k)
      {
        if (data->y[i][j][k] > data->PRECISAO)
        {
          std::cout << "y[" << i << "]["
                            << j << "]["
                            << k << "] = " << data->y[i][j][k] << std::endl;
        }
      }
    }
  }

  // -------- yb ----------
  for (int i = 0; i < data->d_; ++i)
  {
    for (int j = 0; j < data->mb_; ++j)
    {
      if (data->yb[i][j] > data->PRECISAO)
      {
        std::cout << "yb[" << i << "]["
                           << j << "] = " << data->yb[i][j] << std::endl;
      }
    }
  }

  // -------- ws ----------
  for (int i = 0; i < data->d_ - 1; ++i)
  {
    for (int j = i + 1; j < data->d_; ++j)
    {
      int conflict = algorithm->get_conflict_graph().ReturnConflict(static_cast<size_t>(i), static_cast<size_t>(j));

      if (conflict > 0)
      {  // soft constraint
        if (data->ws[i][j] > data->PRECISAO)
        {
          std::cout << "ws[" << i << "]["
                             << j << "] = " << data->ws[i][j] << std::endl;
        }
      }
    }
  }

  // -------- e ----------
  for (int i = 0; i < data->numr_; ++i)
  {
    for (int j = 0; j < data->n_; ++j)
    {
      if (data->e[i][j] > data->PRECISAO)
      {
        std::cout << "e[" << i << "]["
                          << j << "] = " << data->e[i][j] << std::endl;
      }
    }
  }

  // -------- b ----------
  for (int i = 0; i < data->numb_; ++i)
  {
    std::shared_ptr<Storage> storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + static_cast<size_t>(i));

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      for (int j = 0; j <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); ++j)
      {
        if (data->b[i][j] > data->PRECISAO)
        {
          std::cout << "b[" << i << "]["
                            << j << "] = " << data->b[i][j] << std::endl;
        }
      }
    } else {
      exit(1);  // error
    }
  }

  // -------- q ----------
  for (int i = 0; i < data->numb_; ++i)
  {
    std::shared_ptr<Storage> storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + static_cast<size_t>(i));

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      for (int j = 0; j <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); ++j)
      {
        if (data->q[i][j] > data->PRECISAO)
        {
          std::cout << "q[" << i << "]["
                            << j << "] = " << data->q[i][j] << std::endl;
        }
      }
    } else {
      exit(1);  // error
    }
  }

  // -------- v ----------
  for (int i = 0; i < data->m_; ++i)
  {
    for (int j = 0; j < data->t_; ++j)
    {
      if (data->v[i][j] > data->PRECISAO)
      {
        std::cout << "v[" << i << "]["
                          << j << "] = " << data->v[i][j] << std::endl;
      }
    }
  }

  // -------- z ----------
  for (int i = 0; i < data->m_; ++i)
  {
    if (data->z[i] > data->PRECISAO)
    {
      std::cout << "z[" << i << "] = " << data->z[i] << std::endl;
    }
  }

  // -------- z_max ----------
  if (data->z_max > data->PRECISAO)
  {
    std::cout << "z_max" << " = " << data->z_max << std::endl;
  }
}

// Callback da melhor solucao encontrada
ILOINCUMBENTCALLBACK2(CB_incub_sol, struct BEST*, data, struct CPLEX*, cplx)
{
  auto                           val_sol = (double) getObjValue();
  IloCplex::MIPCallbackI::NodeId no_sol  = getNodeId();
  double                         gap     = 100.0 * ((double) getMIPRelativeGap());
  double                         time_f  = ((double) clock() - (double)data->start) / CLOCKS_PER_SEC;
  bool                           DEPU    = true;

  Algorithm* algorithm = data->algorithm_;

  if (DEPU)
  {
    std::cout << "------------------ ACHOU NOVA SOLUCAO ----------------------" << std::endl;
    std::cout << "no      = " << no_sol << std::endl;
    std::cout << "sol     = " << val_sol << std::endl;
    std::cout << "gap     = " << gap << std::endl;
    std::cout << "tempo   = " << time_f << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
  }

  if (val_sol + data->PRECISAO < data->b_sol)
  {
    data->b_sol = val_sol;

    // -------- x ----------
    for (int i = 0; i < data->n_; ++i)
    {
      for (int j = 0; j < data->m_; ++j)
      {
        for (int t = 0; t < data->t_; ++t)
        {
          data->x[i][j][t] = static_cast<int>((float) getValue(cplx->x[i][j][t]));
          //cout<<"************** X_"<<i<<"_"<<j<<"_"<<t<<endl;
        }
      }
    }

    // -------- r ----------
    for (int i = 0; i < data->n_; ++i)
    {
      std::shared_ptr<Activation> task =
          algorithm->GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
      std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

      // for (int j = 0; j < data->d_; ++j)
      for (int j = 0; j < static_cast<int>(input_files.size()); ++j)
      {
        for (int k = 0; k < data->m_; ++k)
        {
          for (int l = 0; l < data->mb_; ++l)
          {
            for (int m = 0; m < data->t_; ++m)
            {
              data->r[i][j][k][l][m] = static_cast<int>((float) getValue(cplx->r[i][j][k][l][m]));
            }
          }
        }
      }
    }

    // -------- w ----------
    for (int i = 0; i < data->n_; ++i)
    {
      std::shared_ptr<Activation>
          task = algorithm->GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
      std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

      // for (int j = 0; j < data->d_; ++j)
      for (int j = 0; j < static_cast<int>(output_files.size()); ++j)
      {
        for (int k = 0; k < data->m_; ++k)
        {
          for (int l = 0; l < data->mb_; ++l)
          {
            for (int m = 0; m < data->t_; ++m)
            {
              data->w[i][j][k][l][m] = static_cast<int>((float) getValue(cplx->w[i][j][k][l][m]));
            }
          }
        }
      }
    }

    // -------- y ----------
    for (int i = 0; i < data->d_; ++i)
    {
      for (int j = 0; j < data->mb_; ++j)
      {
        for (int k = 0; k <= data->t_; ++k)
        {
          data->y[i][j][k] = static_cast<int>((float) getValue(cplx->y[i][j][k]));
        }
      }
    }

    // -------- yb ----------
    for (int i = 0; i < data->d_; ++i)
    {
      for (int j = 0; j < data->mb_; ++j)
      {
        data->yb[i][j] = static_cast<int>((float) getValue(cplx->yb[i][j]));
      }
    }

    // -------- ws ----------
    for (int i = 0; i < data->d_ - 1; ++i)
    {
      for (int j = i + 1; j < data->d_; ++j)
      {
        int conflict = algorithm->get_conflict_graph().ReturnConflict(static_cast<size_t>(i), static_cast<size_t>(j));

        if (conflict > 0) {  // soft constraint
          data->ws[i][j] = static_cast<int>((float) getValue(cplx->ws[i][j]));
        }
      }
    }

    // -------- e ----------
    for (int i = 0; i < data->numr_; ++i)
    {
      for (int j = 0; j < data->n_; ++j)
      {
        data->e[i][j] = (float) getValue(cplx->e[i][j]);
      }
    }

    // -------- b ----------
    for (int i = 0; i < data->numb_; ++i)
    {
      std::shared_ptr<Storage>
          storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + static_cast<size_t>(i));

      if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
      {
        for (int j = 0; j <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); ++j)
        {
          data->b[i][j] = static_cast<int>((float) getValue(cplx->b[i][j]));
        }
      } else {
        exit(1);  // error
      }
    }

    // -------- q ----------
    for (int i = 0; i < data->numb_; ++i)
    {
      std::shared_ptr<Storage>
          storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + static_cast<size_t>(i));

      if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
      {
        for (int j = 0; j <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); ++j)
        {
          data->q[i][j] = (float) getValue(cplx->q[i][j]);
        }
      } else {
        exit(1);  // error
      }
    }

    // -------- v ----------
    for (int i = 0; i < data->m_; ++i)
    {
      for (int j = 0; j < data->t_; ++j)
      {
        data->v[i][j] = static_cast<int>((float) getValue(cplx->v[i][j]));
      }
    }

    // -------- z ----------
    for (int i = 0; i < data->m_; ++i)
    {
      data->z[i] = (float) getValue(cplx->z[i]);
    }

    // -------- z_max ----------
    data->z_max = (float) getValue(cplx->z_max[0]);
  }
}


ILOSTLBEGIN
void Cplex::Run() {
  // bool DEPU = false;

  // tempo
  // clock_t t_start = clock();

  // nome das variaveis
  char var_name[100];

  // Estrutura do Cplex (ambiente, modelo e variaveis)
  int _n       = static_cast<int>(GetTaskSize() - 2);  // less source and target
  int _d       = static_cast<int>(GetFileSize());
  int _m       = static_cast<int>(GetVirtualMachineSize());
  int _mb      = static_cast<int>(GetStorageSize());
  int _numr    = static_cast<int>(GetRequirementsSize());
  int _numb    = static_cast<int>(get_bucket_size());
  int _t       = static_cast<int>(makespan_max_);
  double _cmax = get_budget_max();
  double _smax = get_maximum_security_and_privacy_exposure();

  int max_num_intervals = 2;

  struct CPLEX cplx(_n, _d, _m, _numr, _numb);
  struct BEST best(_n, _d, _m, _numr, _numb, _mb, _t, max_num_intervals, this);

  // variaveis de execucao
  // X_IJT => a tarefa I que esta na maquina J, comeca a executar no periodo T
  for (int i = 0; i < _n; i++)
  {
    cplx.x[i] =  IloArray<IloBoolVarArray>(cplx.env, _m);
    for (int j = 0; j < _m; j++)
    {
      cplx.x[i][j] = IloBoolVarArray(cplx.env, _t);
      for (int k = 0; k < _t; k++)
      {
        sprintf(var_name, "x_%d_%d_%d", (int) i,(int) j, (int) k);     // nome da variavel
        cplx.x[i][j][k] = IloBoolVar(cplx.env, var_name);                     // aloca variavel
        cplx.model.add(cplx.x[i][j][k]);                                      // adiciona variavel ao modelo
      }
    }
  }

  // variaveis de leitura
  // R_IDJPT => a tarefa I que esta na maquina J, comeca a ler o seu D-esimo dado de entrada
  // (note que nao eh o dado de indice D) a partir da maquina P no periodo T
  for (int i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>> input_files = task->get_input_files();
    cplx.r[i] = IloArray<IloArray<IloArray<IloBoolVarArray>>>(cplx.env, static_cast<int>(input_files.size()));

    for (int j = 0; j < static_cast<int>(input_files.size()); j++)
    {
      cplx.r[i][j] = IloArray<IloArray<IloBoolVarArray>>(cplx.env, _m);
      for (int k = 0; k < _m; k++)
      {
        cplx.r[i][j][k] = IloArray<IloBoolVarArray>(cplx.env, _mb);
        for (int l = 0; l < _mb; l++)
        {
          cplx.r[i][j][k][l] = IloBoolVarArray(cplx.env, _t);
          for (int m = 0; m < _t; m++)
          {
            sprintf (var_name, "r_%d_%d_%d_%d_%d", (int) i, (int) j, (int) k, (int) l, (int) m);  // nome da variavel
            cplx.r[i][j][k][l][m] = IloBoolVar(cplx.env, var_name);  // aloca variavel
            cplx.model.add(cplx.r[i][j][k][l][m]);  // adiciona variavel ao modelo
          }
        }
      }
    }
  }

  // variaveis de escrita
  // W_IDJPT => a tarefa I que esta na maquina J, comeca a escrever o seu D-esimo dado de entrada
  // (note que nao eh o dado de indice D) a partir da maquina P no periodo T
  for (int i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>> output_files = task->get_output_files();
    cplx.w[i] = IloArray<IloArray<IloArray<IloBoolVarArray>>>(cplx.env, static_cast<int>(output_files.size()));

    for (int j = 0; j < static_cast<int>(output_files.size()); j++)
    {
      cplx.w[i][j] = IloArray<IloArray<IloBoolVarArray>>(cplx.env, _m);
      for (int k = 0; k < _m; k++)
      {
	      cplx.w[i][j][k] = IloArray<IloBoolVarArray>(cplx.env, _mb);
	      for (int l = 0; l < _mb; l++)
        {
          cplx.w[i][j][k][l] = IloBoolVarArray(cplx.env, _t);
          for (int m = 0; m < _t; m++)
          {
            sprintf (var_name, "w_%d_%d_%d_%d_%d", (int) i, (int) j, (int) k, (int) l, (int) m);      // nome da variavel
            cplx.w[i][j][k][l][m] = IloBoolVar(cplx.env, var_name);                                   // aloca variavel
            cplx.model.add(cplx.w[i][j][k][l][m]);                                                    // adiciona variavel ao modelo
          }
        }
      }
    }
  }

  // variaveis de armazenamento
  // Y_DJT => indica se o dado de indice D esta armazenado no device J no periodo T
  for (int i = 0; i < _d; i++)
  {
    cplx.y[i] =  IloArray<IloBoolVarArray>(cplx.env, _mb);

    for (int j = 0; j < _mb; j++)
    {
      cplx.y[i][j] = IloBoolVarArray(cplx.env, _t + 1);

      for (int k = 0; k <= _t; k++)
      {
        sprintf(var_name, "y_%d_%d_%d", (int) i, (int) j, (int) k);                    // nome da variavel
        cplx.y[i][j][k] = IloBoolVar(cplx.env, var_name);                              // aloca variavel
        cplx.model.add(cplx.y[i][j][k]);                                               // adiciona variavel ao modelo
      }
    }
  }


  // variaveis de armazenamento independente de maquina
  // Y_DJ => indica se o dado de indice D existe em algum tempo na maquina J
  for (int i = 0; i < _d; i++)
  {
    cplx.yb[i] =  IloBoolVarArray(cplx.env, _mb);
    for(int j = 0; j < _mb; j++)
    {
      sprintf(var_name, "yb_%d_%d", (int) i, (int) j);                     // nome da variavel
      cplx.yb[i][j] = IloBoolVar(cplx.env, var_name);                      // aloca variavel
      cplx.model.add(cplx.yb[i][j]);                                       // adiciona variavel ao modelo
    }
  }


  // variaveis de penalidade de violacao de uma aresta soft
  // W_D1D2 => violacao da aresta soft di, d2 \in E_s
  for (int d1 = 0; d1 < _d - 1; d1++)
  {
    cplx.ws[d1] =  IloBoolVarArray(cplx.env, _d);

    for (int d2 = d1 + 1; d2 < _d; d2++)
    {
      int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

      if (conflict > 0) {  // soft constraint
        sprintf(var_name, "ws_%d_%d", (int) d1, (int) d2);                     // nome da variavel
        cplx.ws[d1][d2] = IloBoolVar(cplx.env, var_name);                      // aloca variavel
        cplx.model.add(cplx.ws[d1][d2]);                                       // adiciona variavel ao modelo
      }
    }
  }

  // variaveis de exposição
  // E_RI => nivel de exposicao da tarefa I pelo requerimento R
  for (int r = 0; r < _numr; r++)
  {
    cplx.e[r] =  IloNumVarArray(cplx.env, _n);

    for (int i = 0; i < _n; i++)
    {
      sprintf (var_name, "xe_%d_%d", (int) r,(int) i);                    // nome da variavel
      cplx.e[r][i] = IloNumVar(cplx.env, 0, IloInfinity, var_name);   // aloca variavel
      cplx.model.add(cplx.e[r][i]);                                             // adiciona variavel ao modelo
    }
  }


  // variaveis de uso dos buckets
  // B_JL => se o bucket J esta sendo usada no intervalo L
  // *OBS* ******* O INTERVALO 0 serve para indicar que o BUCKET NAO ESTA SENDO USADO ******
  for (int b = 0; b < _numb; b++) {
    std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      cplx.b[b] = IloBoolVarArray(cplx.env, static_cast<int>(bucket->get_number_of_GB_per_cost_intervals())+1);

      for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
      {
        sprintf (var_name, "b_%d_%d", (int)b,(int)l);                       // nome da variavel
        cplx.b[b][l] = IloBoolVar(cplx.env, var_name);                      // aloca variavel
        cplx.model.add(cplx.b[b][l]);                                       // adiciona variavel ao modelo
      }
    } else {
      exit(1);  // error
    }
  }

  // variaveis de uso dos buckets
  // Q_JL => quantidade de dados usada pelo bucket J no intervalo L
  // *OBS* ******* O INTERVALO 0 serve para indicar que o BUCKET NAO ESTA SENDO USADO ******
  for (int b = 0; b < _numb; b++) {
    std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
      cplx.q[b] = IloNumVarArray(cplx.env, static_cast<int>(bucket->get_number_of_GB_per_cost_intervals())+1);

      for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
      {
        sprintf(var_name, "q_%d_%d", (int) b, (int) l);                     // nome da variavel
        cplx.q[b][l] = IloNumVar(cplx.env, 0, IloInfinity, var_name);       // aloca variavel
        cplx.model.add(cplx.q[b][l]);                                       // adiciona variavel ao modelo
      }
    } else {
      exit(1);  // error
    }
  }

  // variaveis de uso de maquina por tempo
  // V_JT => indica se a maquina J esta em uso (contratada) no periodo T
  for (int i = 0; i < _m; i++)
  {
    cplx.v[i] =  IloBoolVarArray(cplx.env, _t);
    for (int j = 0; j < _t; j++)
    {
      sprintf(var_name, "v_%d_%d", (int) i, (int) j);                     // nome da variavel
      cplx.v[i][j] = IloBoolVar(cplx.env, var_name);                      // aloca variavel
      cplx.model.add(cplx.v[i][j]);                                       // adiciona variavel ao modelo
    }
  }

  // variaveis de tempo total de uso por maquina
  // Z_J => indica tempo total em que a maquina J foi usada (contratada)
  for (int i = 0; i < _m; i++)
  {
    sprintf(var_name, "z_%d", (int) i);                         // nome da variavel
    cplx.z[i] = IloNumVar(cplx.env, 0, IloInfinity, var_name);  // aloca variavel
    cplx.model.add(cplx.z[i]);                                  // adiciona variavel ao modelo
  }

  //variaveis de tempo total (makespam)
  // Z_MAX => makespam do workflow
  sprintf(var_name, "z_max");                                     // nome da variavel
  cplx.z_max[0] = IloNumVar(cplx.env, 0, IloInfinity, var_name);  // aloca variavel
  cplx.model.add(cplx.z_max[0]);                                  // adiciona variavel ao modelo

  // ---------------- funcao objetivo -------------------
  IloExpr fo(cplx.env);

  // ---- Makespam
  fo = alpha_time_ * (cplx.z_max[0] / _t);

  // ----- Custo Financeiro
  for (int j = 0; j < _m; j++)
  {
    std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));
    fo += alpha_budget_ * ((virtual_machine->get_cost() * cplx.z[j]) / _cmax);
  }

  for (int b = 0; b < _numb; b++)
  {
    std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
      {
        fo += alpha_budget_ * ((bucket->get_cost() * cplx.q[b][l]) / _cmax);
      }
    }
    else
    {
      exit(1);  // error
    }
  }

  // Exposicao
  for (int r = 0; r < _numr; r++)
  {
    for(int i = 0; i < _n; i++)
    {
      fo += alpha_security_ * (cplx.e[r][i] / _smax);
    }
  }

  // Penalidades das Soft arestas
  for (int d1 = 0; d1 < _d-1; d1++)
  {
    for (int d2 = d1 + 1; d2 < _d; d2++)
    {
      int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

      if (conflict > 0)
      {  // soft constraint
        fo += alpha_security_ * (conflict * cplx.ws[d1][d2] / _smax);
      }
    }
  }

  cplx.model.add(IloMinimize(cplx.env,fo, "fo"));  // adiciona função objetivo ao modelo
  // -----------------------------------------------------

  // --------------- RESTRIÇÕES ----------

  // Restrição (4)
  for (int i = 0; i < _n; i++)
  {
    IloExpr exp(cplx.env);
    // std::cout << "_m: " << _m << "\n";
    for (int j = 0; j < _m; j++) {
      for (int t = 0; t < _t; t++) {
        exp += cplx.x[i][j][t];
      }
    }

    IloConstraint c(exp == 1);

    sprintf(var_name, "c4_%d", (int) i);
    c.setName(var_name);
    cplx.model.add(c);

    exp.end();
  }

  // Restricao (5)
  for (int i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

    for (int d = 0; d < static_cast<int>(input_files.size()); d++)
    {
      IloExpr exp(cplx.env);

      for (int j = 0; j < _m; j++)
      {
        for (int p = 0; p < _mb; p++)
        {
          for (int t = 0; t < _t; t++)
          {
            exp += cplx.r[i][d][j][p][t];
          }
        }
      }

      IloConstraint c(exp == 1);

      sprintf (var_name, "c5_%d_%d", (int) i, (int) d);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
  }


  // Restricao (6)
  for (int i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

    for (int d = 0; d < static_cast<int>(output_files.size()); d++)
    {
      IloExpr exp(cplx.env);

      for (int j = 0; j < _m; j++)
      {
        for (int p = 0; p < _mb; p++)
        {
          for (int t = 0; t < _t; t++)
          {
            exp += cplx.w[i][d][j][p][t];
          }
        }
      }

      IloConstraint c(exp == 1);
      sprintf (var_name, "c6_%d_%d", (int)i, (int)d);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
  }


  // Restricao (7)
  for (int teto, i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

    for (int d = 0; d < static_cast<int>(output_files.size()); d++)
    {
      for (int j = 0; j < _m; j++)
      {
        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

        for (int p = 0; p < _mb; p++)
        {
          int tempo = std::ceil(task->get_time() * virtual_machine->get_slowdown());

          for (int t = tempo; t < _t; t++)
          {
            IloExpr exp(cplx.env);

            exp += cplx.w[i][d][j][p][t];

            /* (q <= teto) pois o tamanho do intervalo é o mesmo não importa se o tempo comeca de 0 ou 1 */
            teto = std::max(0, t - tempo);
            // Pq temos esse max?
            // Pq é até q <= teto e não q < teto
            for (int q = 0; q <= teto; q++) {
              exp -= cplx.x[i][j][q];
            }

            IloConstraint c(exp <= 0);

            sprintf (var_name, "c7_%d_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) p, (int) t);
            c.setName(var_name);
            cplx.model.add(c);

            exp.end();
          }
        }
      }
    }
  }

  // Restricao (8)
  for (int i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

    for (int d = 0; d < static_cast<int>(output_files.size()); d++)
    {
      for (int j = 0; j < _m; j++)
      {
        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

        for (int p = 0; p < _mb; p++)
        {
          int tempo = std::ceil(task->get_time() * virtual_machine->get_slowdown());
          // for (int t = tempo; t < _t; t++)
          int limite = min(_t, tempo);
          for (int t = 0; t < limite; t++)
          {
            IloExpr exp(cplx.env);
            exp +=cplx.w[i][d][j][p][t];

            IloConstraint c(exp == 0);
            sprintf (var_name, "c8_%d_%d_%d_%d_%d", (int)i, (int)d, (int)j, (int)p, (int)t);
            c.setName(var_name);
            cplx.model.add(c);

            exp.end();
		      }
	      }
  	  }
    }
  }

  // Retrição (9)
  for (int teto, i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

    for (int d = 0; d < static_cast<int>(input_files.size()); d++)
    {
      std::shared_ptr<File> file = input_files[static_cast<size_t>(d)];

      for (int j = 0; j < _m; j++)
      {
        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

        /* vamos fazer para todo t pois assim quando (t-t_djp) < 0, o lado direito sera 0 e impede a
        execucao da tarefa naquele tempo */
        for (int t = 0; t < _t; t++)
        {
          IloExpr exp(cplx.env);

          exp = cplx.x[i][j][t];

          for (int p = 0; p < _mb; p++)
          {
            std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));
            /* (q <= teto) pois o tamanho do intervalo é o mesmo não importa se o tempo comeca de 0 ou 1 */

            if ((t + 1 - ComputeFileTransferTime(file, virtual_machine, storage)) >= 0)
            // if ((t - ComputeFileTransferTime(file, virtual_machine, storage)) >= 0)
            {
              teto = max(0, t + 1 - ComputeFileTransferTime(file, virtual_machine, storage));
              // teto = max(0, t - ComputeFileTransferTime(file, virtual_machine, storage));

              for (int q = 0; q < teto; q++)
              {
                exp -= cplx.r[i][d][j][p][q];
              }
            }

          }

          IloConstraint c(exp <= 0);
          sprintf (var_name, "c9_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) t);
          c.setName(var_name);
          cplx.model.add(c);

          exp.end();
        }
      }
    }
  }

  // Restricao (10)
  for (int piso, j = 0; j < _m; j++)
  {
    std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

    for (int t = 0; t < _t; t++)
    {
      IloExpr exp(cplx.env);

      /* execucao */
      for (int i = 0; i < _n; i++)
      {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));

        int tempo_execucao = static_cast<int>(std::ceil(task->get_time() * virtual_machine->get_slowdown()));

        piso       = max(0, t - tempo_execucao + 1);

        for (int q = piso; q <= t; q++)
        {
          exp += cplx.x[i][j][q];
        }
      }

      /* escrita */
      for (int i = 0; i < _n; i++)
      {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
        std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

        for (int d = 0; d < static_cast<int>(output_files.size()); d++)
        {
          std::shared_ptr<File> file = output_files[static_cast<size_t>(d)];

          for (int p = 0; p < _mb; p++)
          {
            std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));
            piso = max(0, t - ComputeFileTransferTime(file, virtual_machine, storage) + 1);
            for (int rr = piso; rr <= t; rr++)
            {
              exp += cplx.w[i][d][j][p][rr];
            }
          }
        }
      }

      /* leitura */
      for (int i = 0; i < _n; i++)
      {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

        for (int d = 0; d < static_cast<int>(input_files.size()); d++)
        {
          std::shared_ptr<File> file = input_files[static_cast<size_t>(d)];

          for(int p=0; p < _mb; p++)
          {
            std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));
            piso = max(0, t - ComputeFileTransferTime(file, virtual_machine, storage) + 1);
            for (int rr = piso; rr <= t; rr++)
            {
              exp += cplx.r[i][d][j][p][rr];
            }
          }
        }
	    }

      /* contratacao */
      exp -= cplx.v[j][t];

      IloConstraint c(exp <= 0);
      // IloConstraint c(exp == 0);

      sprintf (var_name, "c10_%d_%d", (int) j, (int) t);
      c.setName(var_name);
      cplx.model.add(c);

	    exp.end();
    }
  }

  // Restricao (11)
  for (int j = 0; j < _m; j++)
  {
    std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(j));

    for (int t = 0; t < _t; t++)
    {
      IloExpr exp(cplx.env);

      /* escrita */
      for (int i = 0; i < _n; i++)
      {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
        std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

        for (int d = 0; d < static_cast<int>(output_files.size()); d++)
        {
          std::shared_ptr<File> file = output_files[static_cast<size_t>(d)];

          for (int p = 0; p < _m; p++)
          {
            if (j != p)
            {
              std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(p));
              int piso = max(0, t - ComputeFileTransferTime(file, storage, virtual_machine) + 1);

              for (int rr = piso; rr <= t; rr++)
              {
                exp += cplx.w[i][d][p][j][rr];
              }
            }
          }
        }
      }

      /* leitura */
      for (int i = 0; i < _n; i++)
      {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

        for (int d = 0; d < static_cast<int>(input_files.size()); d++)
        {
          std::shared_ptr<File> file = input_files[static_cast<size_t>(d)];

          for (int p = 0; p < _m; p++)
          {
            if (j != p)
            {
              std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(p));
              int piso = max(0, t - ComputeFileTransferTime(file, storage, virtual_machine) + 1);

              for (int rr = piso; rr <= t; rr++)
              {
                exp += cplx.r[i][d][p][j][rr];
              }
            }
          }
        }
      }

      /* contratacao */
      exp -= cplx.v[j][t] * (_m - 1);

      IloConstraint c(exp <= 0);

      sprintf (var_name, "c11buga_%d_%d", (int) j, (int) t);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
  }

  // Restricao (12)
  for (int b = static_cast<int>(GetVirtualMachineSize());
      b < static_cast<int>(GetVirtualMachineSize()) + _numb;
      b++)
  {
    std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(b));

    for (int t = 0; t < _t; t++)
    {
      IloExpr exp(cplx.env);

      /* escrita */
      for (int i = 0; i < _n; i++)
      {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
        std::vector<std::shared_ptr<File>> output_files = task->get_output_files();

        for (int d = 0; d < static_cast<int>(output_files.size()); d++)
        {
          std::shared_ptr<File> file = output_files[static_cast<size_t>(d)];

          for (int p = 0; p < _m; p++)
          {
            std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(p));
            int piso = max(0, t - ComputeFileTransferTime(file, storage, virtual_machine) + 1);

            for (int rr = piso; rr <= t; rr++) {
              exp += cplx.w[i][d][p][b][rr];
            }
          }
        }
      }

      /* leitura */
      for (int i = 0; i < _n; i++)
      {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

        for (int d = 0; d < static_cast<int>(input_files.size()); d++)
        {
          std::shared_ptr<File> file = input_files[static_cast<size_t>(d)];

          for (int p = 0; p < _m; p++)
          {
            std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(p));
            int piso = max(0, t - ComputeFileTransferTime(file, storage, virtual_machine) + 1);

            for (int rr = piso; rr <= t; rr++) {
              exp += cplx.r[i][d][p][b][rr];
            }
          }
        }
      }

      IloConstraint c(exp <= 1);

      sprintf (var_name, "c11_%d_%d", (int)b, (int)t);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
  }

  // Restricao (12)
  for(int d = 0; d < static_cast<int>(files_.size()); ++d)
  {
    std::shared_ptr<File> file = files_[static_cast<size_t>(d)];

    if (dynamic_pointer_cast<DynamicFile>(file))
    {
      for (int j = 0; j < _mb; j++)
      {
        IloExpr exp(cplx.env);

        exp += cplx.y[d][j][0];

        IloConstraint c(exp == 0);

        sprintf (var_name, "c12_%d_%d", (int)d, (int)j);
        c.setName(var_name);
        cplx.model.add(c);

        exp.end();
      }
    }
  }

  //Restricao (13) e (14)
  for (int d = 0; d < static_cast<int>(files_.size()); ++d)
  {
    std::shared_ptr<File> file = files_[static_cast<size_t>(d)];

    if (auto static_file = dynamic_pointer_cast<StaticFile>(file))
    {
//      int ind = static_cast<int>(static_file->GetFirstVm());
      int ind = static_cast<int>(static_file->GetStorageId());

      for (int mb = 0; mb < static_cast<int>(storages_.size()); ++mb)
      {
        if (ind == mb)
        {
          for (int t = 0; t <= _t; t++)
          {
            IloExpr exp(cplx.env);

            exp += cplx.y[d][ind][t];

            IloConstraint c(exp == 1);

            sprintf (var_name, "c14_%d_%d_%d", (int) d, (int) ind, (int) t);
            c.setName(var_name);
            cplx.model.add(c);

            exp.end();
          }
        }
        else
        {
          IloExpr exp(cplx.env);

          exp += cplx.y[d][mb][0];

          IloConstraint c(exp == 0);

          sprintf (var_name, "c13_%d_%d", (int) d, (int) mb);
          c.setName(var_name);
          cplx.model.add(c);

          exp.end();
        }
      }
    }
  }

  // Restricao (15)
  // for (int indw = -1, pai_i = 0, tempo = 0, /* <RODRIGO> para todo dado "d" dinamico */)
  for (size_t i = 0; i < GetFileSize(); ++i)
  {
    if (auto dynamic_file = dynamic_pointer_cast<DynamicFile>(files_[i]))
    {
      // pai_i      = /* <RODRIGO> tarefa que escreveu o dado "d" */;
      // indw       = /* <RODRIGO> o dado "d" eh a d-esima escrita de "pai_i" */;
      // std::shared_ptr<File>file = input_files[static_cast<size_t>(d)]; /* <RODRIGO> eh esse d mesmo neh ? */

      std::shared_ptr<Activation> pai = dynamic_file->get_parent_task();
      int indw  = static_cast<int>(dynamic_file->get_parent_output_file_index());
      int d     = static_cast<int>(dynamic_file->get_id());
      // Shift por conta da tarefa virtual SOURCE (id: 0)
      int pai_i = static_cast<int>(pai->get_id()) - 1;

      for (int p = 0; p < _mb; p++)
      {
        std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));

        // for (int t = 0; t < _t - 1; t++)
        for (int t = 0; t < _t; t++)
        {
          IloExpr exp(cplx.env);
          // exp += cplx.y[d][p][t+1];
          // exp -= cplx.y[d][p][t];

          exp += cplx.y[d][p][t + 1];
          exp -= cplx.y[d][p][t];

          for (int j = 0; j < _m; j++)
          {
            std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

            int tempo = (t - ComputeFileTransferTime(dynamic_file, storage, virtual_machine) + 1);

            // if (pai_i == 0 and indw == 0 and p == 2 and t == 18)
            // {
            //   std::cout << "tempo: " << tempo << std::endl;
            //   std::cout << "j: " << j << std::endl;
            //   std::cout << "m_: " << _m << std::endl;
            //   std::cout << "t: " << t << std::endl;
            //   std::cout << "storage->get_bandwidth_GBps(): " << storage->get_bandwidth_GBps() << std::endl;
            //   std::cout << "virtual_machine->get_bandwidth_GBps(): " << virtual_machine->get_bandwidth_GBps() << std::endl;
            //   std::cout << "dynamic_file->get_size_in_GB(): " << dynamic_file->get_size_in_GB() << std::endl;

            //   // double link = std::min(storage1->get_bandwidth(), storage2->get_bandwidth_GBps());
            //   // time = static_cast<int>(std::ceil(file->get_size_in_GB() / link));
            // }

            if (tempo >= 0)
            // if (tempo >= 0 && tempo < _t)
            // if (tempo > 0 && tempo < _t)
            {
              // exp-=cplx.w[pai_i][indw][j][p][tempo];
              exp -= cplx.w[pai_i][indw][j][p][tempo];
            }
          }

          // IloConstraint c(exp <= 0);
          IloConstraint c(exp == 0);
          // sprintf (var_name, "c15_%d_%d_%d", (int)d, (int)p, (int)t);
          sprintf (var_name, "c15_%d_%d_%d", (int) d, (int) p, (int) t);
          c.setName(var_name);
          cplx.model.add(c);

          exp.end();
        }
      }
    }
    else  // arquivos estáticos
    {
      // std::shared_ptr<Activation> pai = dynamic_file->get_parent_task();
      std::shared_ptr<File>file = GetFilePerId(i);
      // int indw  = static_cast<int>(dynamic_file->get_parent_output_file_index());
      int d     = static_cast<int>(file->get_id());
      // Shift por conta da tarefa virtual SOURCE (id: 0)
      // int pai_i = static_cast<int>(pai->get_id()) - 1;

      for (int p = 0; p < _mb; p++)
      {
        // std::shared_ptr<Storage>storage = GetStoragePerId(static_cast<size_t>(p));

        for (int t = 0; t < _t - 1; t++)
        {
          IloExpr exp(cplx.env);
          // exp += cplx.y[d][p][t+1];
          // exp -= cplx.y[d][p][t];

          exp += cplx.y[d][p][t + 1];
          exp -= cplx.y[d][p][t];

          // IloConstraint c(exp <= 0);
          IloConstraint c(exp == 0);

          // sprintf (var_name, "c15_%d_%d_%d", (int)d, (int)p, (int)t);
          sprintf (var_name, "c15_%d_%d_%d", (int) d, (int) p, (int) t);
          c.setName(var_name);
          cplx.model.add(c);

          exp.end();
        }
      }
    }
  }

  // Restricao (16)
  for (int dd, i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>> input_files = task->get_input_files();

    for (int d = 0; d < static_cast<int>(input_files.size()); d++)
    {
      for(int p = 0; p < _mb; p++)
      {
        for(int t = 0; t < _t; t++)
        {
          IloExpr exp(cplx.env);

          for (int j = 0; j < _m; j++)
          {
            exp += cplx.r[i][d][j][p][t];
          }

          std::shared_ptr<File> file = input_files[static_cast<size_t>(d)];

          dd = static_cast<int>(file->get_id());
          exp -= cplx.y[dd][p][t];

          IloConstraint c(exp <= 0);

          sprintf (var_name, "c16_%d_%d_%d_%d", (int) i, (int) d, (int) p, (int) t);
          c.setName(var_name);
          cplx.model.add(c);

          exp.end();
        }
      }
    }
  }

  // Restricao (17)
  for (int j = 0; j < _mb; j++)
  {
    std::shared_ptr<Storage>storage = GetStoragePerId(static_cast<size_t>(j));

    for (int t = 0; t <= _t; t++)
    {
      IloExpr exp(cplx.env);

      for (int d = 0; d < _d; d++)
      {
        std::shared_ptr<File> file = GetFilePerId(static_cast<size_t>(d));
        exp += (file->get_size_in_GB() * cplx.y[d][j][t]);
      }

      IloConstraint c(exp <= storage->get_storage());

      sprintf(var_name, "c17_%d_%d", (int)j, (int)t);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
  }

  // Restricao (18)
  for (int i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>>output_files = task->get_output_files();

    for (int d = 0; d < static_cast<int>(output_files.size()); d++)
    {
      std::shared_ptr<File>file = output_files[static_cast<size_t>(d)];

      for (int j = 0; j < _m; j++)
      {
        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

        for (int p = 0; p < _mb; p++)
        {
          std::shared_ptr<Storage>storage = GetStoragePerId(static_cast<size_t>(p));

          for (int t = 0; t < _t; t++)
          {
            IloExpr exp(cplx.env);

            exp += (t + ComputeFileTransferTime(file, storage, virtual_machine)) * cplx.w[i][d][j][p][t];
            exp -= cplx.z_max[0];

            IloConstraint c(exp <= 0);

            sprintf(var_name, "c18_%d_%d_%d_%d_%d", (int)i, (int)d, (int)j, (int)p, (int)t);
            c.setName(var_name);
            cplx.model.add(c);

            exp.end();
          }
        }
      }
    }
  }

  // Restricao (18extra)
  // Restricao para inviabilizar que alguma tarefa comece antes do 't' final e termine em 't' ou
  // após
  {
    IloExpr exp(cplx.env);

    // exp += (t + ComputeFileTransferTime(file, storage, virtual_machine)) * cplx.w[i][d][j][p][t];
    exp += cplx.z_max[0];

    IloConstraint c(exp <= _t);

    sprintf(var_name, "c18extra");
    c.setName(var_name);
    cplx.model.add(c);

    exp.end();
  }

  //Restricao (19)
  for (int j = 0; j < _m; j++)
  {
    for (int t = 0; t < _t; t++)
    {
      IloExpr exp(cplx.env);

      exp += (t + 1) * cplx.v[j][t];  // t+1 porque voce paga os tempos de 0 a t, logo t+1 tempos
      exp -= cplx.z[j];

      IloConstraint c(exp <= 0);

      sprintf(var_name, "c19_%d_%d", (int)j, (int)t);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
  }


  //Restricao (20)
  {
    IloExpr exp(cplx.env);
    for (int j = 0; j < _m; j++)
    {
      std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));
      exp += virtual_machine->get_cost() * cplx.z[j];
    }

    for(int b = 0; b < _numb; b++)
    {
      std::shared_ptr<Storage>storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

      if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
      {
        for(int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
        {
          exp += bucket->get_cost() * cplx.q[b][l]; // lembrando que o custo nessas instancias eh constante
        }
      }
      else
      {
        exit(1);  // error
      }
    }
    IloConstraint c(exp <=  _cmax);
    sprintf (var_name, "c20");
    c.setName(var_name);
    cplx.model.add(c);
    exp.end();
  }

  //Restricao (21)
  for (int i = 0; i < _n; i++)
  {
    std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));
    std::vector<std::shared_ptr<File>>input_files = task->get_input_files();

    for (int d = 0; d < static_cast<int>(input_files.size()); d++)
    {
      for (int j = 0; j < _m; j++)
      {
        for (int t = 0; t < _t; t++)
        {
          IloExpr exp(cplx.env);

          for (int p = 0; p < _mb; p++) {
            exp += (static_cast<int>(input_files.size())) * cplx.r[i][d][j][p][t];
          }

          for (int dd, g = 0; g < static_cast<int>(input_files.size()); g++)
          {
            std::shared_ptr<File>file = input_files[static_cast<size_t>(d)];

            dd = static_cast<int>(file->get_id());

            for (int p = 0; p < _mb; p++) {
              exp -= cplx.y[dd][p][t];
            }
          }

          IloConstraint c(exp <= 0);

          sprintf(var_name, "c21_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) t);
          c.setName(var_name);
          cplx.model.add(c);

          exp.end();
        }
      }
    }
  }

  // Restricao (22)
  for (int d = 0; d < _d; d++)
  {
    for (int j = 0; j < _mb; j++)
    {
      for (int t = 0; t <= _t; t++)
      {
        IloExpr exp(cplx.env);

        exp += cplx.y[d][j][t];
        exp -= cplx.yb[d][j];

        IloConstraint c(exp <= 0);

        sprintf (var_name, "c22_%d_%d_%d", (int) d, (int) j, (int) t);
        c.setName(var_name);
        cplx.model.add(c);

        exp.end();
      }
    }
  }

  // Restricao (23)
  for (int d1 = 0; d1 < _d - 1; d1++)
  {
    for (int d2 = d1 + 1; d2 < _d; d2++)
    {
      int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

      // hard constraint
      if (conflict == -1)
      {
        for (int b = 0; b < _mb; b++)
        {
          IloExpr exp(cplx.env);

          exp += cplx.yb[d1][b];
          exp += cplx.yb[d2][b];

          IloConstraint c(exp <= 1);

          sprintf(var_name, "c23_%d_%d_%d", (int) d1, (int) d2, (int) b);
          c.setName(var_name);
          cplx.model.add(c);

          exp.end();
        }
      }
    }
  }

  // Restricao (24)
  for (int d1 = 0; d1 < _d - 1; d1++)
  {
    for (int d2 = d1 + 1; d2 < _d; d2++)
    {
      int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

      // hard constraint
      if (conflict > 0)
      {
        for (int b = 0; b < _mb; b++)
        {
          IloExpr exp(cplx.env);

          exp += cplx.yb[d1][b];
          exp += cplx.yb[d2][b];
          exp -= cplx.ws[d1][d2];

          IloConstraint c(exp <= 1);

          sprintf(var_name, "c24_%d_%d_%d", (int) d1, (int) d2, (int) b);
          c.setName(var_name);
          cplx.model.add(c);

          exp.end();
        }
      }
    }
  }

  // Restricao (25)
  for (int r = 0; r < _numr; r++)
  {
    // Requirement requirement = GetRequirementPerId(static_cast<size_t>(r));
    for (int i = 0; i < _n; i++)
    {
      std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1UL));

      IloExpr exp(cplx.env);
      exp -= cplx.e[r][i];

      for (int j = 0; j < _m; j++) {
        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));
        for (int t = 0; t < _t; t++) {
          // exp += /* <RODRIGO> l^{lj}_{vm} */ * cplx.x[i][j][t];
          exp -= virtual_machine->GetRequirementValue(static_cast<size_t>(r)) * cplx.x[i][j][t];
        }
      }

      // IloConstraint c(exp <= - /* <RODRIGO> l^{ri}_{task} */);
      IloConstraint c(exp <= -task->GetRequirementValue(static_cast<size_t>(r)));

      sprintf (var_name, "c25_%d_%d", (int) i, (int) r);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
  }

  // Restricao (26)
  for (int b = 0; b < _numb; b++)
  {
    // bucket
    std::shared_ptr<Storage>storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

    // indice do bucket dentre as maquinas
    int j = static_cast<int>(GetVirtualMachineSize()) + b;

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      IloExpr exp(cplx.env);

      for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
      {
        exp += cplx.q[b][l];
      }

      for (int d = 0; d < _d; d++)
      {
        std::shared_ptr<File>file = files_[static_cast<size_t>(d)];
        exp -= (file->get_size_in_GB() * cplx.yb[d][j]);
        // exp -= (file->get_size() * cplx.yb[d][j]);
      }

      IloConstraint c(exp == 0);

      sprintf(var_name, "c26_%d", (int) j);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
    else
    {
      exit(1);  // error
    }
  }

  // Restricao (27)
  for(int b = 0; b < _numb; b++)
  {
    // bucket
    std::shared_ptr<Storage>storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      IloExpr exp(cplx.env);

      for(int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
        exp += cplx.b[b][l];

      IloConstraint c(exp == 1);
      sprintf (var_name, "c27_%d", (int) b);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
  }


 //Restricao (28)
 for (int b = 0; b < _numb; b++)
  {
    // bucket
    std::shared_ptr<Storage>storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

    // indice do bucket dentre as maquinas
    int j = static_cast<int>(GetVirtualMachineSize()) + b;

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      IloExpr exp(cplx.env);

      for (int d=0; d < _d; d++)
      {
        std::shared_ptr<File>file = files_[static_cast<size_t>(d)];
        exp += (file->get_size_in_GB() * cplx.yb[d][j]);
        // exp += (file->get_size() * cplx.yb[d][j]);
      }

      for (int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
        exp -= /* <RODRIGO> sm_{bl}*/ 51200  * cplx.b[b][l];  // sm_{bl} == 0

      IloConstraint c(exp <= 0);
      sprintf (var_name, "c28_%d", (int) b);
      c.setName(var_name);
      cplx.model.add(c);

      exp.end();
    }
    else
    {
      exit(1);  // error
    }

  }

  // Restricao (29)
  for(int b = 0; b < _numb; b++)
  {
    // bucket
    std::shared_ptr<Storage>storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      for (int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
      {
        IloExpr exp(cplx.env);
        // sm_j_l ==> Storage size of bucket $j \in B$ on interval $l \in L_j$,  where $sm_{j0}=0$.
        // exp += /*  <RODRIGO> sm_{b(l-1)} * */   cplx.b[b][l];
        exp += bucket->get_storage_of_the_interval(static_cast<size_t>(l - 1)) * cplx.b[b][l];
        exp -= cplx.q[b][l];

        IloConstraint c(exp <= 0);
        sprintf (var_name, "c29a_%d_%d", (int) b, (int) l);
        c.setName(var_name);
        cplx.model.add(c);

        exp.end();

        IloExpr exp2(cplx.env);
        // exp2 -= /* <RODRIGO> sm_{bl} * */   cplx.b[b][l];
        // exp2 -= 51200 * cplx.b[b][l];
        exp2 -= bucket->get_storage_of_the_interval(static_cast<size_t>(l)) * cplx.b[b][l];
        exp2 += cplx.q[b][l];

        IloConstraint c2(exp2 <= 0);
        sprintf (var_name, "c29b_%d_%d", (int) b, (int) l);
        c2.setName(var_name);
        cplx.model.add(c2);

        exp2.end();
      }
    }
    else
    {
      exit(1);  // error
    }
  }

  // Restricao (30)
  for (int b = 0; b < _numb; b++)
  {
    // bucket
    std::shared_ptr<Storage>storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

    // indice do bucket dentre as maquinas
    int j = static_cast<int>(GetVirtualMachineSize()) + b;

    if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
    {
      for(int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
      {
        IloExpr exp(cplx.env);

        exp += cplx.b[b][l];

        for(int d=0; d < _d; d++)
          exp -= cplx.yb[d][j];

        IloConstraint c(exp <= 0);
        sprintf (var_name, "c30_%d_%d", (int) b, (int) l);
        c.setName(var_name);
        cplx.model.add(c);

        exp.end();
      }
    }
  }

  IloCplex solver(cplx.model);                          // declara variável "solver" sobre o modelo a ser solucionado
  // solver.exportModel("model.lp");                       // escreve modelo no arquivo no formato .lp
  solver.exportModel(FLAGS_cplex_output_file.c_str());  // escreve modelo no arquivo no formato .lp
  // open CPLEX model
  int error;
  CPXENVptr env = CPXopenCPLEX(&error);

  // Parametros
//  solver.setParam(IloCplex::TiLim, 3600);       // tempo limite (3600=1h, 86400=1d)
//  CPXsetdblparam(env, CPX_PARAM_TILIM, 3600.0);
//  CPXsetdblparam(env, CPX_PARAM_MIPINTERVAL, 100.0);
//  solver.setParam(IloCplex::MIPInterval, 100);  // Log a cada N nos

  // -------------------- Callbacks ----------------------

  //Ao encontrar uma nova solucao (incubent solution)
  solver.use(CB_incub_sol(cplx.env, &best, &cplx));

  // Metodo de resolução
  try
  {
    solver.solve();
  }
  catch(IloException& e)
  {
    cout << e;
  }


  // Saida de informação
  // Saida
  bool   solved;
  double res, lb, time_s;

  try
  {
    // if (solver.getStatus() == IloAlgorithm::Optimal)
    //   solved = true;
    // else
    //   solved = false;

    solved = solver.getStatus() == IloAlgorithm::Optimal;

    res    = solver.getObjValue ();                                    // solução
    lb     = solver.getBestObjValue();                                 // limite dual (inferior)
    time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;    // tempo de processamento

    double custo = 0.0;
    double seguranca = 0.0;

    // ----- Custo Financeiro
    for (int j = 0; j < _m; j++)
    {
      std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

      custo += virtual_machine->get_cost() * best.z[j];
    }

    for (int b = 0; b < _numb; b++)
    {
      std::shared_ptr<Storage>storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

      if (auto bucket = dynamic_pointer_cast<Bucket>(storage))
      {
        for (int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
        {
          custo += bucket->get_cost() * best.q[b][l];
        }
      }
      else
      {
        exit(1);  // error
      }
    }

    // Exposicao
    for (int r = 0; r < _numr; r++)
    {
      for(int i = 0; i < _n; i++)
      {
        seguranca += best.e[r][i] / _smax;
      }
    }

    // Penalidades das Soft arestas
    for (int d1 = 0; d1 < _d - 1; d1++)
    {
      for (int d2 = d1 + 1; d2 < _d; d2++)
      {
        int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

        if (conflict > 0)
        {  // soft constraint
          seguranca += conflict * best.ws[d1][d2] / _smax;
        }
      }
    }

    cout << endl;
    cout << "----- ACABOU ----" << endl;
    cout << "SOLVED?  : " << solved << endl;
    cout << "MAKESPAN : " << best.z_max << endl;
    cout << "CUSTO    : " << custo << endl;
    cout << "SEGURANCA: " << seguranca << endl;
    cout << "TEMPO    : " << time_s << endl;
    cout << "LB       : " << lb << endl;
    cout << "UB (X*)  : " << res << endl;
    cout << endl;

    cerr << best.z_max << " "
         << custo << " "
         << seguranca << " "
         << res << endl
         << time_s << endl;

    // ------------------------ Solucao -------------------------
    // if (data.DEPU)
    // {
    //   if (data.b_sol==1.0e+6)
    //     {
    //       cout<<"Nenhuma solucao armazenada"<<endl;
    //     }
    //   else
    //     data.print_sol();
    // }
  }
  catch(IloCplex::Exception &e)
  {
    cout << e;
  }

  cplx.env.end();

  ImprimeBest(&best);
  print_sol(&best);
}  // end of the method run()
