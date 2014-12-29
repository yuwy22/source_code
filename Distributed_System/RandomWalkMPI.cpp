// CIS630 Project Part 2
// Wenyuan Yu on June 3, 2014
// Email: wyu@uoregon.edu
// To compile it, use: 
// mpic++ RandomWalkMPI.cpp -o RandomWalkMPI
// followed by: 
// mpiexec -np 2 RandomWalkMPI fl_compact_part.2 fl_compact.tab 5 (If numPartitions is 2, the last digit is the round number)
// or 
// mpiexec -np 4 RandomWalkMPI fl_compact_part.4 fl_compact.tab 5 (If numPartitions is 4, the last digit is the round number)
//

#include <ctime>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>\

using namespace std;
MPI_Request reqs[4];
MPI_Status stats[2];

int tage = 1;

#define MAXNODE 10000000
#define MAXEDGE 200000000

int rank;
int number_of_tasks;
int node_Num = 0;
int edge_Num = 0;

int partition[MAXNODE][2];
int edges[MAXEDGE][2];
double  *x[20];
double  *y;
vector<double> * credits;
//char ** lines;

// read the partition files 
void readPartition(const char * partition_file)
{
    FILE * readfile = fopen(partition_file, "r");
    
    if(!readfile){
        printf("can't open file %s\n", partition_file);
        return;
    }
    
    
    int node_id,node_degree,partition_num;
    
    
    while(fscanf(readfile, "%d %d %d", &node_id,&node_degree,&partition_num) != EOF)
    {
        node_Num++;
        
        partition[node_id][0] = node_degree;
        partition[node_id][1] = partition_num;
    }
    
    
    
}

// read the file about edges. 
void readEdge(const char * edge_file)
{
    FILE * file = fopen(edge_file, "r");
    
    if(!file){
        printf("can't open file %s\n", edge_file);
        return;
    }
    
    
    int first_node, second_node;
     
	 // read the two nodes of each edge from the file.
    while(fscanf(file, "%d %d", &first_node,&second_node) != EOF)
    {
        edge_Num ++;
        
        edges[edge_Num][0] = first_node;
        edges[edge_Num][1] = second_node;
    }
          
}


void storeResult()
{
    int count=0;
    for(int i=1;i<=node_Num;i++){
        
        if(partition[i][1]==rank){
                   
            credits[count].push_back(x[rank][i]);
            
            count++;
        }
    }

    
}

// print the credits of nodes in a particular partition to current working directory for all the rounds. 
// There will be several output files, the number is same as the value of "rank"
void output_result(int round){
    char fileName[30];
    
	// the file name of partition at rank
    sprintf(fileName, "outputpartition%d.out",rank);
        
    FILE * f=fopen(fileName,"w");
    if(!f){
        printf("can't open output file %s\n", fileName);
        return;
    }
	
    int count=0;
	
	// print the credit to output file based by the value of rank
    for(int i = 1;i <= node_Num; i++){
        
        if(partition[i][1]==rank){
            fprintf(f,"%d\t%d", i,partition[i][0]);
            
            for(int k = 0;k <= round; k++){
                fprintf(f,"\t%.6f", credits[count][k]);
            }
            
            fprintf(f, "\n");
            
            count++;
        }
    }

    
}

