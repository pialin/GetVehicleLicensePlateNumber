#include "main.h"
int GetTitleCol
(
	Mat & TitleLineGrad,
	TextLineInfo TitleLineInfo,
	int TitleStartCol,
	int TitleEndCol,
	double TitleDutyRatio
)
{
	Mat Binary_TitleLineGrad;
	threshold(
		TitleLineGrad, //输入矩阵
		Binary_TitleLineGrad, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);

	double  EstimateTitleWidth = double(TitleLineInfo.TextLineHeight) /
		double(TemplateImageTitleLineHeight)*double(TemplateImageTitleWidth);

	double EstimateTitleCharWidth = EstimateTitleWidth / 13.0;

	int ElementSize = int(EstimateTitleCharWidth / 2) * 2 + 1;
	Mat DilateStructingElement = getStructuringElement(MORPH_RECT, Size(ElementSize, 1));

	Mat Binary_Dilate_TitleLineGrad;
	morphologyEx(Binary_TitleLineGrad, Binary_Dilate_TitleLineGrad, MORPH_DILATE, DilateStructingElement);

	Mat Dilate_ProjectY_TitleLineGrad;

	GetProjectY<uchar>(Binary_Dilate_TitleLineGrad, Dilate_ProjectY_TitleLineGrad);

	Mat ProjectY_Binary_TitleLineGrad;

	threshold(
		Dilate_ProjectY_TitleLineGrad, //输入矩阵
		ProjectY_Binary_TitleLineGrad, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);



	int MatchStartCol; 
	int MatchEndCol;


	if (ProjectY_Binary_TitleLineGrad.cols <= EstimateTitleWidth + ElementSize - 1)
	{
		TitleStartCol = 0;
		TitleEndCol = ProjectY_Binary_TitleLineGrad.cols;
	}
	else
	{
		MatchStartCol = 0;
		MatchEndCol = MatchStartCol + EstimateTitleWidth + ElementSize - 1;

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

	
	GetDutyRatio<uchar>(ProjectY_Binary_TitleLineGrad.colRange(TitleStartCol, TitleEndCol), TitleDutyRatio);


}