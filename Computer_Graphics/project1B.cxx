#include <iostream>
#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkPNGWriter.h>
#include <vtkPointData.h>

using std::cerr;
using std::endl;

double ceil441(double f)
{
    return ceil(f-0.00001);
}

double floor441(double f)
{
    return floor(f+0.00001);
}



vtkImageData *
NewImage(int width, int height)
{
    vtkImageData *img = vtkImageData::New();
    img->SetDimensions(width, height, 1);
    img->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    return img;
}

void
WriteImage(vtkImageData *img, const char *filename)
{
   std::string full_filename = filename;
   full_filename += ".png";
   vtkPNGWriter *writer = vtkPNGWriter::New();
   writer->SetInputData(img);
   writer->SetFileName(full_filename.c_str());
   writer->Write();
   writer->Delete();
}

class Triangle
{
  public:
      double         X[3];
      double         Y[3];
      unsigned char color[3];

  // would some methods for transforming the triangle in place be helpful?
};

class Screen
{
  public:
      unsigned char   *buffer;
      int width, height;

  // would some methods for accessing and setting pixels be helpful?
};

std::vector<Triangle>
GetTriangles(void)
{
   std::vector<Triangle> rv(100);

   unsigned char colors[6][3] = { {255,128,0}, {255, 0, 127}, {0,204,204}, 
                                  {76,153,0}, {255, 204, 204}, {204, 204, 0}};
   for (int i = 0 ; i < 100 ; i++)
   {
       int idxI = i%10;
       int posI = idxI*100;
       int idxJ = i/10;
       int posJ = idxJ*100;
       int firstPt = (i%3);
       rv[i].X[firstPt] = posI;
       if (i == 50)
           rv[i].X[firstPt] = -10;
       rv[i].Y[firstPt] = posJ;
       rv[i].X[(firstPt+1)%3] = posI+99;
       rv[i].Y[(firstPt+1)%3] = posJ;
       rv[i].X[(firstPt+2)%3] = posI+i;
       rv[i].Y[(firstPt+2)%3] = posJ+10*(idxJ+1);
       if (i == 95)
          rv[i].Y[(firstPt+2)%3] = 1050;
       rv[i].color[0] = colors[i%6][0];
       rv[i].color[1] = colors[i%6][1];
       rv[i].color[2] = colors[i%6][2];
   }

   return rv;
}

int main()
{
   vtkImageData *image = NewImage(1000, 1000);
   unsigned char *buffer = 
     (unsigned char *) image->GetScalarPointer(0,0,0);
   int npixels = 1000*1000;
   for (int i = 0 ; i < npixels*3 ; i++)
       buffer[i] = 0;

   std::vector<Triangle> triangles = GetTriangles();
   
   Screen screen;
   screen.buffer = buffer;
   screen.width = 1000;
   screen.height = 1000;

   // YOUR CODE GOES HERE TO DEPOSIT TRIANGLES INTO PIXELS USING THE SCANLINE ALGORITHM
/*
   for (int j=0; j < 100; j++) {
       std::cout << "\n " << j; 
   for (int i = 0; i <3; i++) { 
       std::cout << ", " << triangles[j].X[i] << "  " << triangles[j].Y[i]; //  << " clolor: " << (int)triangles[j].color[i];
   }
 }
*/

  double leftPoint_x, leftPoint_y, rightPoint_x,  rightPoint_y, topPoint_x, topPoint_y;
  double dx, dy, leftSlope, leftIntercept, rightSlope, rightIntercept;

  for (int i = 0; i < 100; i++) 
  {
     //std::cout<<"\n\n i= "<< i; 
     double top = fmax(triangles[i].Y[0], fmax(triangles[i].Y[1], triangles[i].Y[2]));
     double left = fmax(triangles[i].X[0], fmax(triangles[i].X[1], triangles[i].X[2]));
     for (int j = 0; j < 3; j++) 
     {      
        if (triangles[i].Y[j] == top) 
        {
          topPoint_x = triangles[i].X[j];
          topPoint_y = triangles[i].Y[j];
          //std::cout<<"\n top (" <<topPoint_x << " "<<topPoint_y <<")";
         }
        else if (triangles[i].X[j] == left) 
        {
          leftPoint_x = triangles[i].X[j];
          leftPoint_y = triangles[i].Y[j];
          //std::cout<<"\n left (" <<leftPoint_x << " "<<leftPoint_y <<")";         
         }
        else if ((triangles[i].Y[j] != top)&&(triangles[i].X[j] != left))
        {
          rightPoint_x = triangles[i].X[j];
          rightPoint_y = triangles[i].Y[j];
          // std::cout<<"\n right (" <<rightPoint_x << " "<<rightPoint_y <<")";
        }
      }  
   
  if(topPoint_x != rightPoint_x) 
   {
       dx = topPoint_x - rightPoint_x; 
       dy = topPoint_y - rightPoint_y; 
       rightSlope = dy/dx;
       rightIntercept = rightPoint_y - rightSlope* rightPoint_x;
    }

   if(topPoint_x != leftPoint_x) 
   {
      dx = topPoint_x - leftPoint_x; 
      dy = topPoint_y - leftPoint_y; 
      leftSlope = dy/dx;    
      leftIntercept = leftPoint_y - leftPoint_x*leftSlope;
      //std::cout<<"\n leftSlope "<< i << " "<< leftSlope << " intecept "<<leftIntercept;   
    }


   double rowMin = ceil441(rightPoint_y);
   double rowMax = floor441(topPoint_y);
   int scanRow = (int)(rowMin + 0.1);
 
   while((scanRow <= rowMax) && (scanRow <1000)) 
   {
       double leftEnd ;
       double rightEnd;
       if (topPoint_x == leftPoint_x) 
          leftEnd = leftPoint_x;
       else 
       {
           //leftEnd = floor441((scanRow - leftIntercept)/1);
           leftEnd = floor441((scanRow - leftIntercept)/leftSlope);
        }

       if (topPoint_x == rightPoint_x) 
          rightEnd = rightPoint_x;
       else  
       {
          rightEnd = ceil441((scanRow - rightIntercept)/rightSlope);
       }

     //  std::cout << "\nrow:"<< (int)(scanRow + 0.1) << ", two end: " << (int) (rightEnd + 0.1) << " " << (int) (leftEnd + 0.1);
       int scanColumn;
       if(rightEnd < 0) 
          scanColumn = 0;
       else 
       {
          scanColumn = (int)(rightEnd + 0.1);
       }  

       while ((scanColumn <= ((int)(leftEnd+ 0.1))) && (scanColumn <1000))
       {
          buffer[(1000*(scanRow) + scanColumn)*3 + 0] = triangles[i].color[0];
          buffer[(1000*(scanRow) + scanColumn)*3 + 1] = triangles[i].color[1];
          buffer[(1000*(scanRow) + scanColumn)*3 + 2] = triangles[i].color[2];
          scanColumn += 1;  
        }        
       scanRow +=1;
   }
}

   WriteImage(image, "allTriangles");
   std::cout<<"\n";  
}
