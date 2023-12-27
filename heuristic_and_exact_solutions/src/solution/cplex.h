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

#ifndef APPROXIMATE_SOLUTIONS_SRC_CPLEX_H_
#define APPROXIMATE_SOLUTIONS_SRC_CPLEX_H_

#include <ilcplex/ilocplex.h>
#include "src/solution/algorithm.h"

template<typename T= VirtualMachine>
T dynamic_pointer_cast(Storage);

//------------ Cplex environment structure (environment, model and variables) -------------------
struct CPLEX {
    IloEnv env;
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

class Cplex : public Algorithm {
public:
    ///
    Cplex() = default;

    static int ComputeFileTransferTime(const std::shared_ptr<File>& file,
                                       const std::shared_ptr<Storage> &storage1,
                                       const std::shared_ptr<Storage> &storage2);

    static void print_solution_timeline(struct BEST *data);

    ///
    [[nodiscard]] std::string GetName() const override { return name_; }

    ///
    void Run() override;

private:
    std::string name_ = "cplex";
};

struct BEST {
    size_t n_{};
    size_t d_{};
    size_t m_{};
    size_t numr_{};
    size_t numb_{};
    size_t mb_{};
    size_t t_{};
//  int        max_num_intervals_;

    Cplex *algorithm_{};

    clock_t start{};
    double PRECISION = 1.0e-6;
    double b_sol = std::numeric_limits<double>::max();

    // ---------- Best Solution ------------------
    int ***x{};  // Keeps the better solution x
    int *****r{};  // Keeps the better solution r
    int *****w{};  // Keeps the better solution r
    int ***y{};  // Keeps the better solution y
    int **yb{};  // Keeps the better solution yb
    int **ws{};  // Keeps the better solution w
    double **e{};  // Keeps the better solution e
    int **b{};  // Keeps the better solution b
    double **q{};  // Keeps the better solution q
    int **v{};  // Keeps the better solution v
    double *z{};  // Keeps the better solution z
    double z_max{};  // Keeps the better solution z_max

    BEST() = default;

    BEST(int n, int d, int m, int numr, int numb, int mb, int t, int max_num_intervals, Cplex *algorithm)
            : n_(static_cast<size_t>(n)), d_(static_cast<size_t>(d)), m_(static_cast<size_t>(m)),
            numr_(static_cast<size_t>(numr)), numb_(static_cast<size_t>(numb)), mb_(static_cast<size_t>(mb)),
            t_(static_cast<size_t>(t)), algorithm_(algorithm) {
        start = clock();

        x = new int **[static_cast<unsigned long>(n)];
        for (int i = 0; i < n; ++i) {
            x[i] = new int *[static_cast<unsigned long>(m)];
            for (int j = 0; j < m; ++j) {
                x[i][j] = new int[static_cast<unsigned long>(t)];
            }
        }

        r = new int ****[static_cast<unsigned long>(n)];
        for (int i = 0; i < n; ++i) {
            r[i] = new int ***[static_cast<unsigned long>(d)];
            for (int j = 0; j < d; ++j) {
                r[i][j] = new int **[static_cast<unsigned long>(m)];
                for (int k = 0; k < m; ++k) {
                    r[i][j][k] = new int *[static_cast<unsigned long>(mb)];
                    for (int l = 0; l < mb; ++l) {
                        r[i][j][k][l] = new int[static_cast<unsigned long>(t)];
                    }
                }
            }
        }

        w = new int ****[static_cast<unsigned long>(n)];
        for (int i = 0; i < n; ++i) {
            w[i] = new int ***[static_cast<unsigned long>(d)];
            for (int j = 0; j < d; ++j) {
                w[i][j] = new int **[static_cast<unsigned long>(m)];
                for (int k = 0; k < m; ++k) {
                    w[i][j][k] = new int *[static_cast<unsigned long>(mb)];
                    for (int l = 0; l < mb; ++l) {
                        w[i][j][k][l] = new int[static_cast<unsigned long>(t)];
                    }
                }
            }
        }

        y = new int **[static_cast<unsigned long>(d)];
        for (int i = 0; i < d; ++i) {
            y[i] = new int *[static_cast<unsigned long>(mb)];
            for (int j = 0; j < mb; ++j) {
                y[i][j] = new int[static_cast<unsigned long>(t + 1)];
            }
        }

        yb = new int *[static_cast<unsigned long>(d)];
        for (int i = 0; i < d; ++i) {
            yb[i] = new int[static_cast<unsigned long>(mb)];
        }

        ws = new int *[static_cast<unsigned long>(d)];
        for (int i = 0; i < d; ++i) {
            ws[i] = new int[static_cast<unsigned long>(d)];
        }

        e = new double *[static_cast<unsigned long>(numr)];
        for (int i = 0; i < numr; ++i) {
            e[i] = new double[static_cast<unsigned long>(n)];
        }

        b = new int *[static_cast<unsigned long>(numb)];
        for (int i = 0; i < numb; ++i) {
            b[i] = new int[static_cast<unsigned long>(max_num_intervals)];
        }

        q = new double *[static_cast<unsigned long>(numb)];
        for (int i = 0; i < numb; ++i) {
            q[i] = new double[static_cast<unsigned long>(max_num_intervals)];
        }

        v = new int *[static_cast<unsigned long>(m)];
        for (int i = 0; i < m; ++i) {
            v[i] = new int[static_cast<unsigned long>(t)];
        }

        z = new double[static_cast<unsigned long>(m)];
    }

