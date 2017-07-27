#include "main.h"
int GetPlateNumberCol
(
	Mat & PlateNumberLineGrad,
	TextLineInfo PlateNumberLineInfo,
	int PlateNumberLineStartCol,
	int PlateNumberLineEndCol,
	double PlateNumberLineDutyRatio
)
{
	Mat Binary_PlateNumberLineGrad;
	threshold(
		PlateNumberLineGrad, //输入矩阵
		Binary_PlateNumberLineGrad, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);

	double  EstimatePlateNumberWidth = double(PlateNumberLineInfo.TextLineHeight) /
		double(TemplateImagePlateNumberLineHeight)*double(TemplateImagePlateNumberLineWidth);

	double EstimatePlateNumberCharWidth = EstimatePlateNumberWidth / 26.5;

	int ElementSize = int(EstimatePlateNumberCharWidth / 2) * 2 + 1;
	Mat DilateStructingElement = getStructuringElement(MORPH_RECT, Size(ElementSize, 1));

	Mat Binary_Dilate_PlateNumberLineGrad;
	morphologyEx(Binary_PlateNumberLineGrad, Binary_Dilate_PlateNumberLineGrad, MORPH_DILATE, DilateStructingElement);

	Mat Dilate_ProjectY_PlateNumberLineGrad;

	GetProjectY<uchar>(Binary_Dilate_PlateNumberLineGrad, Dilate_ProjectY_PlateNumberLineGrad);

	Mat ProjectY_Binary_PlateNumberLineGrad;

	threshold(
		Dilate_ProjectY_PlateNumberLineGrad, //输入矩阵
		ProjectY_Binary_PlateNumberLineGrad, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);



	int PlateNumberMatchStartCol;
	int PlateNumberMatchEndCol;

	int MatchStartCol, MatchEndCol;
	if (ProjectY_Binary_PlateNumberLineGrad.cols <= EstimatePlateNumberWidth + ElementSize - 1)
	{
		PlateNumberLineStartCol = 0;
		PlateNumberLineEndCol = ProjectY_Binary_PlateNumberLineGrad.cols;
	}
	else
	{
		MatchStartCol = 0;
		MatchEndCol = MatchStartCol + EstimatePlateNumberWidth + ElementSize - 1;

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

	GetDutyRatio<uchar>(ProjectY_Binary_PlateNumberLineGrad.colRange(PlateNumberLineStartCol, PlateNumberLineEndCol), PlateNumberLineDutyRatio);

}