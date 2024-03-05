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
# And considering a following range of number of buckets 2, 4, 8, 16, 32, 64
create_cloud_files() {
    # <id-provider> <provider name> <period - seconds> <max vms> <number of vms> <number of buckets>
    middle_cloud_desc="Amazon-EC2 1 30 4"
    # <id-vm> <type> <slowdown> <storage GB> <bandwidth MBps> <cost $> <requirement-1> <requirement-2>
    middle_vm1_desc="m3.medium 1.53 80 4.0 0.02"
    middle_vm2_desc="m3.large 0.77 120 9.0 0.09"
    middle_vm3_desc="m3.xlarge 0.38 160 10.0 0.16"
    middle_vm4_desc="m3.2xlarge 0.19 200 10.0 0.33"
    # <id-bucket> <capacidade> <cost> <numero de intervalos> [<limite superior> <valor contratado por intervalo>] <requirement-1> <requirement-2 (pode assumir 0 1 2{so bucket})>
    middle_bucket_desc="Standard 51200 25.0 0.023 1 1"

    BUCKETS=(2 4 8 16 32 64)

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

    # 1st - homogeneous cloud (one VM met R1 requirement)
    for elem in "${BUCKETS[@]}"; do
        # Header and cloud
        file_name="${FOLDER}/1_1_4_${elem}_1_4_homogeneous_cloud.vcl"
        echo "${file_name}"
        cat <<EOF >"${file_name}"
1 2

1 ${middle_cloud_desc} ${elem}
0 ${middle_vm1_desc} 1 1
1 ${middle_vm1_desc} 0 1
2 ${middle_vm1_desc} 0 1
3 ${middle_vm1_desc} 0 1
EOF
        # Buckets
        for ((i = 0; i < elem; i++)); do
            echo "${i} ${middle_bucket_desc}" >>"${file_name}"
        done
    done

    # 2nd - homogeneous cloud (two met R1 requirement)
    for elem in "${BUCKETS[@]}"; do
        # Header and cloud
        file_name="${FOLDER}/2_1_4_${elem}_2_4_homogeneous_cloud.vcl"
        echo "${file_name}"
        cat <<EOF >"${file_name}"
1 2

1 ${middle_cloud_desc} ${elem}
0 ${middle_vm1_desc} 1 1
1 ${middle_vm1_desc} 1 1
2 ${middle_vm1_desc} 0 1
3 ${middle_vm1_desc} 0 1
EOF
        # Buckets
        for ((i = 0; i < elem; i++)); do
            echo "${i} ${middle_bucket_desc}" >>"${file_name}"
        done
    done

    # 3rd - heterogeneous cloud (one met R1 requirement)
    for elem in "${BUCKETS[@]}"; do
        # Header and cloud
        file_name="${FOLDER}/3_1_4_${elem}_1_4_heterogeneous_cloud.vcl"
        echo "${file_name}"
        cat <<EOF >"${file_name}"
1 2

1 ${middle_cloud_desc} ${elem}
0 ${middle_vm1_desc} 1 1
1 ${middle_vm2_desc} 0 1
2 ${middle_vm3_desc} 0 1
3 ${middle_vm4_desc} 0 1
EOF
        # Buckets
        for ((i = 0; i < elem; i++)); do
            echo "${i} ${middle_bucket_desc}" >>"${file_name}"
        done
    done

    # 4th - homogeneous cloud (two met R1 requirement)
    for elem in "${BUCKETS[@]}"; do
        # Header anc cloud
        file_name="${FOLDER}/4_1_4_${elem}_2_4_heterogeneous_cloud.vcl"
        echo "${file_name}"
        cat <<EOF >"${file_name}"
1 2

1 ${middle_cloud_desc} ${elem}
0 ${middle_vm1_desc} 1 1
1 ${middle_vm2_desc} 1 1
2 ${middle_vm3_desc} 0 1
3 ${middle_vm4_desc} 0 1
EOF
        # Buckets
        for ((i = 0; i < elem; i++)); do
            echo "${i} ${middle_bucket_desc}" >>"${file_name}"
        done
    done
    #
    ## 	5th - 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - os dois sites atendendo o requisito de confiabilidade e uma máquina em cada site atendendo o requisito de criptografia/anonimização
    #  # 5th - homogeneous clouds (two clouds, one VM attends R1 requirement for each site)
    #  echo "${file_name}"
    #  for elem in "${BUCKETS[@]}" ; do
    #    file_name="${FOLDER}/5_2_8_${elem}_2_8_homogeneous_clouds.vcl"
    #    echo "${file_name}"
    #    # Header and 1st cloud
    #    (( middle_ceil=(elem + 2 - 1)/2 ))
    #    cat << EOF > "${file_name}"
    #2 2
    #
    #1 ${middle_cloud_desc} ${middle_ceil}
    #0 ${middle_vm1_desc} 1 1
    #1 ${middle_vm1_desc} 0 1
    #2 ${middle_vm1_desc} 0 1
    #3 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=0;i<(middle_ceil);i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #    # 2nd cloud
    #    remaining=$((elem-middle_ceil))
    #    cat << EOF >> "${file_name}"
    #2 ${middle_cloud_desc} ${remaining}
    #4 ${middle_vm1_desc} 1 1
    #5 ${middle_vm1_desc} 0 1
    #6 ${middle_vm1_desc} 0 1
    #7 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=middle_ceil;i<elem;i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #  done
    #
    ## 	6th - 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - os dois sites atendendo o requisito de confiabilidade e duas máquinas em cada site atendendo o requisito de criptografia/anonimização
    #  # 6th - homogeneous cloud (two clouds, two VM attend to R1 requirement for each site)
    #  for elem in "${BUCKETS[@]}" ; do
    #    file_name="${FOLDER}/6_2_8_${elem}_4_8_homogeneous_clouds.vcl"
    #    echo "${file_name}"
    #    # Header and 1st cloud
    #    (( middle_ceil=(elem + 2 - 1)/2 ))
    #    cat << EOF > "${file_name}"
    #2 2
    #
    #1 ${middle_cloud_desc} ${middle_ceil}
    #0 ${middle_vm1_desc} 1 1
    #1 ${middle_vm1_desc} 1 1
    #2 ${middle_vm1_desc} 0 1
    #3 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=0;i<(middle_ceil);i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #    # 2nd cloud
    #    remaining=$((elem-middle_ceil))
    #    cat << EOF >> "${file_name}"
    #2 ${middle_cloud_desc} ${remaining}
    #4 ${middle_vm1_desc} 1 1
    #5 ${middle_vm1_desc} 1 1
    #6 ${middle_vm1_desc} 0 1
    #7 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=middle_ceil;i<elem;i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #  done
    #
    ## 	7th - 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - apenas um site atendendo o requisito de confiabilidade e uma máquina em cada site atendendo o requisito de criptografia/anonimização
    #  # 7th - homogeneous cloud (two homogeneous clouds, just 1st site attends to R2, and one VM attend to R1 requirement for each site)
    #  for elem in "${BUCKETS[@]}" ; do
    #    file_name="${FOLDER}/7_2_8_${elem}_2_4_homogeneous_clouds.vcl"
    #    echo "${file_name}"
    #    # Header and 1st cloud
    #    (( middle_ceil=(elem + 2 - 1)/2 ))
    #    cat << EOF > "${file_name}"
    #2 2
    #
    #1 ${middle_cloud_desc} ${middle_ceil}
    #0 ${middle_vm1_desc} 1 1
    #1 ${middle_vm1_desc} 0 1
    #2 ${middle_vm1_desc} 0 1
    #3 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=0;i<(middle_ceil);i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #    # 2nd cloud
    #    remaining=$((elem-middle_ceil))
    #    cat << EOF >> "${file_name}"
    #2 ${middle_cloud_desc} ${remaining}
    #4 ${middle_vm1_desc} 1 0
    #5 ${middle_vm1_desc} 0 0
    #6 ${middle_vm1_desc} 0 0
    #7 ${middle_vm1_desc} 0 0
    #EOF
    #    # Buckets
    #    for ((i=middle_ceil;i<elem;i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #  done
    #
    ## 	8th - 2 cloud sites homogêneos: 4 máquinas do tipo VM1 em cada cloud site - apenas um site atendendo o requisito de confiabilidade e duas máquinas em cada site atendendo o requisito de criptografia/anonimização
    #  # 8th - homogeneous cloud (two homogeneous clouds, just 1st site attends to R2, and one VM attend to R1 requirement for each site)
    #  for elem in "${BUCKETS[@]}" ; do
    #    file_name="${FOLDER}/8_2_8_${elem}_4_4_homogeneous_clouds.vcl"
    #    echo "${file_name}"
    #    # Header and 1st cloud
    #    (( middle_ceil=(elem + 2 - 1)/2 ))
    #    cat << EOF > "${file_name}"
    #2 2
    #
    #1 ${middle_cloud_desc} ${middle_ceil}
    #0 ${middle_vm1_desc} 1 1
    #1 ${middle_vm1_desc} 1 1
    #2 ${middle_vm1_desc} 0 1
    #3 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=0;i<(middle_ceil);i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #    # 2nd cloud
    #    remaining=$((elem-middle_ceil))
    #    cat << EOF >> "${file_name}"
    #2 ${middle_cloud_desc} ${remaining}
    #4 ${middle_vm1_desc} 1 0
    #5 ${middle_vm1_desc} 1 0
    #6 ${middle_vm1_desc} 0 0
    #7 ${middle_vm1_desc} 0 0
    #EOF
    #    # Buckets
    #    for ((i=middle_ceil;i<elem;i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #  done
    #
    ## 	9th - 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - os dois sites atendendo o requisito de confiabilidade e uma máquina em cada site atendendo o requisito de criptografia/anonimização
    #  # 9th - homogeneous cloud (two clouds, one VM attends R1 requirement for each site)
    #  for elem in "${BUCKETS[@]}" ; do
    #    file_name="${FOLDER}/9_2_8_${elem}_2_8_heterogeneous_clouds.vcl"
    #    echo "${file_name}"
    #    # Header and 1st cloud
    #    (( middle_ceil=(elem + 2 - 1)/2 ))
    #    cat << EOF > "${file_name}"
    #2 2
    #
    #1 ${middle_cloud_desc} ${middle_ceil}
    #0 ${middle_vm1_desc} 1 1
    #1 ${middle_vm2_desc} 0 1
    #2 ${middle_vm3_desc} 0 1
    #3 ${middle_vm4_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=0;i<(middle_ceil);i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #    # 2nd cloud
    #    remaining=${middle_ceil}-${elem}
    #    cat << EOF >> "${file_name}"
    #2 ${middle_cloud_desc}
    #4 ${middle_vm1_desc} 1 1
    #5 ${middle_vm2_desc} 0 1
    #6 ${middle_vm3_desc} 0 1
    #7 ${middle_vm4_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=middle_ceil;i<elem;i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #  done
    #
    ## 	10th - 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - os dois sites atendendo o requisito de confiabilidade e duas máquinas em cada site atendendo o requisito de criptografia/anonimização
    #  # 10th - homogeneous cloud (two clouds, two VM attend to R1 requirement for each site)
    #  for elem in "${BUCKETS[@]}" ; do
    #    file_name="${FOLDER}/10_2_8_${elem}_4_8_heterogeneous_clouds.vcl"
    #    echo "${file_name}"
    #    # Header and 1st cloud
    #    (( middle_ceil=(elem + 2 - 1)/2 ))
    #    cat << EOF > "${file_name}"
    #2 2
    #
    #1 ${middle_cloud_desc} ${middle_ceil}
    #0 ${middle_vm1_desc} 1 1
    #1 ${middle_vm1_desc} 1 1
    #2 ${middle_vm1_desc} 0 1
    #3 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=0;i<(middle_ceil);i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #    # 2nd cloud
    #    remaining=$((elem-middle_ceil))
    #    cat << EOF >> "${file_name}"
    #2 ${middle_cloud_desc} ${remaining}
    #4 ${middle_vm1_desc} 1 1
    #5 ${middle_vm1_desc} 1 1
    #6 ${middle_vm1_desc} 0 1
    #7 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=middle_ceil;i<elem;i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #  done
    #
    ## 	11th - 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - apenas um site atendendo o requisito de confiabilidade e uma máquina em cada site atendendo o requisito de criptografia/anonimização
    #  # 11th - homogeneous cloud (two heterogeneous clouds, just 1st site attends to R2, and one VM attend to R1 requirement for each site)
    #  for elem in "${BUCKETS[@]}" ; do
    #    file_name="${FOLDER}/11_2_8_${elem}_4_2_heterogeneous_clouds.vcl"
    #    echo "${file_name}"
    #    # Header and 1st cloud
    #    (( middle_ceil=(elem + 2 - 1)/2 ))
    #    cat << EOF > "${file_name}"
    #2 2
    #
    #1 ${middle_cloud_desc} ${middle_ceil}
    #0 ${middle_vm1_desc} 1 1
    #1 ${middle_vm1_desc} 0 1
    #2 ${middle_vm1_desc} 0 1
    #3 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=0;i<(middle_ceil);i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #    # 2nd cloud
    #    remaining=$((elem-middle_ceil))
    #    cat << EOF >> "${file_name}"
    #2 ${middle_cloud_desc} ${remaining}
    #4 ${middle_vm1_desc} 1 0
    #5 ${middle_vm1_desc} 0 0
    #6 ${middle_vm1_desc} 0 0
    #7 ${middle_vm1_desc} 0 0
    #EOF
    #    # Buckets
    #    for ((i=middle_ceil;i<=elem;i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #  done
    #
    ## 	12th - 2 cloud sites heterogêneos: 4 máquinas - uma de cada tipo em cada cloud site - apenas um site atendendo o requisito de confiabilidade e duas máquinas em cada site atendendo o requisito de criptografia/anonimização
    #  # 12th - homogeneous cloud (two homogeneous clouds, just 1st site attends to R2, and one VM attend to R1 requirement for each site)
    #  for elem in "${BUCKETS[@]}" ; do
    #    file_name="${FOLDER}/12_2_8_${elem}_4_4_heterogeneous_clouds.vcl"
    #    echo "${file_name}"
    #    # Header and 1st cloud
    #    (( middle_ceil=(elem + 2 - 1)/2 ))
    #    cat << EOF > "${file_name}"
    #2 2
    #
    #1 ${middle_cloud_desc} ${middle_ceil}
    #0 ${middle_vm1_desc} 1 1
    #1 ${middle_vm1_desc} 1 1
    #2 ${middle_vm1_desc} 0 1
    #3 ${middle_vm1_desc} 0 1
    #EOF
    #    # Buckets
    #    for ((i=0;i<(middle_ceil);i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #    # 2nd cloud
    #    remaining=$((elem-middle_ceil))
    #    cat << EOF >> "${file_name}"
    #2 ${middle_cloud_desc} ${remaining}
    #4 ${middle_vm1_desc} 1 0
    #5 ${middle_vm1_desc} 1 0
    #6 ${middle_vm1_desc} 0 0
    #7 ${middle_vm1_desc} 0 0
    #EOF
    #    # Buckets
    #    for ((i=middle_ceil;i<elem;i++)); do
    #      echo "${i} ${middle_bucket_desc}" >> "${file_name}"
    #    done
    #  done
}

