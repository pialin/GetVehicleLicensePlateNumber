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

const extern char LogFilePath[] = "E:\\Git\\GetVehicleLicensePlateNumber\\"\
"GetVehicleLicensePlateNumber\\Log.txt";
 template <typename InputMatType>
 int GetAllGrad(Mat& InputMat, Mat& OutputGradXMat, Mat& OutputGradYMat, Mat& OutputGradMat);
 template <typename InputMatType>
 int GetProjectX(Mat &InputMat, Mat & OutputMat);
 template <typename InputMatType>
 int GetProjectY(Mat &InputMat, Mat & OutputMat);
 template <typename InputMatType>
 int GetDiff(Mat &InputMat, Mat & OutputMat);

 struct PeakInfo
 {
	 int PeakIndex = 0;
	 float PeakAmp = 0;
 };

 template <typename InputMatType>
 int GetSortedPeak(Mat & InputMat, vector<PeakInfo> &OutputPeakInfo);
 int FilterSortedPeak(vector<PeakInfo> &InputPeakInfo, 
	 double MinPeakGap, 
	 vector<PeakInfo> & OutputPeakInfo);
 template <typename InputMatType>
 int GetCorrCoef(Mat &InputMatA, Mat & InputMatB, double & CorrCoef);
 template <typename InputMatType>
 int GetDutyRatio(Mat &InputMat, double & DutyRatio);

 int AppendLog(String LogInfo);

 int LoadInputImage
 (
	 String InputImagePath,//输入图片的读取路径
	 Mat Gray_InputImage
 );

 int LoadRectOfInterest(String InputXmlFilePath, Rect OutputRectOfInterest);

 struct TextLineInfo
 {
	 int TextLineIndex = 0;
	 int StartRow = 0;
	 int EndRow = 0;
	 int TextLineHeight = 0;
 };

 int  TextLineSegmentation
 (
	 Mat InputImage_InputGradX,
	 Mat InputImage_InputGradY,
	 vector <TextLineInfo>  InputImageTextLineInfo,
	 double ClosestMatchScale
 );

 int GetTitleCol
 (
	 Mat & TitleLineGrad,
	 TextLineInfo TitleLineInfo,
	 int TitleStartCol,
	 int TitleEndCol
 );