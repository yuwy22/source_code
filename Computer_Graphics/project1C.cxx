#include <iostream>
#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkPNGWriter.h>
#include <vtkPolyLine.h>
#include <vtkTriangle.h>
#include <vtkExtractVectorComponents.h>
#include <vtkPoints.h>
#include <vtkCell.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>

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

// GetTriangles routine
std::vector<Triangle>
GetTriangles(void)
{
    vtkPolyDataReader *rdr = vtkPolyDataReader::New();
    rdr->SetFileName("proj1c_geometry.vtk");
    cerr << "Reading" << endl;
    rdr->Update();
    if (rdr->GetOutput()->GetNumberOfCells() == 0)
    {
        cerr << "Unable to open file!!" << endl;
        exit(EXIT_FAILURE);
    }
    vtkPolyData *pd = rdr->GetOutput();
    int numTris = pd->GetNumberOfCells();
    vtkPoints *pts = pd->GetPoints();
    vtkCellArray *cells = pd->GetPolys();
    vtkFloatArray *colors = (vtkFloatArray *) pd->GetPointData()->GetArray("color_nodal");
    float *color_ptr = colors->GetPointer(0);
    std::vector<Triangle> tris(numTris);
    vtkIdType npts;
    vtkIdType *ptIds;
    int idx;
    for (idx = 0, cells->InitTraversal() ; cells->GetNextCell(npts, ptIds) ; idx++)
    {
        if (npts != 3)
        {
            cerr << "Non-triangles!! ???" << endl;
            exit(EXIT_FAILURE);
        }
        tris[idx].X[0] = pts->GetPoint(ptIds[0])[0];
        tris[idx].X[1] = pts->GetPoint(ptIds[1])[0];
        tris[idx].X[2] = pts->GetPoint(ptIds[2])[0];
        tris[idx].Y[0] = pts->GetPoint(ptIds[0])[1];
        tris[idx].Y[1] = pts->GetPoint(ptIds[1])[1];
        tris[idx].Y[2] = pts->GetPoint(ptIds[2])[1];
        tris[idx].color[0] = (unsigned char) color_ptr[4*ptIds[0]+0];
        tris[idx].color[1] = (unsigned char) color_ptr[4*ptIds[0]+1];
        tris[idx].color[2] = (unsigned char) color_ptr[4*ptIds[0]+2];
    }
    cerr << "Done reading" << endl;

    return tris;
}


/////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
    bool isFlatBottom(Triangle &tri) 
    {       
          if ( ((tri.Y[0] == tri.Y[1])&&(tri.Y[0] < tri.Y[2])) || 
               ((tri.Y[0] == tri.Y[2])&&(tri.Y[0] < tri.Y[1])) ||
               ((tri.Y[1] == tri.Y[2])&&(tri.Y[1] < tri.Y[0])) )
               return true;
          else return false;        
    }
   
    bool isFlatTop(Triangle &tri) 
    {        
          if ( ((tri.Y[0] == tri.Y[1])&&(tri.Y[0] > tri.Y[2])) || 
               ((tri.Y[0] == tri.Y[2])&&(tri.Y[0] > tri.Y[1])) ||
               ((tri.Y[1] == tri.Y[2])&&(tri.Y[1] > tri.Y[0])) )
               return true;
          else return false;         
    }

    bool isNoFlat(Triangle &tri) 
    {       
          if ( (tri.Y[0] != tri.Y[1])&&
               (tri.Y[0] != tri.Y[2])&&
               (tri.Y[1] != tri.Y[2]) )
               return true;
          else return false;       
    }
///////////////////////////////////////////////////////////////////////////////////

   int getTopPoint(Triangle &tri) 
   {
      int topPoint;
      double top = fmax(tri.Y[0], fmax(tri.Y[1], tri.Y[2]));
      for (int i = 0; i < 3; i ++) 
      {
         if (tri.Y[i] == top) return i;
      }    
   }
//////////////////////////////////////////////////////////////////
   int getLeftPoint(Triangle &tri, int topPoint)
   {   
       int leftPoint = 200, point1 = 100, point2 = 100;
       for (int i = 0; i < 3; i++) 
       {
          if(i == topPoint) continue;
          else if (point1 == 100) point1 = i;
          else if (point2 == 100) point2 = i;
       }
       if (tri.X[point1] < tri.X[point2]) leftPoint = point1;
       else leftPoint = point2;
      // if (tri.X[point1] < tri.X[point2]) std::cout<< " \n left point1"<< point1;
      // else if (tri.X[point1] > tri.X[point2]) std::cout<< " \n left point2"<< point2;
      return leftPoint;
   }
