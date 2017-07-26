#include "main.h"

int  TextLineSegmentation
(
	Mat & InputImage_InputGradX,
	Mat & InputImage_InputGradY,
	vector <TextLineInfo>  & InputImageTextLineInfo,
	double & ClosestMatchScale
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
	GetDiff<float>(Binary_ProjectX_InputGradY, ProjectX_Diff_InputGradY);

	vector<PeakInfo> InputGradY_InputGradYPeak;
	GetSortedPeak<float>(ProjectX_Diff_InputGradY, InputGradY_InputGradYPeak);


	bool IsMatchHeightChange,IsPeakChange;

	int CurrentMatchHeight = 0;
	int PreviousMatchHeight = 0;

	double MaxCorrCoef = 0.0;
	double ClosestMatchScale = 0.0;

	double MinMatchScale = 0.6;
	double MaxMatchScale = 1.2;

	double MatchScaleStep = 0.01;
	double CurrentMatchScale = MinMatchScale;

	double MinPeakGap;

	vector<PeakInfo> Filter_InputGradYPeak,Filter_InputGradYPeakTemp;

	Mat TemplateYGradY;

	vector <int> TemplateYPeakRow(8);

	double ClosestMatchScale;
	vector<int> ClosestTemplateYPeakRow(8);

	for (int iScale = 0;CurrentMatchScale < MaxMatchScale; iScale++)
	{
		CurrentMatchScale = MinMatchScale + iScale * MatchScaleStep ;

		IsMatchHeightChange = false;
		IsPeakChange = false;

		MinPeakGap = ProjectX_Diff_InputGradY.rows * CurrentMatchScale * 
			(TemplateImageTitleHeight / TemplateImageHeight) ;
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
					TemplateImageGradY,
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
				for (int iPeak = 0; iPeak < TemplateImagePeakRow.size(); iPeak++)
				{

					TemplateYPeakRow[iPeak] = 
						(TemplateImagePeakRow[iPeak] - TemplateImagePeakRow.back())*CurrentMatchHeight / TemplateImageHeight);
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
						ClosestMatchScale = CurrentMatchScale;
						ClosestTemplateYPeakRow = TemplateYPeakRow;
					}
				}
			}
		}
	}
	vector <double> InputTextLineDutyRatio;
	double TextLineDutyRatio;
	TextLineInfo TextLineInfoTemp;

	Mat Binary_ProjectY_TextLineGradX;
	Mat ProjectY_Binary_TextLineGradX;

	InputImageTextLineInfo.swap(vector <TextLineInfo>());

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

		if (TextLineInfoTemp.EndRow  > TextLineInfoTemp.StartRow)
		{
			Mat TextLineGradX = InputImage_InputGradX.rowRange
			(TextLineInfoTemp.StartRow, TextLineInfoTemp.EndRow);
			Mat Binary_TextLineGradX;
			threshold(
				TextLineGradX, //输入矩阵
				Binary_TextLineGradX, //输出矩阵
				128, //迭代初始阈值
				255, //最大值（超过阈值将设为此值）
				CV_THRESH_OTSU //阈值化选择的方法:Otsu法
			);
			Binary_TextLineGradX.convertTo(Binary_TextLineGradX, CV_8UC1);

			GetProjectY<uchar>(Binary_TextLineGradX, Binary_ProjectY_TextLineGradX);

			threshold(
				Binary_ProjectY_TextLineGradX, //输入矩阵
				ProjectY_Binary_TextLineGradX, //输出矩阵
				128, //迭代初始阈值
				255, //最大值（超过阈值将设为此值）
				CV_THRESH_OTSU //阈值化选择的方法:Otsu法
			);

			GetDutyRatio<uchar>(ProjectY_Binary_TextLineGradX, TextLineDutyRatio);
			InputTextLineDutyRatio.push_back(TextLineDutyRatio);
			TextLineInfoTemp.TextLineIndex = iTextLine;
			TextLineInfoTemp.TextLineHeight = ClosestTemplateYPeakRow[iTextLine + 1] -
				ClosestTemplateYPeakRow[iTextLine];
			InputImageTextLineInfo.push_back(TextLineInfoTemp);
		}

	}

	Mat InputDutyRatio
	(
		InputTextLineDutyRatio.size(),
		1,
		CV_64FC1,
		Scalar(0.0)
	);

	Mat TemplateDutyRatio = InputDutyRatio.clone();

	double UpDutyRatioCorrCoef=0.0, DutyRatioCorrCoef=0.0, DownDutyRatioCorrCoef=0.0;

	for (int iRow = 0; iRow < InputDutyRatio.rows; iRow++)
	{
		*InputDutyRatio.ptr<double>(iRow) = InputTextLineDutyRatio[iRow];
		*TemplateDutyRatio.ptr<double>(iRow) = *TemplateImagePeakRow.ptr<double>(InputTextLineIndex[iRow]);
	}
	GetCorrCoef<double>(InputDutyRatio, TemplateDutyRatio, DutyRatioCorrCoef);

	if (InputTextLineIndex.back() < TemplateImagePeakRow.size() - 1)
	{
		Mat InputDutyRatio
		(
			InputTextLineDutyRatio.size(),
			1,
			CV_64FC1,
			Scalar(0.0)
		);
		Mat TemplateDutyRatio = InputDutyRatio.clone();
		for (int iRow = 0; iRow < InputDutyRatio.rows; iRow++)
		{
			*InputDutyRatio.ptr<double>(iRow) = InputTextLineDutyRatio[iRow];
			*TemplateDutyRatio.ptr<double>(iRow) = *TemplateImagePeakRow.ptr<double>(InputTextLineIndex[iRow] + 1);
		}
		GetCorrCoef<double>(InputDutyRatio, TemplateDutyRatio, UpDutyRatioCorrCoef);
	}
	else
	{
		Mat InputDutyRatio
		(
			InputTextLineDutyRatio.size() - 1,
			1,
			CV_64FC1,
			Scalar(0.0)
		);
		Mat TemplateDutyRatio = InputDutyRatio.clone();
		for (int iRow = 0; iRow < InputDutyRatio.rows - 1; iRow++)
		{
			*InputDutyRatio.ptr<double>(iRow) = InputTextLineDutyRatio[iRow];
			*TemplateDutyRatio.ptr<double>(iRow) = *TemplateImagePeakRow.ptr<double>(InputTextLineIndex[iRow] + 1);
		}
		GetCorrCoef<double>(InputDutyRatio, TemplateDutyRatio, UpDutyRatioCorrCoef);
	}

	if (*InputTextLineIndex.begin() > 1)
	{
		Mat InputDutyRatio
		(
			InputTextLineDutyRatio.size(),
			1,
			CV_64FC1,
			Scalar(0.0)
		);
		Mat TemplateDutyRatio = InputDutyRatio.clone();
		for (int iRow = 0; iRow < InputDutyRatio.rows; iRow++)
		{
			*InputDutyRatio.ptr<double>(iRow) = InputTextLineDutyRatio[iRow];
			*TemplateDutyRatio.ptr<double>(iRow) = *TemplateImagePeakRow.ptr<double>(InputTextLineIndex[iRow] - 1);
		}
		GetCorrCoef<double>(InputDutyRatio, TemplateDutyRatio, DownDutyRatioCorrCoef);
	}
	else
	{
		Mat InputDutyRatio
		(
			InputTextLineDutyRatio.size() - 1,
			1,
			CV_64FC1,
			Scalar(0.0)
		);
		Mat TemplateDutyRatio = InputDutyRatio.clone();
		for (int iRow = 0; iRow < InputDutyRatio.rows - 1; iRow++)
		{
			*InputDutyRatio.ptr<double>(iRow) = InputTextLineDutyRatio[iRow];
			*TemplateDutyRatio.ptr<double>(iRow) = *TemplateImagePeakRow.ptr<double>(InputTextLineIndex[iRow] - 1);
		}
		GetCorrCoef<double>(InputDutyRatio, TemplateDutyRatio, DownDutyRatioCorrCoef);
	}


	if (UpDutyRatioCorrCoef > 0.0  && abs(UpDutyRatioCorrCoef) - abs(DutyRatioCorrCoef) > 0.5)
	{
		for (int iTextLine = 0; iTextLine < InputTextLineIndex.size(); iTextLine++)
		{
			if (InputTextLineIndex[iTextLine] + 1 < TemplateImagePeakRow.size())
			{
				InputTextLineIndex[iTextLine] = InputTextLineIndex[iTextLine] + 1;
			}
			else
			{
				InputImageTextLineInfo.erase(InputImageTextLineInfo.begin() + iTextLine);
			}

		}

	}
	else if (DownDutyRatioCorrCoef > 0.0 && abs(DownDutyRatioCorrCoef) - abs(DutyRatioCorrCoef) > 0.5)
	{
		for (int iTextLine = 0; iTextLine < InputTextLineIndex.size(); iTextLine++)
		{
			if (InputTextLineIndex[iTextLine] - 1 < 0)
			{
				InputTextLineIndex[iTextLine] = InputTextLineIndex[iTextLine] - 1;
			}
			else
			{
				InputImageTextLineInfo.erase(InputImageTextLineInfo.begin() + iTextLine);
			}

		}
	}


}