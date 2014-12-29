// CIS630 Project Part 1
// Wenyuan Yu on April 23, 2014
// Email: wyu@uoregon.edu
// To compile it, use:
// javac ManyRandomWalk.java
// followed by: 
//  java ManyRandomWalk input_file output_file round_Num




import java.io.*;
import java.util.*;
import java.text.*;

public class ManyRandomWalk {
	
	public static final int node_Num = 2000000; // number of nodes
	public static int round_Num; // number of nodes
	public static double [][] credit;
	public static int[] node_degree = new int [node_Num];	
	@SuppressWarnings("unchecked")
	public static ArrayList<Integer> node_edge [];	
	@SuppressWarnings("unchecked")
	public static void main (String [] args) throws IOException {
		
		String input = args[0]; // name of input file
		String output = args[1];  //name of output file
		String rounds = args[2];  //rounds of random walk
		round_Num =  Integer.valueOf(rounds);
		System.out.println("\nThe name of input file is " + input);
		System.out.println("The name of output file is " + output);
		System.out.println("Round Number is " + round_Num);
		
		credit = new double [round_Num +1][node_Num];   // credit for node at round t
		node_degree = new int [node_Num];    // degree of each node 
		node_edge = new ArrayList[node_Num +1];   //edges of each node
		
		File file = new File(input);
		BufferedReader reader = new BufferedReader(new FileReader(file)); 	
		int total_edge = 0, number1 = 0, number2 = 0; //number1 and number2 store the two nodes of each edge		
		String input_line;		
		
		// record the edges of each node using Arraylist
		for( int i = 0; i < node_Num; i++) {
		    node_edge[i] = new ArrayList<Integer>();		    
		}
		
		// Begin to read input file to array and arraylist
		System.out.println("\nBegin to read input file...");
		try {  
			    long start_time = System.currentTimeMillis();  // Time of beginning to load the input file
		        while ((input_line = reader.readLine()) != null) {
		        	String[] tokens = input_line.split("\\W");		        	    
	        	    for(int i=0; i < tokens.length; i++) {
	        	    	if (i == 0) {
	        	    		// number1 presents the first node of one edge
	        	    		number1 = Integer.valueOf(tokens[i]);
	        	    	    node_degree[number1]++;	        	    				        	    		
	        	    	}
	        	    	if (i == 1) {	
	        	    		// number2 presents the second node of one edge
	        	    		number2 = Integer.valueOf(tokens[i]);
	        	    		node_degree[number2]++;		        	    	   
	        	    	}  
	        	    }	        	    
		        	  total_edge++;
		        	  node_edge[number1].add(number2); // add node number2 to the arraylist of node number1
		        	  node_edge[number2].add(number1);  //add node number1 to the arraylist of node number2
		         }  // end of while loop	
		        long end_time = System.currentTimeMillis();  //end of loading input file
		        long total_time = (end_time - start_time)/1000; // calculate the time of reading input file
		        System.out.println("Time to read input file = " + total_time + " seconds");
		     } finally {reader.close();}  //end of try function
		 
		 //initialize the initial state (t=0), each node i has credit of 1.
		 for (int i= 1; i <node_Num; i++) {
			 credit[0][i] = 1.0;
		 }
		 
		 System.out.println("\nThe time for completing each round (including updating the output file):");
		 // The following for loop calculate the credit for each round for all the nodes, and print the time for each round.
		 Writer single_output = null;
		 try {	     
		    for (int t=1; t <= round_Num; t++) {
		    	  long start_time = System.currentTimeMillis(); //record the time of beginning of each round
		    	  single_output = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(output), "utf-8"));
		    	  
		    	  // The following for loop, calculate the credit of all nodes at round t
		    	  for (int i = 1; i < node_Num; i++) {
			              if (node_degree[i]!=0) {
				                credit[t][i] = credit_neighbour(t, i); //call the function of credit_neighbour(t, i)
				                // writing output to output file 
				                single_output.write(String.valueOf(credit[t][i]));
			                  }	
		           }
		    	   long end_time = System.currentTimeMillis(); //record the end of each round
		           long total_time = (end_time - start_time)/1000; //calculate the time of each round		           
		           System.out.println("Writing Round " + t + " = " + total_time + " seconds");  //print the time of each round
		           // end of calculate the credit of all nodes at round t
		           
		   } 	
	    } finally { try {single_output.close();} catch (Exception ex){}}
		// end of calculating the credit for all rounds for all the nodes
		 
		 // Writing the credits for all nodes and all rounds to output files
		 Writer writer = null;
		 try {
			 System.out.println("\nRight now, the data of total " + round_Num + " rounds are writing to the output file.....");
			 long start_time = System.currentTimeMillis();
		     writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(output), "utf-8"));
		     // from node 1 to node_Num.
		     for (int i = 1; i < node_Num; i++) {
		    	   if (node_degree[i]!=0) {
		    	        writer.write(String.format( "%10d", i) + String.format("%15d", node_degree[i]));
			            for (int t = 1; t <= round_Num; t++){
					         writer.write((String.format( "%15.6f", credit[t][i])));					            	
			                 }
			             ((BufferedWriter) writer).newLine();
			             writer.flush();
		    	       } //end of writing data of node i for all rounds
		    	   
			  } // end of writing data of all nodes for all rounds
		     
		     long end_time = System.currentTimeMillis();
		     long total_time = (end_time - start_time)/1000;
		     System.out.println("Writing total " + round_Num + " Rounds to output file takes " + total_time + " seconds");
		     
		 } catch (IOException ex) {		
		 } finally {
		    try {writer.close();} catch (Exception ex) {}
		 }
		 		 
		   System.out.println("\nThis program has finished, please check the output file.");	 
	} //end of main function
	    
	   // Function credit_neighbour(int round, int node) calculate the credit at 
 	   static double credit_neighbour(int round, int node) {
		     double credit_value = 0;		
		     for (int i=0; i <node_edge[node].size(); i++ ) {
			          int neighbour = node_edge[node].get(i); // find the nodes directly connected to node i
			          credit_value = credit_value + credit[round-1][neighbour]/node_degree[neighbour]; 
		         }
		     return credit_value;
	    } 
 	   //end of function credit_neighbour
 	   
	
  } // end of the whole program