//////////////////////////////////////////////////////////////////////////
   int getRightPoint(Triangle &tri, int topPoint)
    {   
       int rightPoint = 200, point1 = 100, point2 = 100;
       for (int i = 0; i < 3; i++) 
       {
          if(i == topPoint) continue;
          else if (point1 == 100) point1 = i;
          else if (point2 == 100) point2 = i;
       }
       if (tri.X[point1] < tri.X[point2]) rightPoint = point2;
       else rightPoint = point1;
       return rightPoint;
   }
 
///////////////////////////////////////////////////////////////////////////////////

      double getSlope(Triangle &tri, int topPoint, int sidePoint) 
      {
         double dx, dy, slope;
         dx = tri.X[topPoint] - tri.X[sidePoint];
         dy = tri.Y[topPoint] - tri.Y[sidePoint];
        // std::cout<<" \n\ntopPoint "<<topPoint << " sidePoint "<< sidePoint;
        // std::cout<<"\n tri.x 0 1 2: ( "<<tri.X[0] << ", "<< tri.X[1] <<", "<< tri.X[2];;
        // std::cout<<"\n dx is "<<dx<<", dy is: "<< dy<<" ";
         if (dx == 0) std::cerr<<"\n slope dy/dx dx is zero\n"; 
         else slope = dy/dx;
       //  std::cout<< "\n slope is " << slope;
         return slope;
      }
////////////////////////////////////////////////////////////////////////////////
      double getIntercept(Triangle &tri, double slope, int Point)
      {   
         double intercept = tri.Y[Point] - slope*tri.X[Point];
         return intercept;
      }
 //////////////////////////////////////////////////////////////////////////////////     
     int setScanRow (double rowMin) {
         int scanRow;
         if(rowMin<0) scanRow = 0;
         else scanRow = (int)(rowMin+0.1);
         return scanRow;
     }

/////////////////////////////////////////////////////////////////////////////////
     double getSideEnd(Triangle &tri, int point1, int point2, int scanRow) 
     {    
          double sideEnd;
          if (tri.X[point1] == tri.X[point2]) 
             sideEnd = tri.X[point1];
          else 
          {  
             double slope = getSlope(tri, point1, point2);
             double intercept = getIntercept(tri, slope, point2);
             sideEnd = (scanRow - intercept)/slope;
          }
          return sideEnd;
      }
/////////////////////////////////////////////////////////////////////////////////
      int getScanColumn(Triangle &tri, int leftPoint, int topPoint, int scanRow) 
      {    
          int scanColumn;
          double leftEnd = getSideEnd(tri, leftPoint, topPoint, scanRow);
          if(leftEnd < 0.0) {
              scanColumn = 0;
          }
          else 
          {
             scanColumn = (int)(ceil441(leftEnd) + 0.1);
          }  
          return scanColumn; 
      }

////////////////////////////////////////////////////////////////////////////////
   void drawFlatBottom(Triangle &tri, unsigned char *buffer) 
   {
      double leftSlope, leftIntercept, rightSlope, rightIntercept;
      int topPoint, leftPoint, rightPoint;
      
      topPoint = getTopPoint(tri);     
      leftPoint = getLeftPoint(tri, topPoint);
      rightPoint = getRightPoint(tri, topPoint);
      double rowMin = ceil441(tri.Y[leftPoint]);
      double rowMax = floor441(tri.Y[topPoint]);     
      int scanRow = setScanRow (rowMin);

      while((scanRow <= (int)(rowMax + 0.1)) && (scanRow <1344)) //(1786, 1344)
      {     
          double rightEnd = floor441(getSideEnd(tri, rightPoint, topPoint, scanRow));
          int scanColumn = getScanColumn(tri, leftPoint, topPoint, scanRow);
          while ((scanColumn <= ((int)(rightEnd+ 0.1))) && (scanColumn <1786))
          {
              buffer[(1786*(scanRow) + scanColumn)*3 + 0] = tri.color[0];
              buffer[(1786*(scanRow) + scanColumn)*3 + 1] = tri.color[1];
              buffer[(1786*(scanRow) + scanColumn)*3 + 2] = tri.color[2];
              scanColumn += 1;  
          }        
           scanRow +=1;
       }
   }
