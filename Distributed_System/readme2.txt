
CIS630, Spring 2014, Term Project (Part II), Due date: June 5, 2014

Please complete the following information, save this file and submit it along with your program

Your First and Last name: Wenyuan Yu
Your Student ID:  951379094

What programming language did you use to write your code? C++

Does your program compile on ix-trusty: Yes

How should we compile your program on ix-trusty (please provide short but specific commands): 
Step 1: (compile the program)
$ mpic++ RandomWalkMPI.cpp -o RandomWalkMPI
Step 2: (running program)
$ mpiexec -np 4 RandomWalkMPI fl_compact_part.4 fl_compact.tab 5   (for the file with 4 partition, the round number is 5)
or 
$ mpiexec -np 2 RandomWalkMPI fl_compact_part.2 fl_compact.tab 5   (for the file with 2 partition, the round number is 5)

Does your program run on ix-trusty: Yes

Does your program generate the correct results with 2 and 4 partitions on ix-trusty: Yes

Does your program have a limit for the number of nodes in the input graph? Yes
If yes, what is the limit on graph size that your program can handle?
MAXNODE=10,000,000
MAXEDGE=200,000,000

How long does it take for your program to read the input file on ix-trusty?
About 4 seconds.

How long does it take for your program (on average) to complete each round of processing on ix-trusty?
From 1-2 seconds.

-------------------------------------------------------------------------------------------------------------------------
Output Sample

$ mpiexec -np 4 RandomWalkMPI fl_compact_part.4 fl_compact.tab 5

The time to read input files to partition 1 = 4 seconds 

The time to read input files to partition 0 = 4 seconds 

The time to read input files to partition 3 = 4 seconds 

The time to read input files to partition 2 = 4 seconds 

--- time for round 1, partition 1 = 0 sec
--- time for round 1, partition 2 = 0 sec
--- time for round 1, partition 3 = 0 sec
--- time for round 1, partition 0 = 1 sec
Total time for round 1: 1 sec

--- time for round 2, partition 1 = 0 sec
--- time for round 2, partition 3 = 0 sec
--- time for round 2, partition 0 = 0 sec
--- time for round 2, partition 2 = 0 sec
Total time for round 2: 0 sec

--- time for round 3, partition 1 = 0 sec
--- time for round 3, partition 3 = 0 sec
--- time for round 3, partition 0 = 0 sec
--- time for round 3, partition 2 = 0 sec
Total time for round 3: 0 sec

--- time for round 4, partition 2 = 0 sec
--- time for round 4, partition 3 = 0 sec
--- time for round 4, partition 1 = 1 sec
--- time for round 4, partition 0 = 1 sec
Total time for round 4: 1 sec

--- time for round 5, partition 1 = 0 sec
--- time for round 5, partition 0 = 0 sec
--- time for round 5, partition 3 = 0 sec
--- time for round 5, partition 2 = 0 sec
Total time for round 5: 0 sec

-------------------------------------------------------------------------------------------------------------------------