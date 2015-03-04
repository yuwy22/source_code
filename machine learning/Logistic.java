/* CIS572 hw3 Part B By Wenyuan Yu
 * 
 * 
 * $ javac Logistic.java
 * $ java Logistic spambase-train.csv spambase-test.csv 0.01 1 model.txt
 * for ./logistic <train> <test> <eta> <sigma> <model>
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


public class Logistic {

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
    
    static double cal_logist(ArrayList<Integer> row_j, double [] w_d, double b) {	
		double logist = 0, sum=0;
		for(int k = 0; k < row_j.size()-1; k++) {
			sum = sum + w_d[k]*row_j.get(k);
		}
		sum = sum + b;
		if(sum < 0)
		    logist = 1.0*Math.exp(sum)/(1+ Math.exp(sum));
		if(sum >=0)
			logist = 1.0/(1+ Math.exp(-sum));
		//System.out.println("logist " + logist);
		//if(logist == 0.0) logist = 0.00001;
		//else if (logist == 1.0) logist = 0.99999;	
		return logist;
	 }
  
    
    static double cal_magnitude(double [] w_d, double [] w_d_old) {
       double mag = 0;
       for(int i = 0; i < w_d.length; i++) {
    	   mag += (w_d[i]-w_d_old[i])*(w_d[i]-w_d_old[i]);
       }
       return Math.sqrt(mag);
    }
    
    /*
    static double cal_magnitude(double [] w_d, double [] w_d_old, double b, double b_old, ArrayList<ArrayList<Integer>>training_data) {
        double mag_new = 0, mag_old = 0; 
        
        for(int i = 0; i < training_data.size(); i++) {
        	double wx_b_new =0, wx_b_old=0;
        	for(int j = 0; j < training_data.get(i).size() -1; j++) {
        		wx_b_new += w_d[j]*training_data.get(i).get(j);
        		wx_b_old += w_d_old[j]*training_data.get(i).get(j);
        	}
        	mag_new = ((mag_new + b)>0 ? 1:0);
            mag_old = ((mag_old + b)>0 ? 1:0);
        	mag_new += (wx_b_new) - Math.log(1 + Math.exp(wx_b_new));
        	mag_old += (wx_b_old) - Math.log(1 + Math.exp(wx_b_old));
        }
        
        double diff = Math.abs(mag_new - mag_old);
        System.out.println("diff  " + mag_new);
        return diff;
     }
    */
    
    static double logistic_training(ArrayList<ArrayList<Integer>>training_data, double [] w_d, double b, double eta, double sigma) {
    	
    	double [] w_d_old = new double [w_d.length];
    	double [] gradient = new double [w_d.length];
    	double b_old = 0;
    	for(int iter = 0; iter < 1000; iter++) {  // 100 pass     
    		
    		// copy of w_d and b
    		for(int i = 0; i< w_d.length; i++) {
    			w_d_old[i] = w_d[i];
    		}   		
    		b_old = b;
    				
    		for(int i = 0; i < w_d.length; i++) {  // recalculate all the w_d[]
    			double diff_y_x = 0, logist = 0;
    			for(int j = 0; j < training_data.size(); j++) { // go through all the training data
    				double y = training_data.get(j).get(training_data.get(j).size()-1);  // get y label
    				double x = training_data.get(j).get(i);
    				logist = cal_logist(training_data.get(j), w_d_old, b_old);
    				//System.out.println("gradient " + (y - logist)*x);
    				diff_y_x = diff_y_x + (y - logist)*x; 				
    			}
    			//w_d[i] = 1.0/(sigma*sigma)*w_d_old[i] + eta*diff_y_x;
    			w_d[i] = w_d_old[i]/(sigma*sigma) + eta*diff_y_x;
    			
    		}  // end of recalculate all the w_d[]
    		//normalize(w_d);
    		// calculate b
    		double diff_y = 0;
    		for(int k = 0; k < training_data.size(); k++) {
    			double y = training_data.get(k).get(training_data.get(k).size()-1);  // get y label
    			double logist = cal_logist(training_data.get(k), w_d_old, b_old);
    			diff_y = diff_y + (y - logist);
    		} 		 	
    		b = b + eta*(diff_y);
    		// end of calculate b
    		
    		//System.out.println("iter " + iter + " b: " + b);
    		double magnitude = cal_magnitude(w_d, w_d_old);
    		//double magnitude = cal_magnitude(w_d, w_d_old, b, b_old, training_data);
    	    System.out.println("magnitude at iter " + iter + " : " + magnitude);
    	    if(magnitude < 10) return b;
    	}
    		
    	return b;
    }
   /* 
    static int cal_classifier(ArrayList<Integer> example, double [] w_d, double b) {
    	double output = 0;
    	for(int i = 0; i < w_d.length ; i++) {
    		output = output + w_d[i]*example.get(i);
    	} 
    	if(example.get(example.size()-1) == 1) 
    		System.out.println(1.0/(1.0 + Math.exp(-output)));
    	
    	output += b;   	
    	return ((output > 0)? 1 : -1);
    }
    
    static double cal_accuracy(ArrayList<ArrayList<Integer>> testing_data, double [] w_d, double b) {
    	double accuracy = 0;
    	int predict_correct = 0;
    	for(int i=0; i<testing_data.size(); i++) {
    		double label = testing_data.get(i).get(testing_data.get(i).size()-1);
    		if(label == 1) System.out.print("label = 1 for example " + i + ", calculated probability is is ");
    		int output = cal_classifier(testing_data.get(i), w_d, b);
    		if(output == -1) output = 0;
    		if(output == label) predict_correct++;
    	}
    	//System.out.print("\n number of correct predict is " + predict_correct);
    	accuracy = 1.0*predict_correct/testing_data.size();  	
    	return accuracy;
    }
    */
    static double cal_classifier(ArrayList<Integer> example, double [] w_d, double b) {
    	double output = 0;
    	for(int i = 0; i < w_d.length ; i++) {
    		output = output + w_d[i]*example.get(i);
    	} 
    	output += b;   	
    	return output;
    }
    
    static double cal_accuracy(ArrayList<ArrayList<Integer>> testing_data, double [] w_d, double b) {
    	double accuracy = 0;
    	int predict_correct = 0;
    	for(int i=0; i<testing_data.size(); i++) {
    		double label = testing_data.get(i).get(testing_data.get(i).size()-1);
    		double output = cal_classifier(testing_data.get(i), w_d, b);
    		if (label ==1) System.out.println(" The class label of Example " + i + " is 1, the probability is " + (1.0/(1.0 + Math.exp(-output))));
    		
    		if( label == ((output>0) ? 1 : 0)) predict_correct++;
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
		double sigma = Double.valueOf(args[3]);
		String model = args[4];  //model
		
		// get the data for attribute_name, training_data, testing_data
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
		
		 
		 
		 //System.out.print("size " + attribute_name.size());
		 
		 b = logistic_training(training_data, w_d, b, eta, sigma);		 
		 System.out.println("\n new b is " + b);
		 
		 double accuracy_training = cal_accuracy(training_data, w_d, b);		 	 
		 double accuracy = cal_accuracy(testing_data, w_d, b);	
		 
		 System.out.printf("\n With eta = " + eta + ", sigma= "+ sigma + ", accuracy for training data is %.2f%%%n", 100*accuracy_training);
		 System.out.printf("\n With eta = " + eta + ", sigma= "+ sigma + ", accuracy for testing data is %.2f%%%n", 100*accuracy);
		 
		 write_model(model, w_d, b, attribute_name);
		 
   }	
	
}