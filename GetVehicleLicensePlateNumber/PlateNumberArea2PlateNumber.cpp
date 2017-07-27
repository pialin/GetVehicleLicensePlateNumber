#include "main.h"
int PlateNumberArea2PlateNumber
(
	Mat & PlateNumberAreaGradX,
	TextLineInfo PlateNumberLineInfo,
	Rect PlateNumberRect
)

{
	Mat ProjectX_PlateNumberAreaGradX;

	GetProjectX<uchar>(PlateNumberAreaGradX, ProjectX_PlateNumberAreaGradX);

	Mat ProjectX_Binary_PlateNumberAreaGradX;
	threshold(
		ProjectX_PlateNumberAreaGradX, //输入矩阵
		ProjectX_Binary_PlateNumberAreaGradX, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);


	double  EstimatePlateNumberHeight =  double(PlateNumberLineInfo.TextLineHeight
		/ double(TemplateImagePlateNumberLineHeight)*double(TemplateImagePlateNumberHeight);


	int MatchStartRow,MatchEndRow;
	int PlateNumberStartRow, PlateNumberEndRow;

	if (ProjectX_Binary_PlateNumberAreaGradX.rows <= EstimatePlateNumberHeight)
	{
		PlateNumberStartRow = 0;
		PlateNumberEndRow = ProjectX_Binary_PlateNumberAreaGradX.rows;
	}
	else
	{
		MatchStartRow = 0;
		MatchEndRow = MatchStartRow + EstimatePlateNumberHeight;

		vector <int> NumPlateNumberOne;
		NumPlateNumberOne.push_back(0);

		for (int iRow = MatchStartRow; iRow < MatchEndRow; iRow++)
		{
			NumPlateNumberOne.back() = NumPlateNumberOne.back() + *ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(iRow);
		}

		for (MatchStartRow++, MatchEndRow++;
			MatchEndRow <= ProjectX_Binary_PlateNumberAreaGradX.rows;
			MatchStartRow++, MatchEndRow++)
		{
			NumPlateNumberOne.push_back(NumPlateNumberOne.back() -
				*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(MatchStartRow - 1) +
				*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(MatchEndRow - 2));
		}
		vector <int>::iterator itMaxNumPlateNumberOne = max_element(NumPlateNumberOne.begin(), NumPlateNumberOne.end());
		PlateNumberStartRow = int(distance(NumPlateNumberOne.begin(), itMaxNumPlateNumberOne));
		PlateNumberEndRow = int(PlateNumberStartRow + EstimatePlateNumberHeight);
	}
	if (*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(PlateNumberStartRow) == 1)
	{
		while (*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(PlateNumberStartRow) == 1 && 
			PlateNumberStartRow >= 0)
		{
			PlateNumberStartRow--;
		}
	}
	else if (*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(PlateNumberStartRow) == 0)
	{
		while (*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(PlateNumberStartRow) == 0 &&
			PlateNumberStartRow < ProjectX_Binary_PlateNumberAreaGradX.rows)
		{
			PlateNumberStartRow++;
		}

	}

	if (*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(PlateNumberEndRow) == 1)
	{
		while (*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(PlateNumberEndRow) == 1 && 
			PlateNumberEndRow  < ProjectX_Binary_PlateNumberAreaGradX.rows)
		{
			PlateNumberEndRow++;
		}
	}
	else if (*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(PlateNumberEndRow) == 0)
	{
		while (*ProjectX_Binary_PlateNumberAreaGradX.ptr<uchar>(PlateNumberEndRow) == 0 &&
			PlateNumberEndRow >=0 )
		{
			PlateNumberEndRow--;
		}

	}

	PlateNumberAreaGradX = PlateNumberAreaGradX.rowRange(PlateNumberStartRow, PlateNumberEndRow);


	Mat Binary_PlateNumberAreaGradX;
	threshold(
		PlateNumberAreaGradX, //输入矩阵
		Binary_PlateNumberAreaGradX, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);

	double  EstimatePlateNumberWidth = double(PlateNumberEndRow - PlateNumberStartRow) /
		double(TemplateImagePlateNumberHeight)*double(TemplateImagePlateNumberWidth);

	double EstimatePlateNumberCharWidth = EstimatePlateNumberWidth / 7.5;

	int ElementSize = int(EstimatePlateNumberCharWidth / 2) * 2 + 1;
	Mat DilateStructingElement = getStructuringElement(MORPH_RECT, Size(ElementSize, 1));

	Mat  Binary_Dilate_PlateNumberAreaGradX;
	morphologyEx(Binary_PlateNumberAreaGradX, 
		Binary_Dilate_PlateNumberAreaGradX, MORPH_DILATE, DilateStructingElement);

	Mat Dilate_ProjectY_PlateNumberAreaGradX;

	GetProjectY<uchar>(Binary_Dilate_PlateNumberAreaGradX, Dilate_ProjectY_PlateNumberAreaGradX);

	Mat ProjectY_Binary_PlateNumberAreaGradX;

	threshold(
		Dilate_ProjectY_PlateNumberAreaGradX, //输入矩阵
		ProjectY_Binary_PlateNumberAreaGradX, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);



	int MatchStartCol,MatchEndCol;
	int PlateNumberStartCol, PlateNumberEndCol;

	if (ProjectY_Binary_PlateNumberAreaGradX.cols <= EstimatePlateNumberWidth + ElementSize - 1)
	{
		PlateNumberStartCol = 0;
		PlateNumberEndCol = ProjectY_Binary_PlateNumberAreaGradX.cols;
	}
	else
	{
		MatchStartCol = 0;
		MatchEndCol = MatchStartCol + EstimatePlateNumberWidth + ElementSize - 1;

		vector <int> NumPlateNumberOne;
		NumPlateNumberOne.push_back(0);

		for (int iCol = MatchStartCol; iCol < MatchEndCol; iCol++)
		{
			NumPlateNumberOne.back() = NumPlateNumberOne.back() + ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[iCol];
		}

		for (MatchStartCol++, MatchEndCol++;
			MatchEndCol <= ProjectY_Binary_PlateNumberAreaGradX.cols;
			MatchStartCol++, MatchEndCol++)
		{
			NumPlateNumberOne.push_back(NumPlateNumberOne.back() -
				ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[MatchStartCol - 1] +
				ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[MatchEndCol - 2]);
		}
		vector <int>::iterator itMaxNumPlateNumberOne = max_element(NumPlateNumberOne.begin(), NumPlateNumberOne.end());
		PlateNumberStartCol = int(distance(NumPlateNumberOne.begin(), itMaxNumPlateNumberOne));
		PlateNumberEndCol = int(PlateNumberStartCol + EstimatePlateNumberWidth + ElementSize - 1);
	}
	if (ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[PlateNumberStartCol] == 1)
	{
		while (ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[PlateNumberStartCol] == 1 && PlateNumberStartCol >= 0)
		{
			PlateNumberStartCol--;
		}
	}
	else if (ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[PlateNumberStartCol] == 0)
	{
		while (ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[PlateNumberStartCol] == 0 &&
			PlateNumberStartCol < ProjectY_Binary_PlateNumberAreaGradX.cols)
		{
			PlateNumberStartCol++;
		}

	}

	if (ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[PlateNumberEndCol] == 1)
	{
		while (ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[PlateNumberEndCol] == 1 &&
			PlateNumberEndCol <ProjectY_Binary_PlateNumberAreaGradX.cols)
		{
			PlateNumberEndCol++;
		}
	}
	else if (ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[PlateNumberEndCol] == 0)
	{
		while (ProjectY_Binary_PlateNumberAreaGradX.ptr<uchar>(0)[PlateNumberEndCol] == 0 &&
			PlateNumberEndCol >= 0)
		{
			PlateNumberEndCol--;
		}

	}

	PlateNumberStartCol = PlateNumberStartCol + int(EstimatePlateNumberCharWidth / 2);
	PlateNumberEndCol = PlateNumberEndCol - int(EstimatePlateNumberCharWidth / 2);


	PlateNumberRect.x = PlateNumberStartCol;
	PlateNumberRect.y = PlateNumberStartRow;
	PlateNumberRect.width = PlateNumberEndCol - PlateNumberStartCol;
	PlateNumberRect.y = PlateNumberEndRow - PlateNumberStartRow;


}