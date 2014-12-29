import java.net.*;
import java.io.*;
import java.util.*;
import java.util.regex.*;

public class EmailBigram {
	
	
    public static void main(String[] args) throws Exception {
    	
    	HashMap spamwords = new HashMap();
    	HashMap hamwords = new HashMap();

        URL spam= new URL("http://ix.cs.uoregon.edu/~wyu/Spamemail.txt");
        URL ham= new URL("http://ix.cs.uoregon.edu/~wyu/hamemail.txt");
        URL test= new URL("http://ix.cs.uoregon.edu/~wyu/testemail.txt");
        
        BufferedReader spamin = new BufferedReader(new InputStreamReader(spam.openStream()));
        String inputLine;
        String total="";
        String temp;
        while ((temp = spamin.readLine()) != null) {
        	total = total + " " + temp; 
        	}
      
        
             String[] tokens = total.split("\\W");
             for(int i=0; i < tokens.length; i++) {
            	String word1 = tokens[i].toLowerCase();
            	               for(int j=0; j < tokens.length; j++) {
            	            	   String word2 = tokens[j].toLowerCase();
            	            	   String word = word1 + " " + word2;
            	            	
            	            	   spamwords.put(word, word);
            	               }          
             }
          spamin.close();
          
          
          BufferedReader hamin = new BufferedReader(new InputStreamReader(ham.openStream()));
          total=" ";
          while ((temp = hamin.readLine()) != null) {
          	total = total + " " + temp; 
          	}
       
          tokens = total.split("\\W");
               for(int i=0; i < tokens.length; i++) {
              	String word1 = tokens[i].toLowerCase();
              	               for(int j=0; j < tokens.length; j++) {
              	            	   String word2 = tokens[j].toLowerCase();
              	            	   String word = word1 + " " + word2;
              	            	 
              	            	   hamwords.put(word, word);
              	               }          
               }          
            spamin.close();
            
            
            BufferedReader testin = new BufferedReader(new InputStreamReader(test.openStream()));
            while ((inputLine = testin.readLine()) != null) {
            	System.out.print(inputLine);
                         String[] tokens1 = inputLine.split("\\W");
                         int bad=0;
                          int good=0;
                     
                          for(int i=0; i < tokens1.length-1; i++) {
                        	 
            	                  String word = tokens1[i].toLowerCase() + " " + tokens1[i+1].toLowerCase();
            	      
                                if (spamwords.containsKey(word)) {
            	                bad++;
                                 }
               
                                 if (hamwords.containsKey(word)) {
            	                  good++;
                           
                        	   }
                          }
                                              
                                       
           	              if (good < bad) {
           	            	    	            	    
           	                    System.out.println ("  The number match ham is " + good + "; The number match spam is " + bad + ". So this email is spam;"); 
           	                 System.out.println();  
           	                    }
           	              
           	               if (good > bad) {
           	            	
           	            	System.out.println ("  The number match ham is " + good + "; The number match spam is " + bad + ". So this email is ham;"); 
           	            	System.out.println();  
           	               }
           	              
           	              else if (good == bad)
           	               { System.out.println("  The number match ham is " + good + " and match spam is " + bad + " This email is not known for spam or ham, since it did not match any words or they match equal words"); }
           	                              
            }
                        	   
            testin.close();
        
     }
}
            
            