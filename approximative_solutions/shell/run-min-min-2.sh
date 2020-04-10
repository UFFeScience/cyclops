#!/bin/bash
cd ..
#echo  GLOG_log_dir=`pwd`/log
export GLOG_log_dir=`pwd`/log
#export OMP_NUM_THREADS=4
./bin/wf_security_greedy.x --tasks_and_files `pwd`/input/tasks_and_files/CyberShake_30.xml.dag \
  --cluster `pwd`/input/clouds/cluster2.vcl \
  --conflict_graph `pwd`/input/conflict_graph/CyberShake_30.xml.scg \
  --algorithm min_min \
  --alpha_time 0.4 \
  --alpha_budget 0.4 \
  --alpha_security 0.2
cd shell

