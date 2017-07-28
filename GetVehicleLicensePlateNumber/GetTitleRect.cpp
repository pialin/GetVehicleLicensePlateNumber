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


template <typename InputMatType>
int GetProjectY
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

int GetTitleRect
(
	Mat TitleLineGrad,
	TextLineInfo TitleLineInfo,
	double EstimateTitleWidth,

	Rect & InputImageTitleRect,
	double & TitleDutyRatio
)
{
	Mat Binary_TitleLineGrad;
	threshold(
		TitleLineGrad, //�������
		Binary_TitleLineGrad, //�������
		128, //������ʼ��ֵ
		1, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);

	

	double EstimateTitleCharWidth = EstimateTitleWidth / 13.0;

	int ElementSize = int(EstimateTitleCharWidth / 2) * 2 + 1;
	Mat DilateStructingElement = getStructuringElement(MORPH_RECT, Size(ElementSize, 1));

	Mat Binary_Dilate_TitleLineGrad;
	morphologyEx(Binary_TitleLineGrad, Binary_Dilate_TitleLineGrad, MORPH_DILATE, DilateStructingElement);

	Mat Dilate_ProjectY_TitleLineGrad;

	GetProjectY<uchar>(Binary_Dilate_TitleLineGrad, Dilate_ProjectY_TitleLineGrad);

	Mat ProjectY_Binary_TitleLineGrad;

	threshold(
		Dilate_ProjectY_TitleLineGrad, //�������
		ProjectY_Binary_TitleLineGrad, //�������
		128, //������ʼ��ֵ
		1, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);



	int TitleStartCol, TitleEndCol;


	if (ProjectY_Binary_TitleLineGrad.cols <= EstimateTitleWidth + ElementSize - 1)
	{
		TitleStartCol = 0;
		TitleEndCol = ProjectY_Binary_TitleLineGrad.cols;
	}
	else
	{
		int MatchStartCol = 0;
		int MatchEndCol= int( MatchStartCol + EstimateTitleWidth + ElementSize - 1);

		vector <int> NumTitleOne;
		NumTitleOne.push_back(0);

		for (int iCol = MatchStartCol; iCol < MatchEndCol; iCol++)
		{
			NumTitleOne.back() = NumTitleOne.back() + ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[iCol];
		}

		for (MatchStartCol++,MatchEndCol++; 
			MatchEndCol <= ProjectY_Binary_TitleLineGrad.cols; 
			MatchStartCol++, MatchEndCol++)
		{
			NumTitleOne.push_back(NumTitleOne.back() -
				ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchStartCol - 1] +
				ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchEndCol - 2]);
		}
		vector <int>::iterator itMaxNumTitleOne = max_element(NumTitleOne.begin(), NumTitleOne.end());
		TitleStartCol = int(distance(NumTitleOne.begin(), itMaxNumTitleOne));
		TitleEndCol = int(TitleStartCol  + EstimateTitleWidth + ElementSize - 1);
	}
	if (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[TitleStartCol] == 1)
	{
		while (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[TitleStartCol] == 1 && TitleStartCol >= 0)
		{
			TitleStartCol--;
		}
	}
	else if (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[TitleStartCol] == 0)
	{
		while (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[TitleStartCol] == 0 && 
			TitleStartCol < ProjectY_Binary_TitleLineGrad.cols)
		{
			TitleStartCol++;
		}

	}

	if (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[TitleEndCol] == 1)
	{
		while (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[TitleEndCol] == 1 && 
			TitleEndCol < ProjectY_Binary_TitleLineGrad.cols)
		{
			TitleEndCol++;
		}
	}
	else if (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[TitleEndCol] == 0)
	{
		while (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[TitleEndCol] == 0 &&
			TitleEndCol >= 0)
		{
			TitleEndCol--;
		}

	}

	TitleStartCol = TitleStartCol + int(EstimateTitleCharWidth / 2);
	TitleEndCol = TitleEndCol - int(EstimateTitleCharWidth / 2);

	InputImageTitleRect.x = TitleStartCol;
	InputImageTitleRect.width = TitleEndCol - TitleStartCol;
	InputImageTitleRect.y = TitleLineInfo.StartRow;
	InputImageTitleRect.height = TitleLineInfo.EndRow - TitleLineInfo.StartRow;

	GetDutyRatio<uchar>(ProjectY_Binary_TitleLineGrad.colRange(TitleStartCol, TitleEndCol), TitleDutyRatio);

	return 0;
}