//compute the credit for each round by MPI communication
void compute_credit(int round)
{
 
    credits = new vector<double>[node_Num+1];
    
	// develop two dimensions arrays 
    for(int i = 0; i < number_of_tasks; i++)
    {
        x[i] = new double[node_Num+1];
    }

    
    y = new double[node_Num+1];
    
    // initialize the values to one.
    for(int i = 1; i <= node_Num; i++)
    {
        x[rank][i] = 1;
        y[i] = 0;
    }
	
    int u,v;
    
    double * t;
    
    storeResult();
    
	// Blocks until all processes in the communicator have reached this routine
     MPI_Barrier(MPI_COMM_WORLD);
    
    clock_t roundStart;
    clock_t roundEnd;
    for(int i = 1; i <= round; i++)
    {
        // start of round i
		roundStart = clock();
		
        // printf("current rank is %d \n", rank);
        for(int k = 0; k < number_of_tasks; k++){
            if(k != rank){
                MPI_Isend(x[rank], node_Num+1, MPI_DOUBLE, k, i, MPI_COMM_WORLD, &reqs[0]);
            }
        }

        for(int k = 0;k < number_of_tasks; k++){

            if(k != rank){
                MPI_Recv(x[k], node_Num+1, MPI_DOUBLE, k, i, MPI_COMM_WORLD, &stats[0]);
            }
        }

        
        std::fill(y, y+node_Num+1, 0);
        
		//
        for(int j = 1; j <= edge_Num; j++)
        {            
            u = edges[j][0];
            v = edges[j][1];
                     
            if(partition[u][1] == rank)
                 y[u] += x[partition[v][1]][v] / partition[v][0];  //     partition[node_id][0] = node_degree; partition[node_id][1] = partition_num;

            if(partition[v][1] == rank)

                y[v] += x[partition[u][1]][u] / partition[u][0];

        }
              
        t = x[rank];
        x[rank] = y;
        
        y = t;
        
         storeResult();
		 
        // end of round i
        roundEnd = clock();
        int roundCost = (int)(roundEnd-roundStart)/CLOCKS_PER_SEC;	
        
        // print the running time at round i at a rank		
        printf("--- time for round %d, partition %d = %d sec\n",i,rank,roundCost);
        
        
        MPI_Barrier (MPI_COMM_WORLD);
        
        if(rank == 0){
            roundEnd = clock();
            roundCost = (int)(roundEnd-roundStart)/CLOCKS_PER_SEC;
            printf("Total time for round %d: %d sec\n\n",i,roundCost);
        }
        
        
    }
    
   
}

void output()
{
    for(int i = 1; i <= node_Num; i++)
    {
        if(partition[i][1] == rank){
        
         double r = x[rank][i]/partition[i][0];
         printf("%d %d %f\n", i, rank, r);
            
            
        }
    }
    
}

int main(int argc,  char * argv[])
{
    if(argc != 5 && argc != 4){
        printf("incorrect argument number, please enter again:\n");
        return 0;
    }
    
    // node of partition files from input command
    char * node_partition_file = argv[argc-3];
	
	// edges information file from input command
    char * edge_file = argv[argc-2];
	
	//number of rounds from input command (the last number in the command)
    int num_rounds = atoi(argv[argc-1]);
	
    
    // MPI initiation   
     MPI_Init(&argc, &argv);
    
	// rank of MPI
     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	 
    
	//size of MPI
     MPI_Comm_size(MPI_COMM_WORLD, &number_of_tasks);
	// printf("number of task is %d\n", number_of_tasks);
    
    // compute reading time of node partition file and edge file
	clock_t start_read_time = clock();  // start of read time
    readPartition(node_partition_file);   //read partition file
    readEdge(edge_file);  // 
    clock_t end_read_time = clock();  //end of read time
    int readTime=(int)(end_read_time - start_read_time)/CLOCKS_PER_SEC;  //
    printf("The time to read input files to partition %d = %d seconds \n", rank, readTime);
    printf("\n");
	
    // print numbe of nodes and edges
    // printf("%d %d",node_Num,edge_Num);
	
	//printf("Start computing credit for round %d \n",num_rounds);
    compute_credit(num_rounds);
    output_result(num_rounds);
	
    for(int i = 0; i < number_of_tasks; i++){
        delete x[i];
    }

    delete[] y;

    delete[] credits;
    
    MPI_Finalize();
  
  //  output();
  // printf("The program ends, please check file <outputpartition%d.out> for node credits in partition %d \n");
   
    return 0;
}

