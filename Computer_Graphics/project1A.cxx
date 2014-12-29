#include <iostream>
#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkPNGWriter.h>
#include <vtkPointData.h>

using std::cerr;
using std::endl;

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


int main()
{
   std::cerr << "In main!" << endl;
   vtkImageData *image = NewImage(1024, 1350);
   unsigned char *buffer = 
   (unsigned char *) image->GetScalarPointer(0,0,0);
   
   for(int i = 0; i < 1024*1350; i++) {
        int x = i/(1024*50);
        {
        if(x%3 == 0) buffer[3*i+2] = 0;
        else if (x%3 == 1) buffer[3*i+2] = 128;
        else if (x%3 == 2) buffer[3*i+2] = 255;
        }
        {
        if((x/3)%3 == 0) buffer[3*i+1] = 0;
        else if ((x/3)%3 == 1) buffer[3*i+1] = 128;
        else if ((x/3)%3 == 2) buffer[3*i+1] = 255;         
         }
         {
        if(x/9 == 0) buffer[3*i+0] = 0;
        else if (x/9 == 1) buffer[3*i+0] = 128;
        else if (x/9 == 2) buffer[3*i+0] = 255;         
         }
   }
   WriteImage(image, "oneRedPixel")
;
}

