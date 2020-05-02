#!/bin/bash
cd ..
#echo  GLOG_log_dir=`pwd`/log
export GLOG_log_dir=`pwd`/log
#export OMP_NUM_THREADS=4
./bin/wf_security_greedy.x  --tasks_and_files `pwd`/input/tasks_and_files/3_toy_5_A.dag \
  --cluster `pwd`/input/clouds/cluster2.vcl \
  --conflict_graph `pwd`/input/conflict_graph/3_toy_5_A.scg \
  --algorithm cplex \
  --cplex_input_file `pwd`/input/cplex/graph1.col

cd shell
