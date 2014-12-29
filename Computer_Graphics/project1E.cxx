#include <iostream>
#include <cmath>    
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
#include <vtkDoubleArray.h>
#include <vtkDataSetWriter.h>

using std::cerr;
using std::endl;

/////////////////////////////////////////////////////////////////////
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
      double         Z[3];
      double         colors[3][3];
      double         normals[3][3];

  // would some methods for transforming the triangle in place be helpful?
    
};

class Screen
{
  public:
      unsigned char   *buffer;
      int width, height;

  // would some methods for accessing and setting pixels be helpful?
};

struct LightingParameters
{
    LightingParameters(void)
    {
         lightDir[0] = -0.6;
         lightDir[1] = 0;
         lightDir[2] = -0.8;
         Ka = 0.3;
         Kd = 0.7;
         Ks = 5.3;
         alpha = 7.5;
    };
  

    double lightDir[3]; // The direction of the light source
    double Ka;           // The coefficient for ambient lighting.
    double Kd;           // The coefficient for diffuse lighting.
    double Ks;           // The coefficient for specular lighting.
    double alpha;        // The exponent term for specular lighting.
};

LightingParameters lp;



// GetTriangles routine
std::vector<Triangle>
GetTriangles(void)
{
    vtkPolyDataReader *rdr = vtkPolyDataReader::New();
    rdr->SetFileName("proj1e_geometry.vtk");
    cerr << "Reading" << endl;
    rdr->Update();
    cerr << "Done reading" << endl;
    if (rdr->GetOutput()->GetNumberOfCells() == 0)
    {
        cerr << "Unable to open file!!" << endl;
        exit(EXIT_FAILURE);
    }
    vtkPolyData *pd = rdr->GetOutput();

    int numTris = pd->GetNumberOfCells();
    vtkPoints *pts = pd->GetPoints();
    vtkCellArray *cells = pd->GetPolys();
    vtkDoubleArray *var = (vtkDoubleArray *) pd->GetPointData()->GetArray("hardyglobal");
    double *color_ptr = var->GetPointer(0);
    //vtkFloatArray *var = (vtkFloatArray *) pd->GetPointData()->GetArray("hardyglobal");
    //float *color_ptr = var->GetPointer(0);
    vtkFloatArray *n = (vtkFloatArray *) pd->GetPointData()->GetNormals();
    float *normals = n->GetPointer(0);
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
        double *pt = NULL;
        pt = pts->GetPoint(ptIds[0]);
        tris[idx].X[0] = (pt[0]+10)*50.0;
        tris[idx].Y[0] = (pt[1]+10)*50.0;
        tris[idx].Z[0] = (pt[2]-10)*0.05;
        tris[idx].normals[0][0] = normals[3*ptIds[0]+0];
        tris[idx].normals[0][1] = normals[3*ptIds[0]+1];
        tris[idx].normals[0][2] = normals[3*ptIds[0]+2];
        pt = pts->GetPoint(ptIds[1]);
        tris[idx].X[1] = (pt[0]+10)*50.0;
        tris[idx].Y[1] = (pt[1]+10)*50.0;
        tris[idx].Z[1] = (pt[2]-10)*0.05;
        tris[idx].normals[1][0] = normals[3*ptIds[1]+0];
        tris[idx].normals[1][1] = normals[3*ptIds[1]+1];
        tris[idx].normals[1][2] = normals[3*ptIds[1]+2];
        pt = pts->GetPoint(ptIds[2]);
        tris[idx].X[2] = (pt[0]+10)*50.0;
        tris[idx].Y[2] = (pt[1]+10)*50.0;
        tris[idx].Z[2] = (pt[2]-10)*0.05;
        tris[idx].normals[2][0] = normals[3*ptIds[2]+0];
        tris[idx].normals[2][1] = normals[3*ptIds[2]+1];
        tris[idx].normals[2][2] = normals[3*ptIds[2]+2];

        // 1->2 interpolate between light blue, dark blue
        // 2->2.5 interpolate between dark blue, cyan
        // 2.5->3 interpolate between cyan, green
        // 3->3.5 interpolate between green, yellow
        // 3.5->4 interpolate between yellow, orange
        // 4->5 interpolate between orange, brick
        // 5->6 interpolate between brick, salmon
        double mins[7] = { 1, 2, 2.5, 3, 3.5, 4, 5 };
        double maxs[7] = { 2, 2.5, 3, 3.5, 4, 5, 6 };
        unsigned char RGB[8][3] = { { 71, 71, 219 }, 
                                    { 0, 0, 91 },
                                    { 0, 255, 255 },
                                    { 0, 128, 0 },
                                    { 255, 255, 0 },
                                    { 255, 96, 0 },
                                    { 107, 0, 0 },
                                    { 224, 76, 76 } 
                                  };
        for (int j = 0 ; j < 3 ; j++)
        {
            float val = color_ptr[ptIds[j]];
            int r;
            for (r = 0 ; r < 7 ; r++)
            {
                if (mins[r] <= val && val < maxs[r])
                    break;
            }
            if (r == 7)
            {
                cerr << "Could not interpolate color for " << val << endl;
                exit(EXIT_FAILURE);
            }
            double proportion = (val-mins[r]) / (maxs[r]-mins[r]);
            tris[idx].colors[j][0] = (RGB[r][0]+proportion*(RGB[r+1][0]-RGB[r][0]))/255.0;
            tris[idx].colors[j][1] = (RGB[r][1]+proportion*(RGB[r+1][1]-RGB[r][1]))/255.0;
            tris[idx].colors[j][2] = (RGB[r][2]+proportion*(RGB[r+1][2]-RGB[r][2]))/255.0;
        }
    }

    return tris;
}

