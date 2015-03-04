/* CIS572 hw3 Part B By Wenyuan Yu
 * 
 * 
 * $ javac NaiveBayes.java
 * $ java NaiveBayes spambase-train.csv spambase-test.csv 2 model.txt
 * for ./nb <train> <test> <beta> <model>
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


public class NaiveBayes {

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
    

    
    static double NB_training(ArrayList<ArrayList<Integer>>training_data, double [] w_d, double b, double beta) {
    	
    	double pos_num = 0, neg_num = 0;
    	for(int i = 0; i < training_data.size(); i++) {
    		double y = training_data.get(i).get(training_data.get(i).size()-1);  // get y label
    		if(y == 1) pos_num++;
    		else neg_num++;
    	}
    	b = b + Math.log(pos_num/neg_num);
    	
    	//double [] p_x1_y1 = new double [w_d.length]; 
    	//double [] p_x1_y0 = new double [w_d.length];
    	
    	for(int attr_len = 0; attr_len < w_d.length; attr_len++){
    		double x1_y1 = 0, x0_y1 = 0; 
    	    double x1_y0 = 0, x0_y0 = 0;
    	    double p_x1_y1 = 0, p_x0_y1 = 0; 
    	    double p_x1_y0 = 0, p_x0_y0 = 0;
    		for(int i = 0; i < training_data.size(); i++) {
    			double y = training_data.get(i).get(training_data.get(i).size()-1);
    			double x = training_data.get(i).get(attr_len);
    			if(y == 1) {
    				if(x == 1) x1_y1++;
    				else if (x==0) x0_y1++;
    			}
    			else {
    				if(x == 1) x1_y0++;
    				else if (x==0) x0_y0++;
    			}
    		}
    		p_x1_y1 = (x1_y1 + beta -1)/(pos_num + 2*beta -2);
    		p_x0_y1 = (x0_y1 + beta -1)/(pos_num + 2*beta -2);
    		p_x0_y0 = (x0_y0 + beta -1)/(neg_num + 2*beta -2);
    		p_x1_y0 = (x1_y0 + beta -1)/(neg_num + 2*beta -2);
    		
    		b = b + Math.log((p_x0_y1)/(p_x0_y0));
    		w_d[attr_len] = Math.log((p_x1_y1)/(p_x1_y0)) - Math.log((p_x0_y1)/(p_x0_y0));
    		//System.out.println("total_num: " + (p_x1_y1 + p_x0_y1 + p_x1_y0 + p_x0_y0));
    	}
    
    	System.out.println("pos_num: " + pos_num);
    	System.out.println("neg_num: " + neg_num);
    	System.out.println("neg_num: " + training_data.size());
    	
    	return b;
    }
    
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
		
		String training_set = args[0]; // name of training file
		String test_set = args[1];  //name of test file
		double beta = Double.valueOf(args[2]);  // beta	
		String model = args[3];  //model
		
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
		 
		 b = NB_training(training_data, w_d, b, beta);		 
		 System.out.println("\n new b is " + b);
		 
		 double accuracy_training = cal_accuracy(training_data, w_d, b);		 	 
		 double accuracy = cal_accuracy(testing_data, w_d, b);	
		 
		 System.out.printf("\n With beta = " + beta + ", accuracy for training data is %.2f%%%n", 100*accuracy_training);
		 System.out.printf("\n With beta = " + beta + ", accuracy for testing data is %.2f%%%n", 100*accuracy);
		 
		 
		 write_model(model, w_d, b, attribute_name);
		 
   }	
	
}