    ~BEST() {
        for (auto i = 0ul; i < n_; ++i) {
            for (auto j = 0ul; j < m_; ++j) {
                delete[] x[i][j];
            }
            delete[] x[i];
        }
        delete[] x;

        for (auto i = 0ul; i < n_; ++i) {
            for (auto j = 0ul; j < d_; ++j) {
                for (auto k = 0ul; k < m_; ++k) {
                    for (auto l = 0ul; l < mb_; ++l) {
                        delete[] r[i][j][k][l];
                    }
                    delete[] r[i][j][k];
                }
                delete[] r[i][j];
            }
            delete[] r[i];
        }
        delete[] r;

        for (auto i = 0ul; i < n_; ++i) {
            for (auto j = 0ul; j < d_; ++j) {
                for (auto k = 0ul; k < m_; ++k) {
                    for (auto l = 0ul; l < mb_; ++l) {
                        delete[] w[i][j][k][l];
                    }
                    delete[] w[i][j][k];
                }
                delete[] w[i][j];
            }
            delete[] w[i];
        }
        delete[] w;

        for (auto i = 0ul; i < d_; ++i) {
            for (auto j = 0ul; j < mb_; ++j) {
                delete[] y[i][j];
            }
            delete[] y[i];
        }
        delete[] y;

        for (auto i = 0ul; i < d_; ++i) {
            delete[] yb[i];
        }
        delete[] yb;

        for (auto i = 0ul; i < d_; ++i) {
            delete[] ws[i];
        }
        delete[] ws;

        for (auto i = 0ul; i < numr_; ++i) {
            delete[] e[i];
        }
        delete[] e;

        for (auto i = 0ul; i < numb_; ++i) {
            delete[] b[i];
        }
        delete[] b;

        for (auto i = 0ul; i < numb_; ++i) {
            delete[] q[i];
        }
        delete[] q;

        for (auto i = 0ul; i < m_; ++i) {
            delete[] v[i];
        }
        delete[] v;

        delete[] z;
    }

    /// Concatenation operator
    friend std::ostream &operator<<(std::ostream &os, const struct BEST &obj) {
        return obj.Write(os);
    }

