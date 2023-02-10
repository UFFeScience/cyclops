import argparse
import os.path
import pathlib
import re
import subprocess
import sys

import shlex

from pathlib import Path

# Config path
app_path = "/home/luan/project/IVMP/"
instances_path = "/home/luan/project/IVMP/instances_new/"
output_path = "/home/luan/project/IVMP/output/gupta_conf4/"
output_file_name = "../output/wf_security_executions.csv"
average_output_file_name = "../output/wf_security.csv"

input_inst = ""

verbose = False
write_flag = True
combination = False


def exec_command(cmd):
    p = None
    try:
        p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
    except Exception as ex:
        print(ex)
    return p


def run_greedy_randomized_constructive_heuristic(instance: str,
                                                 task_and_files: str,
                                                 clouds: str,
                                                 conflict_graph: str,
                                                 alpha_time: float,
                                                 alpha_cost: float,
                                                 alpha_security: float,
                                                 number_of_iterations: int,
                                                 allocation_experiments: int,
                                                 repeat: int,
                                                 # average_output_file: str,
                                                 output_executions_file: str,
                                                 test_scenery: str,
                                                 number_vm: str,
                                                 number_cloud_sites: str,
                                                 number_buckets: str,
                                                 number_vm_req_cryptography: str,
                                                 number_vm_req_confidentiality: str,
                                                 cloud_type: str,
                                                 log_dir
                                                 ):
    print(f'greedy_randomized_constructive_heuristic: {task_and_files}')
    # exec_times = []
    # exec_makes-pan = []
    # exec_cost = []
    # exec_security = []
    # exec_objective_function = []
    for r in range(repeat):
        current_path = pathlib.Path().resolve()
        # print(log_dir)
        cloud_file = Path(clouds).stem
        actual_log_dir = log_dir + '/' + str(alpha_time) + '_' + str(alpha_cost) + '_' + str(alpha_security) + '/' \
                         + cloud_file + '/' + str(r) + '/' + instance + '/'
        os.makedirs(actual_log_dir)

        print('Log directory:', actual_log_dir)

        command_list = [f'{current_path}/bin/wf_security_greedy.x',
                        '--tasks_and_files ' + task_and_files,
                        '--cluster ' + clouds,
                        '--conflict_graph ' + conflict_graph,
                        '--algorithm greedy_randomized_constructive_heuristic',
                        '--alpha_time ' + str(alpha_time),
                        '--alpha_budget ' + str(alpha_cost),
                        '--alpha_security ' + str(alpha_security),
                        '--number_of_iteration ' + str(number_of_iterations),
                        '--number_of_allocation_experiments ' + str(allocation_experiments),
                        '--log_dir ' + actual_log_dir,
                        '--logbuflevel=-1'
                        ]
        command = " ".join(command_list)
        output, error = exec_command(command)
        regex = "^"
        regex += "([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)\n([0-9]*[.]?[0-9]*)"
        regex += "$"
        m = re.search(regex, error.decode(sys.stdout.encoding), re.M)
        # print(error.decode(sys.stdout.encoding))
        if hasattr(m, 'group'):
            # exec_makespan.append(float(m.group(1)))
            # exec_cost.append(float(m.group(2)))
            # exec_security.append(float(m.group(3)))
            # exec_objective_function.append(float(m.group(4)))
            # exec_times.append(float(m.group(5)))
            out_file = 'greedy_randomized_constructive_heuristic'
            out_file += f",{instance}"
            out_file += f',{test_scenery}'
            out_file += f',{alpha_time}'
            out_file += f',{alpha_cost}'
            out_file += f',{alpha_security}'
            out_file += f',{number_cloud_sites}'
            out_file += f',{number_vm}'
            out_file += f',{number_buckets}'
            out_file += f',{number_vm_req_cryptography}'
            out_file += f',{number_vm_req_confidentiality}'
            out_file += f',{cloud_type}'
            out_file += f',{str(number_of_iterations)}'
            out_file += f',{str(allocation_experiments)}'
            out_file += f',{float(m.group(4)):f}'
            out_file += f',{float(m.group(1)):f}'
            out_file += f',{float(m.group(2)):f}'
            out_file += f',{float(m.group(3)):f}'
            out_file += f',{float(m.group(5)):f}'
            out_file += f"\n"
            with open(output_executions_file, "a") as file:
                file.write(out_file)
        else:
            print(f'In the {r} iteration, program do not return any value. That means unfeasible solution.')
            out_file = 'greedy_randomized_constructive_heuristic'
            out_file += f",{instance}"
            out_file += f',{test_scenery}'
            out_file += f',{alpha_time}'
            out_file += f',{alpha_cost}'
            out_file += f',{alpha_security}'
            out_file += f',{number_cloud_sites}'
            out_file += f',{number_vm}'
            out_file += f',{number_buckets}'
            out_file += f',{number_vm_req_cryptography}'
            out_file += f',{number_vm_req_confidentiality}'
            out_file += f',{cloud_type}'
            out_file += f',{str(number_of_iterations)}'
            out_file += f',{str(allocation_experiments)}'
            out_file += f',{float("-1"):f}'
            out_file += f',{float("-1"):f}'
            out_file += f',{float("-1"):f}'
            out_file += f',{float("-1"):f}'
            out_file += f',{float("-1"):f}'
            out_file += f"\n"
            with open(output_executions_file, "a") as file:
                file.write(out_file)

    # with open(output_executions_file, 'a') as file:
    #     exec_times_str = ",".join(str(item) for item in exec_times)
    #     file.write(f'greedy_randomized_constructive_heuristic,{instance},{exec_times_str}\n')
    # if len(exec_makespan) > 0:
    #     avg_exec_makespan = numpy.mean(exec_makespan)
    #     std_exec_makespan = numpy.std(exec_makespan)
    #     avg_exec_cost = numpy.mean(exec_cost)
    #     std_exec_cost = numpy.std(exec_cost)
    #     avg_exec_security = numpy.mean(exec_security)
    #     std_exec_security = numpy.std(exec_security)
    #     avg_exec_objective_function = numpy.mean(exec_objective_function)
    #     std_exec_objective_function = numpy.std(exec_objective_function)
    #     avg_exec_times = numpy.mean(exec_times)
    #     std_exec_times = numpy.std(exec_times)
    # else:
    #     avg_exec_makespan = -1
    #     std_exec_makespan = -1
    #     avg_exec_cost = -1
    #     std_exec_cost = -1
    #     avg_exec_security = -1
    #     std_exec_security = -1
    #     avg_exec_objective_function = -1
    #     std_exec_objective_function = -1
    #     avg_exec_times = -1
    #     std_exec_times = -1
    # out_file += f"{workflow_name:80}"
    # out_file += f", {avg_exec_objective_function:12.5f}"
    # out_file += f", {avg_exec_makespan:12.5f}"
    # out_file += f", {avg_exec_cost:12.5f}"
    # out_file += f", {avg_exec_security:12.5f}"
    # out_file += f", {avg_exec_times:12.5f}"
    # out_file += f"\n"
    # out_file = 'greedy_randomized_constructive_heuristic'
    # out_file += f",{instance}"
    # out_file += test_scenery
    # out_file += alpha_time
    # out_file += alpha_cost
    # out_file += alpha_security
    # out_file += number_cloud_sites
    # out_file += number_vm
    # out_file += number_buckets
    # out_file += number_vm_req_criptography
    # out_file += number_vm_req_confidentiality
    # out_file += cloud_type
    # out_file += str(number_of_iterations)
    # out_file += str(allocation_experiments)
    # out_file += f",{avg_exec_objective_function:f}"
    # # out_file += f',{std_exec_objective_function:f}'
    # out_file += f',{avg_exec_makespan:f}'
    # # out_file += f',{std_exec_makespan:f}'
    # out_file += f',{avg_exec_cost:f}'
    # # out_file += f',{std_exec_cost:f}'
    # out_file += f",{avg_exec_security:f}"
    # # out_file += f',{std_exec_security:f}'
    # out_file += f",{avg_exec_times:f}"
    # # out_file += f',{std_exec_times:f}'
    # out_file += f"\n"
    # with open(average_output_file, "a") as file:
    #     file.write(out_file)