# ### Main ###

#now=$(date +"%Y-%m-%dT%H:%M:%S")
now=$(date +"%Y%m%d%H%M%S")
echo "${now}"

create_cloud_files

#alphas[$((0 * 3 + 0))]=0.90
#alphas[$((0 * 3 + 1))]=0.05
#alphas[$((1 * 3 + 0))]=0.05
#alphas[$((0 * 3 + 2))]=0.05
#alphas[$((1 * 3 + 1))]=0.90
#alphas[$((1 * 3 + 2))]=0.05
#alphas[$((2 * 3 + 0))]=0.05
#alphas[$((2 * 3 + 1))]=0.05
#alphas[$((2 * 3 + 2))]=0.90
#alphas[$((3 * 3 + 0))]=0.30
#alphas[$((3 * 3 + 1))]=0.30
#alphas[$((3 * 3 + 2))]=0.40
#alphas[$((4 * 3 + 0))]=1.00
#alphas[$((4 * 3 + 1))]=0.00
#alphas[$((4 * 3 + 2))]=0.00

# For each time/cost/security experiment
#for ((i = 0; i < 5; i++)); do
#    alpha_time=${alphas[$((i * 3 + 0))]}
#    alpha_cost=${alphas[$((i * 3 + 1))]}
#    alpha_security=${alphas[$((i * 3 + 2))]}

	# For each cluster