    /// Write this object to the output stream
    std::ostream &Write(std::ostream &os) const {
        // -------- x ----------
        for (auto i = 0ul; i < n_; ++i) {
            for (auto j = 0ul; j < m_; ++j) {
                for (auto t = 0ul; t < t_; ++t) {
                    auto int_i = static_cast<int>(i);
                    auto int_j = static_cast<int>(j);
                    auto int_t = static_cast<int>(t);

                    if (x[i][j][t] > PRECISION) {
                        os << "x[" << i << "][" << j << "][" << t << "] = " << x[int_i][int_j][int_t];
                    }
                }
            }
        }

        // -------- r ----------
        for (auto i = 0ul; i < n_; ++i) {
            auto activation = algorithm_->GetActivationPerId(i + 1ul);
            auto input_files = activation->get_input_files();

            for (auto j = 0ul; j < input_files.size(); ++j) {
                for (auto k = 0ul; k < m_; ++k) {
                    for (auto l = 0ul; l < mb_; ++l) {
                        for (auto m = 0ul; m < t_; ++m) {
                            auto int_i = static_cast<int>(i);
                            auto int_j = static_cast<int>(j);
                            auto int_k = static_cast<int>(k);
                            auto int_l = static_cast<int>(l);
                            auto int_m = static_cast<int>(m);

                            if (r[int_i][int_j][int_k][int_l][int_m] > PRECISION) {
                                os << "r[" << i << "][" << j << "][" << k << "][" << l << "][" << m << "] = "
                                        << r[int_i][int_j][int_k][int_l][int_m];
                            }
                        }
                    }
                }
            }
        }

        // -------- w ----------
        for (auto i = 0ul; i < n_; ++i) {
            auto activation = algorithm_->GetActivationPerId(i + 1ul);
            auto output_files = activation->get_output_files();

            for (auto j = 0ul; j < output_files.size(); ++j) {
                for (auto k = 0ul; k < m_; ++k) {
                    for (auto l = 0ul; l < mb_; ++l) {
                        for (auto m = 0ul; m < t_; ++m) {
                            auto int_i = static_cast<int>(i);
                            auto int_j = static_cast<int>(j);
                            auto int_k = static_cast<int>(k);
                            auto int_l = static_cast<int>(l);
                            auto int_m = static_cast<int>(m);

                            if (w[int_i][int_j][int_k][int_l][int_m] > PRECISION) {
                                std::cout << "w[" << i << "][" << j << "][" << k << "][" << l << "][" << m << "] = "
                                        << w[int_i][int_j][int_k][int_l][int_m];
                            }
                        }
                    }
                }
            }
        }

        // -------- y ----------
        for (auto i = 0ul; i < d_; ++i) {
            for (auto j = 0ul; j < mb_; ++j) {
                for (auto k = 0ul; k < t_; ++k) {
                    if (y[i][j][k] > PRECISION) {
                        os << "y[" << i << "][" << j << "][" << k << "] = " << y[i][j][k];
                    }
                }
            }
        }

        // -------- yb ----------
        for (auto i = 0ul; i < d_; ++i) {
            for (auto j = 0ul; j < mb_; ++j) {
                if (yb[i][j] > PRECISION) {
                    os << "yb[" << i << "][" << j << "] = " << yb[i][j];
                }
            }
        }

        // -------- ws ----------
        for (auto i = 0ul; i < d_ - 1; ++i) {
            for (auto j = i + 1; j < d_; ++j) {
                int conflict = algorithm_->get_conflict_graph().ReturnConflict(i, j);

                if (conflict > 0) {  // soft constraint
                    if (ws[i][j] > PRECISION) {
                        os << "ws[" << i << "][" << j << "] = " << ws[i][j];
                    }
                }
            }
        }

        // -------- e ----------
        for (auto i = 0ul; i < numr_; ++i) {
            for (auto j = 0ul; j < n_; ++j) {
                if (e[i][j] > PRECISION) {
                    os << "e[" << i << "][" << j << "] = " << e[i][j];
                }
            }
        }

        // -------- b ----------
        for (auto i = 0ul; i < numb_; ++i) {
            std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(algorithm_->GetVirtualMachineSize() + i);

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (auto j = 0ul; j <= bucket->get_number_of_GB_per_cost_intervals(); ++j) {
                    if (b[i][j] > PRECISION) {
                        os << "b[" << i << "][" << j << "] = " << b[i][j];
                    }
                }
            } else {
                exit(1);  // error
            }
        }