/////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
    int flatBottom(std::vector<Triangle> &tri) 
    {  
       int count=0;
       for(int i = 0; i < tri.size(); i++) 
       {
          if ( ((tri[i].Y[0] == tri[i].Y[1])&&(tri[i].Y[0] < tri[i].Y[2])) || 
               ((tri[i].Y[0] == tri[i].Y[2])&&(tri[i].Y[0] < tri[i].Y[1])) ||
               ((tri[i].Y[1] == tri[i].Y[2])&&(tri[i].Y[1] < tri[i].Y[0])) )
          count++;
       }
        return count;
    }
   
    int flatTop(std::vector<Triangle> &tri) 
    {  
       int count=0;
       for(int i = 0; i < tri.size(); i++) 
       {
          if ( ((tri[i].Y[0] == tri[i].Y[1])&&(tri[i].Y[0] > tri[i].Y[2])) || 
               ((tri[i].Y[0] == tri[i].Y[2])&&(tri[i].Y[0] > tri[i].Y[1])) ||
               ((tri[i].Y[1] == tri[i].Y[2])&&(tri[i].Y[1] > tri[i].Y[0])) )
          count++;
       }
        return count;
    }
   
    int noFlat(std::vector<Triangle> &tri) 
    {  
       int count=0;
       for(int i = 0; i < tri.size(); i++) 
       {
          if ( (tri[i].Y[0] != tri[i].Y[1])&&
               (tri[i].Y[0] != tri[i].Y[2])&&
               (tri[i].Y[1] != tri[i].Y[2]) )
          count++;
       }
        return count;
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

//////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
   
    std::vector<double> getSideColor(Triangle &tri, int point1, int point2, int scanRow) {
        std::vector<double> sideColor(3);
        double sideY = tri.Y[point1];
        double topY = tri.Y[point2];
       // std::cout<< "\nside: ("<<tri.X[point1]<<" "<<sideY<< ") topY: ("<<tri.X[point2]<<topY<<")";
      // std::cout<<"\n scanRow = " << scanRow ;
        for (int i = 0; i < 3; i++) {        
            if ((topY - sideY) == 0) std::cout<< "\n divide 0; topY- sideY = 0";
            else {
               sideColor[i] = tri.colors[point1][i] + (tri.colors[point2][i]- tri.colors[point1][i]) *((scanRow-sideY)/(topY-sideY));
             //  std::cout <<" " << sideColor[i];
            }
        }
        
        return sideColor;
    }
 ////////////////////////////////////////////////////////////////////////////

   double getSideZ(Triangle &tri, int point1, int point2, int scanRow) {
      double topZ = tri.Z[point2];
      double sideZ = tri.Z[point1];
      double sideY = tri.Y[point1];
      double topY = tri.Y[point2];
      double deepZ;
      if ((topY - sideY) == 0) std::cout<< "\n divide 0; topY- sideY = 0";
      else 
         deepZ = tri.Z[point1] + (tri.Z[point2]- tri.Z[point1]) *((scanRow-sideY)/(topY-sideY));
      return deepZ;

    

   }
/////////////////////////////////////////////////////////////////////////////
   double getPointZ(double leftPointX, double rightPointX, int scanColumn, double leftZ, double rightZ) {
      double pointZ = leftZ + (rightZ - leftZ)*((scanColumn - leftPointX)/(rightPointX - leftPointX));
      return pointZ;
   }
///////////////////////////////////////////////////////////////////////////////

   std::vector<double> getPointColor(double leftPointX, double rightPointX, int scanColumn,     std::vector<double> leftColor, std::vector<double> rightColor) {

   std::vector<double> pointColor(3);
   for(int i = 0; i < 3; i++) {
      pointColor[i] = leftColor[i] + (rightColor[i] - leftColor[i])*((scanColumn - leftPointX)/(rightPointX - leftPointX));
      }
   return pointColor;   
   }
////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
/*
  double calcDiffuse(Triangle &tri) {
     double Ax, Ay, Az, Bx, By, Bz; 
        Ax = tri.X[2] - tri.X[0];
        Ay = tri.Y[2] - tri.Y[0];
        Az = tri.Z[2] - tri.Z[0];
        Bx = tri.X[1] - tri.X[0];
        By = tri.Y[1] - tri.Y[0];
        Bz = tri.Z[1] - tri.Z[0];
     double AcrossB = (Ay*Bz - Az*By)*lightDir[0]  (Az*Bx - Ax*Bz)   (Ax*By - Ay*Bx)  
  } */

  double calcDiffuse(Triangle &tri) {
     

  }


/////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
   
    std::vector<double> getSideVector(Triangle &tri, int point1, int point2, int scanRow) {
        std::vector<double> sideVector(3);
        double sideY = tri.Y[point1];
        double topY = tri.Y[point2];
        for (int i = 0; i < 3; i++) {        
            if ((topY - sideY) == 0) std::cout<< "\n divide 0; topY- sideY = 0";
            else {
               sideVector[i] = tri.normals[point1][i] + (tri.normals[point2][i]- tri.normals[point1][i]) *((scanRow-sideY)/(topY-sideY));
             
            }
        }
        
        return sideVector;
    }
 ////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

   std::vector<double> getPointVector(double leftPointX, double rightPointX, int scanColumn,     std::vector<double> leftVector, std::vector<double> rightVector) {

   std::vector<double> pointVector(3);
   for(int i = 0; i < 3; i++) {
      pointVector[i] = leftVector[i] + (rightVector[i] - leftVector[i])*((scanColumn - leftPointX)/(rightPointX - leftPointX));
      }
 /*
   double absPoint = sqrt(pointVector[0]*pointVector[0] + pointVector[1]*pointVector[1] + pointVector[2]*pointVector[2]);
   for(int i = 0; i < 3; i++) {
      pointVector[i] = pointVector[i]/absPoint;
   }
*/
   return pointVector;   
   }
////////////////////////////////////////////////////////////////////////////

   double getDiffuse(std::vector<double> pointVector) {
      return fabs(pointVector[0]*(lp.lightDir[0]) + pointVector[1]*(lp.lightDir[1]) + pointVector[2]*(lp.lightDir[2]));
   }
////////////////////////////////////////////////////////////////////////////////

  double getSpecular(std::vector<double> pointVector) {
      double specular, LN, VR;
      std::vector<double> R(3);
/*
   double absPoint = sqrt(pointVector[0]*pointVector[0] + pointVector[1]*pointVector[1] + pointVector[2]*pointVector[2]);
   for(int i = 0; i < 3; i++) {
      pointVector[i] = pointVector[i]/absPoint;
   }
*/
      LN = (pointVector[0]*(lp.lightDir[0]) + pointVector[1]*(lp.lightDir[1]) + pointVector[2]*(lp.lightDir[2]));


      for(int i = 0; i < 3; i++) {
         R[i] = 2*(LN)*pointVector[i] - lp.lightDir[i];
      }
      VR = R[0]*0 + R[1]*0 + R[2]*(-1.0);
      specular = fmax(0, 1*(pow(VR, lp.alpha)));
      return specular;
  }

//////////////////////////////////////////////////////////////////////////////
   void drawFlatBottom(Triangle &tri, unsigned char *buffer, double *valueZ) 
   {
    //  for (int j = 0; j <3; j++) { 
     //      std::cout << "\n xyz= (" << tri.X[j] << ", " << tri.Y[j] <<", " <<tri.Z[j] <<");" ; 
     // }
    
      double leftSlope, leftIntercept, rightSlope, rightIntercept;
      int topPoint, leftPoint, rightPoint;
      
      topPoint = getTopPoint(tri);     
      leftPoint = getLeftPoint(tri, topPoint);
      rightPoint = getRightPoint(tri, topPoint);
      double rowMin = ceil441(tri.Y[leftPoint]);
      double rowMax = floor441(tri.Y[topPoint]);     
      int scanRow = setScanRow (rowMin);
      
      while((scanRow <= (int)(rowMax + 0.1)) && (scanRow <1000)) //(1000, 1000)
      {     

          double rightEnd = floor441(getSideEnd(tri, rightPoint, topPoint, scanRow));
          int scanColumn = getScanColumn(tri, leftPoint, topPoint, scanRow); //leftEnd
          std::vector<double> leftColor(3), rightColor(3);         
          leftColor = getSideColor(tri, leftPoint, topPoint, scanRow);
          rightColor = getSideColor(tri, rightPoint, topPoint, scanRow);

          std::vector<double> leftVector(3), rightVector(3);
          leftVector = getSideVector(tri, leftPoint, topPoint, scanRow);
          rightVector = getSideVector(tri, rightPoint, topPoint, scanRow);

          /*
          std::cout<<"\n scanline left "<<scanRow;
          for (int i = 0; i < 3; i++) {        
              std::cout <<", " << leftVector[i];
          }
          std::cout<<" right ";
          for (int i = 0; i < 3; i++) {        
              std::cout <<" " << rightVector[i];
          }
          */
          double leftZ, rightZ;
          leftZ = getSideZ(tri, leftPoint, topPoint, scanRow);
          rightZ = getSideZ(tri, rightPoint, topPoint, scanRow);
          double leftPointX = getSideEnd(tri, leftPoint, topPoint, scanRow);
          double rightPointX = getSideEnd(tri, rightPoint, topPoint, scanRow);
          //std::cout <<"\n " << leftPointX << " " << rightPointX;
          //std::cout <<"\n " << leftZ << " " << rightZ;
     
          while ((scanColumn <= ((int)(rightEnd+ 0.1))) && (scanColumn < 1000))
          {   
              std::vector<double> pointColor(3);
              pointColor = getPointColor(leftPointX, rightPointX, scanColumn, leftColor, rightColor);

              std::vector<double> pointVector(3);
              pointVector = getPointVector(leftPointX, rightPointX, scanColumn, leftVector, rightVector);
              
              /*
              std::cout<<endl;
              for (int i = 0; i < 3; i++) {        
              std::cout <<"  " << pointVector[i];
              }
              */

              double diffuse = getDiffuse(pointVector);
              //std::cout <<"\n diffuse: " << diffuse;

              double specular = getSpecular(pointVector);
              double shading = lp.Ka + lp.Kd*diffuse + lp.Ks*specular; 
              //double shading = lp.Ks*specular;
              double pointZ = getPointZ(leftPointX, rightPointX, scanColumn, leftZ, rightZ); 
              //std::cout <<"\n pointZ: " << pointZ;
              int pointNum = 1000*(scanRow) + scanColumn;
              if (pointZ > valueZ[pointNum]) {
                 valueZ[pointNum] = pointZ;
                 buffer[(pointNum)*3 + 0] = (unsigned char)ceil441(fmin((pointColor[0]*shading),1.0)*255.0);
                 buffer[(pointNum)*3 + 1] = (unsigned char)ceil441(fmin((pointColor[1]*shading),1.0)*255.0);
                 buffer[(pointNum)*3 + 2] = (unsigned char)ceil441(fmin((pointColor[2]*shading),1.0)*255.0);
              }
              scanColumn += 1;     //*(lp.Kd*diffuse)
          }       
           scanRow += 1;
       }
   }


////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////
 void drawFlatTop(Triangle &tri, unsigned char *buffer, double *valueZ) 
   {
    //  for (int j = 0; j <3; j++) { 
    //       std::cout << "\n xyz= (" << tri.X[j] << ", " << tri.Y[j] <<", " <<tri.Z[j] <<");" ; 
    //  }
 
      double leftSlope, leftIntercept, rightSlope, rightIntercept;
      int bottomPoint, leftPoint, rightPoint;
      bottomPoint = getBottomPoint(tri);     
      leftPoint = getLeftPoint(tri, bottomPoint);
      rightPoint = getRightPoint(tri, bottomPoint);
      double rowMin = ceil441(tri.Y[bottomPoint]);
      double rowMax = floor441(tri.Y[rightPoint]);     
      int scanRow = setScanRow (rowMin);
     
      while((scanRow <= (int)(rowMax + 0.1)) && (scanRow <1000)) //(1000, 1000)
      {     
          double rightEnd = floor441(getSideEnd(tri, rightPoint, bottomPoint, scanRow));
          int scanColumn = getScanColumn(tri, leftPoint, bottomPoint, scanRow); //leftEnd
          std::vector<double> leftColor(3), rightColor(3);         
          leftColor = getSideColor(tri, leftPoint, bottomPoint, scanRow);
          rightColor = getSideColor(tri, rightPoint, bottomPoint, scanRow);

          std::vector<double> leftVector(3), rightVector(3);
          leftVector = getSideVector(tri, leftPoint, bottomPoint, scanRow);
          rightVector = getSideVector(tri, rightPoint, bottomPoint, scanRow);


      //  std::cout<<"\n scanline left "<<scanRow;
   /*
          for (int i = 0; i < 3; i++) {        
              std::cout <<", " << leftColor[i];
          }
          std::cout<<" right ";
          for (int i = 0; i < 3; i++) {        
              std::cout <<" " << rightColor[i];
          }
     */   
          double leftZ, rightZ;
          leftZ = getSideZ(tri, leftPoint, bottomPoint, scanRow);
          rightZ = getSideZ(tri, rightPoint, bottomPoint, scanRow);
          double leftPointX = getSideEnd(tri, leftPoint, bottomPoint, scanRow);
          double rightPointX = getSideEnd(tri, rightPoint, bottomPoint, scanRow);
          //std::cout <<"\n " << leftPointX << " " << rightPointX;
          //std::cout <<"\n " << leftZ << " " << rightZ;
     
          while ((scanColumn <= ((int)(rightEnd + 0.1))) && (scanColumn < 1000))
          {   
              std::vector<double> pointColor(3);
              pointColor = getPointColor(leftPointX, rightPointX, scanColumn, leftColor, rightColor);
              double pointZ = getPointZ(leftPointX, rightPointX, scanColumn, leftZ, rightZ); 

              std::vector<double> pointVector(3);
              pointVector = getPointVector(leftPointX, rightPointX, scanColumn, leftVector, rightVector);

              double diffuse = getDiffuse(pointVector);
              double specular = getSpecular(pointVector);
              double shading = lp.Ka + lp.Kd*diffuse + lp.Ks*specular; 
              //double shading = lp.Ks*specular;
              //std::cout <<"\n pointZ: " << pointZ;
              int pointNum = 1000*(scanRow) + scanColumn;
              if (pointZ > valueZ[pointNum]) {
                 valueZ[pointNum] = pointZ;
                 buffer[(pointNum)*3 + 0] = (unsigned char)ceil441(fmin((pointColor[0]*shading),1.0)*255.0);
                 buffer[(pointNum)*3 + 1] = (unsigned char)ceil441(fmin((pointColor[1]*shading),1.0)*255.0);
                 buffer[(pointNum)*3 + 2] = (unsigned char)ceil441(fmin((pointColor[2]*shading),1.0)*255.0);
              }
              scanColumn += 1;  
          }       
          scanRow += 1;
       }
   }
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////   
   void drawNoFlat(Triangle &tri, unsigned char *buffer, double *valueZ) 
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
      std::vector<double> pointColor(3);
      std::vector<double> pointVector(3);
      double sideY = tri.Y[topPoint];
      double topY = tri.Y[bottomPoint];
      for (int i = 0; i < 3; i++) {        
            if ((topY - sideY) == 0) std::cout<< "\n divide 0; topY- sideY = 0";
            else {
               pointColor[i] = tri.colors[topPoint][i] + (tri.colors[bottomPoint][i]- tri.colors[topPoint][i]) *((tri.Y[middlePoint]-sideY)/(topY-sideY));
               pointVector[i] = tri.normals[topPoint][i] + (tri.normals[bottomPoint][i]- tri.normals[topPoint][i]) *((tri.Y[middlePoint]-sideY)/(topY-sideY));
            }
      }


  // double absPoint = sqrt(pointVector[0]*pointVector[0] + pointVector[1]*pointVector[1] + pointVector[2]*pointVector[2]);
 //  for(int i = 0; i < 3; i++) {
 //     pointVector[i] = pointVector[i]/absPoint;
 //  }


      double deepZ = tri.Z[topPoint] + (tri.Z[bottomPoint]- tri.Z[topPoint]) *((tri.Y[middlePoint]-tri.Y[topPoint])/(tri.Y[bottomPoint]-tri.Y[topPoint]));


     newTemp.Z[bottomPoint] = deepZ;
     for(int j=0; j<3;j++) {
        newTemp.colors[bottomPoint][j] = pointColor[j];
        newTemp.normals[bottomPoint][j] = pointVector[j];
     }
      drawFlatBottom(newTemp, buffer,valueZ);  

      newTemp.X[topPoint] = tri.X[bottomPoint];
      newTemp.Y[topPoint] = tri.Y[bottomPoint];
      newTemp.Z[topPoint] = tri.Z[bottomPoint];
      for(int j=0; j<3;j++) {
        newTemp.colors[topPoint][j] = tri.colors[bottomPoint][j];
        newTemp.normals[topPoint][j] = tri.normals[bottomPoint][j];
      }

      drawFlatTop(newTemp, buffer,valueZ);
   }

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////




