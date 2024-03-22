# TODO: Inserting documentation inside the methods

import argparse
import os.path
import pathlib
import re
import subprocess
# import sys
import time
import shlex
import traceback

from multiprocessing import Pool, Lock, Manager
from os import walk

from pathlib import Path

global_lock = Lock()
# global_iteration = 0
start_time = time.time()
total = 0


def progress_bar(valor):
    global total
    # print(valor.value)
    # with valor.get_lock():
    valor.value += 1
    progress = valor.value / total
    bar_length = 80
    elapsed_time = time.time() - start_time
    estimated_time = (elapsed_time / progress) - elapsed_time if progress > 0 else 0
    # Limpar a tela
    os.system('cls' if os.name == 'nt' else 'clear')
    # Imprimir a barra de progresso
    bar = '|' + '#' * int(progress * bar_length) + '-' * (bar_length - int(progress * bar_length)) + '|'
    print(f'Progresso: {bar} {progress * 100:.2f}%', flush=True)
    # Imprimir estimativa do tempo restante
    print(f'Tempo estimado restante: {estimated_time:.2f} segundos', flush=True)

    # Converter o tempo restante em minutos e horas
    estimated_minutes = estimated_time / 60
    estimated_hours = estimated_minutes / 60
    print(f'Tempo estimado restante: {estimated_minutes:.2f} minutos', flush=True)
    print(f'Tempo estimado restante: {estimated_hours:.2f} horas', flush=True)

    # Imprimir tempo decorrido em segundos, minutos e horas
    print(f'Tempo decorrido: {elapsed_time:.2f} segundos', flush=True)
    elapsed_minutes = elapsed_time / 60
    elapsed_hours = elapsed_minutes / 60
    print(f'Tempo decorrido: {elapsed_minutes:.2f} minutos', flush=True)
    print(f'Tempo decorrido: {elapsed_hours:.2f} horas', flush=True)


def exec_command(cmd):
    p_result = None
    try:
        # print('cmd:', cmd)
        p_result = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
    except Exception as ex:
        print(ex)
    return p_result


