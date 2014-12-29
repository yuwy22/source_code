

import java.util.Scanner;

public class BackTracking {
	
	    public static int totalStep= 0;
	    public static int count = 0;

	    public static void main(String[] args) {
	    	
	    	Scanner input = new Scanner(System.in);
	    	
	    	System.out.println("Enter a String of minuend, Five Characters at most, Such as: 'EIGHT', enter here: ");
	    	String ss1= input.nextLine();
	      	
	    	System.out.println("Enter a String of subtrahend, Four Characters or less,  Such as: 'FOUR', enter here: ");
	    	String ss2= input.nextLine();
	    	
	    	System.out.println("Enter a String of difference, Four Characters or less,  Such as: 'FOUR', enter here: ");
	    	String ss3= input.nextLine();
	    	
	    	input.close();
	    	
	    	char[] s1 = new char[10];
            char[] s2 = new char[10];
            char[] s3 = new char[10];
            s1=ss1.toCharArray();
            s2=ss2.toCharArray();
            s3=ss3.toCharArray();
            
            
	    	System.out.println("Minuend is n" + ss1);
            System.out.println("Subtrahends is  " + ss2);
            System.out.println("Difference is  " + ss3);
            
            int[] assinged = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            char[] c = new char[20];

            int[] val = new int[10];
           
            
            int flag=0;
        	
	        for(int i=0;i<s1.length;i++)
	        {
	            for(int j=0;j<=count;j++)
		        {
	                if(s1[i]!=c[j])
		            flag=1;
	                else
	                {
		                flag =0;
		                break;
		            }
		        }
	            if(flag==1)
		        c[count++] =s1[i];
	        }

	        for(int i=0;i<s2.length;i++)
	        {
                for(int j=0;j<=count;j++)
	            {
	                if(s2[i]!=c[j])
		            	flag=1;
	                else
	                {
		                flag =0;
		                break;
		            }
	             }
                    if(flag==1)
	             c[count++] =s2[i];
	        }
	        
	        for(int i=0;i<s3.length;i++)
	        {
	            for(int j=0;j<=count;j++)
		        {
	                if(s3[i]!=c[j])
		            flag=1;
	                else
	                {
		                flag =0;
		                break;
		            }
		        }
	            if(flag==1)
		        c[count++] =s3[i];
	        }
	        
	       if (count>10) 
	    	   System.out.println("Sorry, the characters is more than 10, and there are only 10 digits from 0-9. No Answer is found"); 
	               
	       else if ((solve(0, assinged, val, s1, s2, s3, c))==false)
	    	   System.out.println("Sorry, no answer");  
	   
	       System.out.println("Total Step Is " + totalStep);  
	       
	    }
	        
	    public static boolean verify(int val[], char[] s1, char[] s2, char[] s3, char[] c)
	        {
	    	       
	    	    long n1=0,n2=0,n3=0;
		        long power=1;
		        char ch;
		        int i=s1.length-1;
		        int in1;
		        int flag=0;
		        while(i>=0)
			    {
			        ch=s1[i];
			        in1=0;
			        while(in1!=count)
			        {
			            if(c[in1]==ch)
				       		break;
			            else
				        	in1++;
			        }
				    n1+=power*val[in1];
				    power *=10;
				    i--;
			    }
		        
		        power=1;
		        i=s2.length-1;
		        while(i>=0)
			    {
				    ch=s2[i];
				    in1=0;
				    while(in1!=count)
				    {
				        if(c[in1]==ch)
					    	break;
				        else
					        in1++;
				    }
					n2+=power*val[in1];
					power *=10;
					i--;
			    }
		        
		        power=1;
		        i=s3.length-1;
		        while(i>=0)
			    {
				    ch=s3[i];
				    in1=0;
				    while(in1!=count)
				    {
				        if(c[in1]==ch)
					    	break;
				        else
					        in1++;
				    }
					n3+=power*val[in1];
					power *=10;
					i--;
			    }
		        		          
				if((n1-n2==n3) && (n1>(Math.pow(10,s1.length-1))))
				{    
					System.out.print(s1); 
					System.out.println("   " + n1);  
					System.out.print(s2); 
					System.out.println("   - " + n2);  
					System.out.println("-------------- ");  
					System.out.print(s3); 
					System.out.println("    =" + n3);  
					System.out.println();  
					System.out.println();  
					flag=1;
	             }
				 if(flag!=0)
			            return true;
		            else
			            return false;					             
			}
                             		    	   	    	   	      
	    public static boolean solve(int ind, int[] temp1, int[] val,char[] s1, char[] s2, char[] s3, char[] c)
        {
	    	totalStep++; 
	    	int [] temp2 = new int[10];
	        int flag=0;
	         
	        for(int i=0;i<10;i++)
	        {
		        if(temp1[i]==0)
		        {
		            for(int j=0;j<10;j++)
			           temp2[j]=temp1[j];
		            
	                temp2[i]=1;
	                val[ind]=i;
	                if(ind==(count-1))
	                          {	                	        
		                        if((verify(val, s1, s2, s3, c))==true)
		                	    flag=1;
			                  }
	                 else
	                          {
                                if ((solve(ind+1,temp2,val,s1, s2, s3, c))==true)
                        		flag=1;                         
                               }
	            }
            }
       
	        if(flag!=0)
	            return true;
            else
	            return false;
        }     
}