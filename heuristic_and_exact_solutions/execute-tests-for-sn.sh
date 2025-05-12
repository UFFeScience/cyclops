#!/usr/bin/env bash

# ### functions ###

# Contents of the cloud file
# <number of provider> <n# requerimentos>
#
# <id-provider> <provider name> <period - seconds> <max vms> <number of vms> <number of buckets>
# <id-vm> <type> <slowdown> <storage GB> <bandwidth MBps> <cost $> <requirement-1> <requirement-2>
# <id-bucket> <type> <storage GB> <bandwidth MBps> <cost> <requirement-1> <requirement-2 (pode assumir 0 1 2{so bucket})>
#
# <id-bucket> <capacidade> <cost> <numero de intervalos> [<limite superior> <valor contratado por intervalo>] <requirement-1> <requirement-2 (pode assumir 0 1 2{so bucket})>
#
# ex.
# <limite superior> <valor contratado por intervalo> <limite superior> <valor contratado por intervalo>
# 50TB 0.023 450TB 0.022 500TB 0.021
#
# R1. Anonimização / Criptografia
# R2. Confiabilidade da Nuvem (GDPR - General Data Protection Regulation)
#
# VM1 -  80 GB disco, csi = 1.00, Rede =  4 Mbps, Dolar = 0.02
# VM2 - 120 GB disco, csi = 0.77, Rede =  9 Mbps, Dolar = 0.09
# VM3 - 160 GB disco, csi = 0.38, Rede = 10 Mbps, Dolar = 0.16
# VM4 - 200 GB disco, csi = 0.19, Rede = 10 Mbps, Dolar = 0.32
# 1) 1 cloud site homogêneo: 4 máquinas do tipo VM1
# 	1st - 1 cloud site homogêneo: 4 máquinas do tipo VM1 com uma só atendendo ao requisito de criptografia/anonimização e todas atendendo ao requisito de  confiabilidade
# 	2nd - 1 cloud site homogêneo: 4 máquinas do tipo VM1 com duas atendendo ao requisito de criptografia/anonimização e todas atendendo ao requisito de  confiabilidade
# 2) 1 cloud site heterogêneo: 4 máquinas - uma de cada tipo:
# 	3th - 1 cloud site heterogêneo: 4 máquinas - uma de cada tipo com uma só atendendo ao requisito de criptografia/anonimização e todas atendendo ao requisito de  confiabilidade
# 	4th - 1 cloud site heterogêneo: 4 máquinas - uma de cada tipo com duas atendendo ao requisito de criptografia/anonimização e todas atendendo ao requisito de  confiabilidade
# 3) 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - a conexão entre VMs de sites diferentes tem que ser mais lenta no momento de montar a matriz
# 	5th - 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - os dois sites atendendo o requisito de confiabilidade e uma máquina em cada site atendendo o requisito de criptografia/anonimização
# 	6th - 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - os dois sites atendendo o requisito de confiabilidade e duas máquinas em cada site atendendo o requisito de criptografia/anonimização
# 	7th - 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - apenas um site atendendo o requisito de confiabilidade e uma máquina em cada site atendendo o requisito de criptografia/anonimização
# 	8th - 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - apenas um site atendendo o requisito de confiabilidade e duas máquinas em cada site atendendo o requisito de criptografia/anonimização
# 4) 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - a conexão entre VMs de sites diferentes tem que ser mais lenta no momento de montar a matriz:
# 	9th - 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - os dois sites atendendo o requisito de confiabilidade e uma máquina em cada site atendendo o requisito de criptografia/anonimização
# 	10th - 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - os dois sites atendendo o requisito de confiabilidade e duas máquinas em cada site atendendo o requisito de criptografia/anonimização
# 	11th - 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - apenas um site atendendo o requisito de confiabilidade e uma máquina em cada site atendendo o requisito de criptografia/anonimização
# 	12th - 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - apenas um site atendendo o requisito de confiabilidade e duas máquinas em cada site atendendo o requisito de criptografia/anonimização

