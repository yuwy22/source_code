//PairwiseAlignment.java
//By Wenyuan Yu for CIS554 Project 3 (Pairwise Alignment)
//Due Feb.10 2014
//

import java.net.*;
import java.io.*;
import java.util.*;
import java.util.regex.*;


//Start of program for the PairwiseAlignment
public class PairwiseAlignmentURLFasta {

  public static final int gapscore = 2; // score 2 for gap cost
  public static final int matchscore = 0; // no penalty for match
  public static final int mismatchscore = 1; // score 1 for mismatch

  private String pair1;  // Original First string
  private String pair2;  // Original Second string
  private int pair1Len, pair2Len; // lengths of two strings
  private int[][] scoreArray;
  private String pair1Align, pair2Align;  // two strings after pairwise alignment of the two strings

  
  public static void main(String[] args) throws Exception {
	   
	// URL inputfile = new URL("http://ix.cs.uoregon.edu/~wyu/test.fasta");
      URL inputfile = new URL("http://ix.cs.uoregon.edu/~wyu/trna.fasta");
      BufferedReader input = new BufferedReader(new InputStreamReader(inputfile.openStream()));
      
      String in = ""; // temporary store each line from the fasta file
      String seq1 = "";  // store the first sequence file for the fasta file
      String seq2 = "";  // store the second sequence file for the fasta file
          
       try {
	        while((input.readLine()) != null) {
	        	
	        	in = input.readLine();
	        	if(in.charAt( 0 ) != '>')
	        	      {
	        		   if (seq1.length() == 0)
	        		           { seq1 = in.toString();
	        		            System.out.println("original seq1 is");
	        		            System.out.println(seq1);
	        	               }
	        		   else if (seq2.length() == 0)
   		                   { seq2 = in.toString();
   		                     System.out.println("original seq2 is");
  		                         System.out.println(seq2);
   	                        }
	        	      }
	        }
	       
	    } finally {
	     input.close();
}	  
       
     // pass the seq1 and seq2 to the PairwiseAlignment function, and print out the sequence after alignment
	PairwiseAlignment pairInfo = new PairwiseAlignment(seq1, seq2); // initialize parameters of two original input strings
	pairInfo.fillScoreMatrix(); // fill the matrix with score depending on the values of gap, mismatch and match
//	pairInfo.printMatrix(); //uncomment this line if you want to print the matrix
	pairInfo.setAlignment();  //run the pairwise alignment 
	pairInfo.printAlignment(); // print the sequence after pairwise alignment
  }
  
  
 // initialize parameters of two original input strings
  public PairwiseAlignmentURLFasta (String a, String b) {
    pair1 = a;
    pair2 = b;
    pair1Len = pair1.length();
    pair2Len = pair2.length();
    scoreArray = new int[pair2Len+1][pair1Len+1];
  } 
  
  
//Fill the matrix according to the values of gap, mismatch and match.
  public void fillScoreMatrix() {
    int col,row;                      //  index of Matrix
    int diagonal, north, west;        // computing the values of three adjoining cells
    int min;                          // set the minimum value for Optimal substructure
    
    for (col=0; col<=pair1Len; col++) scoreArray[0][col] = gapscore*col; // initial entries for pair1 (base case)
    for (row=0; row<=pair2Len; row++) scoreArray[row][0] = gapscore*row; // initial entries for pair2 (base case)
    
// Fill in the rest of rows with the values of minimum from the three adjoining cells
    for (row=1; row<=pair2Len; row++) {
      for (col=1; col<=pair1Len; col++) {
        if (pair1.charAt(col-1)==pair2.charAt(row-1)) 
        	diagonal = scoreArray[row-1][col-1] + matchscore;
        else diagonal = scoreArray[row-1][col-1] + mismatchscore;
        west = scoreArray[row][col-1] + gapscore;
        north = scoreArray[row-1][col] + gapscore;
        min = diagonal;
        if (north<min) min = north;
        if (west<min) min = west;
        scoreArray[row][col] = min;
      }
    }
  }

  //print a single value of element from matrix
  public void printValue(int x) {
    String s = ""+x;
    if (s.length() == 1) System.out.print("  "+s);
    else if (s.length() == 2) System.out.print(" "+s);
    else if (s.length() == 3) System.out.print(s);
    else System.out.print("***");
  }

  //print the whole matrix, call the print function of printValue
  public void printMatrix() {
    for (int row=0; row<scoreArray.length; row++) {
      for (int col=0; col<scoreArray[row].length; col++) 
        printValue(scoreArray[row][col]);
      System.out.println();
    }
  }

  //do the alignment of two input sequences
  public void setAlignment() {
    int row = pair2Len;     // alignment starts at end of sequence 2
    int col = pair1Len;     // alignment starts at end of sequence 1
    pair1Align = "";         // store pair1 sequence after alignment, starting with empty alignment 
    pair2Align = "";         // store pair2 sequence after alignment, starting with empty alignment 
    while ((col>0) || (row>0)) {  // start from at the end of sequence, and to the start of the both strings
      
      if ((row>0) && (scoreArray[row][col] == scoreArray[row-1][col] + gapscore)) {
        // came from north
    	  pair1Align = "-" + pair1Align;             // gap in sequence 1
    	  pair2Align = pair2.charAt(row-1) + pair2Align; // consume char from sequence 2
        row--;                           // move up in array
      }
      else if ((col > 0) && (scoreArray[row][col] == scoreArray[row][col-1] + gapscore)) {
        // came from west
    	  pair1Align = pair1.charAt(col-1) + pair1Align; // consume char from sequence 1
    	  pair2Align = "-" + pair2Align;             // gap in sequence 2
        col--;                           // move left
      }
      else {
        // came from northwest
    	  pair1Align = pair1.charAt(col-1) + pair1Align; // consume char from sequence 1
    	  pair2Align = pair2.charAt(row-1) + pair2Align; // and from sequence 2
        col--;                           // move left
        row--;                           // and up
      }
      // System.out.println(pair1Align); //uncommment if you want to see the detail alignment procedure
     //  System.out.println(pair2Align); //uncommment if you want to see the detail alignment procedure
    }
  }
//print the two sequence after alignment
  public void printAlignment() {
	System.out.println("The two pairs after Pairwise Alignment are: ");
    System.out.println(pair1Align);
    System.out.println(pair2Align);
  }

}