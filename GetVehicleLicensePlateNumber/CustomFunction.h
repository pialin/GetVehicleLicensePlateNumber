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

int AppendLog(String LogInfo);

int FilterSortedPeak
(
	vector<PeakInfo> InputPeakInfo,
	double MinPeakGap,
	vector<PeakInfo> & OutputPeakInfo
);

template <typename InputMatType>
int GetAllGrad
(
	Mat InputMat,
	Mat& OutputGradXMat,
	Mat& OutputGradYMat,
	Mat& OutputGradMat
);

template <typename InputMatType>
int GetCorrCoef
(
	Mat InputMatA,
	Mat InputMatB,
	double & CorrCoef
);

template <typename InputMatType>
int GetDiff
(
	Mat InputMat,
	Mat & OutputMat
);

template <typename InputMatType>
int GetDutyRatio
(
	Mat InputMat,
	double & DutyRatio
);

template <typename InputMatType>
int GetGradX
(
	Mat InputMat,
	Mat& OutputGradXMat
);

template <typename InputMatType>
int GetGradXY
(
	Mat InputMat,
	Mat& OutputGradXMat,
	Mat& OutputGradYMat
);

template <typename InputMatType>
int GetGradY
(
	Mat InputMat,
	Mat& OutputGradYMat
);


template <typename InputMatType>
int GetProjectX
(
	Mat InputMat,
	Mat & OutputMat
);

template <typename InputMatType>
int GetProjectY
(
	Mat InputMat,
	Mat & OutputMat
);

template <typename InputMatType>
int GetSortedPeak
(
	Mat InputMat,
	vector<PeakInfo> &OutputPeakInfo
);





int LoadRectOfInterest
(
	String InputXmlFilePath,
	Rect & OutputRectOfInterest
);



int LoadInputImage
(
	String InputImagePath,//����ͼƬ�Ķ�ȡ·��
	Mat & Gray_InputImage
);




