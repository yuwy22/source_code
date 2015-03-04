/* CIS572 hw2 Part B By Wenyuan Yu
 * Perceptron.java
 * 
 * $ javac Perceptron.java
 * $ java Perceptron spambase-train.csv spambase-test.csv 0.1 model.txt
 * 
*/

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.List;


public class Perceptron {

	public static void load_attributes(String trainfile, List<String> attribute_name) {		
		try {
			BufferedReader br = new BufferedReader(new FileReader(trainfile));
			String line;
			line = br.readLine();
			String splitBy = ","; 
			String[] tokens = line.split(splitBy);
			for(int i =0; i< tokens.length; i++) {
				attribute_name.add(tokens[i]);
			}		
			br.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	} // Have a separate list to store the name of attributes
	
	// load the instances
    public static void load_examples(String filename, List<ArrayList<Integer>> data_set) {
			
		try {
			BufferedReader br = new BufferedReader(new FileReader(filename));
			String line;
			line = br.readLine();  //ignore the first line, it is attributes
			int row = 0;
			while ((line = br.readLine()) != null) {
				ArrayList<Integer> temp = new ArrayList<Integer>();
				data_set.add(temp);
				String splitBy = ","; 
				String[] tokens = line.split(splitBy);
				for(int i =0; i< tokens.length; i++) {
					data_set.get(row).add(Integer.valueOf(tokens[i]));
				}	
				row++;
			}
			//System.out.println("\n total row for " + filename + " is " + row);
			br.close();
		} catch (Exception e) {
			e.printStackTrace();
		}		
	}  // end of load function.
	
    static int cal_classifier(ArrayList<Integer> example, double [] w_d, double b) {
    	double output = 0;
    	for(int i = 0; i < w_d.length ; i++) {
    		output = output + w_d[i]*example.get(i);
    	} 
    	output += b;   	
    	return ((output > 0)? 1 : -1);
    }
   
    public static void normalize(double [] array) {
    	double magnitude = 0;
    	for(int i =0; i< array.length; i++) {
    		magnitude += array[i]*array[i];
    	}
    	magnitude = Math.sqrt(magnitude);
    	
    	if(magnitude != 0)  {
    	    for(int i =0; i< array.length; i++) {
    		    array[i] = array[i]/magnitude;
    	    }
    	}
    }
    
    static double perceptron_train(ArrayList<ArrayList<Integer>>training_data, double [] w_d, double b, double eta) {
    	double max_b=b, max_accuracy=0; 
    	double [] max_w_b = new double[w_d.length];
    	for(int iter = 0; iter < 100; iter++) {  // 100 pass  
   
    		for(int i = 0; i < training_data.size(); i++) { // get the rows of each examples 
    			double y = training_data.get(i).get(training_data.get(i).size()-1);  // get y
    			//double y = training_data.get(i).get(54);  // get y
    		    double activation = 0;
    			for(int j=0; j<training_data.get(i).size()-1; j++) {  // get the value of each attribute
    				activation = w_d[j]*(training_data.get(i).get(j));				
    			}
    			activation += b;
    			normalize(w_d);
    		
    			System.out.println("iter " + iter + " example  " + i + "  activation is " + activation);
    			
    			if(y==0) y =-1;
    			if(y*activation <= 0 ) { 				
    				int output = cal_classifier(training_data.get(i),w_d, b); //either -1 or 1
    				for(int k = 0; k < w_d.length; k++) {    					
    					w_d[k] = w_d[k] + eta*(y - output)*training_data.get(i).get(k);
    					//System.out.print(" " + w_d[k]);
    				}
    				//System.out.print("\n");
    				b = b + eta*(y - output);
    				//System.out.println(" b is " + b);
    			}
    		}
    		
    		double accuracy = cal_accuracy(training_data, w_d, b);	
    		if(accuracy > max_accuracy) {
    			max_b = b;
    			for(int i=0; i < w_d.length; i++) {
    				max_w_b[i] = w_d[i];
    			}
    			max_accuracy = accuracy;
    		}
    		
    		if(max_accuracy == 1.0) return max_b; // if convergence, terminate the program
    		
    	}   // end of 100 pass	
    	
    	for(int i=0; i < w_d.length; i++) {
			w_d[i]= max_w_b[i];
		}
    	return max_b;
    }
    
    static double cal_accuracy(ArrayList<ArrayList<Integer>> testing_data, double [] w_d, double b) {
    	double accuracy = 0;
    	int predict_correct = 0;
    	for(int i=0; i<testing_data.size(); i++) {
    		int output = cal_classifier(testing_data.get(i), w_d, b);
    		if(output == -1) output = 0;
    		if(output == testing_data.get(i).get(testing_data.get(i).size()-1)) predict_correct++;
    	}
    	//System.out.print("\n number of correct predict is " + predict_correct);
    	accuracy = 1.0*predict_correct/testing_data.size();  	
    	return accuracy;
    }
    
   public static void write_model(String model, double [] w_b, double b, ArrayList<String> attribute_name){
	  /*
	   System.out.println(b);
	   for(int i=0; i <w_b.length; i++) {
		   System.out.println(attribute_name.get(i) + "  " + w_b[i]);
	   }	   
	  */
	   // Writing the credits for all nodes and all rounds to output files
	 Writer writer = null;
	 try {
	     writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(model), "utf-8"));
	     writer.write(String.format( "%15.6f", b));
	     ((BufferedWriter) writer).newLine();
	     for (int i = 0; i < w_b.length; i++) {    	   
	    	        writer.write(attribute_name.get(i) + "  " + String.format("%15.6f", w_b[i]));		            
		             ((BufferedWriter) writer).newLine();
		             writer.flush();
	    	       } //end of writing data of node i for all rounds
	    	      
         }catch (IOException ex) {		
		 } finally {
			    try {writer.close();} catch (Exception ex) {}
			 }   
 }
       
   public static void main(String[] args) {
		
		String training_set = args[0]; // name of input file
		String test_set = args[1];  //name of file
		double eta = Double.valueOf(args[2]);  // eta
		String model = args[3];  //model
		
		ArrayList<String> attribute_name = new ArrayList<String>();
		ArrayList<ArrayList<Integer>> training_data = new ArrayList<ArrayList<Integer>>();
		ArrayList<ArrayList<Integer>> testing_data = new ArrayList<ArrayList<Integer>>();	
		load_attributes(training_set, attribute_name);
		load_examples(training_set, training_data);
		load_examples(test_set, testing_data);
		double [] w_d = new double[attribute_name.size()-1];
		double b=0; 
		/*
		System.out.print("size " + attribute_name.size());
		for(int i=0; i< attribute_name.size() -1; i++) {
			System.out.print(" " + w_d[i]);
		}		
		System.out.println("\n training: " + training_set);
		System.out.println("\n testing: " + test_set);
		System.out.println("\n eta is: " + eta);
		System.out.println("\n model: " + model);
		for(int i=0; i< attribute_name.size(); i++) {
			//System.out.print(" " + attribute_name.get(i));
		}
		
		for(int i=0; i<training_data.size();i++) {
			System.out.print("\n");
			for(int j=0; j<training_data.get(i).size(); j++) {
				System.out.print(training_data.get(i).get(j));
			}
		}
		*/
		
		//System.out.print("size " + training_data.size());
		//System.out.print("size " + training_data.get(0).size());
		
		 
		 //System.out.println("\n new b is " + b);
		 //System.out.print("size " + attribute_name.size());
		 
		 b = perceptron_train(training_data, w_d, b, eta);		 
		 double accuracy = cal_accuracy(training_data, w_d, b);		 
		 System.out.printf("\n With eta = " + eta + " accuracy for trainning data is %.2f%%%n", 100*accuracy);		 
		 accuracy = cal_accuracy(testing_data, w_d, b);		 
		 System.out.printf("\n With eta = " + eta + " accuracy for testing data is %.2f%%%n", 100*accuracy);
		 
		 write_model(model, w_d, b, attribute_name);
   }	
	
}
