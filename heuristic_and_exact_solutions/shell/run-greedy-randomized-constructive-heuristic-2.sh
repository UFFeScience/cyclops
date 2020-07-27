#!/bin/bash
cd ..
#echo  GLOG_log_dir=`pwd`/log
export GLOG_log_dir=`pwd`/log
#export OMP_NUM_THREADS=4
./bin/wf_security_greedy.x --tasks_and_files `pwd`/input/tasks_and_files/Sipht_100.xml.dag \
  --cluster `pwd`/input/clouds/cluster2.vcl \
  --conflict_graph `pwd`/input/conflict_graph/Sipht_100.xml.scg \
  --algorithm greedy_randomized_constructive_heuristic \
  --alpha_time 0.3 \
  --alpha_budget 0.3 \
  --alpha_security 0.4 \
  --number_of_iteration 100 \
  --number_of_allocation_experiments 1
cd shell