def run(inner_instance: str, task_and_files: str, clouds: str, conflict_graph: str, inner_algorithm: str,
        alpha_time: float, alpha_cost: float, alpha_security: float, alpha_restricted_candidate_list: float,
        number_of_iterations: int, allocation_experiments: int, output_executions_file: str,
        test_scenery: str, number_vm: str, number_cloud_sites: str, number_buckets: str,
        number_vm_req_cryptography: str, number_vm_req_confidentiality: str, cloud_type: str, log_dir: str,
        temp_dir: str, test_number: int, valor_compartilhado):
    try:
        cloud_file = Path(clouds).stem
        # actual_log_dir = log_dir + '/' + str(alpha_time) + '_' + str(alpha_cost) + '_' + str(alpha_security) + '/' \
        #                  + cloud_file + '/' + str(r) + '/' + inner_instance + '/'
        actual_log_dir = f'{log_dir}/{alpha_time}_{alpha_cost}_{alpha_security}_{alpha_restricted_candidate_list}/' \
                         f'{cloud_file}/{test_number}/{inner_instance}/'
        os.makedirs(actual_log_dir)
        current_path = pathlib.Path().resolve()
        command_list = [f'{current_path}/bin/wf_security_greedy.x',
                        f'--tasks_and_files={task_and_files}',
                        f'--cluster={clouds}',
                        f'--conflict_graph={conflict_graph}',
                        f'--algorithm={inner_algorithm}',
                        f'--alpha_time={alpha_time}',
                        f'--alpha_budget={alpha_cost}',
                        f'--alpha_security={alpha_security}',
                        f'--alpha_restrict_candidate_list={alpha_restricted_candidate_list}',
                        f'--number_of_iteration={number_of_iterations}',
                        f'--number_of_allocation_experiments={allocation_experiments}',
                        f'--log_dir={actual_log_dir}',
                        f'--logbuflevel=-1']
        if inner_algorithm == 'cplex':
            cplex_output_dir = temp_dir + '/' + str(alpha_time) + '_' + str(alpha_cost) + '_' + str(alpha_security) \
                               + '/' + cloud_file + '/' + str(test_number) + '/' + inner_instance
            cplex_output_file = cplex_output_dir + '/output.lp'
            Path(cplex_output_dir).mkdir(parents=True, exist_ok=True)
            if not os.path.isfile(cplex_output_file):
                open(cplex_output_file, 'w')
            command_list.append('--cplex_output_file=' + cplex_output_file)
        command = " ".join(command_list)
        # print('Command:', command)
        stdout, stderr = exec_command(command)
        # print('Print output:', stdout.decode())
        # print('Print error:', stderr.decode())
        # Decodificar a saída em texto
        lines = stdout.decode().splitlines()
        line = ''
        # Retorna a última line da saída
        if lines:
            line = lines[-1]
        regex = "^"
        regex += "([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        regex += " ?([0-9]*[.]?[0-9]*)"
        # regex += "$"
        # m = re.search(regex, error.decode(sys.stdout.encoding), re.M)
        m = re.search(regex, line, re.M)
        of = float("-1")
        makespan = float("-1")
        cost = float("-1")
        security = float("-1")
        execution_time = float("-1")
        number_of_iterations = float("-1")
        best_solution_iteration = float("-1")
        best_solution_time = float("-1")
        lsn_time_1 = float("-1")
        lsn_noi_1 = float("-1")
        lsn_time_2 = float("-1")
        lsn_noi_2 = float("-1")
        lsn_time_3 = float("-1")
        lsn_noi_3 = float("-1")
        if hasattr(m, 'group'):
            of = float(m.group(1))
            makespan = float(m.group(2))
            cost = float(m.group(3))
            security = float(m.group(4))
            execution_time = float(m.group(5))
            number_of_iterations = int(m.group(6))
            best_solution_iteration = int(m.group(7))
            best_solution_time = float(m.group(8))
            if "grasp" in inner_algorithm:
                lsn_time_1 = float(m.group(9))
                lsn_noi_1 = int(m.group(10))
                lsn_time_2 = float(m.group(1))
                lsn_noi_2 = int(m.group(12))
                lsn_time_3 = float(m.group(13))
                lsn_noi_3 = int(m.group(14))
        else:
            print(f'In the {test_number} iteration, program do not return any value. That means unfeasible solution.')
            # of = float("-1")
            # makespan = float("-1")
            # cost = float("-1")
            # security = float("-1")
            # execution_time = float("-1")
            # number_of_iterations = float("-1")
            # best_solution_iteration = float("-1")
            # best_solution_time = float("-1")
            # lsn_time_1 = float("-1")
            # lsn_noi_1 = float("-1")
            # lsn_time_2 = float("-1")
            # lsn_noi_2 = float("-1")
            # lsn_time_3 = float("-1")
            # lsn_noi_3 = float("-1")
        out_file = inner_algorithm
        out_file += f',{inner_instance}'
        out_file += f',{test_scenery}'
        out_file += f',{alpha_time}'
        out_file += f',{alpha_cost}'
        out_file += f',{alpha_security}'
        out_file += f',{alpha_restricted_candidate_list}'
        out_file += f',{number_cloud_sites}'
        out_file += f',{number_vm}'
        out_file += f',{number_buckets}'
        out_file += f',{number_vm_req_cryptography}'
        out_file += f',{number_vm_req_confidentiality}'
        out_file += f',{cloud_type}'
        out_file += f',{number_of_iterations}'
        out_file += f',{allocation_experiments}'
        out_file += f',{of:f}'
        out_file += f',{makespan:f}'
        out_file += f',{cost:f}'
        out_file += f',{security:f}'
        out_file += f',{execution_time:f}'
        out_file += f',{best_solution_iteration}'
        out_file += f',{best_solution_time:f}'
        out_file += f',{lsn_time_1:f}'
        out_file += f',{lsn_noi_1}'
        out_file += f',{lsn_time_2:f}'
        out_file += f',{lsn_noi_2}'
        out_file += f',{lsn_time_3:f}'
        out_file += f',{lsn_noi_3}'
        out_file += f"\n"
        global_lock.acquire()
        progress_bar(valor_compartilhado)
        with open(output_executions_file, "a") as file:
            file.write(out_file)
        global_lock.release()
    except BaseException:
        print("A-ha!")
        traceback.print_exc()
        exit(1)


