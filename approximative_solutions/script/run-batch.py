import argparse
import time
import numpy
import multiprocessing
import subprocess
import shlex
import re
import sys

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


def read_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('--inst', nargs='+')
    parser.add_argument('--input_inst', default='instances.txt')
    parser.add_argument('--repeat', nargs='?', type=int)
    parser.add_argument('--gupta', help='Print more data', action='store_true')
    parser.add_argument('--gupta_comb', help='Print more data', action='store_true')
    parser.add_argument('--ils', help='Print more data', action='store_true')
    parser.add_argument('--write', help='write output in a file', action='store_true')
    parser.add_argument('--verbose', help='Print more data', action='store_true')
    parser.add_argument('--output_path', default=".")

    global verbose, write_flag, output_path, combination, input_inst

    instances = parser.parse_args().inst
    repeat = parser.parse_args().repeat
    gupta_flag = parser.parse_args().gupta
    ils_flag = parser.parse_args().ils

    write_flag = parser.parse_args().write

    verbose = parser.parse_args().verbose
    output_path = parser.parse_args().output_path
    combination = parser.parse_args().gupta_comb

    input_inst = parser.parse_args().input_inst

    # print verbose, write_flag, output_path

    if repeat is None:
        repeat = 1

    return instances, repeat, gupta_flag, ils_flag


def write_file(output, path, file_name):
    # print path + file_name
    with open(path + file_name, "w") as f:
        f.write(output)


def prepare_cmd(instance, parameters=[], algorithm='i'):
    return app_path + "interference " + instance + "  " + algorithm


def exec_command(cmd):
    p = None

    try:
        p = subprocess.Popen(shlex.split(cmd),
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE).communicate()

    except Exception as ex:
        print(ex.message)

    return p


def top_msg(msg):
    out = 20 * "#"
    out += "\n\t"+msg + "\n"
    out += 20 * "#"
    return out


def print_warning(msg):
    if verbose:
        print(msg)


def exec_dmer(repeat, i_file, i_cluster):
    # Defining the programs to run
    versions = []
    versions.append("greedy-randomized-constructive-heuristic")
    # versions.append("mpi-lb")
    # versions.append("mpi-pthread-lb")
    # versions.append("mpi-openmp-lb")
    # versions.append("mpi")
    # versions.append("mpi-pthread")
    # versions.append("mpi-openmp")
    exec_batch(versions,
               repeat,
               task_and_files=i_file,
               cluster=i_cluster,
               conflict_graph=i_file)


def exec_batch(versions,
               repeat,
               task_and_files="Sipht_100.xml",
               cluster="cluster2",
               conflict_graph="Sipht_100.xml",
               alpha_time=0.3,
               alpha_cost=0.3,
               alpha_security=0.4,
               number_of_iterations=100,
               allocation_experiments=4):
    out_file = ""
    for version in versions:
        # with open(output_file_name, "a") as f:
            # f.write(version + "\n")
        exec_times = []
        exec_makespan = []
        exec_cost = []
        exec_security = []
        exec_objective_function = []
        # exec_times_ttt = []
        # command = "/usr/bin/time -f '%e' ../shell/run-" + version + ".sh"
        command = "../shell/run-" + version + ".sh"
        command += " " + str(task_and_files) + ".dag"
        command += " " + str(cluster) + ".vcl"
        command += " " + str(conflict_graph) + ".scg"
        command += " " + str(alpha_time)
        command += " " + str(alpha_cost)
        command += " " + str(alpha_security)
        command += " " + str(number_of_iterations)
        command += " " + str(allocation_experiments)
        print(command)
        for i in range(repeat):
            # print version + " " + str(i)
            output, error = exec_command(command)
            print(output.decode(sys.stdout.encoding))
            print(error.decode(sys.stderr.encoding))
            # m = re.search('^[0-9]*[.][0-9]*$', error.decode(sys.stdout.encoding), re.M)
            # exec_times.append(float(m.group(0)))
            regex = "^"
            regex += "([0-9]*[.]?[0-9]*)"
            regex += " ([0-9]*[.]?[0-9]*)"
            regex += " ([0-9]*[.]?[0-9]*)"
            regex += " ([0-9]*[.]?[0-9]*)\n([0-9]*[.]?[0-9]*)"
            regex += "$"
            m = re.search(regex, error.decode(sys.stdout.encoding), re.M)
            # try:
            exec_makespan.append(float(m.group(1)))
            exec_cost.append(float(m.group(2)))
            exec_security.append(float(m.group(3)))
            exec_objective_function.append(float(m.group(4)))
            exec_times.append(float(m.group(5)))
            # except Exception as ex:
            #     print(ex.message)
            print(m.group(1), m.group(2), m.group(3), m.group(4), str(exec_times[i]))
            if (i == 0):
                with open(output_file_name, "a") as f:
                    f.write(version + str(task_and_files) + "," + str(exec_times[i]))
            else:
                with open(output_file_name, "a") as f:
                    f.write("," + str(exec_times[i]))
        with open(output_file_name, "a") as f:
            f.write("\n")
        avg_exec_makespan = numpy.mean(exec_makespan)
        std_exec_makespan = numpy.std(exec_makespan)
        avg_exec_cost = numpy.mean(exec_cost)
        std_exec_cost = numpy.std(exec_cost)
        avg_exec_security = numpy.mean(exec_security)
        std_exec_security = numpy.std(exec_security)
        avg_exec_objective_function = numpy.mean(exec_objective_function)
        std_exec_objective_function = numpy.std(exec_objective_function)
        avg_exec_times = numpy.mean(exec_times)
        std_exec_times = numpy.std(exec_times)
        print("Average of execution times: " + str(avg_exec_times))
        print("Standart deviation: " + str(std_exec_times))
        out_file += str(file_name)
        out_file += ", " + str(avg_exec_makespan)
        out_file += ", " + str(std_exec_makespan)
        out_file += ", " + str(avg_exec_cost)
        out_file += ", " + str(std_exec_cost)
        out_file += ", " + str(avg_exec_security)
        out_file += ", " + str(std_exec_security)
        out_file += ", " + str(avg_exec_objective_function)
        out_file += ", " + str(std_exec_objective_function)
        out_file += ", " + str(avg_exec_times)
        out_file += ", " + str(std_exec_times)
        out_file += "\n"
    with open(average_output_file_name, "a") as f:
        f.write(out_file)

# --- Main Program ---


versions = []
instances, repeat, gupta_flag, ils_flag = read_args()

# Cleaning files
with open(output_file_name, "w") as f:
    f.write("")
with open(average_output_file_name, "w") as f:
    f.write("")

# file_name = "3_toy_5_A"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

# file_name = "3_toy_5_B"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

# file_name = "3_toy_5_C"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

# file_name = "3_toy_10_A"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

# file_name = "3_toy_10_B"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

# file_name = "3_toy_10_C"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

# file_name = "3_toy_15_A"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

# file_name = "3_toy_15_B"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

# file_name = "3_toy_15_C"
# with open(output_file_name, "a") as f:
#     f.write(file_name + "\n")
# exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "CyberShake_30.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "CyberShake_50.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "CyberShake_100.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)


file_name = "Epigenomics_24.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "Epigenomics_46.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "Epigenomics_100.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)


file_name = "Inspiral_30.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "Inspiral_50.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "Inspiral_100.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)


file_name = "Montage_25.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "Montage_50.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "Montage_100.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)


file_name = "Sipht_30.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "Sipht_60.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)

file_name = "Sipht_100.xml"
with open(output_file_name, "a") as f:
    f.write(file_name + "\n")
exec_dmer(repeat, file_name, "cluster_" + file_name)