create_cloud_files_for_sn() {
    # <id-provider> <provider name> <period - seconds> <max vms> <number of vms> <number of buckets>
    middle_cloud_desc="Amazon-EC2 1 300"
    # <id-vm> <type> <slowdown> <storage GB> <bandwidth Gbps> <cost per hour $> <requirement-1> <requirement-2>
#    middle_vm1_desc="nano 0.40 20 0.1 0.02"
#    middle_vm2_desc="micro 0.36 20 0.1 0.03"
#    middle_vm3_desc="small 0.32 20 0.1 0.045"
#    middle_vm4_desc="medium 0.28 20 0.1 0.06"
#    middle_vm5_desc="large 0.24 20 0.1 0.09"
#    middle_vm6_desc="xlarge 0.20 20 0.1 0.12"
#    middle_vm7_desc="2xlarge 0.16 20 0.1 0.16"
#    middle_vm8_desc="3xlarge 0.12 20 0.1 0.33"
#    Montage
    middle_vm1_desc="nano 2.60 20 0.1 0.02"
    middle_vm2_desc="micro 2.60 20 0.1 0.03"
    middle_vm3_desc="small 2.60 20 0.1 0.045"
    middle_vm4_desc="medium 2.60 20 0.1 0.06"
    middle_vm5_desc="large 2.60 20 0.1 0.09"
    middle_vm6_desc="xlarge 2.60 20 0.1 0.12"
    middle_vm7_desc="2xlarge 2.60 20 0.1 0.16"
    middle_vm8_desc="3xlarge 2.60 20 0.1 0.33"
#   Epigenomics - Não deu certo
#   Inspiral
#    middle_vm1_desc="nano 0.30 20 1000000000.1 0.20"
#    middle_vm2_desc="micro 0.30 20 1000000000.1 0.20"
#    middle_vm3_desc="small 0.30 20 1000000000.1 0.20"
#    middle_vm4_desc="medium 0.30 20 1000000000.1 0.20"
#    middle_vm5_desc="large 0.30 20 1000000000.1 0.20"
#    middle_vm6_desc="xlarge 0.30 20 1000000000.1 0.20"
#    middle_vm7_desc="2xlarge 0.30 20 1000000000.1 0.20"
#    middle_vm8_desc="3xlarge 0.30 20 1000000000.1 0.20"
#   Sipht
    middle_vm1_desc="nano 0.05 20 1000000000.1 0.20"
    middle_vm2_desc="micro 0.05 20 1000000000.1 0.20"
    middle_vm3_desc="small 0.05 20 1000000000.1 0.20"
    middle_vm4_desc="medium 0.05 20 1000000000.1 0.20"
    middle_vm5_desc="large 0.05 20 1000000000.1 0.20"
    middle_vm6_desc="xlarge 0.05 20 1000000000.1 0.20"
    middle_vm7_desc="2xlarge 0.05 20 1000000000.1 0.20"
    middle_vm8_desc="3xlarge 0.05 20 1000000000.1 0.20"

    # <id-bucket> <capacidade> <cost> <numero de intervalos> [<limite superior> <valor contratado por intervalo>] <requirement-1> <requirement-2 (pode assumir 0 1 2{so bucket})>
#    middle_bucket_desc="Standard 51200 1000000000.0 0.023 1 1"

    # Bucket variable from 1 to 64
#    BUCKETS=($(seq 1 64))
    BUCKETS=(0)

    # certify folder exists
    FOLDER=./temp
    if ! [[ -d ${FOLDER} ]]; then mkdir ${FOLDER}; fi
    FOLDER=./temp/clouds
    if [[ -d ${FOLDER} ]]; then
        rm -r ${FOLDER}
        mkdir ${FOLDER}
    else
        mkdir ${FOLDER}
    fi

    # Nomenclature
    # <scenery>_<#Cloud>_<#vm>_<#buckets>_<#fulfill R1>_<#fulfill R2>_<description>.vcl

    # 12 scenarios

# 4th - homogeneous cloud (two met R1 requirement)
    for elem in "${BUCKETS[@]}"; do
        # Header anc cloud
        file_name="${FOLDER}/4_1_4_${elem}_2_4_heterogeneous_cloud.vcl"
        echo "${file_name}"
        cat <<EOF >"${file_name}"
1 2

1 ${middle_cloud_desc} $((8 * 1)) ${elem}
EOF
        # VMs
        for ((i = 0; i < 1; i++)); do
            echo "$((8*i+0)) ${middle_vm1_desc} 1 1" >> "${file_name}"
            echo "$((8*i+1)) ${middle_vm2_desc} 1 1" >> "${file_name}"
            echo "$((8*i+2)) ${middle_vm3_desc} 0 1" >> "${file_name}"
            echo "$((8*i+3)) ${middle_vm4_desc} 0 1" >> "${file_name}"
            echo "$((8*i+4)) ${middle_vm5_desc} 1 1" >> "${file_name}"
            echo "$((8*i+5)) ${middle_vm6_desc} 1 1" >> "${file_name}"
            echo "$((8*i+6)) ${middle_vm7_desc} 0 1" >> "${file_name}"
            echo "$((8*i+7)) ${middle_vm8_desc} 0 1" >> "${file_name}"
        done

        # Buckets
        for ((i = 0; i < elem; i++)); do
            echo "${i} ${middle_bucket_desc}" >>"${file_name}"
        done
    done
}

# ### Main ###

now=$(date +"%Y%m%d%H%M%S")
echo "${now}"

create_cloud_files_for_sn

home_dir=$(pwd)
regex="([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9a-zA-Z]+)_cloud.vcl"

out_dir="${home_dir}/output/${now}"
log_dir="${home_dir}/log/${now}"
temp_dir="${home_dir}/temp/${now}"

mkdir -p "${out_dir}"
mkdir -p "${log_dir}"
mkdir -p "${temp_dir}"

pipenv run python script/run-batch.py \
    --instances-file="_instances_desenv.txt" \
    --algorithms-file="_algorithms_desenv.txt" \
    --number-of-iterations=100 \
    --allocation_experiments=4 \
    --repeat 1 \
    --output-path="${out_dir}" \
    --log-dir="${log_dir}" \
    --temp-dir="${temp_dir}"
