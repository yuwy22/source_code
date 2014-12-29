// Project_4 by 
// source code change based on http://www.itu.dk/people/sestoft/bsa/Match7.java


import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.util.Scanner;

abstract class Cluster {
  public abstract void draw(Graphics g, int w, int h);
}

// UPGMA clusters or trees, built by the UPGMA algorithm

class UPCluster extends Cluster {
  int lab;			// Cluster identifier
  int card;			// The number of sequences in the cluster
  double height;		// The height of the node
  UPCluster left, right;	// Left and right children, or null
  double[] dmat;		// Distances to lower-numbered nodes, or null
  
  public UPCluster(int lab, double[] dmat) {	// Leaves = single sequences
    this.lab = lab + 1; 
    card = 1;
    this.dmat = dmat;
  }

  public UPCluster(int lab, UPCluster left, UPCluster right, double height, 
		 double[] dmat) { 
    this.lab = lab + 1; 
    this.left = left;
    this.right = right;
    card = left.card + right.card;
    this.height = height;
    //System.out.println("height  " + height);
    this.dmat = dmat;
  }

  public boolean live()
  { return dmat != null; }

  public void kill() 
  { dmat = null; }

  public void print() 
  { print(0); }

  void print(int n) {
    if (right != null)
      right.print(n + 6);
    indent(n); 
    System.out.println("[" + lab + "] (" + (int)(100*height)/100.0 + ")"); 
    if (left != null)
    left.print(n + 6);
  }

  void indent(int n) {
    for (int i=0; i<n; i++)
      System.out.print(" ");
  }

  public void draw(Graphics g, int w, int h) {
    draw(g, w, h, 0, (double)w/card, (double)(h-50)/height, 10); 
  }

  // Draw tree and return x-coordinate of root

  public int draw(Graphics g, int w, int h, int leftcard, 
		  double xsc, double ysc, int fromy) {
	 // System.out.println(" w is " + w + " h is " +  h + " leftcard " + leftcard + " xsc is " + xsc + " ysc is " + ysc + " fromy is " + fromy);
	  PhylogeneticInference m = new PhylogeneticInference ();
	  String [] newspecs = m.getSpec();
	  
	//  String diff = Double.toString(height);
    if (left != null && right != null) {	// Internal node
      int y = (int)(h - 30 - height * ysc);
      
      int leftx  = left.draw(g, w, h, leftcard, xsc, ysc, y);
      int rightx = right.draw(g, w, h, leftcard+left.card, xsc, ysc, y);
      g.drawLine(leftx, y, rightx, y);
     
      int x = (leftx + rightx) / 2; 
      g.drawLine(x, y, x, fromy);
      g.drawString(String.format("%.2f", height), x, (y+fromy+70)/2);
      g.fillOval(x-4, y-4, 8, 8);
      //System.out.println("height  " + height);
      return x;
    }
    else {					// Leaf node
      int x = (int)((leftcard + 0.5) * xsc);
      g.drawLine(x, h-30, x, fromy);
      
     // g.drawString(Double.toString(height), x-5, (h-30+fromy)/2);
      g.fillOval(x-4, h-30-4, 8, 8);
      g.drawString(newspecs[lab-1], x-5, h-10);
      return x;
    }
  }
}


// The UPGMA algorithm

class UPGMA {
  int K;			// The number of clusters created so far
  UPCluster[] cluster;		// The nodes (clusters) of the resulting tree

  public UPGMA(double[][] ds) {
    int N = ds.length;
    
    cluster = new UPCluster[2*N-1];
    for (int i=0; i<N; i++) 
          cluster[i] = new UPCluster(i, ds[i]);
    K = N;
    while (K < 2*N-1)
         findAndJoin();
  }

  public UPCluster getRoot()
  { return cluster[K-1]; }
  
  public double d(int i, int j) 
  { return cluster[Math.max(i, j)].dmat[Math.min(i, j)]; }

  void findAndJoin() { // Find closest two live clusters and join them
    int mini = -1, minj = -1;
    double mind = Double.POSITIVE_INFINITY;
    for (int i=1; i<K; i++) 
      if (cluster[i].live())
       	for (int j=0; j<i; j++) 
           	  if (cluster[j].live()) {
	                  double d = d(i, j);
	                      if (d < mind) {
	                        mind = d;
	                        mini = i;
	                        minj = j;
	                        }
	  }
    join(mini, minj);
  }

  public void join(int i, int j) { // Join i and j to form node K
    System.out.println("Joining " + (i+1) + " and " + (j+1) + " to form " + (K+1) + " at height " + (int)(d(i, j) * 50)/100.0);
    double[] dmat = new double[K];
    for (int m=0; m<K; m++)
      if (cluster[m].live() && m != i && m != j) 
	dmat[m] = (d(i, m) * cluster[i].card + d(j, m) * cluster[j].card)
	          / (cluster[i].card + cluster[j].card);
    cluster[K] = new UPCluster(K, cluster[i], cluster[j], d(i, j) / 2, dmat);
    cluster[i].kill(); 
    cluster[j].kill();
    K++;
  }
}