//////////////////////////////////////////////////////////////////////////////////////

   int getBottomPoint(Triangle &tri) 
   {
      double bottom = fmin(tri.Y[0], fmin(tri.Y[1], tri.Y[2]));
      for (int i = 0; i < 3; i ++) 
      {
         if (tri.Y[i] == bottom) return i;
      }    
   }
/////////////////////////////////////////////////////////////////////////////////
 
   void drawFlatTop(Triangle &tri, unsigned char *buffer) 
   {
      double leftSlope, leftIntercept, rightSlope, rightIntercept;
      int bottomPoint, leftPoint, rightPoint;
      
      bottomPoint = getBottomPoint(tri);     
      leftPoint = getLeftPoint(tri, bottomPoint);
      rightPoint = getRightPoint(tri, bottomPoint);
      double rowMin = ceil441(tri.Y[bottomPoint]);
      double rowMax = floor441(tri.Y[rightPoint]);     
      int scanRow = setScanRow (rowMin);

      while((scanRow <= (int)(rowMax + 0.1)) && (scanRow <1344)) //(1786, 1344)
      {     
          double rightEnd = floor441(getSideEnd(tri, rightPoint, bottomPoint, scanRow));
          int scanColumn = getScanColumn(tri, leftPoint, bottomPoint, scanRow);
          while ((scanColumn <= ((int)(rightEnd+ 0.1))) && (scanColumn <1786))
          {
              buffer[(1786*(scanRow) + scanColumn)*3 + 0] = tri.color[0];
              buffer[(1786*(scanRow) + scanColumn)*3 + 1] = tri.color[1];
              buffer[(1786*(scanRow) + scanColumn)*3 + 2] = tri.color[2];
              scanColumn += 1;  
          }        
           scanRow += 1;
       }
   }
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////   
   void drawNoFlat(Triangle &tri, unsigned char *buffer) 
   {     
      int topPoint, bottomPoint, middlePoint;
      topPoint = getTopPoint(tri); 
      bottomPoint = getBottomPoint(tri);
      middlePoint = 3 - (topPoint + bottomPoint);

      double slope, intercept, middleX;
      if(tri.X[topPoint] == tri.X[bottomPoint]) 
      {
          middleX = tri.X[topPoint];
      }
      else 
      {
          slope = getSlope(tri, topPoint, bottomPoint);
          intercept = getIntercept(tri, slope, topPoint);
          middleX = (tri.Y[middlePoint] - intercept)/slope;
      }
      Triangle newTemp = tri;
      newTemp.X[bottomPoint] = middleX;
      newTemp.Y[bottomPoint] = tri.Y[middlePoint];
      drawFlatBottom(newTemp, buffer);  
      newTemp.X[topPoint] = tri.X[bottomPoint];
      newTemp.Y[topPoint] = tri.Y[bottomPoint];
      drawFlatTop(newTemp, buffer);
   }

////////////////////////////////////////////////////////////////////////////////////

int main() {
   vtkImageData *image = NewImage(1786, 1344);
   unsigned char *buffer = 
     (unsigned char *) image->GetScalarPointer(0,0,0);
   int npixels = 1786*1344;
   for (int i = 0 ; i < npixels*3 ; i++)
       buffer[i] = 0;

   std::vector<Triangle> triangles = GetTriangles();
   
   Screen screen;
   screen.buffer = buffer;
   screen.width = 1786;
   screen.height = 1344;

   // YOUR CODE GOES HERE TO DEPOSIT TRIANGLES INTO PIXELS USING THE SCANLINE ALGORITHM
  
   int countBottom=0, countTop=0, countNoFlat=0;
   for(int i = 0; i < triangles.size(); i++) 
   {     
      if(isFlatBottom(triangles[i]))
      {        
         drawFlatBottom(triangles[i], buffer);
         countBottom++;       
      }

      else if(isFlatTop(triangles[i]))
      {
         drawFlatTop(triangles[i], buffer);
         countTop++;      
      }
      
      else if(isNoFlat(triangles[i]))
      {        
         drawNoFlat(triangles[i], buffer);
         countNoFlat++;
      }
   }
   
   WriteImage(image, "allTriangles");
}


  //size of vector is 2566541

 //Num of flatBottom is: 192

 //Num of flatBottom is: 184

 //Num of flatBottom is: 2566165

 //Num of three is: 2566541
