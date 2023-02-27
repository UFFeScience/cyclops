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

//template<typename T= VirtualMachine>
//T dynamic_pointer_cast(Storage);

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
 * \param[in]  file          File to be transferred
 * \param[in]  storage1      Storage origin/destination
 * \param[in]  storage2      Storage origin/destination
 * \retval     time          The time to transfer \c file from \c file_vm to \c vm with possible applied penalty
 */
// TODO: Use a common function to do the job of calculate the file transfer time.
int Cplex::ComputeFileTransferTime(const File *file,
                                   const std::shared_ptr<Storage> &storage1,
                                   const std::shared_ptr<Storage> &storage2) {
    int time = 0;

    // Calculate time
    if (storage1->get_id() != storage2->get_id()) {
        // get the smallest link
        double link = std::min(storage1->get_bandwidth_GBps(), storage2->get_bandwidth_GBps());
        time = static_cast<int>(std::ceil(file->get_size_in_GB() / link));
    } else {
        time = 1;
    }

    return time;
}

/*
 * Print timeline
 * Show execution
 * Show resources through the time
 */
void Cplex::print_solution_timeline(struct BEST *data) {
    Algorithm *algorithm = data->algorithm_;

    int *tempo = new int[static_cast<size_t>(data->mb_)];

    /* Relate each device with its array of data */
    int **maq_dado = new int *[static_cast<size_t>(data->mb_)];

    // Initializations

    for (auto i = 0ul; i < data->mb_; i++) {
        tempo[i] = 0;
    }

    for (auto i = 0ul; i < data->mb_; i++) {
        maq_dado[i] = new int[static_cast<size_t>(data->d_)];
        for (auto j = 0ul; j < data->d_; j++) {
            maq_dado[i][j] = -1;
        }
    }

    // Vector for TEST that saves the actual time the machine spends performing its action (exec., read, Write)
    // Save files per machine

    std::cout << "---------------- Timeline --------------" << std::endl;
    std::cout << "t)\t";
    for (auto j = 0ul; j < data->mb_; j++) {
        std::cout << "(M" << j << ")\t" << "(Y)\t" << "(V)\t";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    // Time
    for (auto check = 0ul, t1 = 0ul; t1 < static_cast<size_t>(data->t_) and check <= 1ul; t1++) {
        std::cout << "t" << t1 << ")\t";

        // Devices
        // Virtual Machines
        for (auto j = 0ul; j < data->mb_ and check <= 1; j++) {
            std::shared_ptr<Storage> device = algorithm->GetStoragePerId(static_cast<size_t>(j));

            if (auto virtual_machine = std::dynamic_pointer_cast<VirtualMachine>(device)) {
                // Check if more than one action is started at the same time on the same machine
                check = 0ul;

                // During an action (read, execute, Write)
                tempo[j] = tempo[j] - 1;

                if (tempo[j] > 0) {
                    check += 1;
                    std::cout << "(" << tempo[j] << ")" << "\t";
                }

                // Execution
                for (size_t i = 0; i < static_cast<size_t>(data->n_) and check <= 1ul; i++) {
                    std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(1ul + i));
                    std::vector<File *> input_files = task->get_input_files();

                    auto x_ijt = static_cast<float>(data->x[i][j][t1]);  // Returns the value of the variable

                    if (x_ijt >= (1 - data->PRECISION)) {
                        check += 1;
                        std::cout << "x" << i << "\t";

                        if (tempo[j] > 0) {
                            std::cout << "*** RUN ON TOP OF SOMETHING ***";
                            check += 1;
                            break;
                        }

                        tempo[j] = std::ceil(task->get_time() * virtual_machine->get_slowdown());

                        // Checks if the readings were taken by the machine before execution
                        for (int d1 = 0; d1 < static_cast<int>(input_files.size()) and check <= 1; d1++) {
                            bool has_read_file = false;

                            for (auto k = 0ul; k < data->mb_ and check <= 1; k++) {
                                for (auto q = 0ul; q < data->t_ and check <= 1; q++) {
                                    auto r_idjpt = static_cast<float>(data->r[i][d1][j][k][q]);
                                    // Returns the value of the variable
                                    if (r_idjpt >= (1 - data->PRECISION)) {
                                        has_read_file = true;
                                    }
                                }
                            }

                            if (!has_read_file) {
                                std::cout << "*** PERFORMED THE TASK WITHOUT READING ALL THE ENTRIES *** missing data ="
                                          << input_files[static_cast<size_t>(d1)]->get_id()
                                          << "\n" << input_files[static_cast<size_t>(d1)];
                                check += 1;
                                break;
                            }
                        }
                        // ----------------------------------------------------------------
                    }

                    if (check > 1) {
                        std::cout << "*** CONFLICT WITH EXECUTION ***";
                        break;
                    }
                }

                // Read
                for (size_t i = 0; i < static_cast<size_t>(data->n_) and check <= 1; i++) {
                    std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(1ul + i));
                    std::vector<File *> input_files = task->get_input_files();

                    for (int d1 = 0; d1 < static_cast<int>(input_files.size()) and check <= 1; d1++) {
                        File *file = input_files[static_cast<size_t>(d1)];

                        for (size_t k = 0ul; k < static_cast<size_t>(data->mb_) and check <= 1ul; k++) {
                            std::shared_ptr<Storage> storage = algorithm->GetStoragePerId(static_cast<size_t>(k));

                            auto r_idjpt = static_cast<float>(data->r[i][d1][j][k][t1]);

                            // Returns the value of the variable
                            if (r_idjpt >= (1 - data->PRECISION)) {
                                int read_time = ComputeFileTransferTime(file, virtual_machine, storage);

                                check += 1;
                                std::cout << "r" << i << "(" << input_files[static_cast<size_t>(d1)]->get_id() << ")<"
                                          << k << "\t";

                                if (tempo[j] > 0) {
                                    std::cout << "*** READ OVER SOMETHING ***";
                                    check += 1;
                                    break;
                                }

                                tempo[j] = read_time;

                                // Check file existence
                                auto y_djt = static_cast<float>(data->y[static_cast<int>(file->get_id())][k][t1]);
                                // Returns the value of the variable

                                if (y_djt < (1 - data->PRECISION)) {
                                    std::cout << "*** READ NON-EXISTENT FILE IN THE MACHINE ***";
                                    check += 1;
                                    break;
                                }
                                // ---------------------------

                            }

                            if (check > 1) {
                                std::cout << "*** CONFLICT WITH READING ***";
                                break;
                            }
                        }
                    }
                }

                // Writing
                for (size_t has_passed = 0ul, i = 0ul; i < static_cast<size_t>(data->n_) and check <= 1; i++) {
                    std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(1ul + i));
                    std::vector<File *> output_files = task->get_output_files();

                    for (int d1 = 0; d1 < static_cast<int>(output_files.size()) and check <= 1; d1++) {
                        File *file = output_files[static_cast<size_t>(d1)];

                        for (auto k = 0ul; k < data->mb_ and check <= 1; k++) {
                            std::shared_ptr<Storage> storage = algorithm->GetStoragePerId(static_cast<size_t>(k));

                            auto w_idjpt = static_cast<float>(data->w[i][d1][j][k][t1]);
                            // Returns the value of the variable

                            if (w_idjpt >= (1 - data->PRECISION)) {
                                int write_time = ComputeFileTransferTime(file, virtual_machine, storage);

                                check += 1;
                                std::cout << "w" << i << "(" << output_files[static_cast<size_t>(d1)]->get_id() << ")<"
                                          << k << "\t";

                                if (tempo[j] > 0) {
                                    std::cout << "*** WRITE ON SOMETHING ***";
                                    check += 1;
                                    break;
                                }

                                tempo[j] = write_time;

                                // Checks if the task has been executed on the machine before
                                has_passed = 0;
                                for (auto q = 0ul; q < data->t_ and check <= 1; q++) {
                                    auto x_ijt = static_cast<float>(data->x[i][j][q]);

                                    if (x_ijt >= (1 - data->PRECISION)) {
                                        has_passed = 1;
                                    }
                                }

                                if (has_passed == 0) {
                                    std::cout << "*** WROTE DATA WITHOUT HAVING PERFORMED THE TASK ON THE MACHINE ***";
                                    check += 1;
                                    break;
                                }
                                // -----------------------------------------------
                            }

                            if (check > 1) {
                                std::cout << "*** CONFLICT WITH WRITING ***";
                                break;
                            }
                        }
                    }
                }

                // Idle machine at this time
                if (check == 0ul) {
                    std::cout << "-\t";
                }

                // Data
                for (auto d1 = 0ul; d1 < data->d_ and check <= 1; d1++) {
                    auto y_djt = static_cast<float>(data->y[d1][j][t1]);
                    // Returns the value of the variable

                    if (y_djt >= (1 - data->PRECISION)) {
                        maq_dado[j][d1] = 1;
                    }

                    if (maq_dado[j][d1] == 1) {
                        std::cout << d1 << ",";
                    }
                }

                std::cout << "\t";

                // Hiring
                auto v_jt = static_cast<float>(data->v[j][t1]);
                // Returns the value of the variable

                if (v_jt >= (1 - data->PRECISION)) {
                    std::cout << "*\t";
                } else {
                    std::cout << "\t";
                }
            } else {
                // Idle machine at this time
                // if (check == 0)
                // {
                std::cout << "N\\A\t";
                // }

                // Data
                for (auto d1 = 0ul; d1 < data->d_ and check <= 1; d1++) {
                    auto y_djt = static_cast<float>(data->y[d1][j][t1]);  // Returns the value of the variable

                    if (y_djt >= (1 - data->PRECISION)) {
                        maq_dado[j][d1] = 1;
                    }

                    if (maq_dado[j][d1] == 1) {
                        std::cout << d1 << ",";
                    }
                }

                std::cout << "\t";

                // Hiring
                std::cout << "N\\A\t";
            }
        }

        std::cout << std::endl;
    }

    std::cout << "---------------------------------------------" << std::endl;

    // Releasing resources

    delete[] tempo;
    for (auto i = 0ul; i < data->mb_; i++) {
        delete[] maq_dado[i];
    }
    delete[] maq_dado;
}

