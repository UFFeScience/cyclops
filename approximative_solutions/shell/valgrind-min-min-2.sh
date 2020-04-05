#!/bin/bash
cd ..
#echo  GLOG_log_dir=`pwd`/log
export GLOG_log_dir=`pwd`/log
#export OMP_NUM_THREADS=4
# --show-lead-kinds=all
valgrind --leak-check=full ./bin/wf_security_greedy.x --tasks_and_files `pwd`/input/tasks_and_files/CyberShake_30.xml.dag \
  --cluster `pwd`/input/clouds/cluster2.vcl \
  --conflict_graph `pwd`/input/conflict_graph/CyberShake_30.xml.scg \
  --algorithm min_min
cd shell