class TreeFrame extends ClosableFrame {
  String title;
  Button printButton = new Button("Print tree");
  TreeCanvas tc;

  public TreeFrame(String title, Cluster c) {
    super(title);
    this.title = title;
    tc = new TreeCanvas(c);
    add(tc, "Center");
    Panel p = new Panel();
    p.add(printButton);
    printButton.addActionListener(new buttonListener());
    add(p, "South");
    pack(); show();
  }

  public void setCluster(Cluster c) 
  { tc.setCluster(c); }
  
  public class buttonListener implements ActionListener {
    public void actionPerformed(ActionEvent e) {
      Toolkit t = getToolkit();
      PrintJob pj = t.getPrintJob(TreeFrame.this, "Printing " + title, null);
      if (pj != null) {
	Graphics pg = pj.getGraphics();
	printAll(pg);
	pg.dispose();
	pj.end();
      }
    }
  }
}

class TreeCanvas extends Canvas {
  Cluster c;

  public TreeCanvas(Cluster c)
  { this.c = c; }

  public void setCluster(Cluster c) 
  { this.c = c; repaint(); }

  public void paint(Graphics g) {
    Dimension d = getSize();
    if (c != null)
      c.draw(g, d.width, d.height);
  }
   
  public Dimension getPreferredSize() 
  { return new Dimension(300, 300); }

  public Dimension getMinimumSize() 
  { return getPreferredSize(); }
}


public class PhylogeneticInference {
	public static String []  species = new String [100];
	public String []  spec;
	public static int speciesNum = 0;
	public static double speciesDis [][] = new double [100][100];
	
	PhylogeneticInference () {
    	this.spec = species;
       }
	  
	  public String [] getSpec() {
		  return this.spec;
	  }

    public static void main(String[] args)  throws Exception {
    
    File myFile = new File("sarich.txt"); 
    Scanner input = new Scanner(myFile);
    String in = "";
    
   
    
    try {
	        while(input.hasNextLine()) {
	        	
	        	in = input.nextLine();
	        	if (in.length() == 0) {continue;}
	        	else if (in.charAt(0) == '#') {continue;}	        	
	        	else {
	        		 String[] tokens = in.split("\\s+");
	        		 int index_x = -1, index_y = -1; 	
	        		 for(int i = 0; i < tokens.length; i++) {	        			 	        			    
		        	    	if (i == 0) {
		        	    		for (int j = 0; j < speciesNum; j++) {
		        	    			
		        	    			if (species[j].equals(tokens[i])) {
		        	    				index_x = j;
		        	    			}
		        	    		}
		        	    		 if (index_x == -1) {
		        	    			index_x = speciesNum;
		        	    			species[speciesNum++] = tokens[i];
		        	    		}
		        	    	}
		        	    	
		        	    	else if (i == 1) {
		        	    		for (int j = 0; j < speciesNum; j++) {
		        	    			if (species[j].equals(tokens[i])) {
		        	    				index_y = j;
		        	    			}
		        	    		}
		        	    		if (index_y == -1) {
		        	    			index_y = speciesNum;
		        	    			species[speciesNum++] = tokens[i];
		        	    		}		        	    	        	    		        	    				        	    		
		        	    	}   	    			        	    	 		        	    	
		        	    	else if (i == 2) {		        	    		
		        	    		speciesDis[index_y][index_x] = Float.valueOf(tokens[i]);
		        	    	}
		        	    }
	        	}
	        	      
	        }
	       
	    } finally { input.close();}	
    double[][] dist = new double [speciesNum][];
   
    for (int i= 0; i < speciesNum; i++) {
   	 dist[i] = new double [i];
   	//System.out.println(dist[i].length);
    }
    System.out.println("The distance between two species are shown as below: ");
    for (int i= 0; i < speciesNum; i++) {
    	System.out.format("%10s", species[i]);
   	 for (int j=0; j <i;j++) {
   	     dist[i][j] = speciesDis[i][j];
   	        
            System.out.format("%7.1f", dist[i][j]);
   	 }
     System.out.println();
    }
    System.out.println();
    UPGMA upclu = new UPGMA(dist);
    new TreeFrame("UPGMA tree", upclu.getRoot());
   
  }
}

class CloseListener extends WindowAdapter {
  public void windowClosing(WindowEvent e) {
     e.getWindow().dispose();
     System.exit(0);
  }
}

class ClosableFrame extends Frame {
  public ClosableFrame(String name) {
    super(name);
    addWindowListener(new CloseListener());
  }
}
