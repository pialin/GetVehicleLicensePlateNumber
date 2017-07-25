/*���OpenCV���ͷ�ļ�*/
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


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

/*ʹ�õ������ռ�*/
//ʹ��C++��׼�������ռ�
using namespace std;
//ʹ��OpenCV�������ռ�
using namespace cv;

extern char LogFilePath[];

 int GetGradXY(Mat& SourceMat, Mat& OutputGradXMat, Mat& OutputGradYMat);
 int GetProjectX(Mat &InputMat, Mat & OutputMat);
 int GetProjectY(Mat &InputMat, Mat & OutputMat);
 int GetDiff(Mat &InputMat, Mat & OutputMat);

 struct PeakInfo
 {
	 int PeakIndex = 0;
	 float PeakAmp = 0;
 };

 int D1Mat2SortedPeak(Mat & InputMat, vector<PeakInfo> &OutputPeakInfo);
 int FilterSortedPeak(vector<PeakInfo> &InputPeakInfo, 
	 double MinPeakGap, 
	 vector<PeakInfo> & OutputPeakInfo);

 int GetCorrCoef(Mat &InputMatA, Mat & InputMatB, double & CorrCoef);
 int GetDutyRatio(Mat &InputMat, double & DutyRatio);