#!/bin/bash
cd ..
#echo  GLOG_log_dir=`pwd`/log
export GLOG_log_dir=`pwd`/log
#export OMP_NUM_THREADS=4
./bin/wf_security_greedy.x  --tasks_and_files `pwd`/input/tasks_and_files/Sipht_100.xml.dag \
  --cluster `pwd`/input/clouds/cluster2.vcl \
  --conflict_graph `pwd`/input/conflict_graph/Sipht_100.xml.scg \
  --algorithm cplex \
  --cplex_input_file `pwd`/input/cplex/graph1.col

cd shell
