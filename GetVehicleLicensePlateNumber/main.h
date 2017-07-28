/*��ӱ�׼��*/
//Ϊ��ʹ��cout��������������
#include  <iostream>
//Ϊ��ʹ��swap��
#include<algorithm>
//Ϊ��ʹ��iota
#include<numeric>
//Ϊ��ʹ��ifstream�ж��ļ��Ƿ����
#include <fstream> 
//Ϊ�˻�ȡ��ǰʱ��
#include <time.h>
#include <String>


/*���OpenCV���ͷ�ļ�*/
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "tinyxml2.h"

/*ʹ�õ������ռ�*/
//ʹ��C++��׼�������ռ�
using namespace std;
//ʹ��OpenCV�������ռ�
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