/*
 * Print variables
 */
void PrintBest(struct BEST *data) {
    Algorithm *algorithm = data->algorithm_;

    // -------- x ----------
    for (auto i = 0ul; i < data->n_; ++i) {
        for (auto j = 0ul; j < data->m_; ++j) {
            for (auto t = 0ul; t < data->t_; ++t) {
                if (data->x[i][j][t] > data->PRECISION) {
                    std::cout << "x[" << i << "][" << j << "][" << t << "] = " << data->x[i][j][t] << std::endl;
                }
            }
        }
    }

    // -------- r ----------
    for (size_t i = 0ul; i < static_cast<size_t>(data->n_); ++i) {
        std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(i + 1));
//        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();
        std::vector<File *> input_files = task->get_input_files();

        // for (int j = 0; j < data->d_; ++j)
        for (auto j = 0ul; j < input_files.size(); ++j) {
            for (auto k = 0ul; k < data->m_; ++k) {
                for (auto l = 0ul; l < data->mb_; ++l) {
                    for (auto m = 0ul; m < data->t_; ++m) {
                        if (data->r[i][j][k][l][m] > data->PRECISION) {
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
    for (auto i = 0ul; i < static_cast<size_t>(data->n_); ++i) {
        std::shared_ptr<Activation> task = algorithm->GetActivationPerId(static_cast<size_t>(i + 1));
//        std::vector<std::shared_ptr<File>> output_files = task->get_output_files();
        std::vector<File *> output_files = task->get_output_files();

        // for (int j = 0; j < data->d_; ++j)
        for (auto j = 0ul; j < output_files.size(); ++j) {
            for (auto k = 0ul; k < data->m_; ++k) {
                for (auto l = 0ul; l < data->mb_; ++l) {
                    for (auto m = 0ul; m < data->t_; ++m) {
                        if (data->w[i][j][k][l][m] > data->PRECISION) {
                            std::cout << "w[" << i << "][" << j << "][" << k << "][" << l << "][" << m << "] = "
                                      << data->w[i][j][k][l][m] << std::endl;
                        }
                    }
                }
            }
        }
    }

    // -------- y ----------
    for (auto i = 0ul; i < data->d_; ++i) {
        for (auto j = 0ul; j < data->mb_; ++j) {
            for (auto k = 0ul; k < data->t_; ++k) {
                if (data->y[i][j][k] > data->PRECISION) {
                    std::cout << "y[" << i << "][" << j << "][" << k << "] = " << data->y[i][j][k] << std::endl;
                }
            }
        }
    }

    // -------- yb ----------
    for (auto i = 0ul; i < data->d_; ++i) {
        for (auto j = 0ul; j < data->mb_; ++j) {
            if (data->yb[i][j] > data->PRECISION) {
                std::cout << "yb[" << i << "]["
                          << j << "] = " << data->yb[i][j] << std::endl;
            }
        }
    }

    // -------- ws ----------
    for (auto i = 0ul; i < data->d_ - 1; ++i) {
        for (auto j = i + 1ul; j < data->d_; ++j) {
            int conflict = algorithm->get_conflict_graph().ReturnConflict(static_cast<size_t>(i),
                                                                          static_cast<size_t>(j));

            if (conflict > 0) {  // soft constraint
                if (data->ws[i][j] > data->PRECISION) {
                    std::cout << "ws[" << i << "]["
                              << j << "] = " << data->ws[i][j] << std::endl;
                }
            }
        }
    }

    // -------- e ----------
    for (auto i = 0ul; i < data->numr_; ++i) {
        for (auto j = 0ul; j < data->n_; ++j) {
            if (data->e[i][j] > data->PRECISION) {
                std::cout << "e[" << i << "][" << j << "] = " << data->e[i][j] << std::endl;
            }
        }
    }

    // -------- b ----------
    for (auto i = 0ul; i < data->numb_; ++i) {
        std::shared_ptr<Storage> storage = algorithm->GetStoragePerId(
                algorithm->GetVirtualMachineSize() + static_cast<size_t>(i));

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            for (auto j = 0ul; j <= bucket->get_number_of_GB_per_cost_intervals(); ++j) {
                if (data->b[i][j] > data->PRECISION) {
                    std::cout << "b[" << i << "][" << j << "] = " << data->b[i][j] << std::endl;
                }
            }
        } else {
            exit(1);  // error
        }
    }

    // -------- q ----------
    for (auto i = 0ul; i < data->numb_; ++i) {
        std::shared_ptr<Storage> storage = algorithm->GetStoragePerId(
                algorithm->GetVirtualMachineSize() + static_cast<size_t>(i));

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            for (auto j = 0ul; j <= bucket->get_number_of_GB_per_cost_intervals(); ++j) {
                if (data->q[i][j] > data->PRECISION) {
                    std::cout << "q[" << i << "][" << j << "] = " << data->q[i][j] << std::endl;
                }
            }
        } else {
            exit(1);  // error
        }
    }

    // -------- v ----------
    for (auto i = 0ul; i < data->m_; ++i) {
        for (auto j = 0ul; j < data->t_; ++j) {
            if (data->v[i][j] > data->PRECISION) {
                std::cout << "v[" << i << "][" << j << "] = " << data->v[i][j] << std::endl;
            }
        }
    }

    // -------- z ----------
    for (auto i = 0ul; i < data->m_; ++i) {
        if (data->z[i] > data->PRECISION) {
            std::cout << "z[" << i << "] = " << data->z[i] << std::endl;
        }
    }

    // -------- z_max ----------
    if (data->z_max > data->PRECISION) {
        std::cout << "z_max" << " = " << data->z_max << std::endl;
    }
}

// Callback of the best solution found
ILOINCUMBENTCALLBACK2(CB_incub_sol, struct BEST*, data, struct CPLEX*, cplx) {
    auto val_sol = (double) getObjValue();
    IloCplex::MIPCallbackI::NodeId no_sol = getNodeId();
    double gap = 100.0 * ((double) getMIPRelativeGap());
    double time_f = ((double) clock() - (double) data->start) / CLOCKS_PER_SEC;
    bool print_debug = true;

    Algorithm *algorithm = data->algorithm_;

    if (print_debug) {
        std::cout << "------------------ FOUND NEW SOLUTION ----------------------" << std::endl;
        std::cout << "no      = " << no_sol << std::endl;
        std::cout << "sol     = " << val_sol << std::endl;
        std::cout << "gap     = " << gap << std::endl;
        std::cout << "tempo   = " << time_f << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
    }

    if (val_sol + data->PRECISION < data->b_sol) {
        data->b_sol = val_sol;

        // -------- x ----------
        for (auto i = 0ul; i < data->n_; ++i) {
            for (auto j = 0ul; j < data->m_; ++j) {
                for (auto t = 0ul; t < data->t_; ++t) {
                    data->x[i][j][t] = static_cast<int>((float) getValue(
                            cplx->x[static_cast<IloInt>(i)][static_cast<IloInt>(j)][static_cast<IloInt>(t)]));
                }
            }
        }

        // -------- r ----------
        for (auto i = 0ul; i < data->n_; ++i) {
            std::shared_ptr<Activation> task =
                    algorithm->GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
            std::vector<File *> input_files = task->get_input_files();

            for (auto j = 0ul; j < input_files.size(); ++j) {
                for (auto k = 0ul; k < data->m_; ++k) {
                    for (auto l = 0ul; l < data->mb_; ++l) {
                        for (auto m = 0ul; m < data->t_; ++m) {
                            data->r[i][j][k][l][m] = static_cast<int>((float) getValue(
                                    cplx->r[static_cast<IloInt>(i)][static_cast<IloInt>(j)][static_cast<IloInt>(k)]
                                            [static_cast<IloInt>(l)][static_cast<IloInt>(m)]));
                        }
                    }
                }
            }
        }

        // -------- w ----------
        for (auto i = 0ul; i < data->n_; ++i) {
            std::shared_ptr<Activation>
                    task = algorithm->GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//            std::vector<std::shared_ptr<File>> output_files = task->get_output_files();
            std::vector<File *> output_files = task->get_output_files();

            for (auto j = 0ul; j < output_files.size(); ++j) {
                for (auto k = 0ul; k < data->m_; ++k) {
                    for (auto l = 0ul; l < data->mb_; ++l) {
                        for (auto m = 0ul; m < data->t_; ++m) {
                            data->w[i][j][k][l][m] = static_cast<int>((float) getValue(
                                    cplx->w[static_cast<IloInt>(i)][static_cast<IloInt>(j)][static_cast<IloInt>(k)]
                                            [static_cast<IloInt>(l)][static_cast<IloInt>(m)]));
                        }
                    }
                }
            }
        }

        // -------- y ----------
        for (auto i = 0ul; i < data->d_; ++i) {
            for (auto j = 0ul; j < data->mb_; ++j) {
                for (auto k = 0ul; k <= data->t_; ++k) {
                    data->y[i][j][k] = static_cast<int>((float) getValue(
                            cplx->y[static_cast<IloInt>(i)][static_cast<IloInt>(j)][static_cast<IloInt>(k)]));
                }
            }
        }

        // -------- yb ----------
        for (auto i = 0ul; i < data->d_; ++i) {
            for (auto j = 0ul; j < data->mb_; ++j) {
                data->yb[i][j] = static_cast<int>((float) getValue(
                        cplx->yb[static_cast<IloInt>(i)][static_cast<IloInt>(j)]));
            }
        }

        // -------- ws ----------
        for (auto i = 0ul; i < data->d_ - 1; ++i) {
            for (auto j = i + 1ul; j < data->d_; ++j) {
                int conflict = algorithm->get_conflict_graph().ReturnConflict(static_cast<size_t>(i),
                                                                              static_cast<size_t>(j));

                if (conflict > 0) {  // soft constraint
                    data->ws[i][j] = static_cast<int>((float) getValue(
                            cplx->ws[static_cast<IloInt>(i)][static_cast<IloInt>(j)]));
                }
            }
        }

        // -------- e ----------
        for (auto i = 0ul; i < data->numr_; ++i) {
            for (auto j = 0ul; j < data->n_; ++j) {
                data->e[i][j] = (float) getValue(cplx->e[static_cast<IloInt>(i)][static_cast<IloInt>(j)]);
            }
        }

        // -------- b ----------
        for (auto i = 0ul; i < data->numb_; ++i) {
            std::shared_ptr<Storage>
                    storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + static_cast<size_t>(i));

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (auto j = 0ul; j <= bucket->get_number_of_GB_per_cost_intervals(); ++j) {
                    data->b[i][j] = static_cast<int>((float) getValue(
                            cplx->b[static_cast<IloInt>(i)][static_cast<IloInt>(j)]));
                }
            } else {
                exit(1);  // Error
            }
        }

        // -------- q ----------
        for (auto i = 0ul; i < data->numb_; ++i) {
            std::shared_ptr<Storage>
                    storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + static_cast<size_t>(i));

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (auto j = 0ul; j <= bucket->get_number_of_GB_per_cost_intervals(); ++j) {
                    data->q[i][j] = (float) getValue(cplx->q[static_cast<IloInt>(i)][static_cast<IloInt>(j)]);
                }
            } else {
                exit(1);  // Error
            }
        }

        // -------- v ----------
        for (auto i = 0ul; i < data->m_; ++i) {
            for (auto j = 0ul; j < data->t_; ++j) {
                data->v[i][j] = static_cast<int>((float) getValue(
                        cplx->v[static_cast<IloInt>(i)][static_cast<IloInt>(j)]));
            }
        }

        // -------- z ----------
        for (auto i = 0ul; i < data->m_; ++i) {
            data->z[i] = (float) getValue(cplx->z[static_cast<IloInt>(i)]);
        }

        // -------- z_max ----------
        data->z_max = (float) getValue(cplx->z_max[0]);
    }
}