home_dir=$(pwd)
regex="([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9a-zA-Z]+)_cloud.vcl"
#    for f in ${home_dir}/temp/clouds/*; do
#        echo "$f"

out_dir="${home_dir}/output/${now}"
log_dir="${home_dir}/log/${now}"
temp_dir="${home_dir}/temp/${now}"

mkdir -p "${out_dir}"
mkdir -p "${log_dir}"
mkdir -p "${temp_dir}"

#      if [[ $f =~ $regex ]]; then
#          scenario="${BASH_REMATCH[1]}"
#          number_cloud="${BASH_REMATCH[2]}"
#          number_vm="${BASH_REMATCH[3]}"
#          number_buckets="${BASH_REMATCH[4]}"
#          number_vm_req_cryptography="${BASH_REMATCH[5]}"
#          number_vm_req_confidentiality="${BASH_REMATCH[6]}"
#          cloud_type="${BASH_REMATCH[7]}"
#      else
#          echo "$f doesn't match" >&2 # this could get noisy if there are a lot of non-matching files
#      fi

pipenv run python script/run-batch.py \
    --instances-file="_instances.txt" \
    --algorithms-file="_algorithms.txt" \
    --number-of-iterations=100 \
    --allocation_experiments=4 \
    --repeat 10 \
    --output-path="${out_dir}" \
    --log-dir="${log_dir}" \
    --temp-dir="${temp_dir}"
#          --clouds-file="$f" \
#          --alpha-time=${alpha_time} \
#          --alpha-cost=${alpha_cost} \
#          --alpha-security=${alpha_security} \
#          --test-scenery="${scenario}" \
#          --number-cloud-sites="${number_cloud}" \
#          --number-vm="${number_vm}" \
#          --number-buckets="${number_buckets}" \
#          --number-vm-req-cryptography="${number_vm_req_cryptography}" \
#          --number-vm-req-confidentiality="${number_vm_req_confidentiality}" \
#          --cloud-type="${cloud_type}" \
#    done
#done
