#!/bin/bash

#home="/home/vagrant/Wf-Security"
home=$(pwd)
now=$(date +"%Y%m%d%H%M%S")
cluster_file=${home}"/input/clouds/cluster_015_A.vcl"
alpha_time=0.9
alpha_cost=0.05
alpha_security=0.05
out_dir="${home}/output/${now}"
log_dir="${home}/log/${now}"
temp_dir="${home}/temp/${now}"
test_scenario=0
number_cloud=0
number_vm=0
number_buckets=0
number_vm_req_cryptography=0
number_vm_req_confidentiality=0
cloud_type=0

mkdir -p "${out_dir}"
mkdir -p "${log_dir}"
mkdir -p "${temp_dir}"

echo "${out_dir}"
echo "${log_dir}"
echo "${temp_dir}"

pipenv run python script/run-batch.py --cplex \
            --instances-file="_instances_desenv.txt" \
            --clouds-file=${cluster_file} \
            --alpha-time=${alpha_time} \
            --alpha-cost=${alpha_cost} \
            --alpha-security=${alpha_security} \
            --number-of-iterations=100 \
            --allocation_experiments=4 \
            --repeat 1 \
            --output-path="${out_dir}" \
            --test-scenery="${test_scenario}" \
            --number-cloud-sites="${number_cloud}" \
            --number-vm="${number_vm}" \
            --number-buckets="${number_buckets}" \
            --number-vm-req-cryptography="${number_vm_req_cryptography}" \
            --number-vm-req-confidentiality="${number_vm_req_confidentiality}" \
            --cloud-type="${cloud_type}" \
            --log-dir=${log_dir} \
            --temp-dir=${temp_dir}

echo "Finished!"