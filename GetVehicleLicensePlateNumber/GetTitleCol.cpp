#include "main.h"
int GetTitleCol
(
	Mat & TitleLineGradX,
	TextLineInfo TitleLineInfo,
	int TitleStartCol,
	int TitleEndCol
)
{
	Mat Binary_TitleLineGradX = TitleLineGradX.clone();
	threshold(
		TitleLineGradX, //输入矩阵
		Binary_TitleLineGradX, //输出矩阵
		128, //迭代初始阈值
		255, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);
	Binary_TitleLineGradX.convertTo(Binary_TitleLineGradX, CV_8UC1);

	double  EstimateTitleWidth = double(TitleLineInfo.TextLineHeight) /
		double(TemplateImageTitleLineHeight)*double(TemplateImageTitleWidth);
	double EstimateTitleCharWidth = EstimateTitleWidth / 13.0;

	int CurrentTitleStartCol = 0, CurrentTitleEndCol = 0;
	int TitleStartColTemp, TitleEndColTemp;
	int PreviousTitleStartCol = 0, PreviousTitleEndCol = 0;

	Mat DilateStructingElement;
	Mat Binary_Dilate_TitleLineGradX, Dilate_ProjectY_TitleLineGradX, 
		ProjectY_Binary_TitleLineGradX;
	int CurrentTitleWidth = 0;

	for (int iElementSize = 3; iElementSize < EstimateTitleCharWidth * 2.0; iElementSize = iElementSize + 2)
	{

		if (((CurrentTitleEndCol - CurrentTitleStartCol ) - EstimateTitleWidth)*
			((PreviousTitleEndCol - PreviousTitleStartCol) - EstimateTitleWidth) <= 0)
		{
			break;
		}
		PreviousTitleStartCol = CurrentTitleStartCol;
		PreviousTitleEndCol = CurrentTitleEndCol;

		DilateStructingElement = getStructuringElement(MORPH_RECT, Size(iElementSize, 1));
		morphologyEx(Binary_TitleLineGradX, Binary_Dilate_TitleLineGradX, MORPH_DILATE, DilateStructingElement);

		GetProjectY<uchar>(Binary_Dilate_TitleLineGradX, );

		threshold(
			Dilate_ProjectY_TitleLineGradX,
			ProjectY_Binary_TitleLineGradX,
			128, //迭代初始阈值
			255, //最大值（超过阈值将设为此值）
			CV_THRESH_OTSU //阈值化选择的方法:Otsu法
		);


		if (*ProjectY_Binary_TitleLineGradX.ptr<uchar>(0) = 255)
		{
			TitleStartColTemp = 0;
		}
		for (int iCol = 1; iCol < ProjectY_Binary_TitleLineGradX.cols; iCol++)
		{
			if (ProjectY_Binary_TitleLineGradX.ptr<uchar>(0)[iCol] == 255 && ProjectY_Binary_TitleLineGradX.ptr<uchar>(0)[iCol-1] == 0)
			{
				TitleStartColTemp = iCol;

			}
			else if (ProjectY_Binary_TitleLineGradX.ptr<uchar>(0)[iCol] == 0 && ProjectY_Binary_TitleLineGradX.ptr<uchar>(0)[iCol - 1] == 255)
			{

				TitleEndColTemp = iCol;
				if (TitleEndColTemp - TitleStartColTemp > (CurrentTitleEndCol - CurrentTitleStartCol + iElementSize - 1))
				{
					CurrentTitleStartCol = TitleStartColTemp + iElementSize / 2;
					CurrentTitleEndCol = TitleEndColTemp - (iElementSize - 1 - iElementSize / 2);;
				}

			}
		}

	}

	if (abs(CurrentTitleEndCol - CurrentTitleStartCol - EstimateTitleWidth) / EstimateTitleWidth - 
		abs(PreviousTitleEndCol - PreviousTitleStartCol - EstimateTitleWidth) / EstimateTitleWidth > 1.0 / 13.0)
	{
		TitleStartCol = PreviousTitleStartCol;
		TitleEndCol = PreviousTitleEndCol;
	}
	else
	{
		TitleStartCol = CurrentTitleStartCol;
		TitleEndCol = CurrentTitleEndCol;
	}

	double LicenseWidth = double(TitleEndCol - TitleStartCol) /
		double(TemplateImageTitleWidth)*double(TemplateImageWidth);
	double InputImageHorizonScale = double(LicenseWidth) / double(InputImageWidth);

	if (InputImageHorizonScale >= MinMatchScale &&
		InputImageHorizonScale < MaxMatchScale)
	{
		InputImagePlateNumberAreaRect.y = SegmentLineRow[PlateNumberLineIndex];
		InputImagePlateNumberAreaRect.x = int((TitleStartCol + TitleEndCol - 1) / 2.0 - (TemplateImageTitleCenterCol - TemplateImagePlateNumberAreaCenterCol) / double(TemplateImageWidth)
			*double(InputImageWidth)*InputImageHorizonScale - double(TemplateImagePlateNumberAreaRect.width) / double(TemplateImageWidth)*double(InputImageWidth)*InputImageHorizonScale / 2.0);

		InputImagePlateNumberAreaRect.height = int(double(TemplateImagePlateNumberAreaRect.height) / double(TemplateImageHeight)*InputImageHeight*ClosestMatchScale);

		InputImagePlateNumberAreaRect.width = int(double(TemplateImagePlateNumberAreaRect.width) / double(TemplateImageWidth)*double(InputImageWidth)*InputImageHorizonScale);
		InputImagePlateNumberAreaRect = InputImagePlateNumberAreaRect & Rect(0, 0, InputImageHeight, InputImageWidth);


	}
}