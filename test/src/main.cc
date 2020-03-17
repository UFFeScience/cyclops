/**
 * \file src/main.cc
 * \brief Run the Wf-Security Greedy Solution executable.
 *
 * \author Rodrigo Alves Prado da Silva \<rodrigo_prado@id.uff.br\>
 * \date 2020
 *
 * This source file contains the \c main() function that reads the execution mode from the
 * command-line and calls the appropriate methods.
 */

#include "src/main.h"
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "src/solution/Algorithm.h"
#include "src/DataStructure/Matrix.h"

DEFINE_string(tasks_and_files,
              "CyberShake_30.xml.dag",
              "comma-separated list of languages to offer in the 'lang' menu");

DEFINE_string(cluster,
              "cluster.vcl",
              "comma-separated list of languages to offer in the 'lang' menu");

DEFINE_string(conflict_graph,
              "CyberShake_30.xml.scg",
              "comma-separated list of languages to offer in the 'lang' menu");

DEFINE_string(algorithm,
              "greedy",
              "comma-separated list of languages to offer in the 'lang' menu");

/**
 * The \c main() function reads the configuration parameters from JSON files,
 * loads the desired input file, applies the required algorithms and writes
 * output data files.
 */
int main(int argc, char **argv) {
  // Initialise Google's logging library
  ::google::InitGoogleLogging(argv[0]);

  gflags::SetUsageMessage("some usage message");
  gflags::SetVersionString("0.0.1");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  DLOG(INFO) << "Starting ...";

  DLOG(INFO) << "Input File of the Tasks and Files: " << FLAGS_tasks_and_files;
  DLOG(INFO) << "Input File of the Cluster: " << FLAGS_cluster;
  DLOG(INFO) << "Input File of the Conflict Graph: " << FLAGS_conflict_graph;
  DLOG(INFO) << "Input File of the Conflict Graph: " << FLAGS_algorithm;
  // google::FlushLogFiles(google::INFO);

  std::unique_ptr<Algorithm> algorithm = Algorithm::getAlgorithm(FLAGS_algorithm);

  DLOG(INFO) << "... algorithm picked-up ...";

  algorithm->readInputFiles(FLAGS_tasks_and_files, FLAGS_cluster, FLAGS_conflict_graph);

  // Matrix<double, 0> m0 { 1 };

  // Matrix<double, 2> m0 { {1, 2, 3},
  //                        {4, 5, 6},
  //                        {7, 8, 9} };

  // Matrix<double, 1> m1 { {1}, {2}, {3} };
  Matrix_impl::Matrix<int, 1> m1 { 1, 2, 3, 4 };  // one dimension: a vector (4 elements)
  // Matrix_impl::Matrix<int, 1> m1(4);  // one dimension: a vector (4 elements)
  // Matrix<double, 2> m2(10, 10);

  // std::cout << m0.row(0);

  DLOG(INFO) << "... ending.";
  gflags::ShutDownCommandLineFlags();
  return 0;
}
