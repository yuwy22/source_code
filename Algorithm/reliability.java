//Created by Wenyuan Yu (email: wyu@uoregon.edu) for CIS621 dynamic programming
// On Feb. 28 2014
// This program provides both an iterative and memoized solution for maximum reliability problem.
// To run this program, run the following commands: 
//javac reliability.java
//java reliability < inSample.txt
//The input will be a text file, to be read from standard input.

import java.io.IOException;
import java.util.Scanner;

public class reliability {

	private static int [] cost; // cost of each machine
    private static float [] reliability;  // reliability of each machine
    public static int budget, machineNum; //Budget and the number of machines
    public static double [][] opt; //Memoized Version maximum reliability for each specified machine number and budget 
    public static int [][] optNum; //Memoized Version the optimal number of machine for each specified machine number and budget
	
    public static void main (String [] args) throws IOException {
		 
		 Scanner input = new Scanner(System.in); //input file	         
		 int line = 0, count=1; //count the lines of input file
		 String inputLine;
		 
		 // start getting information from input file
		 try {
		        while (input.hasNextLine()) {
		        	
		        	if (line == 0) {
		        		budget = input.nextInt();
		        		System.out.println("Input files:");
		        		System.out.println("Budget: " + budget);
		        		line ++;
		        		 cost = new int[budget+1];
		        		 reliability = new float[budget+1];
		        	}  // The above 7 lines obtain the budget from input file
		        	   // And initialized the sizes of array cost[] and reliability[]
		        	
		        	else if (line == 1) {
		        		machineNum = input.nextInt();		        		
		        		System.out.println("Number machines: " + machineNum);
		        		line ++;
		        		opt = new double [machineNum+1][budget+1]; // used in Memoized Version
		        		optNum = new int [machineNum+1][budget+1]; // used in Memoized Version
		        	} // The above 6 lines obtain number of machines from input value
		        	   // And initialized the sizes of array opt[][] and optNum[][]
		        	
		        	else if ((inputLine = input.nextLine()).length()!=0) {		        		       		
		        	    String[] tokens = inputLine.split(" ");		        	    
		        	    for(int i=0; i < tokens.length; i++) {
		        	    	if (i == 0) {		        	    		
		        	    	    cost [count] = Integer.valueOf(tokens[i]);	        	    				        	    		
		        	    	}
		        	    	if (i == 1) {		        	    		
		        	    		reliability [count] = Float.valueOf(tokens[i]);		        	    	   
		        	    	}  
		        	    } 
		        	   count ++;
		        	   line ++;		        	   
		        	}	// the else if segment initialize values of cost[] and reliability[] from input	        			        	      
		        }
		       
		    } finally { input.close();}
		 // close input file
		 
		// print the values of cost[] and reliability[]
		  for (int i=1; i <= machineNum; i++) {
			 System.out.println(cost[i] + " " + reliability[i]);  				 
			 }	
		 
		 //maximum reliability used in Iterated Version
		double [][] opt1 = new double [machineNum+1][budget+1]; 
		//the optimal number of machine used in Iterated Version
	    int [][] optNum1 = new int[machineNum+1][budget+1];  
		 
	     //Start of Iterated Version program			 
		 for (int b = 0; b <= budget; b ++) {
			 opt1[0][b]=1;
		 }
		 for (int i = 1; i <= machineNum; i ++) {
			 opt1[i][0]=0;
		 } // initialize values of opt1[machineNum][budget]
		 
		 // iterate to compute the values of opt1[][] and optNum1[][]
		 for (int j = 1; j <= machineNum; j++) {
			  for (int b = 1; b <= budget; b++) {
				 for (int k = 1; k <= b/cost[j]; k++) {
					 if (b < cost[j]*k) {
						 continue;
					 }
					 else { 
						 double temp = opt1[j-1][(b-cost[j]*k)]*(1-Math.pow((1-reliability[j]),k));
					            if (temp > opt1[j][b]) {
						            opt1[j][b] = temp;
						            optNum1[j][b] = k; 				     	 
					            }					         
					 } 										
				 } 							 
			 }
	     } // end of loop
		 
		System.out.println("Output files:"); 
		System.out.println("Iterated Version:"); 
		System.out.println("Maximum reliability: " + opt1[machineNum][budget]);
	    for (int j = machineNum, b = budget; j > 0; j--) {	       
	    	System.out.println(optNum1[j][b] + " copies of machine " + j + " of cost " + cost[j]);
		    b = b - cost[j]*optNum1[j][b];
	    }
    // end of program of Iterated Version 
	    
	// start of program of Memoized Version   
	 for (int b=1; b <= budget; b++) {
    	 opt[0][b]=1.0;
     }	// initialize opt values
	 
	 double max = optCompute(machineNum, budget); //call the function (recursive memoization)
	 System.out.println("Memoized Version:");
	 System.out.println("Maximum reliability:" + max);
	 System.out.println(opt[machineNum][budget]);
	 
	    for (int j = machineNum, b = budget; j > 0; j--) {	       
		   System.out.println(optNum[j][b] + " copies of machine " + j + " of cost " + cost[j]);
		   b = b - cost[j]*optNum[j][b];
	    } //print the number of machines for optimal values 
	   
	    //provide memoization statistics
	    int countMemo = 0; 
	     for (int j = 1; j <= machineNum; j++){
			  for (int b = 1; b <= budget; b++) {
				  if (optNum[j][b] != 0) {
					  countMemo ++;
				  }
			  }			 
	    }  	//end of for loop
	     
	    System.out.println("Memoization Statistics:");
	    System.out.println("Number used: " + countMemo);
	    System.out.println("Total locations: " + machineNum*budget);
	    System.out.println("Percentage used: " + countMemo/(machineNum*budget*1.0));	  
	 }	

    //start of memoized version of program
	 static double optCompute(int Num, int b) {	
		 if (b < 0) return 0;
		 if (b == 0 && Num > 0) return 0;
		 if (Num == 0 && b >= 0) return 1;		  
		 if (opt[Num][b]!=0.0) return opt[Num][b];
				 
		 for (int k = 1; k <= b/cost[Num]; k++) {
			 double temp = optCompute(Num-1, b-cost[Num]*k)*(1-Math.pow((1-reliability[Num]),k));		
			 if (temp > opt[Num][b]) {				 
				 opt[Num][b] = temp;
				 optNum[Num][b] = k; 				     	 
			  }
		 }
		 return opt[Num][b]; 		 
	 }
	 // end of program of Memoized Version	 
}
