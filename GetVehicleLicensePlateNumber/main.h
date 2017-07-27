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
	 String InputImagePath,//����ͼƬ�Ķ�ȡ·��
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