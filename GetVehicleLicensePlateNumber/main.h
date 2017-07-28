/*添加标准库*/
//为了使用cout添加输入输出流库
#include  <iostream>
//为了使用swap等
#include<algorithm>
//为了使用iota
#include<numeric>
//为了使用ifstream判断文件是否存在
#include <fstream> 
//为了获取当前时间
#include <time.h>
#include <String>


/*添加OpenCV相关头文件*/
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "tinyxml2.h"

/*使用的命名空间*/
//使用C++标准库命名空间
using namespace std;
//使用OpenCV库命名空间
using namespace cv;



struct PeakInfo
{
	int PeakIndex = 0;
	float PeakAmp = 0;
};

struct TextLineInfo
{
	int TextLineIndex = 0;
	int StartRow = 0;
	int EndRow = 0;
	int TextLineHeight = 0;
};

int GetPlateNumberLineRect
(
	Mat  PlateNumberLineGrad,
	TextLineInfo  PlateNumberLineInfo,
	double EstimatePlateNumberWidth,

	Rect & PlateNumberLineRect,
	double & PlateNumberLineDutyRatio
);

int GetTitleRect
(
	Mat TitleLineGrad,
	TextLineInfo TitleLineInfo,
	double EstimateTitleWidth,

	Rect & InputImageTitleRect,
	double & TitleDutyRatio
);

int  TextLineSegmentation
(
	Mat  InputImage_InputGradY,
	Mat  Binary_ProjectX_TemplateImageGradY,
	vector <int>  TemplateImageLineRow,
	Size TemplateImageSize,

	vector <TextLineInfo>  & InputImageTextLineInfo,
	double & MatchScaleY

);

int PlateNumberArea2PlateNumber
(
	Mat  PlateNumberAreaGradX,
	TextLineInfo  PlateNumberLineInfo,
	double  EstimatePlateNumberHeight,
	Size  TemplateImagePlateNumberSize,

	Rect & PlateNumberRect
);




int PlateNumberLineRect2PlateNumberAreaRect
(
	Rect  TemplateImagePlateNumberLineRect,
	Rect  TemplateImagePlateNumberAreaRect,
	Size  TemplateImageSize,
	Rect  InputImagePlateNumberLineRect,
	TextLineInfo  InputImagePlateNumberLineInfo,
	Size  InputImageSize,
	double InputImageScaleY,

	double & InputImageScaleX,
	Rect & InputImagePlateNumberAreaRect
);


int PrepareTemplateImage
(
	vector <int> & TemplateImageLineRow,
	Mat & Binary_ProjectX_TemplateImageGradY,
	double & TemplateImageMeanLineHeight,
	Size & TemplateImageSize,
	Rect & TemplateImageTitleRect,
	Rect & TemplateImagePlateNumberLineRect,
	Rect & TemplateImagePlateNumberAreaRect,
	Size & TemplateImagePlateNumberSize
);



int TitleRect2PlateNumberAreaRect
(
	Rect  TemplateImageTitleRect,
	Rect  TemplateImagePlateNumberAreaRect,
	Size  TemplateImageSize,
	Rect  InputImageTitleRect,
	TextLineInfo  PlateNumberLineInfo,
	Size  InputImageSize,
	double  InputImageScaleY,

	double  & InputImageScaleX,
	Rect & InputImagePlateNumberAreaRect
);