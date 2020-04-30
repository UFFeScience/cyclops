1 2

55 Amazon-EC2 1 30 4 44
0 m3.medium 1.53 80 4.0 0.02 1 1
1 m3.large 0.77 120 9.0 0.09 1 1
2 m3.xlarge 0.38 160 10.0 0.16 1 1
3 m3.2xlarge 0.19 200 10.0 0.33 1 1
11 Standard 51200 25.0 0.023 1 1
12 Standard 51200 25.0 0.023 1 1
13 Standard 51200 25.0 0.023 1 1
14 Standard 51200 25.0 0.023 1 1
15 Standard 51200 25.0 0.023 1 1
16 Standard 51200 25.0 0.023 1 1
17 Standard 51200 25.0 0.023 1 1
18 Standard 51200 25.0 0.023 1 1
19 Standard 51200 25.0 0.023 1 1
20 Standard 51200 25.0 0.023 1 1
21 Standard 51200 25.0 0.023 1 1
22 Standard 51200 25.0 0.023 1 1
23 Standard 51200 25.0 0.023 1 1
24 Standard 51200 25.0 0.023 1 1
25 Standard 51200 25.0 0.023 1 1
26 Standard 51200 25.0 0.023 1 1
27 Standard 51200 25.0 0.023 1 1
28 Standard 51200 25.0 0.023 1 1
29 Standard 51200 25.0 0.023 1 1
30 Standard 51200 25.0 0.023 1 1
31 Standard 51200 25.0 0.023 1 1
32 Standard 51200 25.0 0.023 1 1
33 Standard 51200 25.0 0.023 1 1
34 Standard 51200 25.0 0.023 1 1
35 Standard 51200 25.0 0.023 1 1
36 Standard 51200 25.0 0.023 1 1
37 Standard 51200 25.0 0.023 1 1
38 Standard 51200 25.0 0.023 1 1
39 Standard 51200 25.0 0.023 1 1
40 Standard 51200 25.0 0.023 1 1
41 Standard 51200 25.0 0.023 1 1
42 Standard 51200 25.0 0.023 1 1
43 Standard 51200 25.0 0.023 1 1
44 Standard 51200 25.0 0.023 1 1
45 Standard 51200 25.0 0.023 1 1
46 Standard 51200 25.0 0.023 1 1
47 Standard 51200 25.0 0.023 1 1
48 Standard 51200 25.0 0.023 1 1
49 Standard 51200 25.0 0.023 1 1
50 Standard 51200 25.0 0.023 1 1
51 Standard 51200 25.0 0.023 1 1
52 Standard 51200 25.0 0.023 1 1
53 Standard 51200 25.0 0.023 1 1
54 Standard 51200 25.0 0.023 1 1

<number of provider> <n# requerimentos>

<id-provider> <provider name> <period - hr> <max vms> <number of vms> <number of buckets>
<id-vm> <type> <slowdown> <storage GB> <bandwidth MBps> <cost $> <requirement-1> <requirement-2>
<id-bucket> <type> <storage GB> <bandwidth MBps> <cost> <requirement-1> <requirement-2 (pode assumir 0 1 2{so bucket})>

<id-bucket> <capacidade> <cost> <numero de intervalos> [<limite superior> <valor contratado por intervalo>] <requirement-1> <requirement-2 (pode assumir 0 1 2{so bucket})>

ex.
<limite superior> <valor contratado por intervalo> <limite superior> <valor contratado por intervalo>
50TB 0.023 450TB 0.022 500TB 0.021
