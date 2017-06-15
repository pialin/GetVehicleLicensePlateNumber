#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
  Mat src, src_gray;
  Mat grad;
  const char* window_name = "Sobel Demo - Simple Edge Detector";
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;

  /// Load an image
  src = imread( argv[1] );

  if( !src.data )
    { return -1; }

  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

  /// Convert it to gray
  cvtColor( src, src_gray, COLOR_RGB2GRAY );

  /// Create window
  //namedWindow( window_name, WINDOW_AUTOSIZE );
  namedWindow( window_name, WINDOW_NORMAL );

  /// Generate grad_x and grad_y
  Mat grad_x;
  Mat abs_grad_x;

  /// Gradient X
  //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( grad_x, abs_grad_x );

  /// Gradient Y
  //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
  //Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  //convertScaleAbs( grad_y, abs_grad_y );

  /// Total Gradient (approximate)
  //addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

  Mat BinaryImage;  
  threshold(abs_grad_x, BinaryImage, 128, 255, CV_THRESH_OTSU);  

  //Mat element = getStructuringElement(MORPH_RECT, Size(12, 12));  
  Mat DilatedImage;  
  //½øÐÐÅòÕÍ²Ù×÷  
  //dilate(BinaryImage, DilatedImage, element);  
  dilate(BinaryImage,DilatedImage,Mat(1,19,CV_8U,cvScalar(1)),Point(0,0));
  imshow( window_name, abs_grad_x );

  waitKey(0);

    imshow( window_name, BinaryImage );

  waitKey(0);

  imshow( window_name, DilatedImage );

  waitKey(0);

  /// Gradient X
  //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  //Sobel( DilatedImage, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  //convertScaleAbs( grad_x, abs_grad_x );

  //imshow( window_name, abs_grad_x );

  //waitKey(0);

  return 0;
}