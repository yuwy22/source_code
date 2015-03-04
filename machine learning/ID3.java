/* 
* CIS572 Assignment 1. By Wenyuan Yu, 01/23/2015
* To run the program, run the following command: format is ./id3 <train> <test> <model>
*    ./javac ID3.java
*    ./java ID3 training_set.csv test_set.csv model.txt
* reference: https://courses.cs.washington.edu/courses/cse446/12wi/ps/hw4/ID3.java
*/


import java.io.*;
import java.util.*;
import java.text.*;


class Instance {
	static int num_branches = 2; // fts can only be 0,1
	int label = -1; // label=1 ; label=0
	int [] example_values;

	public Instance(String line) {
		String splitBy = ","; 
		
		String[] tokens = line.split(splitBy);
		
		//if(uniqueId == 0) {
		//	attributes = new String[tokens.length];
		//	attributes = line.split(splitBy);}
		
		// if (uniqueId > 0) {			
			label = Integer.valueOf(tokens[tokens.length-1]);
			this.example_values = new int[tokens.length-1];
			for(int i = 0; i < tokens.length -1; i++) {
				example_values[i] = Integer.valueOf(tokens[i]);
			}
			//if(label != 0 && label !=1)
			//System.out.print("\n Id " + uniqueId + " label " + label);	
			
			//System.out.print("Id " + (example_values.length));	
			//for (int i = 0; i < example_values.length; i++) {
			//System.out.print(" " + example_values[i]); }
			//System.out.print("\n");
			
		//}				
   }
}

class Node {

	Node parent;
	Node children[];  // binary tree
	int picked_attribute;   // -1 is leaf, if is branch not -1
	List<Instance> instances;
	int label_predicted = -1;

	Node(Node parent, List<Instance> instances) {
		this.parent = parent;
		children = new Node[Instance.num_branches];  // 2

		this.instances = instances;
		picked_attribute = -1;

		int count[] = { 0, 0 };
		for (Instance t : this.instances)
			if(t.label == 0 || t.label ==1)
			count[t.label]++;
		
		label_predicted = count[0] > count[1] ? 0 : 1;  // 0 or 1 depend on majority
	} // end of node initial

	// classify_instance one instance
	public int classify_instance(Instance t) {
		// System.out.println("test\t" + picked_attribute);
		if (picked_attribute == -1) {
			return label_predicted;
		     } 
		else {
			    if (children[t.example_values[picked_attribute]] != null) {
				        return children[t.example_values[picked_attribute]].classify_instance(t);
			         } 
			    else {
				       return -1;
			         }
		        }
	  }