        // -------- q ----------
        for (auto i = 0ul; i < numb_; ++i) {
            std::shared_ptr<Storage> storage = algorithm_->GetStoragePerId(algorithm_->GetVirtualMachineSize() + i);

            if (auto bucket = dynamic_pointer_cast<Bucket>(storage)) {
                for (auto j = 0ul; j <= bucket->get_number_of_GB_per_cost_intervals(); ++j) {
                    if (q[i][j] > PRECISION) {
                        os << "q[" << i << "][" << j << "] = " << q[i][j];
                    }
                }
            } else {
                exit(1);  // error
            }
        }

        // -------- v ----------
        for (auto i = 0ul; i < m_; ++i) {
            for (auto j = 0ul; j < t_; ++j) {
                if (v[i][j] > PRECISION) {
                    os << "v[" << i << "][" << j << "] = " << v[i][j];
                }
            }
        }

        // -------- z ----------
        for (auto i = 0ul; i < m_; ++i) {
            if (z[i] > PRECISION) {
                os << "z[" << i << "] = " << z[i];
            }
        }

        // -------- z_max ----------
        if (z_max > PRECISION) {
            os << "z_max" << " = " << z_max;
        }

        os << "Writing CPLEX solution information";

        int *tempo = new int[mb_];

        /* Relate each device with its array of data */
        int **maq_dado = new int *[mb_];

        // Initializations

        for (auto i = 0ul; i < mb_; i++) {
            tempo[i] = 0;
        }

        for (auto i = 0ul; i < mb_; i++) {
            maq_dado[i] = new int[d_];
            for (auto j = 0ul; j < d_; j++) {
                maq_dado[i][j] = -1;
            }
        }

        // Vector for TEST that saves the actual time the machine spends performing its action (exec., read, Write)
        // Save files per machine

        os << std::endl;
        os << "---------------- Timeline --------------" << std::endl;
        os << "t)\t";
        for (auto j = 0ul; j < mb_; j++) {
            os << "(M" << j << ")\t" << "(Y)\t" << "(V)\t";
        }
        os << std::endl;

        os << std::endl;