if __name__ == "__main__":
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--instance-parameters', nargs='+')
    parser.add_argument('--instances-file',
                        default='_instances_desenv.txt')
    parser.add_argument('--algorithms-file',
                        default='_algorithms_test.txt')
    # parser.add_argument('--clouds-file')
    # parser.add_argument('--alpha-time',
    #                     type=float,
    #                     default=0.3)
    # parser.add_argument('--alpha-cost',
    #                     type=float,
    #                     default=0.3)
    # parser.add_argument('--alpha-security',
    #                     type=float,
    #                     default=0.4)
    parser.add_argument('--number-of-iterations',
                        type=int,
                        default=100)
    parser.add_argument('--allocation_experiments',
                        type=int,
                        default=4)
    parser.add_argument('--repeat',
                        nargs='?',
                        type=int,
                        default=1)
    # parser.add_argument('--grch',
    #                     help='Execute Greedy Randomized Constructive Heuristic',
    #                     action='store_true')
    # parser.add_argument('--grasp',
    #                     help='Execute Grasp Meta-heuristic',
    #                     action='store_true')
    # parser.add_argument('--cplex',
    #                     help='Execute CPLEX exact solution',
    #                     action='store_true')
    # parser.add_argument('--heft',
    #                     help='Execute HEFT',
    #                     action='store_true')
    parser.add_argument('--output-path', default="./output")
    parser.add_argument('--verbose', help='Print more data', action='store_true')
    # parser.add_argument('--test-scenery')
    # parser.add_argument('--number-vm')
    # parser.add_argument('--number-cloud-sites')
    # parser.add_argument('--number-buckets')
    # parser.add_argument('--number-vm-req-cryptography')
    # parser.add_argument('--number-vm-req-confidentiality')
    # parser.add_argument('--cloud-type')
    parser.add_argument('--log-dir')
    parser.add_argument('--temp-dir')
    args = parser.parse_args()

    # Get Scientific Workflow instances
    # instances = []
    with open(args.instances_file) as instances_file:
        instances = instances_file.read().splitlines()

    # Get Scientific Workflow algorithm for execution
    # instances = []
    with open(args.algorithms_file) as algorithms_file:
        algorithms = algorithms_file.read().splitlines()

    # Clear the output files and set the header
    output_execution_file = f'{args.output_path}/wf_security_executions.csv'

    if not os.path.isfile(output_execution_file):
        header = f'algorithm'
        header += f',instance'
        header += f',scenario'
        header += f',alpha_time'
        header += f',alpha_cost'
        header += f',alpha_security'
        header += f',alpha_restricted_candidate_list'
        header += f',num_clouds'
        header += f',num_vm'
        header += f',num_buckets'
        header += f',num_crypt_requirements'
        header += f',num_conf_requirements'
        header += f',cloud_type'
        header += f',num_iterations'
        header += f',num_allocation_experiments'
        header += f',object_function'
        header += f',makespan'
        header += f',cost'
        header += f',security'
        header += f',time'
        header += f',best_solution_iteration'
        header += f',best_solution_time'
        header += f',lsn_time_1'
        header += f',lsn_noi_1'
        header += f',lsn_time_2'
        header += f',lsn_noi_2'
        header += f',lsn_time_3'
        header += f',lsn_noi_3'
        header += f'\n'
        with open(output_execution_file, 'w') as oef_file:
            oef_file.write(header)

    alphas = (
        (0.90, 0.05, 0.05, 0.3),
        (0.05, 0.90, 0.05, 0.3),
        (0.05, 0.05, 0.90, 0.3),
        (0.90, 0.05, 0.05, 0.5),
        (0.05, 0.90, 0.05, 0.5),
        (0.05, 0.05, 0.90, 0.5),
        (0.90, 0.05, 0.05, 0.7),
        (0.05, 0.90, 0.05, 0.7),
        (0.05, 0.05, 0.90, 0.7)
    )

    # get the current working directory
    current_working_directory = os.getcwd()

    # print output to the console
    # print(current_working_directory)

    f = []
    filenames = []
    subdirectory = '/temp/clouds/'
    clouds_file_dir = current_working_directory + subdirectory
    for (dir_path, dir_names, filenames) in walk(clouds_file_dir):
        f.extend(filenames)
        break
    # print(filenames)
    # Criar o valor compartilhado
    # print('antes')
    # Criar um Manager para gerenciar o objeto compartilhado
    manager = Manager()
    my_valor_compartilhado = manager.Value('i', 0)
    # print('depois')
    total = len(filenames) * len(alphas) * len(algorithms) * len(instances) * args.repeat
    pool = Pool(processes=8)
    p = re.compile('([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9]+)_([0-9a-zA-Z]+)_cloud.vcl')
    contador = 0
    for filename in filenames:
        m_filename = p.match(filename)
        if m_filename:
            scenario = m_filename.group(1)
            number_of_cloud = m_filename.group(2)
            number_of_vm = m_filename.group(3)
            number_of_bucket = m_filename.group(4)
            number_of_vm_req_cryptography = m_filename.group(5)
            number_of_vm_req_confidentiality = m_filename.group(6)
            my_cloud_type = m_filename.group(7)
            # print(scenario, number_of_cloud, number_of_vm, number_of_bucket, number_of_vm_req_cryptography,
            #       number_of_vm_req_confidentiality, cloud_type)
            for alpha in alphas:
                my_alpha_time, my_alpha_cost, my_alpha_security, my_alpha_rcl = alpha
                # Run the selected heuristic(s) and meta-heuristic(s)
                for algorithm in algorithms:
                    algorithm_log_dir = args.log_dir + '/' + algorithm
                    algorithm_temp_dir = args.temp_dir + '/' + algorithm
                    for instance in instances:
                        for my_test_number in range(args.repeat):
                            cur_path = pathlib.Path().resolve()  # current path
                            full_file_name = clouds_file_dir + filename
                            clouds_file = full_file_name if full_file_name is not None \
                                else f'{cur_path}/input/clouds/cluster_' + instance + '.vcl'
                            tasks_and_files_file_name = f'{cur_path}/input/tasks_and_files/' + instance + '.dag'
                            conflict_graph_file_name = f'{cur_path}/input/conflict_graph/' + instance + '.scg'
                            # print('chamada')
                            pool.apply_async(run, args=(instance, tasks_and_files_file_name, clouds_file,
                                                        conflict_graph_file_name, algorithm, my_alpha_time,
                                                        my_alpha_cost, my_alpha_security, my_alpha_rcl,
                                                        args.number_of_iterations, args.allocation_experiments,
                                                        output_execution_file, scenario, number_of_cloud, number_of_vm,
                                                        number_of_bucket, number_of_vm_req_cryptography,
                                                        number_of_vm_req_confidentiality, my_cloud_type,
                                                        algorithm_log_dir, algorithm_temp_dir, my_test_number,
                                                        my_valor_compartilhado))
                            contador += 1
        else:
            exit('Something wrong with the file name pattern')
    # close the process pool
    pool.close()
    # wait for all tasks to finish
    pool.join()
    # print(contador, total, my_valor_compartilhado)
    # print(len(filenames), len(alphas), len(algorithms), len(instances), args.repeat)