int main() {
   vtkImageData *image = NewImage(1000,1000);
   unsigned char *buffer = 
     (unsigned char *) image->GetScalarPointer(0,0,0);
   int npixels = 1000*1000;
   double *valueZ = new double[npixels];

   for (int i = 0 ; i < npixels*3; i++) {
       buffer[i] = 0;
   }

    for (int i = 0 ; i < npixels; i++) {
       valueZ[i] = -1;
   }

   std::vector<Triangle> triangles = GetTriangles();
   
   Screen screen;
   screen.buffer = buffer;
   screen.width = 1000;
   screen.height = 1000;

   // YOUR CODE GOES HERE TO DEPOSIT TRIANGLES INTO PIXELS USING THE SCANLINE ALGORITHM
/*
   for (int i = 4; i < 5; i++) {
       std::cout << "\n " << i; 
       for (int j = 0; j <3; j++) { 
           std::cout << "\n xyz= (" << triangles[i].X[j] << ", " << triangles[i].Y[j] <<", " <<triangles[i].Z[j] <<");" ; 
           for (int k = 0; k < 3; k++) {
               std::cout <<" "<< triangles[i].colors[j][k];
           }
       }
   }
*/
    std::cout<<"\n size of vector is " << triangles.size() << "\n";
    int numFlatBottom = flatBottom (triangles);
    std::cout <<"\n Num of flatBottom is: "<< numFlatBottom << "\n";   
    int numFlatTop = flatTop (triangles);
    std::cout <<"\n Num of flattop is: "<< numFlatTop << "\n";  
    int numNoFlat = noFlat (triangles);
    std::cout <<"\n Num of noflat is: "<< numNoFlat << "\n";  
    std::cout <<"\n Num of three is: "<< numNoFlat + numFlatTop + numFlatBottom << "\n";
    int countBottom=0, countTop=0, countNoFlat=0;

    //drawFlatTop(triangles[35365], buffer, valueZ);
  /*  
    for (int j = 0; j <3; j++) { 
           std::cout << "\n xyz= (" << triangles[28273].X[j] << ", " << triangles[28273].Y[j] <<", " <<triangles[28273].Z[j] <<");" ; 
    }
*/
   // drawNoFlat(triangles[28273], buffer, valueZ);


   for(int i = 0; i < triangles.size(); i++) 
   //for(int i = 0; i < 90; i++) 
   {     
      if(isFlatBottom(triangles[i]))
      {  
         //std::cout<<"\n triangle: " << i ;  
         drawFlatBottom(triangles[i], buffer, valueZ);
         countBottom++;       
      }

      else if(isFlatTop(triangles[i]))
      {
         drawFlatTop(triangles[i], buffer, valueZ);
         countTop++;      
      }
      
      else if(isNoFlat(triangles[i]))
      {        
         drawNoFlat(triangles[i], buffer, valueZ);
         countNoFlat++;
      }
   }
  
   //std::cout<<"\n flatBottom " << countBottom<< endl;
   WriteImage(image, "allTriangles");
   delete [] valueZ;
}

