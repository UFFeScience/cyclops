/**
 * \file src/solution/cplex.cc
 * \brief Contains the \c Algorithm class methods
 *
 * \authors Rodrigo Alves Prado da Silva \<rodrigo.raps@gmail.com\>
 * \authors Yuri Frota \<abitbol@gmail.com\>
 * \copyright Fluminense Federal University (UFF)
 * \copyright Computer Science Department
 * \date 2024
 *
 * This source file contains the methods from the \c Cplex class that run the mode of the
 * exact solution
 */

#include "src/solution/cplex.h"

DECLARE_string(cplex_output_file);

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
int Cplex::ComputeFileTransferTime(const std::shared_ptr<File>& file,
                                   const std::shared_ptr<Storage> &storage1,
                                   const std::shared_ptr<Storage> &storage2) {
    int time = 0;

    // Calculate time
    if (storage1->get_id() != storage2->get_id()) {
        // get the smallest link
        double link = std::min(storage1->get_bandwidth_in_GBps(), storage2->get_bandwidth_in_GBps());
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
                for (auto i = 0ul; i < static_cast<size_t>(data->n_) and check <= 1ul; i++) {
                    auto activation = algorithm->GetActivationPerId(1ul + i);
                    auto input_files = activation->get_input_files();

                    auto x_ijt = static_cast<float>(data->x[i][j][t1]);  // Returns the value of the variable

                    if (x_ijt >= (1 - data->PRECISION)) {
                        check += 1;
                        std::cout << "x" << i << "\t";

                        if (tempo[j] > 0) {
                            std::cout << "*** RUN ON TOP OF SOMETHING ***";
                            check += 1;
                            break;
                        }

                        tempo[j] = std::ceil(activation->get_time() * virtual_machine->get_slowdown());

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
                for (auto i = 0ul; i < static_cast<size_t>(data->n_) and check <= 1; i++) {
                    auto activation = algorithm->GetActivationPerId(1ul + i);
                    auto input_files = activation->get_input_files();

                    for (int d1 = 0; d1 < static_cast<int>(input_files.size()) and check <= 1; d1++) {
                        auto file = input_files[static_cast<size_t>(d1)];

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
                for (auto has_passed = 0ul, i = 0ul; i < static_cast<size_t>(data->n_) and check <= 1; i++) {
                    auto activation = algorithm->GetActivationPerId(1ul + i);
                    auto output_files = activation->get_output_files();

                    for (int d1 = 0; d1 < static_cast<int>(output_files.size()) and check <= 1; d1++) {
                        auto file = output_files[static_cast<size_t>(d1)];

                        for (auto k = 0ul; k < data->mb_ and check <= 1; k++) {
                            auto storage = algorithm->GetStoragePerId(static_cast<size_t>(k));

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

                                // Checks if the activation has been executed on the machine before
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
    auto algorithm = data->algorithm_;

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
    for (auto i = 0ul; i < data->n_; ++i) {
        auto activation = algorithm->GetActivationPerId(1ul + i);
        auto input_files = activation->get_input_files();

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
    for (auto i = 0ul; i < data->n_; ++i) {
        auto activation = algorithm->GetActivationPerId(1ul + i);
        auto output_files = activation->get_output_files();

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
    for (auto i = 0ul; i < algorithm->GetFilesSize(); ++i) {
        for (auto j = 0ul; j < data->mb_; ++j) {
            for (auto k = 0ul; k < data->t_; ++k) {
                if (data->y[i][j][k] > data->PRECISION) {
                    std::cout << "y[" << i << "][" << j << "][" << k << "] = " << data->y[i][j][k] << std::endl;
                }
            }
        }
    }

    // -------- yb ----------
    for (auto i = 0ul; i < algorithm->GetFilesSize(); ++i) {
        for (auto j = 0ul; j < data->mb_; ++j) {
            if (data->yb[i][j] > data->PRECISION) {
                std::cout << "yb[" << i << "]["
                          << j << "] = " << data->yb[i][j] << std::endl;
            }
        }
    }

    // -------- ws ----------
    for (auto i = 0ul; i < algorithm->GetFilesSize() - 1ul; ++i) {
        for (auto j = i + 1ul; j < algorithm->GetFilesSize(); ++j) {
            auto conflict = algorithm->get_conflict_graph()->ReturnConflict(static_cast<size_t>(i),
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
        auto storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + i);

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
        auto storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + i);

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
    auto gap = 100.0 * ((double) getMIPRelativeGap());
    auto time_f = ((double) clock() - (double) data->start) / CLOCKS_PER_SEC;
    auto print_debug = true;

    auto algorithm = data->algorithm_;

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
            auto activation = algorithm->GetActivationPerId(1ul + i);
            auto input_files = activation->get_input_files();

            for (auto d = 0ul; d < input_files.size(); ++d) {
                for (auto j = 0ul; j < data->m_; ++j) {
                    for (auto p = 0ul; p < data->mb_; ++p) {
                        for (auto t = 0ul; t < data->t_; ++t) {
                            data->r[i][d][j][p][t] = static_cast<int>((float) getValue(
                                    cplx->r[static_cast<IloInt>(i)][static_cast<IloInt>(d)][static_cast<IloInt>(j)]
                                            [static_cast<IloInt>(p)][static_cast<IloInt>(t)]));
                        }
                    }
                }
            }
        }

        // -------- w ----------
        for (auto i = 0ul; i < data->n_; ++i) {
            auto activation = algorithm->GetActivationPerId(1ul + i);
            auto output_files = activation->get_output_files();

            for (auto d = 0ul; d < output_files.size(); ++d) {
                for (auto j = 0ul; j < data->m_; ++j) {
                    for (auto p = 0ul; p < data->mb_; ++p) {
                        for (auto t = 0ul; t < data->t_; ++t) {
                            data->w[i][d][j][p][t] = static_cast<int>((float) getValue(
                                    cplx->w[static_cast<IloInt>(i)][static_cast<IloInt>(d)][static_cast<IloInt>(j)]
                                            [static_cast<IloInt>(p)][static_cast<IloInt>(t)]));
                        }
                    }
                }
            }
        }

        // -------- y ----------
        for (auto d = 0ul; d < algorithm->GetFilesSize(); ++d) {
            for (auto j = 0ul; j < data->mb_; ++j) {
                for (auto t = 0ul; t <= data->t_; ++t) {
                    data->y[d][j][t] = static_cast<int>((float) getValue(
                            cplx->y[static_cast<IloInt>(d)][static_cast<IloInt>(j)][static_cast<IloInt>(t)]));
                }
            }
        }

        // -------- yb ----------
        for (auto d = 0ul; d < algorithm->GetFilesSize(); ++d) {
            for (auto j = 0ul; j < data->mb_; ++j) {
                data->yb[d][j] = static_cast<int>((float) getValue(
                        cplx->yb[static_cast<IloInt>(d)][static_cast<IloInt>(j)]));
            }
        }

        // -------- ws ----------
        for (auto d1 = 0ul; d1 < algorithm->GetFilesSize() - 1ul; ++d1) {
            for (auto d2 = d1 + 1ul; d2 < algorithm->GetFilesSize(); ++d2) {
                int conflict = algorithm->get_conflict_graph()->ReturnConflict(static_cast<size_t>(d1),
                                                                               static_cast<size_t>(d2));

                if (conflict > 0) {  // soft constraint
                    data->ws[d1][d2] = static_cast<int>((float) getValue(
                            cplx->ws[static_cast<IloInt>(d1)][static_cast<IloInt>(d2)]));
                }
            }
        }

        // -------- e ----------
        for (auto r = 0ul; r < data->numr_; ++r) {
            for (auto i = 0ul; i < data->n_; ++i) {
                data->e[r][i] = (float) getValue(cplx->e[static_cast<IloInt>(r)][static_cast<IloInt>(i)]);
            }
        }

        // -------- b ----------
        for (auto j = 0ul; j < data->numb_; ++j) {
            auto storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + j);

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (auto l = 0ul; l <= bucket->get_number_of_GB_per_cost_intervals(); ++l) {
                    data->b[j][l] = static_cast<int>((float) getValue(
                            cplx->b[static_cast<IloInt>(j)][static_cast<IloInt>(l)]));
                }
            } else {
                exit(1);  // Error
            }
        }

        // -------- q ----------
        for (auto j = 0ul; j < data->numb_; ++j) {
            auto storage = algorithm->GetStoragePerId(algorithm->GetVirtualMachineSize() + j);

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (auto l = 0ul; l <= bucket->get_number_of_GB_per_cost_intervals(); ++l) {
                    data->q[j][l] = (float) getValue(cplx->q[static_cast<IloInt>(j)][static_cast<IloInt>(l)]);
                }
            } else {
                exit(1);  // Error
            }
        }

        // -------- v ----------
        for (auto j = 0ul; j < data->m_; ++j) {
            for (auto t = 0ul; t < data->t_; ++t) {
                data->v[j][t] = static_cast<int>((float) getValue(
                        cplx->v[static_cast<IloInt>(j)][static_cast<IloInt>(t)]));
            }
        }

        // -------- z ----------
        for (auto j = 0ul; j < data->m_; ++j) {
            data->z[j] = (float) getValue(cplx->z[static_cast<IloInt>(j)]);
        }

        // -------- z_max ----------
        data->z_max = (float) getValue(cplx->z_max[0]);
    }
}

ILOSTLBEGIN

void Cplex::Run() {
    LOG(INFO) << "Executing CPLEX ...";

    DLOG(INFO) << "Variable names";
    char var_name[100];

    DLOG(INFO) << "Cplex structure (environment, model and variables)";
    int _n = static_cast<int>(GetActivationSize() - 2);  // less source and target
    int _d = static_cast<int>(GetFilesSize());
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

    DLOG(INFO) << "Execution variables";
    DLOG(INFO) << "X_IJT => task I, which is on machine J, starts executing in period T";
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        cplex.x[int_i] = IloArray<IloBoolVarArray>(cplex.env, _m);

        for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
            auto int_j = static_cast<int>(j);

            cplex.x[int_i][int_j] = IloBoolVarArray(cplex.env, _time_max);

            for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                auto int_t = static_cast<int>(t);

                sprintf(var_name, "x_%d_%d_%d", (int) i, (int) j, (int) t);  // Variable name
                cplex.x[int_i][int_j][int_t] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                cplex.model.add(cplex.x[int_i][int_j][int_t]);  // Add variable to model
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Read variables";
    DLOG(INFO) << "R_IDJPT => activation I that is on machine J, start reading its D-th input data (note that it is not"
                  " index data D) from machine P in period T";
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto activation = GetActivationPerId(1ul + i);
        auto input_files = activation->get_input_files();

        auto int_i = static_cast<int>(i);

        cplex.r[int_i] = IloArray<IloArray<IloArray<IloBoolVarArray>>>(cplex.env,
                                                                       static_cast<int>(input_files.size()));

        for (auto j = 0ul; j < input_files.size(); j++) {
            auto int_j = static_cast<int>(j);

            cplex.r[int_i][int_j] = IloArray<IloArray<IloBoolVarArray>>(cplex.env, _m);

            for (auto k = 0ul; k < static_cast<size_t>(_m); k++) {
                auto int_k = static_cast<int>(k);

                cplex.r[int_i][int_j][int_k] = IloArray<IloBoolVarArray>(cplex.env, _mb);

                for (auto l = 0ul; l < static_cast<size_t>(_mb); l++) {
                    auto int_l = static_cast<int>(l);

                    cplex.r[int_i][int_j][int_k][int_l] = IloBoolVarArray(cplex.env, _time_max);
                    for (auto m = 0ul; m < static_cast<size_t>(_time_max); m++) {
                        auto int_m = static_cast<int>(m);

                        sprintf(var_name, "r_%d_%d_%d_%d_%d", (int) i, (int) j, (int) k, (int) l,
                                (int) m);  // Variable name
                        cplex.r[int_i][int_j][int_k][int_l][int_m] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                        cplex.model.add(cplex.r[int_i][int_j][int_k][int_l][int_m]);  // Add variable to model
                    }
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Writing variables";
    DLOG(INFO) << "W_IDJPT => activation I which is on machine J, starts writing its D-th input data (note that it is "
                  "not index data D) from machine P in period T";
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto task = GetActivationPerId(1ul + i);
        auto output_files = task->get_output_files();

        auto int_i = static_cast<int>(i);

        cplex.w[int_i] = IloArray<IloArray<IloArray<IloBoolVarArray>>>(
                cplex.env,
                static_cast<int>(output_files.size()));

        for (auto d = 0ul; d < output_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            cplex.w[int_i][int_d] = IloArray<IloArray<IloBoolVarArray>>(cplex.env, _m);

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j= static_cast<int>(j);

                cplex.w[int_i][int_d][int_j] = IloArray<IloBoolVarArray>(cplex.env, _mb);

                for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                    auto int_p = static_cast<int>(p);

                    cplex.w[int_i][int_d][int_j][int_p] = IloBoolVarArray(cplex.env, _time_max);

                    for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                        auto int_t = static_cast<int>(t);

                        // Variable name
                        sprintf(var_name, "w_%d_%d_%d_%d_%d", (int) i, (int) d, (int) j, (int) p, (int) t);
                        cplex.w[int_i][int_d][int_j][int_p][int_t] = IloBoolVar(cplex.env,
                                                                                var_name);  // Allocate variable
                        cplex.model.add(cplex.w[int_i][int_d][int_j][int_p][int_t]);  // Add variable to model
                    }
                }
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Storage variables";
    DLOG(INFO) << "Y_DJT => indicates whether index data D is stored on device J in period T";
    for (auto d = 0ul; d < GetFilesSize(); d++) {
        auto int_d = static_cast<int>(d);

        cplex.y[int_d] = IloArray<IloBoolVarArray>(cplex.env, _mb);

        for (auto j = 0ul; j < static_cast<size_t>(_mb); j++) {
            auto int_j = static_cast<int>(j);

            cplex.y[int_d][int_j] = IloBoolVarArray(cplex.env, _time_max + 1);

            for (auto t = 0ul; t <= static_cast<size_t>(_time_max); t++) {
                auto int_t = static_cast<int>(t);

                sprintf(var_name, "y_%d_%d_%d", (int) d, (int) j, (int) t);  // Variable name
                cplex.y[int_d][int_j][int_t] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                cplex.model.add(cplex.y[int_d][int_j][int_t]);  // Add variable to model
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Machine-independent storage variables";
    DLOG(INFO) << "Y_DJ => indicates whether the data at index D exists at some time on machine J";
    for (auto d = 0ul; d < GetFilesSize(); d++) {
        auto int_d = static_cast<int>(d);

        cplex.yb[int_d] = IloBoolVarArray(cplex.env, _mb);

        for (auto j = 0ul; j < static_cast<size_t>(_mb); j++) {
            auto int_j = static_cast<int>(j);

            sprintf(var_name, "yb_%d_%d", (int) d, (int) j);  // Variable name
            cplex.yb[int_d][int_j] = IloBoolVar(cplex.env, var_name);  // Allocate variable
            cplex.model.add(cplex.yb[int_d][int_j]);  // Add variable to model
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "soft edge violation penalty variables";
    DLOG(INFO) << "W_D1D2 => soft edge violation di, d2 in E_s";
    for (auto d1 = 0ul; d1 < GetFilesSize() - 1ul; d1++) {
        auto int_d1 = static_cast<int>(d1);

        cplex.ws[int_d1] = IloBoolVarArray(cplex.env, _d);

        for (auto d2 = d1 + 1ul; d2 < GetFilesSize(); d2++) {
            auto int_d2 = static_cast<int>(d2);

            auto conflict = conflict_graph_->ReturnConflict(d1, d2);

            if (conflict > 0) {  // soft constraint
                sprintf(var_name, "ws_%d_%d", (int) d1, (int) d2);  // Variable name
                cplex.ws[int_d1][int_d2] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                cplex.model.add(cplex.ws[int_d1][int_d2]);  // Add variable to model
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Exposure variables";
    DLOG(INFO) << "E_RI => exposure level of task I by requirement R";
    for (auto r = 0ul; r < static_cast<size_t>(_num_r); r++) {
        auto int_r = static_cast<int>(r);

        cplex.e[int_r] = IloNumVarArray(cplex.env, _n);

        for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
            auto int_i = static_cast<int>(i);

            sprintf(var_name, "xe_%d_%d", (int) r, (int) i);  // Variable name
            // Allocate variable
            cplex.e[int_r][int_i] = IloNumVar(cplex.env, 0, IloInfinity, var_name);
            cplex.model.add(cplex.e[int_r][int_i]);  // Add variable to model
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Bucket usage variables";
    DLOG(INFO) << "B_JL => if bucket J is being used in range L " << std::endl
               << "*NOTE* ******* INTERVAL 0 is used to indicate that the BUCKET IS NOT BEING USED ******";
    for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
        auto int_b = static_cast<int>(b);

        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            cplex.b[int_b] = IloBoolVarArray(cplex.env, static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()) + 1);

            for (auto l = 0ul; l <= bucket->get_number_of_GB_per_cost_intervals(); l++) {
                auto int_l = static_cast<int>(l);

                sprintf(var_name, "b_%d_%d", (int) b, (int) l);  // Variable name
                cplex.b[int_b][int_l] = IloBoolVar(cplex.env, var_name);  // Allocate variable
                cplex.model.add(cplex.b[int_b][int_l]);  // Add variable to model
            }
        } else {
            exit(1);  // Error
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Variables for using buckets";
    DLOG(INFO) << "Q_JL => amount of data used by bucket J in range L " << std::endl
               << "*NOTE* ******* INTERVAL 0 is used to indicate that the BUCKET IS NOT BEING USED ******";
    for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
        auto int_b = static_cast<int>(b);

        auto storage = GetStoragePerId(GetVirtualMachineSize() + b);

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            cplex.q[int_b] = IloNumVarArray(cplex.env,
                                            static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()) + 1);

            for (auto l = 0ul; l <= bucket->get_number_of_GB_per_cost_intervals(); l++) {
                auto int_l = static_cast<int>(l);

                sprintf(var_name, "q_%d_%d", (int) b, (int) l);  // Variable name
                cplex.q[int_b][int_l] = IloNumVar(cplex.env, 0, IloInfinity, var_name);  // Allocate variable
                cplex.model.add(cplex.q[int_b][int_l]);  // Add variable to model
            }
        } else {
            exit(1);  // Error
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Machine usage variables by time";
    DLOG(INFO) << "V_JT => indicates whether machine J is in use (contracted) in period T";
    for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
        auto int_j = static_cast<int>(j);

        cplex.v[int_j] = IloBoolVarArray(cplex.env, _time_max);

        for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
            auto int_t = static_cast<int>(t);

            sprintf(var_name, "v_%d_%d", (int) j, (int) t);  // Variable name
            cplex.v[int_j][int_t] = IloBoolVar(cplex.env, var_name);  // Allocate variable
            cplex.model.add(cplex.v[int_j][int_t]);  // Add variable to model
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Variables of total time of use per machine";
    DLOG(INFO) << "Z_J => indicates total time that machine J was used (contracted)";
    for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
        auto int_j = static_cast<int>(j);

        sprintf(var_name, "z_%d", (int) j);  // Variable name
        cplex.z[int_j] = IloNumVar(cplex.env, 0, IloInfinity, var_name);  // Allocate variable
        cplex.model.add(cplex.z[int_j]);  // Add variable to model
    }

    DLOG(INFO) << var_name << " added to the model.";

    DLOG(INFO) << "Total time variables (makespan)";
    DLOG(INFO) << "Z_MAX => makespan do workflow";
    sprintf(var_name, "z_max");  // Variable name
    cplex.z_max[0] = IloNumVar(cplex.env, 0, IloInfinity, var_name);  // Allocate variable
    cplex.model.add(cplex.z_max[0]);  // Add variable to model

    DLOG(INFO) << var_name << " added to the model.";

    // ---------------- Objective function -------------------
    IloExpr fo(cplex.env);

    // ---- Makespan
    fo = alpha_time_ * (cplex.z_max[0] / _time_max);

    // ----- Financial cost
    for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
        auto int_j = static_cast<int>(j);

        std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(j);
        fo += alpha_budget_ * (virtual_machine->get_cost() * cplex.z[int_j]) / _cost_max;
    }

    for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
        auto int_b = static_cast<int>(b);

        std::shared_ptr<Storage> storage = GetStoragePerId(GetVirtualMachineSize() + b);

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            for (auto l = 0; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
                auto int_l = static_cast<int>(l);

                fo += alpha_budget_ * (bucket->get_cost() * cplex.q[int_b][int_l]) / _cost_max;
            }
        } else {
            exit(1);  // Error
        }
    }

    // Security exposure
    for (auto r = 0ul; r < static_cast<size_t>(_num_r); r++) {
        auto int_r = static_cast<int>(r);

        for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
            auto int_i = static_cast<int>(i);

            fo += alpha_security_ * cplex.e[int_r][int_i] / _security_max;
        }
    }

    // Soft edge penalties
    for (auto d1 = 0ul; d1 < static_cast<size_t>(_d) - 1ul; d1++) {
        auto int_d1 = static_cast<int>(d1);

        for (auto d2 = d1 + 1ul; d2 < static_cast<size_t>(_d); d2++) {
            auto int_d2 = static_cast<int>(d2);

            auto conflict = conflict_graph_->ReturnConflict(d1, d2);

            if (conflict > 0) {  // soft constraint
                fo += alpha_security_ * (conflict * cplex.ws[int_d1][int_d2] / _security_max);
            }
        }
    }

    cplex.model.add(IloMinimize(cplex.env, fo, "fo"));  // Add objective function to the model

    DLOG(INFO) << "Object Function added to the model.";

    // -----------------------------------------------------

    // --------------- RESTRICTIONS ----------

    // Restriction (4)
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        IloExpr exp(cplex.env);

        for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
            auto int_j = static_cast<int>(j);

            for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                auto int_t = static_cast<int>(t);

                exp += cplex.x[int_i][int_j][int_t];
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
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        auto task = GetActivationPerId(1ul + i);
        auto input_files = task->get_input_files();

        for (auto d = 0ul; d < input_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            IloExpr exp(cplex.env);

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j = static_cast<int>(j);

                for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                    auto int_p = static_cast<int>(p);

                    for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                        auto int_t = static_cast<int>(t);

                        exp += cplex.r[int_i][int_d][int_j][int_p][int_t];
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
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        auto activation = GetActivationPerId(1ul + i);
        auto output_files = activation->get_output_files();

        for (auto d = 0ul; d < output_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            IloExpr exp(cplex.env);

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j = static_cast<int>(j);

                for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                    auto int_p = static_cast<int>(p);

                    for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                        auto int_t = static_cast<int>(t);

                        exp += cplex.w[int_i][int_d][int_j][int_p][int_t];
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
    for (auto roof = 0ul, i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        auto activation = GetActivationPerId(1ul + i);
        auto output_files = activation->get_output_files();

        for (auto d = 0ul; d < output_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j = static_cast<int>(j);

                std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(j);

                for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                    auto int_p = static_cast<int>(p);

                    auto run_time = std::ceil(activation->get_time() * virtual_machine->get_slowdown());

                    for (auto t = static_cast<size_t>(run_time); t < static_cast<size_t>(_time_max); t++) {
                        auto int_t = static_cast<int>(t);

                        IloExpr exp(cplex.env);

                        exp += cplex.w[int_i][int_d][int_j][int_p][int_t];

                        /* (q <= roof) because the size of the interval is the same no matter if the time starts from 0
                         * or 1 */
                        roof = static_cast<size_t>(std::max(0, static_cast<int>(t) - static_cast<int>(run_time)));
                        // Why do we have this max?
                        // Why it is up to q <= roof and not q < roof
                        for (auto q = 0; q <= static_cast<int>(roof); q++) {
                            auto int_q = static_cast<int>(q);

                            exp -= cplex.x[int_i][int_j][int_q];
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
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        auto activation = GetActivationPerId(1ul + i);
        auto output_files = activation->get_output_files();

        for (auto d = 0ul; d < output_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j = static_cast<int>(j);

                std::shared_ptr<VirtualMachine> virtual_machine = GetVirtualMachinePerId(j);

                for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                    auto int_p = static_cast<int>(p);

                    auto run_time = std::ceil(activation->get_time() * virtual_machine->get_slowdown());
                    // for (int t = run_time; t < _time_max; t++)
                    auto limit = min(static_cast<size_t>(_time_max), static_cast<size_t>(run_time));

                    for (auto t = 0ul; t < limit; t++) {
                        auto int_t = static_cast<int>(t);

                        IloExpr exp(cplex.env);
                        exp += cplex.w[int_i][int_d][int_j][int_p][int_t];

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
    for (auto roof = 0ul, i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        auto activation = GetActivationPerId(1ul + i);
        auto input_files = activation->get_input_files();

        for (auto d = 0ul; d < input_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            std::shared_ptr<File> file = input_files[d];

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j = static_cast<int>(j);

                auto virtual_machine = GetVirtualMachinePerId(j);

                /* let's do it for all t so when (t-t_djp) < 0, the right side will be 0, and it prevents the execution
                 * of the activation at that time */
                for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                    auto int_t = static_cast<int>(t);

                    IloExpr exp(cplex.env);

                    exp = cplex.x[int_i][int_j][int_t];

                    for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                        auto int_p = static_cast<int>(p);

                        auto storage = GetStoragePerId(p);
                        /* (q <= roof) because the size of the interval is the same no matter if the time starts from 0
                         * or 1*/

                        auto file_transfer_time = ComputeFileTransferTime(file, virtual_machine, storage);

                        auto result = static_cast<int>(t + 1ul) - file_transfer_time;

//                        if (result >= 0) {
                            roof = static_cast<size_t>(max(0, result));
                            // roof = max(0, t - ComputeFileTransferTime(file, virtual_machine, storage));

                            for (auto q = 0ul; q < roof; q++) {
                                auto int_q = static_cast<int>(q);

                                exp -= cplex.r[int_i][int_d][int_j][int_p][int_q];
                            }
//                        }
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
    for (auto my_floor = 0ul, j = 0ul; j < static_cast<size_t>(_m); j++) {
        auto int_j = static_cast<int>(j);

        auto virtual_machine = GetVirtualMachinePerId(j);

        for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
            auto int_t = static_cast<int>(t);

            IloExpr exp(cplex.env);

            /* Execution */
            for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
                auto int_i = static_cast<int>(i);

                auto activation = GetActivationPerId(1ul + i);

                auto execution_time = static_cast<int>(
                        std::ceil(activation->get_time() * virtual_machine->get_slowdown()));

                my_floor = static_cast<size_t>(max(0, static_cast<int>(t) + 1 - execution_time));

                for (auto q = my_floor; q <= t; q++) {
                    auto int_q = static_cast<int>(q);

                    exp += cplex.x[int_i][int_j][int_q];
                }
            }

            /* Write */
            for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
                auto int_i = static_cast<int>(i);

                auto activation = GetActivationPerId(1ul + i);
                auto output_files = activation->get_output_files();

                for (auto d = 0ul; d < output_files.size(); d++) {
                    auto int_d = static_cast<int>(d);

                    std::shared_ptr<File> file = output_files[d];

                    for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                        auto int_p = static_cast<int>(p);

                        auto storage = GetStoragePerId(p);

                        auto file_transfer_time = ComputeFileTransferTime(file, virtual_machine, storage);

                        my_floor = static_cast<size_t>(max(0, static_cast<int>(t) + 1 - file_transfer_time));

                        for (auto rr = my_floor; rr <= t; rr++) {
                            auto int_rr = static_cast<int>(rr);

                            exp += cplex.w[int_i][int_d][int_j][int_p][int_rr];
                        }
                    }
                }
            }

            /* Read */
            for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
                auto int_i = static_cast<int>(i);

                auto activation = GetActivationPerId(1ul + i);
                auto input_files = activation->get_input_files();

                for (auto d = 0ul; d < input_files.size(); d++) {
                    auto int_d = static_cast<int>(d);

                    auto file = input_files[d];

                    for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                        auto int_p = static_cast<int>(p);

                        auto storage = GetStoragePerId(p);

                        auto file_transfer_time = ComputeFileTransferTime(file, virtual_machine, storage);

                        my_floor = static_cast<size_t>(max(0, static_cast<int>(t) + 1 - file_transfer_time));

                        for (auto rr = my_floor; rr <= t; rr++) {
                            auto int_rr = static_cast<int>(rr);

                            exp += cplex.r[int_i][int_d][int_j][int_p][int_rr];
                        }
                    }
                }
            }

            /* Hiring */
            exp -= cplex.v[int_j][int_t];

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
    for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
        auto int_j = static_cast<int>(j);

        auto storage = GetStoragePerId(j);

        for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
            auto int_t = static_cast<int>(t);

            IloExpr exp(cplex.env);

            /* Write */
            for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
                auto int_i = static_cast<int>(i);

                auto task = GetActivationPerId(1ul + i);
                auto output_files = task->get_output_files();

                for (auto d = 0ul; d < output_files.size(); d++) {
                    auto int_d = static_cast<int>(d);

                    auto file = output_files[d];

                    for (auto p = 0ul; p < static_cast<size_t>(_m); p++) {
                        auto int_p = static_cast<int>(p);

                        if (j != p) {
                            auto virtual_machine = GetVirtualMachinePerId(p);

                            auto file_transfer_time = ComputeFileTransferTime(file, storage, virtual_machine);

                            auto root = static_cast<size_t>(max(0, static_cast<int>(t) + 1 - file_transfer_time));

                            for (auto rr = root; rr <= t; rr++) {
                                auto int_rr = static_cast<int>(rr);

                                exp += cplex.w[int_i][int_d][int_p][int_j][int_rr];
                            }
                        }
                    }
                }
            }

            /* Read */
            for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
                auto int_i = static_cast<int>(i);

                auto activation = GetActivationPerId(1ul + i);
                auto input_files = activation->get_input_files();

                for (auto d = 0ul; d < input_files.size(); d++) {
                    auto int_d = static_cast<int>(d);

                    auto file = input_files[d];

                    for (auto p = 0ul; p < static_cast<size_t>(_m); p++) {
                        auto int_p = static_cast<int>(p);

                        if (j != p) {
                            auto virtual_machine = GetVirtualMachinePerId(p);

                            auto file_transfer_time = ComputeFileTransferTime(file, storage, virtual_machine);

                            auto root = static_cast<size_t>(max(0, static_cast<int>(t) + 1 - file_transfer_time));

                            for (auto rr = root; rr <= t; rr++) {
                                auto int_rr = static_cast<int>(rr);

                                exp += cplex.r[int_i][int_d][int_p][int_j][int_rr];
                            }
                        }
                    }
                }
            }

            /* Hiring */
            exp -= cplex.v[int_j][int_t] * (_m - 1);

            IloConstraint c(exp <= 0);

            sprintf(var_name, "c11_%d_%d", (int) j, (int) t);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (12)
    for (auto b = GetVirtualMachineSize(); b < GetVirtualMachineSize() + static_cast<size_t>(_num_b); b++) {
        auto int_b = static_cast<int>(b);  // device

        auto storage = GetStoragePerId(b);

        for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
            IloExpr exp(cplex.env);

            /* Write */
            for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
                auto int_i = static_cast<int>(i);  // activation

                auto activation = GetActivationPerId(1ul + i);
                auto output_files = activation->get_output_files();

                for (auto d = 0ul; d < output_files.size(); d++) {
                    auto int_d = static_cast<int>(d);  // file

                    auto file = output_files[d];

                    for (auto p = 0ul; p < static_cast<size_t>(_m); p++) {
                        auto int_p = static_cast<int>(p);  // virtual machine

                        auto virtual_machine = GetVirtualMachinePerId(p);

                        auto file_transfer_time = ComputeFileTransferTime(file, storage, virtual_machine);

                        auto root = static_cast<size_t>(max(0, static_cast<int>(t) + 1 - file_transfer_time));

                        for (auto rr = root; rr <= t; rr++) {
                            auto int_rr = static_cast<int>(rr);  // time

                            exp += cplex.w[int_i][int_d][int_p][int_b][int_rr];
                        }
                    }
                }
            }

            /* Read */
            for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
                auto int_i = static_cast<int>(i);

                auto task = GetActivationPerId(1ul + i);
                auto input_files = task->get_input_files();

                for (auto d = 0ul; d < input_files.size(); d++) {
                    auto int_d = static_cast<int>(d);

                    auto file = input_files[d];

                    for (auto p = 0ul; p < static_cast<size_t>(_m); p++) {
                        auto int_p = static_cast<int>(p);

                        auto virtual_machine = GetVirtualMachinePerId(p);

                        auto file_transfer_time = ComputeFileTransferTime(file, storage, virtual_machine);

                        auto root = static_cast<size_t>(max(0, static_cast<int>(t) + 1 - file_transfer_time));

                        for (auto rr = root; rr <= t; rr++) {
                            auto int_rr = static_cast<int>(rr);

                            exp += cplex.r[int_i][int_d][int_p][int_b][int_rr];
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
    for (auto d = 0ul; d < files_.size(); ++d) {
        auto int_d = static_cast<int>(d);

        auto file = files_[d];

        if (dynamic_pointer_cast<DynamicFile>(file)) {
            for (auto j = 0ul; j < static_cast<size_t>(_mb); j++) {
                auto int_j = static_cast<int>(j);

                IloExpr exp(cplex.env);

                exp += cplex.y[int_d][int_j][0];

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
    for (auto d = 0ul; d < files_.size(); ++d) {
        auto int_d = static_cast<int>(d);

        auto file = files_[d];

        if (auto static_file = dynamic_pointer_cast<StaticFile>(file)) {
            auto ind = static_file->GetFirstVm();
            auto int_ind = static_cast<int>(ind);

            for (auto mb = 0ul; mb < storages_.size(); ++mb) {
                auto int_mb = static_cast<int>(mb);

                if (ind == mb) {
                    for (auto t = 0ul; t <= static_cast<size_t>(_time_max); t++) {
                        auto int_t = static_cast<int>(t);

                        IloExpr exp(cplex.env);

                        exp += cplex.y[int_d][int_ind][int_t];

                        IloConstraint c(exp == 1);

                        sprintf(var_name, "c14_%d_%d_%d", (int) d, (int) ind, (int) t);
                        c.setName(var_name);
                        cplex.model.add(c);

                        exp.end();
                    }
                } else {
                    IloExpr exp(cplex.env);

                    exp += cplex.y[int_d][int_mb][0];

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
    for (auto o = 0ul; o < GetFilesSize(); ++o) {
        DLOG(INFO) << "o: " << o;

        // Dynamic files
        if (auto dynamic_file = std::dynamic_pointer_cast<DynamicFile>(files_[o])) {
            DLOG(INFO) << "Dynamic file: " << dynamic_file->get_id();
            auto activation = dynamic_file->get_parent_task().lock();

            if (activation) {
                auto i = activation->get_id() - 1ul;
                auto d = dynamic_file->get_id();

                auto int_i = static_cast<int>(i);  // activation
                auto int_d = static_cast<int>(d);  // file to be written

                for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                    DLOG(INFO) << "p: " << p;
                    auto int_p = static_cast<int>(p);  // device to store the file

                    auto storage = GetStoragePerId(p);

                    for (auto t = 0ul; t < static_cast<size_t>(_time_max - 1); t++) {
                        auto int_t = static_cast<int>(t);

                        IloExpr exp(cplex.env);

                        exp += cplex.y[int_d][int_p][int_t + 1];
                        exp -= cplex.y[int_d][int_p][int_t];

                        for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                            auto int_j = static_cast<int>(j);  // virtual machine
                            auto virtual_machine = GetVirtualMachinePerId(j);

                            auto my_time = static_cast<int>(t + 1ul)
                                    - ComputeFileTransferTime(dynamic_file, storage, virtual_machine);

                            if (my_time >= 0) {
                                auto int_time = static_cast<int>(my_time);

                                auto indw = dynamic_file->get_parent_output_file_index();
                                auto int_indw = static_cast<int>(indw);

                                exp -= cplex.w[int_i][int_indw][int_j][int_p][int_time];
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
            }
        // Static files
        } else {
            DLOG(INFO) << "Static file: " << dynamic_file;
            for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                DLOG(INFO) << "p: " << p;
                auto int_p = static_cast<int>(p);

                for (auto t = 0ul; t < static_cast<size_t>(_time_max - 1); t++) {
                    DLOG(INFO) << "t: " << t;
                    auto int_t = static_cast<int>(t);

                    IloExpr exp(cplex.env);

                    auto file = GetFilePerId(o);
                    auto d = file->get_id();

                    auto int_d = static_cast<int>(d);

                    exp += cplex.y[int_d][int_p][int_t + 1];
                    exp -= cplex.y[int_d][int_p][int_t];

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
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        auto activation = GetActivationPerId(1ul + i);
        auto input_files = activation->get_input_files();

        for (auto d = 0ul; d < input_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                auto int_p = static_cast<int>(p);

                for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                    auto int_t = static_cast<int>(t);

                    IloExpr exp(cplex.env);

                    for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                        auto int_j = static_cast<int>(j);

                        exp += cplex.r[int_i][int_d][int_j][int_p][int_t];
                    }

                    auto file = input_files[d];
                    auto int_dd = static_cast<int>(file->get_id());

                    exp -= cplex.y[int_dd][int_p][int_t];

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
    for (auto j = 0ul; j < static_cast<size_t>(_mb); j++) {
        auto int_j = static_cast<int>(j);

        auto storage = GetStoragePerId(static_cast<size_t>(j));

        for (auto t = 0ul; t <= static_cast<size_t>(_time_max); t++) {
            auto int_t = static_cast<int>(t);

            IloExpr exp(cplex.env);

            for (auto d = 0ul; d < static_cast<size_t>(_d); d++) {
                auto int_d = static_cast<int>(d);

                auto file = GetFilePerId(d);

                exp += (file->get_size_in_GB() * cplex.y[int_d][int_j][int_t]);
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
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        auto task = GetActivationPerId(1ul + i);
        auto output_files = task->get_output_files();

        for (auto d = 0ul; d < output_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            auto file = output_files[d];

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j = static_cast<int>(j);

                auto virtual_machine = GetVirtualMachinePerId(j);

                for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                    auto int_p = static_cast<int>(p);

                    auto storage = GetStoragePerId(p);

                    for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                        auto int_t = static_cast<int>(t);

                        IloExpr exp(cplex.env);

                        exp += (int_t + ComputeFileTransferTime(file, storage, virtual_machine))
                                * cplex.w[int_i][int_d][int_j][int_p][int_t];
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
    for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
        auto int_j = static_cast<int>(j);

        for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
            auto int_t = static_cast<int>(t);

            IloExpr exp(cplex.env);

            // (t + 1) because you pay times from 0 to t, so (t + 1) times
            exp += (int_t + 1) * cplex.v[int_j][int_t];
            exp -= cplex.z[int_j];

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

        for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
            auto int_j = static_cast<int>(j);

            auto virtual_machine = GetVirtualMachinePerId(j);

            exp += virtual_machine->get_cost() * cplex.z[int_j];
        }

        for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
            auto int_b = static_cast<int>(b);

            auto storage = GetStoragePerId(GetVirtualMachineSize() + b);

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (auto l = 1ul; l <= bucket->get_number_of_GB_per_cost_intervals(); l++) {
                    auto int_l = static_cast<int>(l);

                    // Remembering that the cost in these instances is constant
                    exp += bucket->get_cost() * cplex.q[int_b][int_l];
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
    for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
        auto int_i = static_cast<int>(i);

        auto activation = GetActivationPerId(1ul + i);
        auto input_files = activation->get_input_files();

        for (auto d = 0ul; d < input_files.size(); d++) {
            auto int_d = static_cast<int>(d);

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j = static_cast<int>(j);

                for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                    auto int_t = static_cast<int>(t);

                    IloExpr exp(cplex.env);

                    for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                        auto int_p = static_cast<int>(p);

                        exp += static_cast<int>(input_files.size())
                                * cplex.r[int_i][int_d][int_j][int_p][int_t];
                    }

                    for (auto g = 0ul; g < input_files.size(); g++) {
                        auto file = input_files[d];

                        auto int_dd = static_cast<int>(file->get_id());

                        for (auto p = 0ul; p < static_cast<size_t>(_mb); p++) {
                            auto int_p = static_cast<int>(p);

                            exp -= cplex.y[int_dd][int_p][int_t];
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
    for (auto d = 0ul; d < static_cast<size_t>(_d); d++) {
        auto int_d = static_cast<int>(d);

        for (auto j = 0ul; j < static_cast<size_t>(_mb); j++) {
            auto int_j = static_cast<int>(j);

            for (auto t = 0ul; t <= static_cast<size_t>(_time_max); t++) {
                auto int_t = static_cast<int>(t);

                IloExpr exp(cplex.env);

                exp += cplex.y[int_d][int_j][int_t];
                exp -= cplex.yb[int_d][int_j];

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
    for (auto d1 = 0ul; d1 < static_cast<size_t>(_d) - 1ul; d1++) {
        auto int_d1 = static_cast<int>(d1);

        for (auto d2 = d1 + 1ul; d2 < static_cast<size_t>(_d); d2++) {
            auto int_d2 = static_cast<int>(d2);

            auto conflict = conflict_graph_->ReturnConflict(d1, d2);

            // hard constraint
            if (conflict == -1) {
                for (auto b = 0ul; b < static_cast<size_t>(_mb); b++) {
                    auto int_b = static_cast<int>(b);

                    IloExpr exp(cplex.env);

                    exp += cplex.yb[int_d1][int_b];
                    exp += cplex.yb[int_d2][int_b];

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
    for (auto d1 = 0ul; d1 < static_cast<size_t>(_d) - 1ul; d1++) {
        auto int_d1 = static_cast<int>(d1);

        for (auto d2 = d1 + 1ul; d2 < static_cast<size_t>(_d); d2++) {
            auto int_d2 = static_cast<int>(d2);

            auto conflict = conflict_graph_->ReturnConflict(d1, d2);

            // Hard constraint
            if (conflict > 0) {
                for (auto b = 0ul; b < static_cast<size_t>(_mb); b++) {
                    auto int_b = static_cast<int>(b);

                    IloExpr exp(cplex.env);

                    exp += cplex.yb[int_d1][int_b];
                    exp += cplex.yb[int_d2][int_b];
                    exp -= cplex.ws[int_d1][int_d2];

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
    for (auto r = 0ul; r < static_cast<size_t>(_num_r); r++) {
        auto int_r = static_cast<int>(r);
        // Requirement requirement = GetRequirementPerId(static_cast<size_t>(r));
        for (auto i = 0ul; i < static_cast<size_t>(_n); i++) {
            auto int_i = static_cast<int>(i);

            auto activation = GetActivationPerId(1ul + i);

            IloExpr exp(cplex.env);
            exp -= cplex.e[int_r][int_i];

            for (auto j = 0ul; j < static_cast<size_t>(_m); j++) {
                auto int_j = static_cast<int>(j);

                auto virtual_machine = GetVirtualMachinePerId(j);

                for (auto t = 0ul; t < static_cast<size_t>(_time_max); t++) {
                    auto int_t = static_cast<int>(t);

                    // exp += /* <RODRIGO> l^{lj}_{vm} */ * cplex.x[i][j][t];
                    exp -= virtual_machine->GetRequirementValue(r) * cplex.x[int_i][int_j][int_t];
                }
            }

            // IloConstraint c(exp <= - /* <RODRIGO> l^{ri}_{activation} */);
            IloConstraint c(exp <= -activation->GetRequirementValue(r));

            sprintf(var_name, "c25_%d_%d", (int) i, (int) r);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (26)
    for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
        auto int_b = static_cast<int>(b);

        // bucket
        auto storage = GetStoragePerId(GetVirtualMachineSize() + b);

        // Bucket index among the machines
        int int_j = static_cast<int>(GetVirtualMachineSize() + b);

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            IloExpr exp(cplex.env);

            for (auto l = 0ul; l <= bucket->get_number_of_GB_per_cost_intervals(); l++) {
                auto int_l = static_cast<int>(l);

                exp += cplex.q[int_b][int_l];
            }

            for (auto d = 0ul; d < static_cast<size_t>(_d); d++) {
                auto int_d = static_cast<int>(d);

                auto file = files_[d];

                exp -= (file->get_size_in_GB() * cplex.yb[int_d][int_j]);
                // exp -= (file->get_size() * cplex.yb[d][int_j]);
            }

            IloConstraint c(exp == 0);

            sprintf(var_name, "c26_%d", (int) int_j);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        } else {
            exit(1);  // error
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (27)
    for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
        auto int_b = static_cast<int>(b);

        // bucket
        auto storage = GetStoragePerId(GetVirtualMachineSize() + b);

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            IloExpr exp(cplex.env);

            for (auto l = 0ul; l <= bucket->get_number_of_GB_per_cost_intervals(); l++) {
                auto int_l = static_cast<int>(l);

                exp += cplex.b[int_b][int_l];
            }

            IloConstraint c(exp == 1);
            sprintf(var_name, "c27_%d", (int) b);
            c.setName(var_name);
            cplex.model.add(c);

            exp.end();
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    // Restriction (28)
    for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
        // bucket
        auto storage = GetStoragePerId(GetVirtualMachineSize() + b);

        // Bucket index among the machines
        auto int_j = static_cast<int>(GetVirtualMachineSize() + b);

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            IloExpr exp(cplex.env);

            for (auto d = 0ul; d < static_cast<size_t>(_d); d++) {
                auto int_d = static_cast<int>(d);

                auto file = files_[d];

                exp += file->get_size_in_GB() * cplex.yb[int_d][int_j];
                // exp += (file->get_size() * cplex.yb[d][int_j]);
            }

            for (auto l = 1ul; l <= bucket->get_number_of_GB_per_cost_intervals(); l++) {
                auto int_b = static_cast<int>(b);
                auto int_l = static_cast<int>(l);

                exp -= /* <RODRIGO> sm_{bl}*/ 51200 * cplex.b[int_b][int_l];  // sm_{bl} == 0
            }

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
    for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
        auto int_b = static_cast<int>(b);

        // bucket
        auto storage = GetStoragePerId(GetVirtualMachineSize() + b);

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            for (auto l = 1ul; l <= bucket->get_number_of_GB_per_cost_intervals(); l++) {
                auto int_l = static_cast<int>(l);

                IloExpr exp(cplex.env);

                // sm_j_l ==> Storage size of bucket $j \in B$ on interval $l \in L_j$,  where $sm_{j0}=0$.
                // exp += /*  <RODRIGO> sm_{b(l-1)} * */   cplex.b[b][l];

                exp += bucket->get_storage_of_the_interval(l - 1ul) * cplex.b[int_b][int_l];
                exp -= cplex.q[int_b][int_l];

                IloConstraint c(exp <= 0);
                sprintf(var_name, "c29a_%d_%d", (int) b, (int) l);
                c.setName(var_name);
                cplex.model.add(c);

                exp.end();

                IloExpr exp2(cplex.env);
                // exp2 -= /* <RODRIGO> sm_{bl} * */   cplex.b[b][l];
                // exp2 -= 51200 * cplex.b[b][l];
                exp2 -= bucket->get_storage_of_the_interval(l) * cplex.b[int_b][int_l];
                exp2 += cplex.q[int_b][int_l];

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
    for (auto b = 0ul; b < static_cast<size_t>(_num_b); b++) {
        auto int_b = static_cast<int>(b);

        // Bucket
        auto storage = GetStoragePerId(GetVirtualMachineSize() + b);

        // Bucket index among the machines
        auto int_j = static_cast<int>(GetVirtualMachineSize() + b);

        if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
            for (auto l = 1ul; l <= bucket->get_number_of_GB_per_cost_intervals(); l++) {
                auto int_l = static_cast<int>(l);

                IloExpr exp(cplex.env);

                exp += cplex.b[int_b][int_l];

                for (auto d = 0ul; d < static_cast<size_t>(_d); d++) {
                    auto int_d = static_cast<int>(d);

                    exp -= cplex.yb[int_d][int_j];
                }

                IloConstraint c(exp <= 0);
                sprintf(var_name, "c30_%d_%d", (int) b, (int) l);
                c.setName(var_name);
                cplex.model.add(c);

                exp.end();
            }
        }
    }

    DLOG(INFO) << var_name << " added to the model.";

    IloCplex solver(cplex.model);  // Declare variable "solver" over the model to be optimal
    // solver.exportModel("model.lp");  // Write model to file in .lp format

    DLOG(INFO) << "Point to " << FLAGS_cplex_output_file.c_str() << " file to receive the output data.";

    solver.exportModel(FLAGS_cplex_output_file.c_str());  // Write model to file in .lp format

    DLOG(INFO) << FLAGS_cplex_output_file.c_str() << " CPLEX model file exported.";

    // Open CPLEX model
//    int error;
//    CPXENVptr env = CPXopenCPLEX(&error);

    DLOG(INFO) << "CPLEX model opened.";

    // Parameters
//    solver.setParam(IloCplex::Param::TimeLimit, 3600);  // limit time (3600=1h, 86400=1d)
    solver.setParam(IloCplex::Param::TimeLimit, 3600);  // limit time (3600=1h, 86400=1d)
    solver.setParam(IloCplex::Param::MIP::Interval, 100);  // Log for each N node
    solver.setParam(IloCplex::Param::Parallel, CPX_PARALLEL_AUTO);

    // Configurar o CPLEX para suprimir a saída no console
//    solver.setOut(cplex.env.getNullStream());
//    solver.setWarning(cplex.env.getNullStream());

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
    bool optimal;
    double res = -1.0, lb = -1.0, time_s;
    double cost = -1.0;
    double security = -1.0;

    try {
        optimal = solver.getStatus() == IloAlgorithm::Optimal;

        res = solver.getObjValue();  // Solution
        lb = solver.getBestObjValue();  // Dual-limit (inferior)
        time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time

        cost = 0.0;
        security = 0.0;

        // ----- Financial cost
        for (int j = 0; j < _m; j++) {
            auto virtual_machine = GetVirtualMachinePerId(static_cast<size_t>(j));

            cost += virtual_machine->get_cost() * best.z[j];
        }

        for (auto b = 0; b < _num_b; b++) {
            auto storage = GetStoragePerId(GetVirtualMachineSize() + static_cast<size_t>(b));

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (auto l = 1; l <= static_cast<int>(bucket->get_number_of_GB_per_cost_intervals()); l++) {
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
        for (auto d1 = 0; d1 < _d - 1; d1++) {
            for (auto d2 = d1 + 1; d2 < _d; d2++) {
                int conflict = conflict_graph_->ReturnConflict(static_cast<size_t>(d1), static_cast<size_t>(d2));

                if (conflict > 0) {  // Soft constraint
                    security += conflict * best.ws[d1][d2] / _security_max;
                }
            }
        }

//        cout << endl;
//        cout << "----- Done ----" << endl;
//        cout << "OPTIMAL?  : " << optimal << endl;
//        cout << "MAKESPAN : " << best.z_max << endl;
//        cout << "COST     : " << cost << endl;
//        cout << "SECURITY : " << security << endl;
//        cout << "TIME     : " << time_s << endl;
//        cout << "LB       : " << lb << endl;
//        cout << "UB (X*)  : " << res << endl;
//        cout << endl;

        LOG(INFO) << endl;
        LOG(INFO) << "----- Done ----" << endl;
        LOG(INFO) << "OPTIMAL?  : " << optimal << endl;
        LOG(INFO) << "MAKESPAN : " << best.z_max << endl;
        LOG(INFO) << "COST     : " << cost << endl;
        LOG(INFO) << "SECURITY : " << security << endl;
        LOG(INFO) << "TIME     : " << time_s << endl;
        LOG(INFO) << "LB       : " << lb << endl;
        LOG(INFO) << "UB (X*)  : " << res << endl;
        LOG(INFO) << endl;

//        cerr << best.z_max << " " << cost << " " << security << " " << res << endl << time_s << endl;

//        std::cout << best.z_max << " " << cost << " " << security << " " << res << " " << time_s << endl;

        LOG(INFO) << "Results:";
        LOG(INFO) << best.z_max << " " << cost << " " << security << " " << res << " " << endl << time_s;

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
        LOG(INFO) << e;
    }

    cplex.env.end();

//    PrintBest(&best);
//    print_solution_timeline(&best);

    time_s = ((double) clock() - (double) t_start) / CLOCKS_PER_SEC;  // Processing time

//    LOG(INFO) << "Elapsed time: " << time_s;

    LOG(INFO) << "Printing BEST:";
    LOG(INFO) << best;

    LOG(INFO) << "... ending CPLEX";

    std::cout << std::endl
              << std::fixed << std::setprecision(6)
              << res
              << " " << best.z_max
              << " " << cost
              << " " << security
              << " " << time_s
              << " " << 1
              << " " << 1
              << " " << time_s
              << std::endl;
}