#!/bin/bash
cd ..
#echo  GLOG_log_dir=`pwd`/log
export GLOG_log_dir=`pwd`/log
#export OMP_NUM_THREADS=4
./bin/wf_security_greedy.x --tasks_and_files `pwd`/input/tasks_and_files/CyberShake_30.xml.dag \
  --cluster `pwd`/input/clouds/cluster2.vcl \
  --conflict_graph `pwd`/input/conflict_graph/CyberShake_30.xml.scg \
  --algorithm greedy_randomized_constructive_heuristic \
  --alpha_time 0.49 \
  --alpha_budget 0.49 \
  --alpha_security 0.02 \
  --number_of_iteration 100 \
  --number_of_allocation_experiments 3000
cd shell
