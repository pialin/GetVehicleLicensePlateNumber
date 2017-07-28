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

int  TextLineSegmentation
(
	Mat  InputImage_InputGradY,
	Mat  Binary_ProjectX_TemplateImageGradY,
	vector <int>  TemplateImageLineRow,
	Size TemplateImageSize,

	vector <TextLineInfo>  & InputImageTextLineInfo,
	double & MatchScaleY

)
{
	int InputImageHeight = InputImage_InputGradY.rows;
	Mat Binary_InputGradY;
	threshold(
		InputImage_InputGradY, //输入矩阵
		Binary_InputGradY, //输出矩阵
		128, //迭代初始阈值
		255, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);
	Binary_InputGradY.convertTo(Binary_InputGradY, CV_8UC1);

	////创建X方向梯度投影向量
	Mat  Binary_ProjectX_InputGradY;
	GetProjectX<uchar>(Binary_InputGradY, Binary_ProjectX_InputGradY);

	Mat ProjectX_Diff_InputGradY;
	GetDiff<uchar>(Binary_ProjectX_InputGradY, ProjectX_Diff_InputGradY);

	vector<PeakInfo> InputGradY_InputGradYPeak;
	GetSortedPeak<uchar>(ProjectX_Diff_InputGradY, InputGradY_InputGradYPeak);


	bool IsMatchHeightChange,IsPeakChange;

	int CurrentMatchHeight = 0;
	int PreviousMatchHeight = 0;

	double MaxCorrCoef = 0.0;
	double ClosestMatchScale = 0.0;

	const double MinMatchScale = 0.6;
	const double MaxMatchScale = 1.2;

	const double MatchScaleStep = 0.01;
	double CurrentMatchScale = MinMatchScale;

	double MinPeakGap;

	vector<PeakInfo> Filter_InputGradYPeak,Filter_InputGradYPeakTemp;

	Mat TemplateYGradY;

	vector <int> TemplateYPeakRow(TemplateImageLineRow.size());


	vector<int> ClosestTemplateYPeakRow(TemplateImageLineRow.size());

	for (int iScale = 0;CurrentMatchScale < MaxMatchScale; iScale++)
	{
		CurrentMatchScale = MinMatchScale + iScale * MatchScaleStep ;

		IsMatchHeightChange = false;
		IsPeakChange = false;

		MinPeakGap = ProjectX_Diff_InputGradY.rows * CurrentMatchScale * 
			((TemplateImageLineRow[1]- TemplateImageLineRow[0])/ TemplateImageSize.height) ;
		if (Filter_InputGradYPeak.empty() != true)
		{ 
			Filter_InputGradYPeakTemp = Filter_InputGradYPeak;
		}

		FilterSortedPeak(InputGradY_InputGradYPeak,
			MinPeakGap,
			Filter_InputGradYPeak
		);
		
		if (Filter_InputGradYPeak.size() !=
			Filter_InputGradYPeakTemp.size())
		{
			IsPeakChange = true;
		}
		else 
		{
			for (int iPeak = 0; iPeak < Filter_InputGradYPeak.size(); iPeak++)
			{
				if (Filter_InputGradYPeak[iPeak].PeakIndex !=
					Filter_InputGradYPeakTemp[iPeak].PeakIndex)
				{
					IsPeakChange = true;
					break;
				}
			}
		}
		

		PreviousMatchHeight = CurrentMatchHeight;

		CurrentMatchHeight = int(InputImageHeight * CurrentMatchScale);

		if (CurrentMatchHeight != PreviousMatchHeight)
		{
			IsMatchHeightChange = true;
		}

		if (IsPeakChange == true || IsMatchHeightChange == true)
		{
			if (IsMatchHeightChange == true)
			{
				resize(
					Binary_ProjectX_TemplateImageGradY,
					TemplateYGradY,
					Size(1, CurrentMatchHeight),
					0,
					0,
					INTER_LINEAR
				);
			}
			if (IsMatchHeightChange == true)
			{
				TemplateYPeakRow.clear();
				for (int iPeak = 0; iPeak < TemplateImageLineRow.size(); iPeak++)
				{

					TemplateYPeakRow[iPeak] = 
						(TemplateImageLineRow[iPeak] - TemplateImageLineRow.back())*CurrentMatchHeight / TemplateImageSize.height;
				}
			}
			bool FlagNextStep;
			vector <int> MatchStep;

			for (int iStep = (*(Filter_InputGradYPeak.end()-1)).PeakIndex - 
				(*(Filter_InputGradYPeak.end() - 2)).PeakIndex;
				iStep < InputImageHeight + (*(Filter_InputGradYPeak.end() - 1)).PeakIndex -
				(*(Filter_InputGradYPeak.end() - 2)).PeakIndex;
				iStep++)
			{
				FlagNextStep = false;
				for (int iPeak =0;iPeak <  TemplateYPeakRow.size() && !FlagNextStep;iPeak++)
				{
					if (TemplateYPeakRow[iPeak] + iStep >= 0 && TemplateYPeakRow[iPeak] + iStep < InputImageHeight)
					{
						for (int jPeak = 0;jPeak < TemplateYPeakRow.size();jPeak++)
						{
							if (TemplateYPeakRow[iPeak] + iStep == Filter_InputGradYPeak[jPeak].PeakIndex)
							{
								FlagNextStep = true;
								MatchStep.push_back(iStep);
								break;
							}
						}
					}
				}
				int InputMatchBegin, InputMatchEnd, TemplateMatchBegin, TemplateMatchEnd;

				double CurrentCorrCoef, MaxCorrCoef = 0; 

				for (iStep = 0; iStep < MatchStep.size(); iStep++)
				{
					InputMatchBegin = TemplateYPeakRow[0] + MatchStep[iStep] < 0 ?
						0 : TemplateYPeakRow[0] + MatchStep[iStep];
					InputMatchEnd = TemplateYPeakRow.back() + MatchStep[iStep] <= InputImageHeight ?
						TemplateYPeakRow.back() + MatchStep[iStep] : InputImageHeight;

					TemplateMatchBegin = InputMatchBegin - (TemplateYPeakRow[0] + MatchStep[iStep]);
					TemplateMatchEnd = InputMatchEnd - (TemplateYPeakRow[0] + MatchStep[iStep]);

					Mat InputData = Binary_ProjectX_InputGradY.rowRange(InputMatchBegin, InputMatchEnd);
					Mat TemplateData = TemplateYGradY.rowRange(TemplateMatchBegin, TemplateMatchEnd);

					GetCorrCoef<float>(InputData, TemplateData, CurrentCorrCoef);

					if (CurrentCorrCoef < MaxCorrCoef)
					{
						MaxCorrCoef = CurrentCorrCoef;
						MatchScaleY = CurrentMatchScale;
						ClosestTemplateYPeakRow = TemplateYPeakRow;
					}
				}
			}
		}
	}

	InputImageTextLineInfo.swap(vector <TextLineInfo>());

	TextLineInfo TextLineInfoTemp;

	for (int iTextLine = 0; iTextLine < ClosestTemplateYPeakRow.size() - 1; iTextLine++)
	{

		if (ClosestTemplateYPeakRow[iTextLine] >= 0 &&
			ClosestTemplateYPeakRow[iTextLine] < InputImageHeight)
		{
			TextLineInfoTemp.StartRow = ClosestTemplateYPeakRow[iTextLine];
		}
		else if (ClosestTemplateYPeakRow[iTextLine] < 0)
		{
			TextLineInfoTemp.StartRow = 0;
		}
		else
		{
			TextLineInfoTemp.StartRow = InputImageHeight;
		}

		if (ClosestTemplateYPeakRow[iTextLine + 1] >= 0 &&
			ClosestTemplateYPeakRow[iTextLine + 1] < InputImageHeight)
		{
			TextLineInfoTemp.EndRow = ClosestTemplateYPeakRow[iTextLine + 1];
		}
		else if (ClosestTemplateYPeakRow[iTextLine] < 0)
		{
			TextLineInfoTemp.EndRow = 0;
		}
		else
		{
			TextLineInfoTemp.EndRow = InputImageHeight;
		}
		InputImageTextLineInfo.push_back(TextLineInfoTemp);
	}

	return 0;
	

}