def run(inner_instance: str, task_and_files: str, clouds: str, conflict_graph: str, inner_algorithm: str,
        alpha_time: float, alpha_cost: float, alpha_security: float, number_of_iterations: int,
        allocation_experiments: int, repeat: int, output_executions_file: str, test_scenery: str, number_vm: str, \
        number_cloud_sites: str, number_buckets: str, number_vm_req_cryptography: str,
        number_vm_req_confidentiality: str, cloud_type: str, log_dir: str, temp_dir: str):
    for r in range(repeat):
        # print('')
        # print(log_dir)
        cloud_file = Path(clouds).stem
        actual_log_dir = log_dir + '/' + str(alpha_time) + '_' + str(alpha_cost) + '_' + str(alpha_security) + '/' \
                         + cloud_file + '/' + str(r) + '/' + inner_instance + '/'
        # print("Directory")
        # print(actual_log_dir)
        os.makedirs(actual_log_dir)
        current_path = pathlib.Path().resolve()
        command_list = [f'{current_path}/bin/wf_security_greedy.x', '--tasks_and_files ' + task_and_files, '--cluster '
                        + clouds, '--conflict_graph ' + conflict_graph, '--algorithm ' + inner_algorithm,
                        '--alpha_time ' + str(alpha_time), '--alpha_budget ' + str(alpha_cost), '--alpha_security '
                        + str(alpha_security), '--number_of_iteration ' + str(number_of_iterations),
                        '--number_of_allocation_experiments ' + str(allocation_experiments), '--log_dir '
                        + actual_log_dir, '--logbuflevel=-1']
        if inner_algorithm == 'cplex':
            cplex_output_dir = temp_dir + '/' + str(alpha_time) + '_' + str(alpha_cost) + '_' + str(alpha_security) \
                               + '/' + cloud_file + '/' + str(r) + '/' + inner_instance
            cplex_output_file = cplex_output_dir + '/output.lp'
            Path(cplex_output_dir).mkdir(parents=True, exist_ok=True)
            if not os.path.isfile(cplex_output_file):
                open(cplex_output_file, 'w')
            command_list.append('--cplex_output_file ' + cplex_output_file)
        command = " ".join(command_list)
        output, error = exec_command(command)
        regex = "^"
        regex += "([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)\n([0-9]*[.]?[0-9]*)"
        regex += "$"
        m = re.search(regex, error.decode(sys.stdout.encoding), re.M)
        print(output.decode(sys.stdout.encoding))
        print(error.decode(sys.stdout.encoding))
        if hasattr(m, 'group'):
            of = f',{float(m.group(4)):f}'
            makespan = f',{float(m.group(1)):f}'
            cost = f',{float(m.group(2)):f}'
            security = f',{float(m.group(3)):f}'
            time = f',{float(m.group(5)):f}'
        else:
            print(f'In the {r} iteration, program do not return any value. That means unfeasible solution.')
            of = f',{float("-1"):f}'
            makespan = f',{float("-1"):f}'
            cost = f',{float("-1"):f}'
            security = f',{float("-1"):f}'
            time = f',{float("-1"):f}'

        out_file = inner_algorithm
        out_file += f',{inner_instance}'
        out_file += f',{test_scenery}'
        out_file += f',{alpha_time}'
        out_file += f',{alpha_cost}'
        out_file += f',{alpha_security}'
        out_file += f',{number_cloud_sites}'
        out_file += f',{number_vm}'
        out_file += f',{number_buckets}'
        out_file += f',{number_vm_req_cryptography}'
        out_file += f',{number_vm_req_confidentiality}'
        out_file += f',{cloud_type}'
        out_file += f',{str(number_of_iterations)}'
        out_file += f',{str(allocation_experiments)}'
        out_file += f',{of}'
        out_file += f',{makespan}'
        out_file += f',{cost}'
        out_file += f',{security}'
        out_file += f',{time}'
        out_file += f"\n"
        with open(output_executions_file, "a") as file:
            file.write(out_file)


