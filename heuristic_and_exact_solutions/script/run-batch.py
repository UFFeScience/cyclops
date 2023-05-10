# TODO: Inserting documentation inside the methods

import argparse
import os.path
import pathlib
import re
import subprocess
import sys

import shlex

from pathlib import Path


def exec_command(cmd):
    p = None
    try:
        print('cmd:', cmd)
        p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
    except Exception as ex:
        print(ex)
    return p


def run(inner_instance: str, task_and_files: str, clouds: str, conflict_graph: str, inner_algorithm: str,
        alpha_time: float, alpha_cost: float, alpha_security: float, number_of_iterations: int,
        allocation_experiments: int, repeat: int, output_executions_file: str, test_scenery: str, number_vm: str,
        number_cloud_sites: str, number_buckets: str, number_vm_req_cryptography: str,
        number_vm_req_confidentiality: str, cloud_type: str, log_dir: str, temp_dir: str):
    for r in range(repeat):
        cloud_file = Path(clouds).stem
        actual_log_dir = log_dir + '/' + str(alpha_time) + '_' + str(alpha_cost) + '_' + str(alpha_security) + '/' \
            + cloud_file + '/' + str(r) + '/' + inner_instance + '/'
        os.makedirs(actual_log_dir)
        current_path = pathlib.Path().resolve()
        command_list = [f'{current_path}/bin/wf_security_greedy.x', '--tasks_and_files=' + task_and_files, '--cluster='
                        + clouds, '--conflict_graph=' + conflict_graph, '--algorithm=' + inner_algorithm,
                        '--alpha_time=' + str(alpha_time), '--alpha_budget=' + str(alpha_cost), '--alpha_security '
                        + str(alpha_security), '--number_of_iteration=' + str(number_of_iterations),
                        '--number_of_allocation_experiments=' + str(allocation_experiments), '--log_dir='
                        + actual_log_dir, '--logbuflevel=-1']
        if inner_algorithm == 'cplex':
            cplex_output_dir = temp_dir + '/' + str(alpha_time) + '_' + str(alpha_cost) + '_' + str(alpha_security) \
                               + '/' + cloud_file + '/' + str(r) + '/' + inner_instance
            cplex_output_file = cplex_output_dir + '/output.lp'
            Path(cplex_output_dir).mkdir(parents=True, exist_ok=True)
            if not os.path.isfile(cplex_output_file):
                open(cplex_output_file, 'w')
            command_list.append('--cplex_output_file=' + cplex_output_file)
        command = " ".join(command_list)
        # print('Command:', command)
        output, error = exec_command(command)
        regex = "^"
        regex += "([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)"
        regex += " ([0-9]*[.]?[0-9]*)\n([0-9]*[.]?[0-9]*)"
        regex += "$"
        m = re.search(regex, error.decode(sys.stdout.encoding), re.M)
        print('Print output:', output.decode(sys.stdout.encoding))
        print('Print error:', error.decode(sys.stdout.encoding))
        if hasattr(m, 'group'):
            of = float(m.group(4))
            makespan = float(m.group(1))
            cost = float(m.group(2))
            security = float(m.group(3))
            time = float(m.group(5))
        else:
            print(f'In the {r} iteration, program do not return any value. That means unfeasible solution.')
            of = float("-1")
            makespan = float("-1")
            cost = float("-1")
            security = float("-1")
            time = float("-1")

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
        out_file += f',{of:f}'
        out_file += f',{makespan:f}'
        out_file += f',{cost:f}'
        out_file += f',{security:f}'
        out_file += f',{time:f}'
        out_file += f"\n"
        with open(output_executions_file, "a") as file:
            file.write(out_file)


if __name__ == "__main__":
    # Parse arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--instance-parameters', nargs='+')
    parser.add_argument('--instances-file',
                        default='_instances_desenv.txt')
    parser.add_argument('--algorithms-file',
                        default='_algorithms_test.txt')
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
        algorithm_temp_dir = args.temp_dir + '/' + algorithm

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
