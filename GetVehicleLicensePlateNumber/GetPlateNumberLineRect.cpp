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

int GetPlateNumberLineRect
(
	Mat  PlateNumberLineGrad,
	TextLineInfo  PlateNumberLineInfo,
	double EstimatePlateNumberWidth,

	Rect & PlateNumberLineRect,
	double & PlateNumberLineDutyRatio
)
{
	Mat Binary_PlateNumberLineGrad;
	threshold(
		PlateNumberLineGrad, //�������
		Binary_PlateNumberLineGrad, //�������
		128, //������ʼ��ֵ
		1, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);

	double EstimatePlateNumberCharWidth = EstimatePlateNumberWidth / 26.5;

	int ElementSize = int(EstimatePlateNumberCharWidth / 2) * 2 + 1;
	Mat DilateStructingElement = getStructuringElement(MORPH_RECT, Size(ElementSize, 1));

	Mat Binary_Dilate_PlateNumberLineGrad;
	morphologyEx(Binary_PlateNumberLineGrad, Binary_Dilate_PlateNumberLineGrad, MORPH_DILATE, DilateStructingElement);

	Mat Dilate_ProjectY_PlateNumberLineGrad;

	GetProjectY<uchar>(Binary_Dilate_PlateNumberLineGrad, Dilate_ProjectY_PlateNumberLineGrad);

	Mat ProjectY_Binary_PlateNumberLineGrad;

	threshold(
		Dilate_ProjectY_PlateNumberLineGrad, //�������
		ProjectY_Binary_PlateNumberLineGrad, //�������
		128, //������ʼ��ֵ
		1, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);



	
	int PlateNumberLineStartCol, PlateNumberLineEndCol;

	if (ProjectY_Binary_PlateNumberLineGrad.cols <= EstimatePlateNumberWidth + ElementSize - 1)
	{
		PlateNumberLineStartCol = 0;
		PlateNumberLineEndCol = ProjectY_Binary_PlateNumberLineGrad.cols;
	}
	else
	{
		int MatchStartCol,MatchEndCol;
		MatchStartCol = 0;
		MatchEndCol = int (MatchStartCol + EstimatePlateNumberWidth + ElementSize - 1);

		vector <int> NumPlateNumberOne;
		NumPlateNumberOne.push_back(0);

		for (int iCol = MatchStartCol; iCol < MatchEndCol; iCol++)
		{
			NumPlateNumberOne.back() = NumPlateNumberOne.back() + ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[iCol];
		}

		for (MatchStartCol++, MatchEndCol++;
			MatchEndCol <= ProjectY_Binary_PlateNumberLineGrad.cols;
			MatchStartCol++, MatchEndCol++)
		{
			NumPlateNumberOne.push_back(NumPlateNumberOne.back() -
				ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[MatchStartCol - 1] +
				ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[MatchEndCol - 2]);
		}
		vector <int>::iterator itMaxNumPlateNumberOne = max_element(NumPlateNumberOne.begin(), NumPlateNumberOne.end());
		PlateNumberLineStartCol = int(distance(NumPlateNumberOne.begin(), itMaxNumPlateNumberOne));
		PlateNumberLineEndCol = int(PlateNumberLineStartCol + EstimatePlateNumberWidth + ElementSize - 1);
	}
	if (ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[PlateNumberLineStartCol] == 1)
	{
		while (ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[PlateNumberLineStartCol] == 1 && PlateNumberLineStartCol >= 0)
		{
			PlateNumberLineStartCol--;
		}
	}
	else if (ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[PlateNumberLineStartCol] == 0)
	{
		while (ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[PlateNumberLineStartCol] == 0 &&
			PlateNumberLineStartCol < ProjectY_Binary_PlateNumberLineGrad.cols)
		{
			PlateNumberLineStartCol++;
		}

	}

	if (ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[PlateNumberLineEndCol] == 1)
	{
		while (ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[PlateNumberLineEndCol] == 1 &&
			PlateNumberLineEndCol < ProjectY_Binary_PlateNumberLineGrad.cols)
		{
			PlateNumberLineEndCol++;
		}
	}
	else if (ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[PlateNumberLineEndCol] == 0)
	{
		while (ProjectY_Binary_PlateNumberLineGrad.ptr<uchar>(0)[PlateNumberLineEndCol] == 0 &&
			PlateNumberLineEndCol >= 0)
		{
			PlateNumberLineEndCol--;
		}

	}

	PlateNumberLineStartCol = PlateNumberLineStartCol + int(EstimatePlateNumberCharWidth / 2);
	PlateNumberLineEndCol = PlateNumberLineEndCol - int(EstimatePlateNumberCharWidth / 2);

	PlateNumberLineRect.x = PlateNumberLineStartCol;
	PlateNumberLineRect.width = PlateNumberLineEndCol - PlateNumberLineStartCol;
	PlateNumberLineRect.y = PlateNumberLineInfo.StartRow;
	PlateNumberLineRect.height = PlateNumberLineInfo.EndRow - PlateNumberLineInfo.StartRow;

	GetDutyRatio<uchar>(ProjectY_Binary_PlateNumberLineGrad.colRange(PlateNumberLineStartCol, PlateNumberLineEndCol), PlateNumberLineDutyRatio);

	return 0;
}