if __name__ == "__main__":
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--instance-parameters', nargs='+')
    parser.add_argument('--instances-file',
                        default='instances_desenv.txt')
    parser.add_argument('--algorithms-file',
                        default='algorithms.txt')
    parser.add_argument('--clouds-file')
    parser.add_argument('--alpha-time',
                        type=float,
                        default=0.3)
    parser.add_argument('--alpha-cost',
                        type=float,
                        default=0.3)
    parser.add_argument('--alpha-security',
                        type=float,
                        default=0.4)
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
    parser.add_argument('--grch',
                        help='Execute Greedy Randomized Constructive Heuristic',
                        action='store_true')
    parser.add_argument('--grasp',
                        help='Execute Grasp Meta-heuristic',
                        action='store_true')
    parser.add_argument('--cplex',
                        help='Execute CPLEX exact solution',
                        action='store_true')
    parser.add_argument('--heft',
                        help='Execute HEFT',
                        action='store_true')
    parser.add_argument('--output-path', default="./output")
    parser.add_argument('--verbose', help='Print more data', action='store_true')
    parser.add_argument('--test-scenery')
    parser.add_argument('--number-vm')
    parser.add_argument('--number-cloud-sites')
    parser.add_argument('--number-buckets')
    parser.add_argument('--number-vm-req-cryptography')
    parser.add_argument('--number-vm-req-confidentiality')
    parser.add_argument('--cloud-type')
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
        header = f'Algorithm'
        header += f',Instance'
        header += f',Scenario'
        header += f',Time alpha'
        header += f',Cost alpha'
        header += f',Security alpha'
        header += f',#Clouds'
        header += f',#VMs,'
        header += f',#Buckets'
        header += f',#Cryptografy requirements'
        header += f',#Confidentiality requirements'
        header += f',Cloud type (homogeneous/heterogeneous)'
        header += f',#Iterations'
        header += f',#Allocation experiments'
        header += f',Object Function'
        header += f',Makespan'
        header += f',Cost'
        header += f',Security'
        header += f',Time'
        header += f'\n'
        with open(output_execution_file, 'w') as oef_file:
            oef_file.write(header)

    # Run the selected heuristic(s) and meta-heuristic(s)
    for algorithm in algorithms:
        algorithm_log_dir = args.log_dir + '/' + algorithm
        print(algorithm_log_dir)

        algorithm_temp_dir = args.temp_dir + '/' + algorithm
        print(algorithm_log_dir)

        for instance in instances:
            cur_path = pathlib.Path().resolve()  # current path
            clouds_file = args.clouds_file if args.clouds_file is not None \
                else f'{cur_path}/input/clouds/cluster_' + instance + '.vcl'
            tasks_and_files_file_name = f'{cur_path}/input/tasks_and_files/' + instance + '.dag'
            conflict_graph_file_name = f'{cur_path}/input/conflict_graph/' + instance + '.scg'

            run(instance, tasks_and_files_file_name, clouds_file, conflict_graph_file_name, algorithm,
                args.alpha_time, args.alpha_cost, args.alpha_security, args.number_of_iterations,
                args.allocation_experiments, args.repeat, output_execution_file, args.test_scenery, args.number_vm,
                args.number_cloud_sites, args.number_buckets, args.number_vm_req_cryptography,
                args.number_vm_req_confidentiality, args.cloud_type, algorithm_log_dir, algorithm_temp_dir)

