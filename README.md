# CYCLOPS

CYCLOPS is a scheduling approach designed to execute cloud-based workflows while maintaining confidentiality constraints. The architecture of the CYCLOPS approach, presented below, consists of three core
modules: (i) the conflict graph generator, (ii) the scheduling heuristic, and (iii) the workflow system scheduler. These modules work together through nine steps, each described in detail below.

![image](https://github.com/user-attachments/assets/7ce56bdf-5ed1-451d-9039-6c5e910b0340)

The CYCLOPS execution process begins by receiving a workflow specification as a DAG. This workflow specification is then provided as input to the Conflict Graph Generator module, which analyzes the workflow’s structure to construct a conflict graph that encapsulates confidentiality requirements among data files. By evaluating the workflow specification, the Conflict Graph Generator
identifies hard and soft conflicts between data files, ensuring that all confidentiality constraints are represented. It is important to note that this component must be adapted to reflect the specific confidentiality constraints required by each experiment or user. 

Once the Conflict Graph Generator produces the conflict graph (Step 2), CYCLOPS is prepared to generate a scheduling plan tailored to the workflow’s specific requirements. The Scheduling Heuristic module is then activated and proceeds to analyze four critical inputs: (i) the characteristics and configurations of the cloud environment (Step 3), (ii) the detailed workflow specification, including activations and data dependencies (Step 4), (iii) the previously generated conflict graph that outlines potential conflicts among data files (Step 5), and (iv) any additional privacy and confidentiality requirements relevant to data processing (Step 6).

With these inputs, the Scheduling Heuristic module generates a detailed scheduling plan (Step 7), specifying the allocation of activations to VMs and the exact sequence in which these activations should be executed. This scheduling plan is further refined to consider data dispersion, which dictates the storage locations for data files to ensure that privacy and security requirements are upheld across cloud environments.

Once the scheduling plan is generated, it is submitted to the workflow system’s scheduler (Step 8), who executes the workflow according to the specified plan within the cloud environment (Step 9). It is important to note that the workflow system’s scheduler must either be adapted to accept this custom scheduling plan directly or the scheduling plan must be translated into a compatible format expected by the workflow system. 