ILOSTLBEGIN

void Cplex::Run() {
    DLOG(INFO) << "Executing CPLEX ...";
    // Time
    // clock_t t_start = clock();

    // Variable names
    char var_name[100];

    // Cplex structure (environment, model and variables)
    int _n = static_cast<int>(GetActivationSize() - 2);  // less source and target
    int _d = static_cast<int>(GetFileSize());
    int _m = static_cast<int>(GetVirtualMachineSize());
    int _mb = static_cast<int>(GetStorageSize());
    int _num_r = static_cast<int>(GetRequirementsSize());
    int _num_b = static_cast<int>(get_bucket_size());
    int _time_max = static_cast<int>(makespan_max_);
    double _cost_max = get_budget_max();
    double _security_max = get_maximum_security_and_privacy_exposure();

    int max_num_intervals = 2;

    struct CPLEX cplex(_n, _d, _m, _num_r, _num_b);
    struct BEST best(_n, _d, _m, _num_r, _num_b, _mb, _time_max, max_num_intervals, this);

    // Execution variables
    // X_IJT => task I, which is on machine J, starts executing in period T
    for (int i = 0; i < _n; i++) {
        cplex.x[i] = IloArray<IloBoolVarArray>(cplex.env, _m);
        for (int j = 0; j < _m; j++) {
            cplex.x[i][j] = IloBoolVarArray(cplex.env, _time_max);
            for (int k = 0; k < _time_max; k++) {
                sprintf(var_name, "x_%d_%d_%d", (int) i, (int) j, (int) k);  // Variable name
                cplex.x[i][j][k] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                cplex.model.add(cplex.x[i][j][k]);  // Add variable to model
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Read variables
    // R_IDJPT => activation I that is on machine J, start reading its D-th input data
    // (note that it is not index data D) from machine P in period T
    for (int i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();
        std::vector<File *> input_files = task->get_input_files();
        cplex.r[i] = IloArray<IloArray<IloArray<IloBoolVarArray>>>(cplex.env, static_cast<int>(input_files.size()));

        for (int j = 0; j < static_cast<int>(input_files.size()); j++) {
            cplex.r[i][j] = IloArray<IloArray<IloBoolVarArray>>(cplex.env, _m);
            for (int k = 0; k < _m; k++) {
                cplex.r[i][j][k] = IloArray<IloBoolVarArray>(cplex.env, _mb);
                for (int l = 0; l < _mb; l++) {
                    cplex.r[i][j][k][l] = IloBoolVarArray(cplex.env, _time_max);
                    for (int m = 0; m < _time_max; m++) {
                        sprintf(var_name, "r_%d_%d_%d_%d_%d", (int) i, (int) j, (int) k, (int) l,
                                (int) m);  // Variable name
                        cplex.r[i][j][k][l][m] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                        cplex.model.add(cplex.r[i][j][k][l][m]);  // Add variable to model
                    }
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Writing variables
    // W_IDJPT => activation I which is on machine J, starts writing its D-th input data
    // (note that it is not index data D) from machine P in period T
    for (int i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//        std::vector<std::shared_ptr<File>> output_files = task->get_output_files();
        std::vector<File *> output_files = task->get_output_files();
        cplex.w[i] = IloArray<IloArray<IloArray<IloBoolVarArray>>>(cplex.env, static_cast<int>(output_files.size()));

        for (int j = 0; j < static_cast<int>(output_files.size()); j++) {
            cplex.w[i][j] = IloArray<IloArray<IloBoolVarArray>>(cplex.env, _m);
            for (int k = 0; k < _m; k++) {
                cplex.w[i][j][k] = IloArray<IloBoolVarArray>(cplex.env, _mb);
                for (int l = 0; l < _mb; l++) {
                    cplex.w[i][j][k][l] = IloBoolVarArray(cplex.env, _time_max);
                    for (int m = 0; m < _time_max; m++) {
                        // Variable name
                        sprintf(var_name, "w_%d_%d_%d_%d_%d", (int) i, (int) j, (int) k, (int) l, (int) m);
                        cplex.w[i][j][k][l][m] = IloBoolVar(cplex.env,
                                                            var_name);  // Allocate variable
                        cplex.model.add(cplex.w[i][j][k][l][m]);  // Add variable to model
                    }
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Storage variables
    // Y_DJT => indicates whether index data D is stored on device J in period T
    for (int i = 0; i < _d; i++) {
        cplex.y[i] = IloArray<IloBoolVarArray>(cplex.env, _mb);

        for (int j = 0; j < _mb; j++) {
            cplex.y[i][j] = IloBoolVarArray(cplex.env, _time_max + 1);

            for (int k = 0; k <= _time_max; k++) {
                sprintf(var_name, "y_%d_%d_%d", (int) i, (int) j, (int) k);  // Variable name
                cplex.y[i][j][k] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                cplex.model.add(cplex.y[i][j][k]);  // Add variable to model
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Machine-independent storage variables
    // Y_DJ => indicates whether the data at index D exists at some time on machine J
    for (int i = 0; i < _d; i++) {
        cplex.yb[i] = IloBoolVarArray(cplex.env, _mb);
        for (int j = 0; j < _mb; j++) {
            sprintf(var_name, "yb_%d_%d", (int) i, (int) j);  // Variable name
            cplex.yb[i][j] = IloBoolVar(cplex.env, var_name);  // Allocate variable
            cplex.model.add(cplex.yb[i][j]);  // Add variable to model
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // soft edge violation penalty variables
    // W_D1D2 => soft edge violation di, d2 in E_s
    for (int d1 = 0; d1 < _d - 1; d1++) {
        cplex.ws[d1] = IloBoolVarArray(cplex.env, _d);

        for (int d2 = d1 + 1; d2 < _d; d2++) {
            int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

            if (conflict > 0) {  // soft constraint
                sprintf(var_name, "ws_%d_%d", (int) d1, (int) d2);  // Variable name
                cplex.ws[d1][d2] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                cplex.model.add(cplex.ws[d1][d2]);  // Add variable to model
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Exposure variables
    // E_RI => exposure level of task I by requirement R
    for (int r = 0; r < _num_r; r++) {
        cplex.e[r] = IloNumVarArray(cplex.env, _n);

        for (int i = 0; i < _n; i++) {
            sprintf(var_name, "xe_%d_%d", (int) r, (int) i);  // Variable name
            cplex.e[r][i] = IloNumVar(cplex.env, 0, IloInfinity, var_name);  // Allocate variable
            cplex.model.add(cplex.e[r][i]);  // Add variable to model
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Bucket usage variables
    // B_JL => if bucket J is being used in range L
    // *NOTE* ******* INTERVAL 0 is used to indicate that the BUCKET IS NOT BEING USED ******
    for (int b = 0; b < _num_b; b++) {
        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            cplex.b[b] = IloBoolVarArray(cplex.env, static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()) + 1);

            for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                sprintf(var_name, "b_%d_%d", (int) b, (int) l);  // Variable name
                cplex.b[b][l] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                cplex.model.add(cplex.b[b][l]);  // Add variable to model
            }
        } else {
            exit(1);  // Error
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Variables for using buckets
    // Q_JL => amount of data used by bucket J in range L
    // *NOTE* ******* INTERVAL 0 is used to indicate that the BUCKET IS NOT BEING USED ******
    for (int b = 0; b < _num_b; b++) {
        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            cplex.q[b] = IloNumVarArray(cplex.env, static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()) + 1);

            for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                sprintf(var_name, "q_%d_%d", (int) b, (int) l);  // Variable name
                cplex.q[b][l] = IloNumVar(cplex.env, 0, IloInfinity, var_name);  // Allocate variable
                cplex.model.add(cplex.q[b][l]);  // Add variable to model
            }
        } else {
            exit(1);  // Error
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Machine usage variables by time
    // V_JT => indicates whether machine J is in use (contracted) in period T
    for (int i = 0; i < _m; i++) {
        cplex.v[i] = IloBoolVarArray(cplex.env, _time_max);
        for (int j = 0; j < _time_max; j++) {
            sprintf(var_name, "v_%d_%d", (int) i, (int) j);  // Variable name
            cplex.v[i][j] = IloBoolVar(cplex.env, var_name);  // Allocate variable
            cplex.model.add(cplex.v[i][j]);  // Add variable to model
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Variables of total time of use per machine
    // Z_J => indicates total time that machine J was used (contracted)
    for (int i = 0; i < _m; i++) {
        sprintf(var_name, "z_%d", (int) i);  // Variable name
        cplex.z[i] = IloNumVar(cplex.env, 0, IloInfinity, var_name);  // Allocate variable
        cplex.model.add(cplex.z[i]);  // Add variable to model
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Total time variables (makespan)
    // Z_MAX => makespan do workflow
    sprintf(var_name, "z_max");  // Variable name
    cplex.z_max[0] = IloNumVar(cplex.env, 0, IloInfinity, var_name);  // Allocate variable
    cplex.model.add(cplex.z_max[0]);  // Add variable to model

    DLOG(INFO) << var_name << " added to the model.";

    // ---------------- Objective function -------------------
    IloExpr fo(cplex.env);

    // ---- Makespan
    fo = alpha_time_ * (cplex.z_max[0] / _time_max);

    // ----- Financial cost
    for (int j = 0; j < _m; j++) {
        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));
        fo += alpha_budget_ * ((virtual_machine->get_cost() * cplex.z[j]) / _cost_max);
    }

    for (int b = 0; b < _num_b; b++) {
        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                fo += alpha_budget_ * ((bucket->get_cost() * cplex.q[b][l]) / _cost_max);
            }
        } else {
            exit(1);  // Error
        }
    }

    // Security exposure
    for (int r = 0; r < _num_r; r++) {
        for (int i = 0; i < _n; i++) {
            fo += alpha_security_ * (cplex.e[r][i] / _security_max);
        }
    }

    // Soft edge penalties
    for (int d1 = 0; d1 < _d - 1; d1++) {
        for (int d2 = d1 + 1; d2 < _d; d2++) {
            int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

            if (conflict > 0) {  // soft constraint
                fo += alpha_security_ * (conflict * cplex.ws[d1][d2] / _security_max);
            }
        }
    }

    cplex.model.add(IloMinimize(cplex.env, fo, "fo"));  // Add objective function to the model

    DLOG(INFO) << "Object Function added to the model.";

    // -----------------------------------------------------

    // --------------- RESTRICTIONS ----------

    // Restriction (4)
    for (int i = 0; i < _n; i++) {
        IloExpr exp(cplex.env);

        for (int j = 0; j < _m; j++) {
            for (int t = 0; t < _time_max; t++) {
                exp += cplex.x[i][j][t];
            }
        }

        IloConstraint c(exp == 1);

        sprintf(var_name, "c4_%d", (int) i);
        c.setName(var_name);
        cplex.model.add(c);

        exp.end();
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (5)
    for (int i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();
        std::vector<File *> input_files = task->get_input_files();

        for (int d = 0; d < static_cast<int>(input_files.size()); d++) {
            IloExpr exp(cplex.env);

            for (int j = 0; j < _m; j++) {
                for (int p = 0; p < _mb; p++) {
                    for (int t = 0; t < _time_max; t++) {
                        exp += cplex.r[i][d][j][p][t];
                    }
                }
            }

            IloConstraint c(exp == 1);

            sprintf(var_name, "c5_%d_%d", (int) i, (int) d);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (6)
    for (int i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//        std::vector<std::shared_ptr<File>> output_files = task->get_output_files();
        std::vector<File *> output_files = task->get_output_files();

        for (int d = 0; d < static_cast<int>(output_files.size()); d++) {
            IloExpr exp(cplex.env);

            for (int j = 0; j < _m; j++) {
                for (int p = 0; p < _mb; p++) {
                    for (int t = 0; t < _time_max; t++) {
                        exp += cplex.w[i][d][j][p][t];
                    }
                }
            }

            IloConstraint c(exp == 1);
            sprintf(var_name, "c6_%d_%d", (int) i, (int) d);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (7)
    for (int roof, i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
        std::vector<File *> output_files = task->get_output_files();

        for (int d = 0; d < static_cast<int>(output_files.size()); d++) {
            for (int j = 0; j < _m; j++) {
                std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

                for (int p = 0; p < _mb; p++) {
                    int tempo = std::ceil(task->get_time() * virtual_machine->get_slowdown());

                    for (int t = tempo; t < _time_max; t++) {
                        IloExpr exp(cplex.env);

                        exp += cplex.w[i][d][j][p][t];

                        /* (q <= roof) because the size of the interval is the same no matter if the time starts from 0
                         * or 1 */
                        roof = std::max(0, t - tempo);
                        // Why do we have this max?
                        // Why it is up to q <= roof and not q < roof
                        for (int q = 0; q <= roof; q++) {
                            exp -= cplex.x[i][j][q];
                        }

                        IloConstraint c(exp <= 0);

                        sprintf(var_name, "c7_%d_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) p, (int) t);
                        c.setName(var_name);
                        cplex.model.add(c);

                        exp.end();
                    }
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (8)
    for (int i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
        std::vector<File *> output_files = task->get_output_files();

        for (int d = 0; d < static_cast<int>(output_files.size()); d++) {
            for (int j = 0; j < _m; j++) {
                std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

                for (int p = 0; p < _mb; p++) {
                    int tempo = std::ceil(task->get_time() * virtual_machine->get_slowdown());
                    // for (int t = tempo; t < _time_max; t++)
                    int limit = min(_time_max, tempo);
                    for (int t = 0; t < limit; t++) {
                        IloExpr exp(cplex.env);
                        exp += cplex.w[i][d][j][p][t];

                        IloConstraint c(exp == 0);
                        sprintf(var_name, "c8_%d_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) p, (int) t);
                        c.setName(var_name);
                        cplex.model.add(c);

                        exp.end();
                    }
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (9)
    for (int roof, i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();
        std::vector<File *> input_files = task->get_input_files();

        for (int d = 0; d < static_cast<int>(input_files.size()); d++) {
//            std::shared_ptr<File> file = input_files[static_cast<size_t>(d)];
            File *file = input_files[static_cast<size_t>(d)];

            for (int j = 0; j < _m; j++) {
                std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

                /* let's do it for all t so when (t-t_djp) < 0, the right side will be 0, and it prevents the execution
                 * of the task at that time */
                for (int t = 0; t < _time_max; t++) {
                    IloExpr exp(cplex.env);

                    exp = cplex.x[i][j][t];

                    for (int p = 0; p < _mb; p++) {
                        std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));
                        /* (q <= roof) because the size of the interval is the same no matter if the time starts from 0
                         * or 1*/

                        if ((t + 1 - ComputeFileTransferTime(file, virtual_machine, storage)) >= 0)
                        {
                            roof = max(0, t + 1 - ComputeFileTransferTime(file, virtual_machine, storage));
                            // roof = max(0, t - ComputeFileTransferTime(file, virtual_machine, storage));

                            for (int q = 0; q < roof; q++) {
                                exp -= cplex.r[i][d][j][p][q];
                            }
                        }
                    }

                    IloConstraint c(exp <= 0);
                    sprintf(var_name, "c9_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) t);
                    c.setName(var_name);
                    cplex.model.add(c);

                    exp.end();
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (10)
    for (int piso, j = 0; j < _m; j++) {
        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

        for (int t = 0; t < _time_max; t++) {
            IloExpr exp(cplex.env);

            /* Execution */
            for (int i = 0; i < _n; i++) {
                std::shared_ptr<Activation> task = GetActivationPerId(
                        static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));

                int execution_time = static_cast<int>(std::ceil(task->get_time() * virtual_machine->get_slowdown()));

                piso = max(0, t - execution_time + 1);

                for (int q = piso; q <= t; q++) {
                    exp += cplex.x[i][j][q];
                }
            }

            /* Write */
            for (int i = 0; i < _n; i++) {
                std::shared_ptr<Activation> task = GetActivationPerId(
                        static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
                std::vector<File *> output_files = task->get_output_files();

                for (int d = 0; d < static_cast<int>(output_files.size()); d++) {
                    File *file = output_files[static_cast<size_t>(d)];

                    for (int p = 0; p < _mb; p++) {
                        std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));
                        piso = max(0, t - ComputeFileTransferTime(file, virtual_machine, storage) + 1);
                        for (int rr = piso; rr <= t; rr++) {
                            exp += cplex.w[i][d][j][p][rr];
                        }
                    }
                }
            }

            /* Read */
            for (int i = 0; i < _n; i++) {
                std::shared_ptr<Activation> task = GetActivationPerId(
                        static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
                std::vector<File *> input_files = task->get_input_files();

                for (int d = 0; d < static_cast<int>(input_files.size()); d++) {
                    File *file = input_files[static_cast<size_t>(d)];

                    for (int p = 0; p < _mb; p++) {
                        std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));
                        piso = max(0, t - ComputeFileTransferTime(file, virtual_machine, storage) + 1);
                        for (int rr = piso; rr <= t; rr++) {
                            exp += cplex.r[i][d][j][p][rr];
                        }
                    }
                }
            }

            /* Hiring */
            exp -= cplex.v[j][t];

            IloConstraint c(exp <= 0);
            // IloConstraint c(exp == 0);

            sprintf(var_name, "c10_%d_%d", (int) j, (int) t);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (11)
    for (int j = 0; j < _m; j++) {
        std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(j));

        for (int t = 0; t < _time_max; t++) {
            IloExpr exp(cplex.env);

            /* Write */
            for (int i = 0; i < _n; i++) {
                std::shared_ptr<Activation> task = GetActivationPerId(
                        static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
                std::vector<File *> output_files = task->get_output_files();

                for (int d = 0; d < static_cast<int>(output_files.size()); d++) {
                    File *file = output_files[static_cast<size_t>(d)];

                    for (int p = 0; p < _m; p++) {
                        if (j != p) {
                            std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(
                                    static_cast<size_t>(p));
                            int piso = max(0, t - ComputeFileTransferTime(file, storage, virtual_machine) + 1);

                            for (int rr = piso; rr <= t; rr++) {
                                exp += cplex.w[i][d][p][j][rr];
                            }
                        }
                    }
                }
            }

            /* Read */
            for (int i = 0; i < _n; i++) {
                std::shared_ptr<Activation> task = GetActivationPerId(
                        static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
                std::vector<File *> input_files = task->get_input_files();

                for (int d = 0; d < static_cast<int>(input_files.size()); d++) {
                    File *file = input_files[static_cast<size_t>(d)];

                    for (int p = 0; p < _m; p++) {
                        if (j != p) {
                            std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(
                                    static_cast<size_t>(p));
                            int piso = max(0, t - ComputeFileTransferTime(file, storage, virtual_machine) + 1);

                            for (int rr = piso; rr <= t; rr++) {
                                exp += cplex.r[i][d][p][j][rr];
                            }
                        }
                    }
                }
            }

            /* Hiring */
            exp -= cplex.v[j][t] * (_m - 1);

            IloConstraint c(exp <= 0);

            sprintf(var_name, "c11_%d_%d", (int) j, (int) t);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (12)
    for (int b = static_cast<int>(GetVirtualMachineSize());
         b < static_cast<int>(GetVirtualMachineSize()) + _num_b;
         b++) {
        std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(b));

        for (int t = 0; t < _time_max; t++) {
            IloExpr exp(cplex.env);

            /* Write */
            for (int i = 0; i < _n; i++) {
                std::shared_ptr<Activation> task = GetActivationPerId(
                        static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
                std::vector<File *> output_files = task->get_output_files();

                for (int d = 0; d < static_cast<int>(output_files.size()); d++) {
                    File *file = output_files[static_cast<size_t>(d)];

                    for (int p = 0; p < _m; p++) {
                        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(
                                static_cast<size_t>(p));
                        int piso = max(0, t - ComputeFileTransferTime(file, storage, virtual_machine) + 1);

                        for (int rr = piso; rr <= t; rr++) {
                            exp += cplex.w[i][d][p][b][rr];
                        }
                    }
                }
            }

            /* Read */
            for (int i = 0; i < _n; i++) {
                std::shared_ptr<Activation> task = GetActivationPerId(
                        static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
                std::vector<File *> input_files = task->get_input_files();

                for (int d = 0; d < static_cast<int>(input_files.size()); d++) {
                    File *file = input_files[static_cast<size_t>(d)];

                    for (int p = 0; p < _m; p++) {
                        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(
                                static_cast<size_t>(p));
                        int piso = max(0, t - ComputeFileTransferTime(file, storage, virtual_machine) + 1);

                        for (int rr = piso; rr <= t; rr++) {
                            exp += cplex.r[i][d][p][b][rr];
                        }
                    }
                }
            }

            IloConstraint c(exp <= 1);

            sprintf(var_name, "c11_%d_%d", (int) b, (int) t);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (12)
    for (int d = 0; d < static_cast<int>(files_.size()); ++d) {
        std::shared_ptr<File> file = files_[static_cast<size_t>(d)];

        if (dynamic_pointer_cast<DynamicFile>(file)) {
            for (int j = 0; j < _mb; j++) {
                IloExpr exp(cplex.env);

                exp += cplex.y[d][j][0];

                IloConstraint c(exp == 0);

                sprintf(var_name, "c12_%d_%d", (int) d, (int) j);
                c.setName(var_name);
                cplex.model.add(c);

                exp.end();
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restrictions (13) and (14)
    for (int d = 0; d < static_cast<int>(files_.size()); ++d) {
        std::shared_ptr<File> file = files_[static_cast<size_t>(d)];

        if (auto static_file = dynamic_pointer_cast<StaticFile>(file)) {
            int ind = static_cast<int>(static_file->GetFirstVm());

            for (int mb = 0; mb < static_cast<int>(storages_.size()); ++mb) {
                if (ind == mb) {
                    for (int t = 0; t <= _time_max; t++) {
                        IloExpr exp(cplex.env);

                        exp += cplex.y[d][ind][t];

                        IloConstraint c(exp == 1);

                        sprintf(var_name, "c14_%d_%d_%d", (int) d, (int) ind, (int) t);
                        c.setName(var_name);
                        cplex.model.add(c);

                        exp.end();
                    }
                } else {
                    IloExpr exp(cplex.env);

                    exp += cplex.y[d][mb][0];

                    IloConstraint c(exp == 0);

                    sprintf(var_name, "c13_%d_%d", (int) d, (int) mb);
                    c.setName(var_name);
                    cplex.model.add(c);

                    exp.end();
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (15)
    // For all dynamic "d" data
    for (size_t i = 0; i < GetFileSize(); ++i) {
        if (auto dynamic_file = dynamic_cast<DynamicFile *>(files_[i].get())) {  // Dynamic files
            std::shared_ptr<Activation> pai = dynamic_file->get_parent_task();
            int ind_w = static_cast<int>(dynamic_file->get_parent_output_file_index());
            int d = static_cast<int>(dynamic_file->get_id());
            // Shift on account of SOURCE virtual task (id: 0)
            int pai_i = static_cast<int>(pai->get_id()) - 1;

            for (int p = 0; p < _mb; p++) {
                std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));

                for (int t = 0; t < _time_max; t++) {
                    IloExpr exp(cplex.env);

                    exp += cplex.y[d][p][t + 1];
                    exp -= cplex.y[d][p][t];

                    for (int j = 0; j < _m; j++) {
                        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(
                                static_cast<size_t>(j));

                        int tempo = (t - ComputeFileTransferTime(dynamic_file, storage, virtual_machine) + 1);

                        if (tempo >= 0)
                        {
                            exp -= cplex.w[pai_i][ind_w][j][p][tempo];
                        }
                    }

                    // IloConstraint c(exp <= 0);
                    IloConstraint c(exp == 0);
                    // sprintf (var_name, "c15_%d_%d_%d", (int)d, (int)p, (int)t);
                    sprintf(var_name, "c15_%d_%d_%d", (int) d, (int) p, (int) t);
                    c.setName(var_name);
                    cplex.model.add(c);

                    exp.end();
                }
            }
        } else {  // Static files
            // std::shared_ptr<Activation> pai = dynamic_file->get_parent_task();
            std::shared_ptr<File> file = GetFilePerId(i);
            // int ind_w  = static_cast<int>(dynamic_file->get_parent_output_file_index());
            int d = static_cast<int>(file->get_id());
            // Shift on account of SOURCE virtual task (id: 0)
            // int pai_i = static_cast<int>(pai->get_id()) - 1;

            for (int p = 0; p < _mb; p++) {
                // std::shared_ptr<Storage>storage = GetStoragePerId(static_cast<size_t>(p));

                for (int t = 0; t < _time_max - 1; t++) {
                    IloExpr exp(cplex.env);

                    exp += cplex.y[d][p][t + 1];
                    exp -= cplex.y[d][p][t];

                    // IloConstraint c(exp <= 0);
                    IloConstraint c(exp == 0);

                    sprintf(var_name, "c15_%d_%d_%d", (int) d, (int) p, (int) t);
                    c.setName(var_name);
                    cplex.model.add(c);

                    exp.end();
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (16)
    for (int dd, i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();
        std::vector<File *> input_files = task->get_input_files();

        for (int d = 0; d < static_cast<int>(input_files.size()); d++) {
            for (int p = 0; p < _mb; p++) {
                for (int t = 0; t < _time_max; t++) {
                    IloExpr exp(cplex.env);

                    for (int j = 0; j < _m; j++) {
                        exp += cplex.r[i][d][j][p][t];
                    }

//                    std::shared_ptr<File> file = input_files[static_cast<size_t>(d)];
                    File *file = input_files[static_cast<size_t>(d)];

                    dd = static_cast<int>(file->get_id());
                    exp -= cplex.y[dd][p][t];

                    IloConstraint c(exp <= 0);

                    sprintf(var_name, "c16_%d_%d_%d_%d", (int) i, (int) d, (int) p, (int) t);
                    c.setName(var_name);
                    cplex.model.add(c);

                    exp.end();
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (17)
    for (int j = 0; j < _mb; j++) {
        std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(j));

        for (int t = 0; t <= _time_max; t++) {
            IloExpr exp(cplex.env);

            for (int d = 0; d < _d; d++) {
                std::shared_ptr<File> file = GetFilePerId(static_cast<size_t>(d));
                exp += (file->get_size_in_GB() * cplex.y[d][j][t]);
            }

            IloConstraint c(exp <= storage->get_storage());

            sprintf(var_name, "c17_%d_%d", (int) j, (int) t);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (18)
    for (int i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//        std::vector<std::shared_ptr<File>> output_files = task->get_output_files();
        std::vector<File *> output_files = task->get_output_files();

        for (int d = 0; d < static_cast<int>(output_files.size()); d++) {
//            std::shared_ptr<File> file = output_files[static_cast<size_t>(d)];
            File *file = output_files[static_cast<size_t>(d)];

            for (int j = 0; j < _m; j++) {
                std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

                for (int p = 0; p < _mb; p++) {
                    std::shared_ptr<Storage> storage = GetStoragePerId(static_cast<size_t>(p));

                    for (int t = 0; t < _time_max; t++) {
                        IloExpr exp(cplex.env);

                        exp += (t + ComputeFileTransferTime(file, storage, virtual_machine)) * cplex.w[i][d][j][p][t];
                        exp -= cplex.z_max[0];

                        IloConstraint c(exp <= 0);

                        sprintf(var_name, "c18_%d_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) p, (int) t);
                        c.setName(var_name);
                        cplex.model.add(c);

                        exp.end();
                    }
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (18extra)
    // Restriction to prevent any task from starting before the final 't' and ending in 't' or after
    {
        IloExpr exp(cplex.env);

        // exp += (t + ComputeFileTransferTime(file, storage, virtual_machine)) * cplex.w[i][d][j][p][t];
        exp += cplex.z_max[0];

        IloConstraint c(exp <= _time_max);

        sprintf(var_name, "c18extra");
        c.setName(var_name);
        cplex.model.add(c);

        exp.end();
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (19)
    for (int j = 0; j < _m; j++) {
        for (int t = 0; t < _time_max; t++) {
            IloExpr exp(cplex.env);

            exp += (t + 1) * cplex.v[j][t];  // (t + 1) because you pay times from 0 to t, so (t + 1) times
            exp -= cplex.z[j];

            IloConstraint c(exp <= 0);

            sprintf(var_name, "c19_%d_%d", (int) j, (int) t);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (20)
    {
        IloExpr exp(cplex.env);
        for (int j = 0; j < _m; j++) {
            std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));
            exp += virtual_machine->get_cost() * cplex.z[j];
        }

        for (int b = 0; b < _num_b; b++) {
            std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                    exp += bucket->get_cost() * cplex.q[b][l];  // Remembering that the cost in these instances is
                                                                // constant
                }
            } else {
                exit(1);  // error
            }
        }
        IloConstraint c(exp <= _cost_max);
        sprintf(var_name, "c20");
        c.setName(var_name);
        cplex.model.add(c);
        exp.end();
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (21)
    for (int i = 0; i < _n; i++) {
        std::shared_ptr<Activation> task = GetActivationPerId(static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));
//        std::vector<std::shared_ptr<File>> input_files = task->get_input_files();
        std::vector<File *> input_files = task->get_input_files();

        for (int d = 0; d < static_cast<int>(input_files.size()); d++) {
            for (int j = 0; j < _m; j++) {
                for (int t = 0; t < _time_max; t++) {
                    IloExpr exp(cplex.env);

                    for (int p = 0; p < _mb; p++) {
                        exp += (static_cast<int>(input_files.size())) * cplex.r[i][d][j][p][t];
                    }

                    for (int dd, g = 0; g < static_cast<int>(input_files.size()); g++) {
//                        std::shared_ptr<File> file = input_files[static_cast<size_t>(d)];
                        File *file = input_files[static_cast<size_t>(d)];

                        dd = static_cast<int>(file->get_id());

                        for (int p = 0; p < _mb; p++) {
                            exp -= cplex.y[dd][p][t];
                        }
                    }

                    IloConstraint c(exp <= 0);

                    sprintf(var_name, "c21_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) t);
                    c.setName(var_name);
                    cplex.model.add(c);

                    exp.end();
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (22)
    for (int d = 0; d < _d; d++) {
        for (int j = 0; j < _mb; j++) {
            for (int t = 0; t <= _time_max; t++) {
                IloExpr exp(cplex.env);

                exp += cplex.y[d][j][t];
                exp -= cplex.yb[d][j];

                IloConstraint c(exp <= 0);

                sprintf(var_name, "c22_%d_%d_%d", (int) d, (int) j, (int) t);
                c.setName(var_name);
                cplex.model.add(c);

                exp.end();
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (23)
    for (int d1 = 0; d1 < _d - 1; d1++) {
        for (int d2 = d1 + 1; d2 < _d; d2++) {
            int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

            // hard constraint
            if (conflict == -1) {
                for (int b = 0; b < _mb; b++) {
                    IloExpr exp(cplex.env);

                    exp += cplex.yb[d1][b];
                    exp += cplex.yb[d2][b];

                    IloConstraint c(exp <= 1);

                    sprintf(var_name, "c23_%d_%d_%d", (int) d1, (int) d2, (int) b);
                    c.setName(var_name);
                    cplex.model.add(c);

                    exp.end();
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (24)
    for (int d1 = 0; d1 < _d - 1; d1++) {
        for (int d2 = d1 + 1; d2 < _d; d2++) {
            int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

            // Hard constraint
            if (conflict > 0) {
                for (int b = 0; b < _mb; b++) {
                    IloExpr exp(cplex.env);

                    exp += cplex.yb[d1][b];
                    exp += cplex.yb[d2][b];
                    exp -= cplex.ws[d1][d2];

                    IloConstraint c(exp <= 1);

                    sprintf(var_name, "c24_%d_%d_%d", (int) d1, (int) d2, (int) b);
                    c.setName(var_name);
                    cplex.model.add(c);

                    exp.end();
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (25)
    for (int r = 0; r < _num_r; r++) {
        // Requirement requirement = GetRequirementPerId(static_cast<size_t>(r));
        for (int i = 0; i < _n; i++) {
            std::shared_ptr<Activation> task = GetActivationPerId(
                    static_cast<size_t>(static_cast<unsigned long>(i) + 1ul));

            IloExpr exp(cplex.env);
            exp -= cplex.e[r][i];

            for (int j = 0; j < _m; j++) {
                std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));
                for (int t = 0; t < _time_max; t++) {
                    // exp += /* <RODRIGO> l^{lj}_{vm} */ * cplex.x[i][j][t];
                    exp -= virtual_machine->GetRequirementValue(static_cast<size_t>(r)) * cplex.x[i][j][t];
                }
            }

            // IloConstraint c(exp <= - /* <RODRIGO> l^{ri}_{task} */);
            IloConstraint c(exp <= -task->GetRequirementValue(static_cast<size_t>(r)));

            sprintf(var_name, "c25_%d_%d", (int) i, (int) r);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (26)
    for (int b = 0; b < _num_b; b++) {
        // bucket
        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        // Bucket index among the machines
        int j = static_cast<int>(GetVirtualMachineSize()) + b;

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            IloExpr exp(cplex.env);

            for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                exp += cplex.q[b][l];
            }

            for (int d = 0; d < _d; d++) {
                std::shared_ptr<File> file = files_[static_cast<size_t>(d)];
                exp -= (file->get_size_in_GB() * cplex.yb[d][j]);
                // exp -= (file->get_size() * cplex.yb[d][j]);
            }

            IloConstraint c(exp == 0);

            sprintf(var_name, "c26_%d", (int) j);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        } else {
            exit(1);  // error
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (27)
    for (int b = 0; b < _num_b; b++) {
        // bucket
        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            IloExpr exp(cplex.env);

            for (int l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
                exp += cplex.b[b][l];

            IloConstraint c(exp == 1);
            sprintf(var_name, "c27_%d", (int) b);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (28)
    for (int b = 0; b < _num_b; b++) {
        // bucket
        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        // Bucket index among the machines
        int j = static_cast<int>(GetVirtualMachineSize()) + b;

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            IloExpr exp(cplex.env);

            for (int d = 0; d < _d; d++) {
                std::shared_ptr<File> file = files_[static_cast<size_t>(d)];
                exp += (file->get_size_in_GB() * cplex.yb[d][j]);
                // exp += (file->get_size() * cplex.yb[d][j]);
            }

            for (int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++)
                exp -= /* <RODRIGO> sm_{bl}*/ 51200 * cplex.b[b][l];  // sm_{bl} == 0

            IloConstraint c(exp <= 0);
            sprintf(var_name, "c28_%d", (int) b);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        } else {
            exit(1);  // Error
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (29)
    for (int b = 0; b < _num_b; b++) {
        // bucket
        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            for (int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                IloExpr exp(cplex.env);
                // sm_j_l ==> Storage size of bucket $j \in B$ on interval $l \in L_j$,  where $sm_{j0}=0$.
                // exp += /*  <RODRIGO> sm_{b(l-1)} * */   cplex.b[b][l];
                exp += bucket->get_storage_of_the_interval(static_cast<size_t>(l - 1)) * cplex.b[b][l];
                exp -= cplex.q[b][l];

                IloConstraint c(exp <= 0);
                sprintf(var_name, "c29a_%d_%d", (int) b, (int) l);
                c.setName(var_name);
                cplex.model.add(c);

                exp.end();

                IloExpr exp2(cplex.env);
                // exp2 -= /* <RODRIGO> sm_{bl} * */   cplex.b[b][l];
                // exp2 -= 51200 * cplex.b[b][l];
                exp2 -= bucket->get_storage_of_the_interval(static_cast<size_t>(l)) * cplex.b[b][l];
                exp2 += cplex.q[b][l];

                IloConstraint c2(exp2 <= 0);
                sprintf(var_name, "c29b_%d_%d", (int) b, (int) l);
                c2.setName(var_name);
                cplex.model.add(c2);

                exp2.end();
            }
        } else {
            exit(1);  // Error
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (30)
    for (int b = 0; b < _num_b; b++) {
        // Bucket
        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        // Bucket index among the machines
        int j = static_cast<int>(GetVirtualMachineSize()) + b;

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            for (int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                IloExpr exp(cplex.env);

                exp += cplex.b[b][l];

                for (int d = 0; d < _d; d++)
                    exp -= cplex.yb[d][j];

                IloConstraint c(exp <= 0);
                sprintf(var_name, "c30_%d_%d", (int) b, (int) l);
                c.setName(var_name);
                cplex.model.add(c);

                exp.end();
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    IloCplex solver(cplex.model);  // Declare variable "solver" over the model to be solved
    // solver.exportModel("model.lp");  // Write model to file in .lp format

    DLOG(INFO) << "Point to " << FLAGS_cplex_output_file.c_str() << " file to receive the output data.";

    solver.exportModel(FLAGS_cplex_output_file.c_str());  // Write model to file in .lp format

    DLOG(INFO) << FLAGS_cplex_output_file.c_str() << " CPLEX model file exported.";

    // Open CPLEX model
    int error;
    CPXENVptr env = CPXopenCPLEX(&error);

    DLOG(INFO) << "CPLEX model opened.";

    // Parameters
    //  solver.setParam(IloCplex::TiLim, 3600);       // limit time (3600=1h, 86400=1d)
    //  CPX set dbl param(env, CPX_PARAM_ TIL IM, 3600.0);
    //  CPX set dbl param(env, CPX_PARAM_MIP INTERVAL, 100.0);
    //  solver.setParam(IloCplex::MIPInterval, 100);  // Log a cada N nos

    // -------------------- Callbacks ----------------------

    // When finding a new solution (incumbent solution)
    solver.use(CB_incub_sol(cplex.env, &best, &cplex));

    DLOG(INFO) << "Solver configured.";

    // Resolution method
    try {
        solver.solve();
    } catch (IloException &e) {
        cout << e;
    }

    DLOG(INFO) << "Solver executed.";

    // Information output
    bool solved;
    double res, lb, time_s;

    try {
        solved = solver.getStatus() == IloAlgorithm::Optimal;

        res = solver.getObjValue();  // Solution
        lb = solver.getBestObjValue();  // Dual-limit (inferior)
        time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time

        double cost = 0.0;
        double security = 0.0;

        // ----- Financial cost
        for (int j = 0; j < _m; j++) {
            std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

            cost += virtual_machine->get_cost() * best.z[j];
        }

        for (int b = 0; b < _num_b; b++) {
            std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (int l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                    cost += bucket->get_cost() * best.q[b][l];
                }
            } else {
                exit(1);  // error
            }
        }

        // Exposure
        for (int r = 0; r < _num_r; r++) {
            for (int i = 0; i < _n; i++) {
                security += best.e[r][i] / _security_max;
            }
        }

        // Soft edge penalties
        for (int d1 = 0; d1 < _d - 1; d1++) {
            for (int d2 = d1 + 1; d2 < _d; d2++) {
                int conflict = conflict_graph_.ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

                if (conflict > 0) {  // Soft constraint
                    security += conflict * best.ws[d1][d2] / _security_max;
                }
            }
        }

        cout << endl;
        cout << "----- Done ----" << endl;
        cout << "SOLVED?  : " << solved << endl;
        cout << "MAKESPAN : " << best.z_max << endl;
        cout << "COST     : " << cost << endl;
        cout << "SECURITY : " << security << endl;
        cout << "TIME     : " << time_s << endl;
        cout << "LB       : " << lb << endl;
        cout << "UB (X*)  : " << res << endl;
        cout << endl;

        cerr << best.z_max << " " << cost << " " << security << " " << res << endl << time_s << endl;

        DLOG(INFO) << "Results:";
        DLOG(INFO) << best.z_max << " " << cost << " " << security << " " << res << endl << time_s;

        // ------------------------ Solution -------------------------
        // if (data.D E P U)
        // {
        //   if (data.b_sol==1.0e+6)
        //     {
        //       c out << "No solution stored" << endl;
        //     }
        //   else
        //     data.print_solution_timeline();
        // }
    } catch (IloCplex::Exception &e) {
        cout << e;
    }

    cplex.env.end();

    PrintBest(&best);
    print_solution_timeline(&best);

    DLOG(INFO) << "Printing BEST:";
    DLOG(INFO) << best;

    DLOG(INFO) << "... ending CPLEX";
}  // void Cplex::Run()

