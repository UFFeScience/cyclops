2 2

55 Amazon-EC2 1 30 4 3
0 m3.medium 1.53 80 4.0 0.1
1 m3.large 0.77 120 9.0 0.1
2 m3.xlarge 0.38 160 10.0 0.1
3 m3.2xlarge 0.19 200 10.0 0.1
10 100TB 3 100 50TB 0.023 450TB 0.022 500TB 0.021
11 100TB 3 100 50TB 0.023 450TB 0.022 500TB 0.021
12 100TB 3 100 50TB 0.023 450TB 0.022 500TB 0.021
56 Amazon-EC2 1 30 4
0 m3.medium 1.53 80 4.0 0.1
1 m3.large 0.77 120 9.0 0.1
2 m3.xlarge 0.38 160 10.0 0.1
3 m3.2xlarge 0.19 200 10.0 0.1
13 100TB 3 50TB 0.023 450TB 0.022 500TB 0.021
14 100TB 3 50TB 0.023 450TB 0.022 500TB 0.021
15 100TB 3 50TB 0.023 450TB 0.022 500TB 0.021

<number of provider> <n# requerimentos>

<id-provider> <provider name> <period - hr> <max vms> <number of vms> <number of buckets>
<id-vm> <type> <slowdown> <storage GB> <bandwidth MBps> <cost $> <requirement-1> <requirement-2>
<id-bucket> <capacidade> <numero de intervalos> [<limite superior> <valor contratado por intervalo>] <requirement-1> <requirement-2 (pode assumir 0 1 2{so bucket})>

ex. 
<limite superior> <valor contratado por intervalo> <limite superior> <valor contratado por intervalo>
50TB 0.023 450TB 0.022 500TB 0.021