        // Time
        for (auto check = 0ul, t1 = 0ul; t1 < t_ and check <= 1ul; t1++) {
            os << "t" << t1 << ")\t";

            // Devices
            for (auto j = 0ul; j < mb_ and check <= 1; j++) {
                std::shared_ptr<Storage> device = algorithm_->GetStoragePerId(j);

                if (auto virtual_machine = std::dynamic_pointer_cast<VirtualMachine>(device)) {
                    // Check if more than one action is started at the same time on the same machine
                    check = 0ul;

                    // During an action (read, execute, Write)
                    tempo[j] = tempo[j] - 1;

                    if (tempo[j] > 0) {
                        check += 1;
                        os << "(" << tempo[j] << ")" << "\t";
                    }

                    // Execution
                    for (auto i = 0ul; i < n_ and check <= 1ul; i++) {
                        auto activation = algorithm_->GetActivationPerId(1ul + i);
                        auto input_files = activation->get_input_files();

                        auto int_i = static_cast<int>(i);
                        auto int_j = static_cast<int>(j);
                        auto int_t1 = static_cast<int>(t1);

                        auto x_ijt = static_cast<float>(x[int_i][int_j][int_t1]);  // Returns the value of the variable

                        if (x_ijt >= (1 - PRECISION)) {
                            check += 1;
                            os << "x" << i << "\t";

                            if (tempo[j] > 0) {
                                os << "*** RUN ON TOP OF SOMETHING ***";
                                check += 1;
                                break;
                            }

                            tempo[j] = std::ceil(activation->get_time() * virtual_machine->get_slowdown());

                            // Checks if the readings were taken by the machine before execution
                            for (auto d1 = 0ul; d1 < input_files.size() and check <= 1; d1++) {
                                bool has_read_file = false;

                                for (auto k = 0ul; k < mb_ and check <= 1; k++) {
                                    for (auto q_other = 0ul; q_other < t_ and check <= 1; q_other++) {
                                        auto int_d1 = static_cast<int>(d1);
                                        auto int_k = static_cast<int>(k);
                                        auto int_q_other = static_cast<int>(q_other);

                                        auto r_idjpt = static_cast<float>(r[int_i][int_d1][int_j][int_k][int_q_other]);

                                        // Returns the value of the variable
                                        if (r_idjpt >= (1 - PRECISION)) {
                                            has_read_file = true;
                                        }
                                    }
                                }

                                if (!has_read_file) {
                                    os << "*** PERFORMED THE TASK WITHOUT READING ALL THE ENTRIES *** missing data ="
                                       << input_files[d1]->get_id()
                                       << "\n" << input_files[d1];
                                    check += 1;
                                    break;
                                }
                            }
                            // ----------------------------------------------------------------
                        }

                        if (check > 1) {
                            os << "*** CONFLICT WITH EXECUTION ***";
                            break;
                        }
                    }

                    // Read
                    for (auto i = 0ul; i < n_ and check <= 1; i++) {
                        auto activation = algorithm_->GetActivationPerId(1ul + i);
                        auto input_files = activation->get_input_files();

                        for (auto d1 = 0ul; d1 < input_files.size() and check <= 1; d1++) {
                            auto file = input_files[d1];

                            for (auto k = 0ul; k < mb_ and check <= 1ul; k++) {
                                auto storage = algorithm_->GetStoragePerId(k);

                                auto int_i = static_cast<int>(i);
                                auto int_d1 = static_cast<int>(d1);
                                auto int_j = static_cast<int>(j);
                                auto int_k = static_cast<int>(k);
                                auto int_t1 = static_cast<int>(t1);


                                auto r_idjpt = static_cast<float>(r[int_i][int_d1][int_j][int_k][int_t1]);

                                // Returns the value of the variable
                                if (r_idjpt >= (1 - PRECISION)) {
//                                    auto read_time = algorithm_->ComputeFileTransferTime(file, virtual_machine,
//                                                                                         storage);
                                    auto read_time = Cplex::ComputeFileTransferTime(
                                            file, virtual_machine, storage);
                                    check += 1;
                                    os << "r" << i << "(" << input_files[d1]->get_id() << ")<" << k << "\t";

                                    if (tempo[j] > 0) {
                                        os << "*** READ OVER SOMETHING ***";
                                        check += 1;
                                        break;
                                    }

                                    tempo[j] = read_time;

                                    // Check file existence
                                    auto y_djt = static_cast<float>(y[int_d1][int_k][int_t1]);

                                    // Returns the value of the variable
                                    if (y_djt < (1 - PRECISION)) {
                                        os << "*** READ NON-EXISTENT FILE IN THE MACHINE ***";
                                        check += 1;
                                        break;
                                    }
                                    // ---------------------------
                                }

                                if (check > 1) {
                                    os << "*** CONFLICT WITH READING ***";
                                    break;
                                }
                            }
                        }
                    }

                    // Writing
                    for (auto has_passed = 0ul, i = 0ul; i < n_ and check <= 1; i++) {
                        auto activation = algorithm_->GetActivationPerId(1ul + i);
                        auto output_files = activation->get_output_files();

                        for (auto d1 = 0ul; d1 < output_files.size() and check <= 1; d1++) {
                            auto file = output_files[d1];

                            for (auto k = 0ul; k < mb_ and check <= 1; k++) {
                                auto storage = algorithm_->GetStoragePerId(k);

                                auto int_i = static_cast<int>(i);
                                auto int_d1 = static_cast<int>(d1);
                                auto int_j = static_cast<int>(j);
                                auto int_k = static_cast<int>(k);
                                auto int_t1 = static_cast<int>(t1);

                                auto w_idjpt = static_cast<float>(w[int_i][int_d1][int_j][int_k][int_t1]);

                                // Returns the value of the variable
                                if (w_idjpt >= (1 - PRECISION)) {
//                                    int write_time = algorithm_->ComputeFileTransferTime(file, virtual_machine, storage);
                                    auto write_time = Cplex::ComputeFileTransferTime(
                                            file, virtual_machine, storage);

                                    check += 1;

                                    os << "w" << i << "(" << output_files[d1]->get_id() << ")<"
                                       << k << "\t";

                                    if (tempo[j] > 0) {
                                        os << "*** WRITE ON SOMETHING ***";
                                        check += 1;
                                        break;
                                    }

                                    tempo[j] = write_time;

                                    // Checks if the activation has been executed on the machine before
                                    has_passed = 0;
                                    for (auto q_other = 0ul; q_other < t_ and check <= 1; q_other++) {
                                        auto int_q_other = static_cast<int>(q_other);

                                        auto x_ijt = static_cast<float>(x[int_i][int_j][int_q_other]);

                                        if (x_ijt >= (1 - PRECISION)) {
                                            has_passed = 1;
                                        }
                                    }

                                    if (has_passed == 0) {
                                        os << "*** WROTE DATA WITHOUT HAVING PERFORMED THE TASK ON THE MACHINE ***";
                                        check += 1;
                                        break;
                                    }
                                    // -----------------------------------------------
                                }

                                if (check > 1) {
                                    os << "*** CONFLICT WITH WRITING ***";
                                    break;
                                }
                            }
                        }
                    }

                    // Idle machine at this time
                    if (check == 0ul) {
                        os << "-\t";
                    }

                    // Data
                    for (auto d1 = 0ul; d1 < d_ and check <= 1; d1++) {
                        auto int_d1 = static_cast<int>(d1);
                        auto int_j = static_cast<int>(j);
                        auto int_t1 = static_cast<int>(t1);

                        auto y_djt = static_cast<float>(y[int_d1][int_j][int_t1]);
                        // Returns the value of the variable

                        if (y_djt >= (1 - PRECISION)) {
                            maq_dado[int_j][int_d1] = 1;
                        }

                        if (maq_dado[int_j][int_d1] == 1) {
                            os << d1 << ",";
                        }
                    }

                    os << "\t";

                    // Hiring
                    auto int_j = static_cast<int>(j);
                    auto int_t1 = static_cast<int>(t1);

                    auto v_jt = static_cast<float>(v[int_j][int_t1]);

                    // Returns the value of the variable
                    if (v_jt >= (1 - PRECISION)) {
                        os << "*\t";
                    } else {
                        os << "\t";
                    }
                } else {
                    os << "N\\A\t";

                    // Data
                    for (auto d1 = 0ul; d1 < d_ and check <= 1; d1++) {
                        auto int_d1 = static_cast<int>(d1);
                        auto int_j = static_cast<int>(j);
                        auto int_t1 = static_cast<int>(t1);

                        // Returns the value of the variable
                        auto y_djt = static_cast<float>(y[int_d1][int_j][int_t1]);

                        if (y_djt >= (1 - PRECISION)) {
                            maq_dado[int_j][int_d1] = 1;
                        }

                        if (maq_dado[int_j][int_d1] == 1) {
                            os << d1 << ",";
                        }
                    }

                    os << "\t";

                    // Hiring
                    os << "N\\A\t";
                }
            }
            os << std::endl;
        }
        os << "---------------------------------------------" << std::endl;

        // Releasing resources

        delete[] tempo;
        for (auto i = 0ul; i < mb_; i++) {
            delete[] maq_dado[i];
        }
        delete[] maq_dado;

        return os;
    }
};

#endif  // APPROXIMATE_SOLUTIONS_SRC_CPLEX_H_
