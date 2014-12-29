#include <iostream>
#include <sstream>
#include <string.h> 
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
using namespace std;
int cameraPosition;

/////////////////////////////////////////////////////

double ceil441(double f)
{
    return ceil(f-0.00001);
}


double floor441(double f)
{
    return floor(f+0.00001);
}
////////////////////////////////////////////////////


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
   string str1;
   stringstream convert;
   convert << cameraPosition;
   str1 = convert.str();
   std::string full_filename = filename;
   full_filename = full_filename + str1 + ".png";
   vtkPNGWriter *writer = vtkPNGWriter::New();
   writer->SetInputData(img);
   writer->SetFileName(full_filename.c_str());
   writer->Write();
   writer->Delete();
}
////////////////////////////////////


struct LightingParameters
{
    LightingParameters(void)
    {
         lightDir[0] = 0.6;
         lightDir[1] = 0;
         lightDir[2] = 0.8;
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
////////////////////////////////////////////////////////////


class Matrix
{
  public:
    double          A[4][4];

    void            TransformPoint(const double *ptIn, double *ptOut);
    static Matrix   ComposeMatrices(const Matrix &, const Matrix &);
    void            Print(ostream &o);
};

void
Matrix::Print(ostream &o)
{
    for (int i = 0 ; i < 4 ; i++)
    {
        char str[256];
        sprintf(str, "(%.7f %.7f %.7f %.7f)\n", A[i][0], A[i][1], A[i][2], A[i][3]);
        o << str;
    }
}

Matrix
Matrix::ComposeMatrices(const Matrix &M1, const Matrix &M2)
{
    Matrix rv;
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
        {
            rv.A[i][j] = 0;
            for (int k = 0 ; k < 4 ; k++)
                rv.A[i][j] += M1.A[i][k]*M2.A[k][j];
        }

    return rv;
}

void
Matrix::TransformPoint(const double *ptIn, double *ptOut)
{
    ptOut[0] = ptIn[0]*A[0][0]
             + ptIn[1]*A[1][0]
             + ptIn[2]*A[2][0]
             + ptIn[3]*A[3][0];
    ptOut[1] = ptIn[0]*A[0][1]
             + ptIn[1]*A[1][1]
             + ptIn[2]*A[2][1]
             + ptIn[3]*A[3][1];
    ptOut[2] = ptIn[0]*A[0][2]
             + ptIn[1]*A[1][2]
             + ptIn[2]*A[2][2]
             + ptIn[3]*A[3][2];
    ptOut[3] = ptIn[0]*A[0][3]
             + ptIn[1]*A[1][3]
             + ptIn[2]*A[2][3]
             + ptIn[3]*A[3][3];
}
////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
 class Camera
 {
  public:
    double          near, far;
    double          angle;
    double          position[3];
    double          focus[3];
    double          up[3];

    Matrix          ViewTransform(void);
    Matrix          CameraTransform(void);
    Matrix          DeviceTransform(void);
    
    Matrix getTotalMatrix() 
    {
          Matrix totalMatrix;
          Matrix camera1= CameraTransform();
          Matrix camera2= ViewTransform();
          Matrix camera3= DeviceTransform();
          totalMatrix = Matrix::ComposeMatrices(
                Matrix::ComposeMatrices(camera1, camera2), camera3);
          return totalMatrix;
    }

    std::vector<double> getV3() 
    {
       std::vector<double> v_3(3);
       for(int i=0; i<3; i++) 
       {
           v_3[i] = position[i] - focus[i];
       }
    double absValue = sqrt(v_3[0]*v_3[0] + 
                              v_3[1]*v_3[1] + 
                              v_3[2]*v_3[2]);
       for(int i = 0; i < 3; i++) 
       {
           if (absValue != 0)
           v_3[i] = v_3[i]/absValue;
       }

       //cout<< "\n v_3 "<<" = " << v_3[0]<< "  " << v_3[1] << "  " << v_3[2];
        
       return v_3;
    }

    std::vector<double> getV1(std::vector<double> v3) 
    {
       std::vector<double> v_1(3);
       v_1[0] = up[1]*v3[2] - up[2]*v3[1];
       v_1[1] = up[2]*v3[0] - up[0]*v3[2];
       v_1[2] = up[0]*v3[1] - up[1]*v3[0];
       double absValue = sqrt(v_1[0]*v_1[0] + 
                              v_1[1]*v_1[1] + 
                              v_1[2]*v_1[2]);
       for(int i = 0; i < 3; i++) {
          if (absValue != 0)
          v_1[i] = v_1[i]/absValue;
       }

       //cout<< "\n v_1 "<<" = " << v_1[0]<< "  " << v_1[1] << "  " << v_1[2];
        
       return v_1;
    }

    std::vector<double> getV2(std::vector<double> v1, std::vector<double> v3) 
    {
       std::vector<double> v_2(3);
       
       v_2[0] = v3[1]*v1[2] - v3[2]*v1[1];
       v_2[1] = v3[2]*v1[0] - v3[0]*v1[2];
       v_2[2] = v3[0]*v1[1] - v3[1]*v1[0];
       double absValue = sqrt(v_2[0]*v_2[0] + 
                              v_2[1]*v_2[1] + 
                              v_2[2]*v_2[2]);
       for(int i = 0; i < 3; i++) {
          if (absValue != 0)
          v_2[i] = v_2[i]/absValue;
       }

       //cout<< "\n v_2"<<" = " << v_2[0]<< "  " << v_2[1] << "  " << v_2[2];
       return v_2;
    }
////////////////////////////////////////////////////


///////////
 };


//////////////////////////////////////////////////////
double SineParameterize(int curFrame, int nFrames, int ramp)
{
    int nNonRamp = nFrames-2*ramp;
    double height = 1./(nNonRamp + 4*ramp/M_PI);
    if (curFrame < ramp)
    {
        double factor = 2*height*ramp/M_PI;
        double eval = cos(M_PI/2*((double)curFrame)/ramp);
        return (1.-eval)*factor;
    }
    else if (curFrame > nFrames-ramp)
    {
        int amount_left = nFrames-curFrame;
        double factor = 2*height*ramp/M_PI;
        double eval =cos(M_PI/2*((double)amount_left/ramp));
        return 1. - (1-eval)*factor;
    }
    double amount_in_quad = ((double)curFrame-ramp);
    double quad_part = amount_in_quad*height;
    double curve_part = height*(2*ramp)/M_PI;
    return quad_part+curve_part;
}

Camera
GetCamera(int frame, int nframes)
{
    double t = SineParameterize(frame, nframes, nframes/10);
    Camera c;
    c.near = 5;
    c.far = 200;
    c.angle = M_PI/6;
    c.position[0] = 40*sin(2*M_PI*t);  // O = camera position
    c.position[1] = 40*cos(2*M_PI*t);
    c.position[2] = 40;
    c.focus[0] = 0;  // V3 = O-focus
    c.focus[1] = 0;
    c.focus[2] = 0;
    c.up[0] = 0;  // V2 = up
    c.up[1] = 1;
    c.up[2] = 0;
    return c;
}
// O = camera position 
// v1 = Up x (O-focus) 
// v2 = (O-focus) x v1 
// v3 = O-focus 
///////////////////////////////////////////////////////

   Matrix          
   Camera::CameraTransform() 
   {   

     /*  std::cout<<"\n near = " << near
                <<"\n far = " << far
                <<"\n angle = " << angle
                <<"\n o = " << position[0]
                <<"\n o = " << position[1]
                <<"\n o = " << position[2];
      */         
       Matrix rv;
       std::vector<double> v1(3), v2(3), v3(3);
       v3 = getV3();
       v1 = getV1(v3);  
       v2 = getV2(v1, v3);
    /*
       std::cout<<"\n v1 0 = " << v1[0]
                <<"\n v1 1 = " << v1[1]
                <<"\n v1 2 = " << v1[2]
                <<"\n v2 0 = " << v2[0]
                <<"\n v2 1 = " << v2[1]
                <<"\n v2 2 = " << v2[2];
     */
       double cameraMatrix[4][4];
       double v1t, v2t, v3t;
      v1t = v1[0]*(-position[0]) + v1[1]*(-position[1]) + v1[2]*(-position[2]);
      v2t = v2[0]*(-position[0]) + v2[1]*(-position[1]) + v2[2]*(-position[2]);
      v3t = v3[0]*(-position[0]) + v3[1]*(-position[1]) + v3[2]*(-position[2]);
      
      for(int i=0; i<3; i++) 
      {         
           rv.A[i][0] = v1[i];
           rv.A[i][1] = v2[i];
           rv.A[i][2] = v3[i];
           rv.A[i][3] = 0;    
      }
      rv.A[3][0] = v1t;
      rv.A[3][1] = v2t;  
      rv.A[3][2] = v3t;
      rv.A[3][3] = 1;   

      return rv;
   }

   Matrix 
   Camera::ViewTransform() 
   {  
       Matrix viewTrans;
       for(int i=0; i<4; i++) 
       {   
           for(int j=0; j<4; j++)
           {
               viewTrans.A[i][j] = 0;
           }               
       }
       viewTrans.A[0][0] = cos(angle/2)/sin(angle/2); 
       viewTrans.A[1][1] = cos(angle/2)/sin(angle/2);
       viewTrans.A[2][2] = (far + near)/(far - near);
       viewTrans.A[3][2] = (2*far*near)/(far - near);
       viewTrans.A[2][3] = -1;

       return viewTrans;
   }


  Matrix
  Camera::DeviceTransform()
  {
      Matrix deviceTrans;
      for(int i=0; i<4; i++) 
       {   
           for(int j=0; j<4; j++)
           {
               deviceTrans.A[i][j] = 0;
           }               
       }
      deviceTrans.A[0][0] = 1000/2;
      deviceTrans.A[1][1] = 1000/2;
      deviceTrans.A[2][2] = 1;
      deviceTrans.A[3][3] = 1;
      deviceTrans.A[3][0] = 1000/2;
      deviceTrans.A[3][1] = 1000/2;
      return deviceTrans;
  }
  //////////////////////////
   


//////////////////////////////////////////////////////////
class Triangle
{
  public:
      double         X[3];
      double         Y[3];
      double         Z[3];
      double         colors[3][3];
      double         normals[3][3];

  // would some methods for transforming the triangle in place be helpful?
    
     bool outsideFrame()
     {
         if(((X[0]<0) || (X[0]>1000) || (Y[0]<0) || (Y[0] >1000)) &&
            ((X[1]<0) || (X[1]>1000) || (Y[1]<0) || (Y[2] >1000)) &&
            ((X[2]<0) || (X[2]>1000) || (Y[2]<0) || (Y[2] >1000)))
         return true;
         return false; 
     } 

///////////////////////////////////////////////////////
    bool isFlatBottom() 
    {       
          if ( ((Y[0] == Y[1])&&(Y[0] < Y[2])) || 
               ((Y[0] == Y[2])&&(Y[0] < Y[1])) ||
               ((Y[1] == Y[2])&&(Y[1] < Y[0])) )
               return true;
          else return false;        
    }
   
    bool isFlatTop() 
    {        
          if ( ((Y[0] == Y[1])&&(Y[0] > Y[2])) || 
               ((Y[0] == Y[2])&&(Y[0] > Y[1])) ||
               ((Y[1] == Y[2])&&(Y[1] > Y[0])) )
               return true;
          else return false;         
    }

    bool isNoFlat() 
    {       
          if ( (Y[0] != Y[1])&&
               (Y[0] != Y[2])&&
               (Y[1] != Y[2]) )
               return true;
          else return false;       
    }
/////////////////////////////////////////////////////////

    int getTopPoint() 
   {
      int topPoint;
      double top = fmax(Y[0], fmax(Y[1], Y[2]));
      for (int i = 0; i < 3; i ++) 
      {
         if (Y[i] == top) return i;
      }    
   }

   int getLeftPoint(int topPoint)
   {   
       int leftPoint, point1 = 100, point2 = 100;
       for (int i = 0; i < 3; i++) 
       {
          if(i == topPoint) continue;
          else if (point1 == 100) point1 = i;
          else if (point2 == 100) point2 = i;
       }
       if (X[point1] < X[point2]) leftPoint = point1;
       else leftPoint = point2;
      // if (tri.X[point1] < tri.X[point2]) std::cout<< " \n left point1"<< point1;
      // else if (tri.X[point1] > tri.X[point2]) std::cout<< " \n left point2"<< point2;
       return leftPoint;
   }
 
   int getBottomPoint() 
   {
      double bottom = fmin(Y[0], fmin(Y[1], Y[2]));
      for (int i = 0; i < 3; i ++) 
      {
         if (Y[i] == bottom) return i;
      }    
   }
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
   
      double getSlope(int topPoint, int sidePoint) 
      {
         double dx, dy, slope;
         dx = X[topPoint] - X[sidePoint];
         dy = Y[topPoint] - Y[sidePoint];
        // std::cout<<" \n\ntopPoint "<<topPoint << " sidePoint "<< sidePoint;
        // std::cout<<"\n tri.x 0 1 2: ( "<<tri.X[0] << ", "<< tri.X[1] <<", "<< tri.X[2];;
        // std::cout<<"\n dx is "<<dx<<", dy is: "<< dy<<" ";
         if (dx == 0) std::cerr<<"\n slope dy/dx dx is zero\n"; 
         else slope = dy/dx;
       //  std::cout<< "\n slope is " << slope;
         return slope;
      }

      double getIntercept( double slope, int Point)
      {   
         double intercept = Y[Point] - slope*X[Point];
         return intercept;
      }
////////////////////////////////////////////////////////////////////

     double getSideEnd(int point1, int point2, int scanRow) 
     {    
          double sideEnd;
          if (X[point1] == X[point2]) 
             sideEnd = X[point1];
          else 
          {  
             double slope = getSlope(point1, point2);
             double intercept = getIntercept(slope, point2);
             sideEnd = (scanRow - intercept)/slope;
          }
          return sideEnd;
      }

      int getScanColumn( int leftPoint, int topPoint, int scanRow) 
      {    
          int scanColumn;
          double leftEnd = getSideEnd(leftPoint, topPoint, scanRow);
          if(leftEnd < 0.0) {
              scanColumn = 0;
          }
          else 
          {
             scanColumn = (int)(ceil441(leftEnd) + 0.1);
          }  
          return scanColumn; 
      }
//////////////////////////////////////////////////////////////////////////////
  
///////////////////////////////////////////////////////////////////////////////
   
    std::vector<double> getSideColor(int point1, int point2, int scanRow) {
        std::vector<double> sideColor(3);
        double sideY = Y[point1];
        double topY = Y[point2];
       // std::cout<< "\nside: ("<<tri.X[point1]<<" "<<sideY<< ") topY: ("<<tri.X[point2]<<topY<<")";
      // std::cout<<"\n scanRow = " << scanRow ;
        for (int i = 0; i < 3; i++) 
        {      
            if ((topY - sideY) == 0) std::cout<< "\n divide 0; topY- sideY = 0";
            else 
            {
                sideColor[i] = colors[point1][i] + (colors[point2][i]- colors[point1][i]) *((scanRow-sideY)/(topY-sideY));
             //  std::cout <<" " << sideColor[i];
            }
        }       
        return sideColor;
    }
 ////////////////////////////////////////////////////////////////////////////

   double getSideZ(int point1, int point2, int scanRow) {

      double topZ = Z[point2];
      double sideZ = Z[point1];
      double sideY = Y[point1];
      double topY = Y[point2];
      double deepZ;
      if ((topY - sideY) == 0) std::cout<< "\n divide 0; topY- sideY = 0";
      else 
         deepZ = Z[point1] + (Z[point2]- Z[point1]) *((scanRow-sideY)/(topY-sideY));
      return deepZ;
   }
///////////////////////////////////////////////////////////////
 
  double getPointZ(double leftPointX, double rightPointX, int scanColumn, double leftZ, double rightZ) {
      double pointZ = leftZ + (rightZ - leftZ)*((scanColumn - leftPointX)/(rightPointX - leftPointX));
      return pointZ;
   }
///////////////////////////////////////////////////////////////////////////////

   std::vector<double> getPointColor(double leftPointX, double rightPointX, int scanColumn,     std::vector<double> leftColor, std::vector<double> rightColor) 
   {
       std::vector<double> pointColor(3);
       for(int i = 0; i < 3; i++) 
       {
          pointColor[i] = leftColor[i] + (rightColor[i] - leftColor[i])*((scanColumn - leftPointX)/(rightPointX - leftPointX));
       }
       return pointColor;   
   }
////////////////////////////////////////////////////////////////////////////

    std::vector<double> getSideVector(int point1, int point2, int scanRow) 
    {
        std::vector<double> sideVector(3);
        double sideY = Y[point1];
        double topY = Y[point2];
        for (int i = 0; i < 3; i++) 
        {        
            if ((topY - sideY) == 0) std::cout<< "\n divide 0; topY- sideY = 0";
            else 
            {
               sideVector[i] = normals[point1][i] + (normals[point2][i]- normals[point1][i]) *((scanRow-sideY)/(topY-sideY));           
            }
        }       
        return sideVector;
    }
 ////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

   std::vector<double> getPointVector(double leftPointX, double rightPointX, int scanColumn, std::vector<double> leftVector, std::vector<double> rightVector)
  {
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

////////////////////////////////////////////////////

};


////////////////////////////////////////////////////////////////////////
class Screen
{
  public:
      unsigned char   *buffer;
      int width, height;

  // would some methods for accessing and setting pixels be helpful?

    int setScanRow (double rowMin) {
         int scanRow;
         if(rowMin<0) scanRow = 0;
         else scanRow = (int)(rowMin+0.1);
         return scanRow;
     }
 
  
   inline int getIdx(int x, int y)
    {
      return y * width + x;
    }
///////////////////////////////////////////////////////////////
   void InitializeScreen()
   {
      for (int i = 0 ; i < width*height*3; i++) 
      {
         buffer[i] = 0;
      }
   }
////////////////////////////////////////////////////////////////////////////

   double getDiffuse(std::vector<double> pointVector)
   {
      return fabs(pointVector[0]*(lp.lightDir[0]) + pointVector[1]*(lp.lightDir[1]) + pointVector[2]*(lp.lightDir[2]));
   }
////////////////////////////////////////////////////////////////////////////////

  double 
  getSpecular(std::vector<double> pointVector, std::vector<double> viewVector) 
  {
      double specular, LN, VR;
      std::vector<double> R(3);
/*
   double absPoint = sqrt(pointVector[0]*pointVector[0] + pointVector[1]*pointVector[1] + pointVector[2]*pointVector[2]);
   for(int i = 0; i < 3; i++) {
      pointVector[i] = pointVector[i]/absPoint;
   }
*/
        
      LN = (pointVector[0]*(lp.lightDir[0]) + pointVector[1]*(lp.lightDir[1]) + pointVector[2]*(lp.lightDir[2]));
      for(int i = 0; i < 3; i++) 
      {
         R[i] = 2*(LN)*pointVector[i] - lp.lightDir[i];
      }
      
      VR = R[0]*viewVector[0]+R[1]*viewVector[1]+R[2]*viewVector[2];
      //VR = R[0]*0 +R[1]*0.707107 + R[2]*0.707107;
      specular = fmax(0, 1*(pow(VR, lp.alpha)));
      return specular;
  }

//////////////////////////////////////////////////////////////////////////////
   void drawFlatBottom(Triangle &tri, double *valueZ) 
   {
 
      double leftSlope, leftIntercept, rightSlope, rightIntercept;
      int topPoint, leftPoint, rightPoint;   
      topPoint = tri.getTopPoint();     
      leftPoint = tri.getLeftPoint(topPoint);
      rightPoint = (0+1+2) - topPoint - leftPoint;
      double rowMin = ceil441(tri.Y[leftPoint]);
      double rowMax = floor441(tri.Y[topPoint]);     
      int scanRow = setScanRow (rowMin);
      std::vector<double> viewVector(3);
      Camera c2 = GetCamera(cameraPosition, 1000);
      viewVector = c2.getV3();
      while((scanRow <= (int)(rowMax + 0.1)) && (scanRow <1000)) //(1000, 1000)
      {     

          double rightEnd = floor441(tri.getSideEnd(rightPoint, topPoint, scanRow));
          int scanColumn = tri.getScanColumn(leftPoint, topPoint, scanRow); //leftEnd
          std::vector<double> leftColor(3), rightColor(3);         
          leftColor = tri.getSideColor(leftPoint, topPoint, scanRow);
          rightColor = tri.getSideColor(rightPoint, topPoint, scanRow);

          std::vector<double> leftVector(3), rightVector(3);
          leftVector = tri.getSideVector(leftPoint, topPoint, scanRow);
          rightVector = tri.getSideVector(rightPoint, topPoint, scanRow);

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
          leftZ = tri.getSideZ(leftPoint, topPoint, scanRow);
          rightZ = tri.getSideZ(rightPoint, topPoint, scanRow);
          double leftPointX = tri.getSideEnd(leftPoint, topPoint, scanRow);
          double rightPointX = tri.getSideEnd(rightPoint, topPoint, scanRow);
          //std::cout <<"\n " << leftPointX << " " << rightPointX;
          //std::cout <<"\n " << leftZ << " " << rightZ;
     
          while ((scanColumn <= ((int)(rightEnd+ 0.1))) && (scanColumn < 1000))
          {   
              std::vector<double> pointColor(3);
              pointColor = tri.getPointColor(leftPointX, rightPointX, scanColumn, leftColor, rightColor);

              std::vector<double> pointVector(3);
              pointVector = tri.getPointVector(leftPointX, rightPointX, scanColumn, leftVector, rightVector);
              
              /*
              std::cout<<endl;
              for (int i = 0; i < 3; i++) {        
              std::cout <<"  " << pointVector[i];
              }
              */

              double diffuse = getDiffuse(pointVector);
              //std::cout <<"\n diffuse: " << diffuse;
              
              double specular = getSpecular(pointVector, viewVector);
              double shading = lp.Ka + lp.Kd*diffuse + lp.Ks*specular; 
              //double shading = lp.Ks*specular;
              double pointZ = tri.getPointZ(leftPointX, rightPointX, scanColumn, leftZ, rightZ); 
              //std::cout <<"\n pointZ: " << pointZ;
              int pointNum = getIdx(scanColumn, scanRow);
              if (pointZ > valueZ[pointNum]) 
              {
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
 void drawFlatTop(Triangle &tri, double *valueZ) 
   {
    //  for (int j = 0; j <3; j++) { 
    //       std::cout << "\n xyz= (" << tri.X[j] << ", " << tri.Y[j] <<", " <<tri.Z[j] <<");" ; 
    //  }
 
      double leftSlope, leftIntercept, rightSlope, rightIntercept;
      int bottomPoint, leftPoint, rightPoint;
      bottomPoint = tri.getBottomPoint();     
      leftPoint = tri.getLeftPoint(bottomPoint);
      rightPoint = (0+1+2) - bottomPoint - leftPoint;

      double rowMin = ceil441(tri.Y[bottomPoint]);
      double rowMax = floor441(tri.Y[rightPoint]);     
      int scanRow = setScanRow (rowMin);
     
      while((scanRow <= (int)(rowMax + 0.1)) && (scanRow <1000)) //(1000, 1000)
      {     
          double rightEnd = floor441(tri.getSideEnd(rightPoint, bottomPoint, scanRow));
          int scanColumn = tri.getScanColumn(leftPoint, bottomPoint, scanRow); //leftEnd
          std::vector<double> leftColor(3), rightColor(3);         
          leftColor = tri.getSideColor(leftPoint, bottomPoint, scanRow);
          rightColor = tri.getSideColor(rightPoint, bottomPoint, scanRow);

          std::vector<double> leftVector(3), rightVector(3);
          leftVector = tri.getSideVector(leftPoint, bottomPoint, scanRow);
          rightVector = tri.getSideVector(rightPoint, bottomPoint, scanRow);


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
          leftZ = tri.getSideZ(leftPoint, bottomPoint, scanRow);
          rightZ = tri.getSideZ(rightPoint, bottomPoint, scanRow);
          double leftPointX = tri.getSideEnd(leftPoint, bottomPoint, scanRow);
          double rightPointX = tri.getSideEnd(rightPoint, bottomPoint, scanRow);
          //std::cout <<"\n " << leftPointX << " " << rightPointX;
          //std::cout <<"\n " << leftZ << " " << rightZ;
          std::vector<double> viewVector(3);
          Camera c2 = GetCamera(cameraPosition, 1000);
          viewVector = c2.getV3();
          while ((scanColumn <= ((int)(rightEnd + 0.1))) && (scanColumn < 1000))
          {   
              std::vector<double> pointColor(3);
              pointColor = tri.getPointColor(leftPointX, rightPointX, scanColumn, leftColor, rightColor);
              double pointZ = tri.getPointZ(leftPointX, rightPointX, scanColumn, leftZ, rightZ); 

              std::vector<double> pointVector(3);
              pointVector = tri.getPointVector(leftPointX, rightPointX, scanColumn, leftVector, rightVector);

              double diffuse = getDiffuse(pointVector);
              double specular = getSpecular(pointVector, viewVector);
              double shading = lp.Ka + lp.Kd*diffuse + lp.Ks*specular; 
              //double shading = lp.Ks*specular;
              //std::cout <<"\n pointZ: " << pointZ;
              int pointNum = getIdx(scanColumn, scanRow);
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
   void drawNoFlat(Triangle &tri, double *valueZ) 
   {     
      int topPoint, bottomPoint, middlePoint;
      topPoint = tri.getTopPoint(); 
      bottomPoint = tri.getBottomPoint();
      middlePoint = 3 - (topPoint + bottomPoint);

      double slope, intercept, middleX;
      if(tri.X[topPoint] == tri.X[bottomPoint]) 
      {
          middleX = tri.X[topPoint];
      }
      else 
      {
          slope = tri.getSlope(topPoint, bottomPoint);
          intercept = tri.getIntercept(slope, topPoint);
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
         else 
         {
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
     for(int j=0; j<3;j++) 
     {
        newTemp.colors[bottomPoint][j] = pointColor[j];
        newTemp.normals[bottomPoint][j] = pointVector[j];
     }
      drawFlatBottom(newTemp, valueZ);  

      newTemp.X[topPoint] = tri.X[bottomPoint];
      newTemp.Y[topPoint] = tri.Y[bottomPoint];
      newTemp.Z[topPoint] = tri.Z[bottomPoint];

      for(int j=0; j<3;j++) 
      {
        newTemp.colors[topPoint][j] = tri.colors[bottomPoint][j];
        newTemp.normals[topPoint][j] = tri.normals[bottomPoint][j];
      }

      drawFlatTop(newTemp, valueZ);
   }

///////////////////////////////////////////////////////////////
 
    void scan(const std::vector<Triangle> &tris, double *valueZ) 
    {
       int numTri = tris.size();  
       int countBottom=0, countTop=0, countNoFlat=0, countOutOfFrame=0; 
       for (int i = 0 ; i < 1000*1000; i++) 
        {
           valueZ[i] = -2;
        }
       for(int i = 0; i < numTri; i++) 
       {   
          Triangle tri = tris[i];
          if(tri.outsideFrame()) { countOutOfFrame++; continue;}
       
          if(tri.isFlatBottom())
          {        
             drawFlatBottom(tri, valueZ);  
          }
          else if(tri.isFlatTop())
          {
             drawFlatTop(tri, valueZ);   
          }    
          else if(tri.isNoFlat())
          {        
             drawNoFlat(tri, valueZ);    
          }
       }
      //std::cout<<"\n total tris is "<<numTri<<" out of frame is "<< countOutOfFrame<<"\n";
    }
//total tris is 170939, out of frame is 10000
/////////////////////////////////////////////


};
///////////////////////////////////////////////////////

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
/*
vtkDataSetWriter *writer = vtkDataSetWriter::New();
writer->SetInput(pd);
writer->SetFileName("hrc.vtk");
writer->Write();
 */

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
        tris[idx].X[0] = pt[0];
        tris[idx].Y[0] = pt[1];
        tris[idx].Z[0] = pt[2];
        tris[idx].normals[0][0] = normals[3*ptIds[0]+0];
        tris[idx].normals[0][1] = normals[3*ptIds[0]+1];
        tris[idx].normals[0][2] = normals[3*ptIds[0]+2];
        pt = pts->GetPoint(ptIds[1]);
        tris[idx].X[1] = pt[0];
        tris[idx].Y[1] = pt[1];
        tris[idx].Z[1] = pt[2];
        tris[idx].normals[1][0] = normals[3*ptIds[1]+0];
        tris[idx].normals[1][1] = normals[3*ptIds[1]+1];
        tris[idx].normals[1][2] = normals[3*ptIds[1]+2];
        pt = pts->GetPoint(ptIds[2]);
        tris[idx].X[2] = pt[0];
        tris[idx].Y[2] = pt[1];
        tris[idx].Z[2] = pt[2];
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
   std::vector<Triangle> 
   getNewTriangles(std::vector<Triangle>triangles, Matrix totalMatrix)
   {  
      std::vector<Triangle> newTriangles(triangles.size());
      double input[4], output[4];
      for(int i = 0; i < triangles.size(); i++) 
      {    
          for(int j=0; j<3; j++) 
          {
              input[0] = triangles[i].X[j]; 
              input[1] = triangles[i].Y[j]; 
              input[2] = triangles[i].Z[j]; 
              input[3] = 1;
              totalMatrix.TransformPoint(input, output);
              newTriangles[i].X[j] = output[0]/output[3];
              newTriangles[i].Y[j] = output[1]/output[3];
              newTriangles[i].Z[j] = output[2]/output[3];
              for(int k=0; k<3; k++) 
              {
                  newTriangles[i].colors[j][k] = 
                           triangles[i].colors[j][k];
                  newTriangles[i].normals[j][k] = 
                           triangles[i].normals[j][k];
              }
          }  
      } 
      return newTriangles;
   }

/////////////////////////////////////////////////////////////////////////

int main() {
   vtkImageData *image = NewImage(1000,1000);
   unsigned char *buffer = (unsigned char *) image->GetScalarPointer(0,0,0);
   int npixels = 1000*1000;
   double *valueZ = new double[npixels];
   std::vector<Triangle> triangles = GetTriangles();  
   Screen screen;
   screen.buffer = buffer;
   screen.width = 1000;
   screen.height = 1000;
////////////////////////////////////////////////////////////////////////
   
   
   //AllocateScreen();
   //for(int i=0; i<1000; i++) 
   {
       //InitializeScreen();
      // Camera c1 = GetCamera(0, 1000); 
       //TransformTrianglesToDeviceSpace();
      // RenderTriangles();
      // SaveImage(); 
   }

   
    std::vector<Triangle> newTriangles(triangles.size());
    for(int i=0; i<1000; i++) 
    {   
        if (i==0 || i==250 || i==500 || i==750) 
        {
        std::cout<<"Processing Camera Position " << i <<"\n";
        cameraPosition = i;
        screen.InitializeScreen();
        Camera c1 = GetCamera(i, 1000);
        Matrix totalMatrix = c1.getTotalMatrix();
        newTriangles = getNewTriangles(triangles,totalMatrix);
        screen.scan(newTriangles, valueZ);
        WriteImage(image, "image");
        }
    }

   delete [] valueZ;
}