	// Write the decision tree to filename
	public void writeTree(String filename, ArrayList<String> attribute_values) {
		try {
			int depth = 0;
			PrintWriter writer = new PrintWriter(new FileWriter(filename));			
			writeFile(writer, attribute_values, depth);
			writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void writeFile(PrintWriter writer,  ArrayList<String> attribute_values, int depth) {
		/*
		Node parent;
		Node children[];
		int picked_attribute;   // -1 is leaf, the selected feature
		int numOfFts;
		List<Instance> instances;
		int label_predicted = -1;
		*/
		String bar = "";
		int count = depth;
		while(count > 0) {
   		 bar += "| ";
   		 count--;
   	    }
		if(picked_attribute !=-1) {
			for (int i = 0; i < Instance.num_branches; i++)
				//if(this == parent.children[i])
				{   			  
					writer.print(bar + attribute_values.get(picked_attribute) + " = " + i + " : ");
					if(children[i].children[0] == null) writer.println(children[i].label_predicted);
					else writer.println();
					if (children[i] != null)
					children[i].writeFile(writer, attribute_values, depth +1);
					
				}
		}
		//else writer.println(bar + attribute_values.get(picked_attribute) + " = " +  + " : " + label_predicted);
	}		
			
	/*
	public void writeTree(String filename) {
		 // Writing the credits for all nodes and all rounds to output files
		 Writer writer = null;
		 try {
			 System.out.println("\n The decision tree is: \n");		 
		     writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(filename), "utf-8"));	
		     if (picked_attribute != -1) writer.write(testFts + " = : "  );
		     ((BufferedWriter) writer).newLine();
		     
		     if (testFts != -1) {
					for (int i = 0; i < Instance.num_branches; i++) {
						if (children[i] != null)
							children[i].writeTree(filename);
					}
				} 
		     else {
					writer.write("<leaf>");
				}
		     
		     
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
			  
	}catch (IOException ex) {		
	 } finally {
		    try {writer.close();} catch (Exception ex) {}
		 }

	//private void writeTreeML(PrintWriter writer) {}
   }
	*/
}  // end of node class

public class ID3 {
	
	Node root; //root node
	
	public static void load_attributes(String trainfile, List<String> attribute_values) {		
		try {
			BufferedReader br = new BufferedReader(new FileReader(trainfile));
			String line;
			line = br.readLine();
			String splitBy = ","; 
			String[] tokens = line.split(splitBy);
			for(int i =0; i< tokens.length; i++) {
				attribute_values.add(tokens[i]);
			}		
			br.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	} // Have a separate list to store the name of attributes
	
	
	// load the instances
    public static void load_examples(String trainfile, String testfile, List<Instance> training_instances, List<Instance> testing_instances) {
		
		
		try {
			BufferedReader br = new BufferedReader(new FileReader(trainfile));
			String line;
			line = br.readLine();  //ignore the first line, it is attributes
			while ((line = br.readLine()) != null) {
				Instance ins = new Instance(line); 
					   training_instances.add(ins);
				
			}
			br.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		try {
			BufferedReader br = new BufferedReader(new FileReader(testfile));
			String line;
			line = br.readLine();  // ignore the first line
			while ((line = br.readLine()) != null) {
				Instance ins = new Instance(line);
				testing_instances.add(ins);
			}
			br.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}  // end of load function.
	
	
	public abstract static class ImpurityFunction {
		public abstract double calc(int a, int b);
	}

	public static ImpurityFunction impurity_entropy = new ImpurityFunction() {
		
		public double calc(int a, int b) {
			double pa = a / ((double) a + (double) b);
			double pb = b / ((double) a + (double) b);
			double res = 0;
			if (a > 0)
				res += -pa * Math.log(pa);
			if (b > 0)
				res += -pb * Math.log(pb);
			return res / Math.log(2);
		}
	};
	
	public static class ChiSquareTest {
		double threshold;	
		ChiSquareTest(double threshold) {
			this.threshold = threshold;
		}

		public boolean test(int[][] count) {
			double chi_square = 0;
			int m_x_l = count.length;
			int m_y_l = count[0].length;
			double m_x[] = new double[m_x_l];// margin for x
			double m_y[] = new double[m_y_l];// margin for y
			double m = 0;
			
			for (int i = 0; i < m_x_l; i++) {
				for (int j = 0; j < m_y_l; j++) {
					m_x[i] += count[i][j];
					m += count[i][j];
				}
			} // end of for
			
			for (int j = 0; j < m_y_l; j++) {
				for (int i = 0; i < m_x_l; i++) {
					m_y[j] += count[i][j];
				}
			} //end of for
			
			for (int i = 0; i < m_x_l; i++) {
				for (int j = 0; j < m_y_l; j++) {
					double e_ij = 1.0 * m_x[i] * m_y[j] / m;
					double o_ij = count[i][j];
					if (e_ij > 0) {
						chi_square += (e_ij - o_ij) * (e_ij - o_ij) / e_ij;
					}
				}
			} //end of for loop
			
			return chi_square > threshold + 1e-8;
		} //end of test method
		
	} // end of ChiSquareTest
	
	
	public static ChiSquareTest chi_square_100 = new ChiSquareTest(0);
	
	
	static void expand(Node node, ImpurityFunction impurityFunction, ChiSquareTest cst, int depth) {

		double max_gain = -1000;
		int best_attri = -1;   // current node with maximum gain
		//int num_examples = node.instances.size();     // size of instances, List<instance> instances;
		int num_attributes = node.instances.get(0).example_values.length;  // number of attributes of one instance	
		int mcount[][] = new int[Instance.num_branches][2];  // [2][2]

		int neg_parent = 0, pos_parent = 0;
		for (int i = 0; i < node.instances.size(); i++) {
			if (node.instances.get(i).label == 1) {
				pos_parent++;
			} else {
				neg_parent++;
			}
		}    //check the number of positive and negative instances in current Node.
		
		/* Iterate over all attributes, find the best attribute */
		for (int s = 0; s < num_attributes; ++s) {

			int count[][] = new int[Instance.num_branches][2];
			for (Instance t : node.instances) {
				if (t.label == 1)
					count[t.example_values[s]][1]++;
				else if (t.label == 0)
					count[t.example_values[s]][0]++;
				//for(int i = 0; i < t.attributes.length; i++)
				     //System.out.print(t.attributes[i]);
				//System.out.print("\n");
			}
			
			double gain = impurityFunction.calc(pos_parent, neg_parent);  // Entropy(S) 
			
			for (int i = 0; i < Instance.num_branches; i++) {
				gain -= 1.0 * (count[i][0] + count[i][1])
						/ (pos_parent + neg_parent)
						* impurityFunction.calc(count[i][0], count[i][1]);
				// error += Math.min(count[i][0], count[i][1]);
			}  // calculate gain for each attribute

			if (gain > max_gain) {
				max_gain = gain;
				best_attri = s;
				for (int i = 0; i < Instance.num_branches; i++) {
					mcount[i][0] = count[i][0];
					mcount[i][1] = count[i][1];				
				}
			}  // end of above if

		}  // end of above two for loops
		
		//if(mcount[0][0] + mcount[0][1] == mcount[0][0]) System.out.print(0);
   	   // else if (mcount[0][0] + mcount[0][1] == mcount[0][1]) System.out.print(1);

		// Grow the tree by checking the max_gain
		if (max_gain > 1e-10 && cst.test(mcount)) {
			node.picked_attribute = best_attri;  // best attribute to testFts

			ArrayList<ArrayList<Instance>> ts = new ArrayList<ArrayList<Instance>>();
			for (int i = 0; i < Instance.num_branches; ++i) {
				ts.add(new ArrayList<Instance>());  
			}

			for (Instance t : node.instances) {
				//if(t.example_values[best_attri] != 0 && t.example_values[best_attri] !=1) System.out.println(" there is none  0 or 1 ");
				ts.get(t.example_values[best_attri]).add(t);
			}
			
			for (int i = 0; i < Instance.num_branches; i++) {
				//if (maxGainDecision == 16 && i == 2) {int x = 0;}
				if (ts.get(i).size() > 0) {
					node.children[i] = new Node(node, ts.get(i));
			    	 //if(mcount[i][0] + mcount[i][1] == mcount[i][0]) node.typical_instance = 0;
			    	// else if (mcount[i][0] + mcount[i][1] == mcount[i][1]) node.typical_instance = 1;
					expand(node.children[i], impurityFunction, cst, depth + 1);
				}
			} // end of for 
			
		}
	}	//end of expand function
	
	
	public static Node generate(List<Instance> instances, ImpurityFunction f) {
		Node root = new Node(null, instances);
		expand(root, f, ID3.chi_square_100, 0);
		return root;
	}
	
	public static Node generate(List<Instance> instances, ImpurityFunction f, ChiSquareTest cst) {
		Node root = new Node(null, instances);
		expand(root, f, cst, 0);
		return root;
	}
	
	public void learn(List<Instance> instances) {
		this.root = generate(instances, ID3.impurity_entropy);
	}
	
	public void learn(ArrayList<Instance> instances, ChiSquareTest cts) {
		this.root = generate(instances, ID3.impurity_entropy, cts);
	}
	
	public List<Integer> classify_instance(List<Instance> testing_instances) {
		List<Integer> testing_training = new ArrayList<Integer>();
		for (Instance t : testing_instances) {
			int predictedCategory = root.classify_instance(t);
			testing_training.add(predictedCategory);
		}
		return testing_training;
	}
	
	// calculate accuracy of instances
	public double cal_accuracy(List<Integer> predictions, List<Instance> testing_instances) {
		if (predictions.size() != testing_instances.size()) {
			return 0;} 
		else {
			int num_right = 0, num_wrong = 0;
			for (int i = 0; i < predictions.size(); i++) {
				if (predictions.get(i) == null) {
					num_wrong++;
				} 
				else if (predictions.get(i) == testing_instances.get(i).label) {
					num_right++; } 
				else {	num_wrong++;}
			}
			return num_right * 1.0 / (num_right + num_wrong);
		}
	}

    //
	// javac ID3.java
	// java ID3 training_set.csv test_set.csv model.txt

	
	public static void main(String[] args) {
		
		String training_set = args[0]; // name of input file
		String test_set = args[1];  //name of output file
		String model = args[2];  //rounds of random walk
		
		ArrayList<Instance> training_instances = new ArrayList<Instance>();
		ArrayList<Instance> testing_instances = new ArrayList<Instance>();
		ArrayList<String> attribute_values = new ArrayList<String>();
		
		//load("training.txt", "test.txt", training_instances,testing_instances);
		load_examples(training_set, test_set, training_instances, testing_instances);
		load_attributes(training_set, attribute_values);
		//System.out.println();
		System.out.println("\nnumber of training set is " + training_instances.size());
		System.out.println("number of testing set is " + testing_instances.size());
		System.out.println("number of attributes is " + attribute_values.size());
		
		//for (int i = 0; i < attribute_values.size(); i++) {
		//     System.out.print(" " + attribute_values.get(i)); }
		  
		// full tree test
		{	ID3 id3 = new ID3();		    
			id3.learn(training_instances);
			
			id3.root.writeTree(model, attribute_values);
			
			List<Integer> predict_training = id3.classify_instance(training_instances);
			System.out.println("\nID3 with full tree on training\t" + 
					(String.format ("%6.4f", id3.cal_accuracy(predict_training, training_instances))) );

			List<Integer> testing_training = id3.classify_instance(testing_instances);
			System.out.println("ID3 with full tree on test\t" + 
					(String.format ("%6.4f", id3.cal_accuracy(testing_training, testing_instances))));
		}
		
		
		// ChiSquare test
		{   ID3 id3 = new ID3();
			ChiSquareTest cst = new ChiSquareTest(6.635);
			id3.learn(training_instances, cst);
			//id3.root.writeTree(model, attribute_values);
			
			List<Integer> predict_training = id3.classify_instance(training_instances);
			System.out.println("\nID3 with ChiqSuareTest 6.635 on training\t" + 
					(String.format ("%6.4f", id3.cal_accuracy(predict_training, training_instances))) );

			List<Integer> testing_training = id3.classify_instance(testing_instances);
			System.out.println("ID3 with ChiSquareTest 6.635 on test\t" + 
					(String.format ("%6.4f", id3.cal_accuracy(testing_training, testing_instances))));		
		}		
	} // end of main function
	
} // end of Class ID3