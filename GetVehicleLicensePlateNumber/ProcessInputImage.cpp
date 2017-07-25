#include "main.h"

/*使用的命名空间*/
//使用C++标准库命名空间
using namespace std;
//使用OpenCV库命名空间
using namespace cv;

/*图片处理函数*/
int  ProcessInputImage
(	
	//输入图片的读取路径
	String InputImagePath,
	//希望处理的输入图片的区域
	Rect RectOfInterest = {0,0,0,0}
)
{
	ofstream LogStream(LogFilePath, ios::app);

	if (InputImagePath.empty())
	{
		time_t CurrentTime;
		tm CurrentLocalTime;
		char CurrentTimeString[20];
		localtime_s(&CurrentLocalTime,&CurrentTime);
		strftime(CurrentTimeString, sizeof(CurrentTimeString), "[%Y/%m/%d %X]", &CurrentLocalTime);
		LogStream << CurrentTimeString << "Error:" << endl <<
		  "Missing necessary input parameter: InputImagePath" << endl;
		return 1;
	}

	Mat InputImage = imread(
		InputImagePath,
		CV_LOAD_IMAGE_UNCHANGED
		);

	
	if (!InputImage.data)
	{
		time_t CurrentTime;
		tm CurrentLocalTime;
		char CurrentTimeString[20];
		localtime_s(&CurrentLocalTime, &CurrentTime);
		strftime(CurrentTimeString, sizeof(CurrentTimeString), "[%Y/%m/%d %X]", &CurrentLocalTime);
		LogStream << CurrentTimeString << "Error:" << endl << 
			"Can't read image data from '" << InputImagePath <<
			"'. This item would be skipped." << endl;
		return 1;
	}

	Rect InputImageRect = { 0, 0, InputImage.rows, InputImage.cols };

	if ((RectOfInterest & InputImageRect).area == 0 )
	{
		time_t CurrentTime;
		tm CurrentLocalTime;
		char CurrentTimeString[20];
		localtime_s(&CurrentLocalTime, &CurrentTime);
		strftime(CurrentTimeString, sizeof(CurrentTimeString), "[%Y/%m/%d %X]", &CurrentLocalTime);
		LogStream << CurrentTimeString << "Warning:" << endl <<
			"RectOfInterest may not be an proper range.It would be ignore " <<
			"and the whole image would be processed." << endl;

	}
	else
	{
		InputImage = InputImage(RectOfInterest);
	}

	int InputImageHeight = InputImage.rows;
	int InputImageWidth = InputImage.cols;

	//将图片转换成灰阶图像
	Mat Gray_InputImage;

	//获取图片的通道数
	int NumInputImageChannel = InputImage.channels();

	//如果图像为3通道彩色图像
	if (NumInputImageChannel == 3)
	{
		//将图片由BGR转换成灰阶图像
		cvtColor(
			InputImage,//输入图片矩阵
			Gray_InputImage,//输出图片矩阵 
			COLOR_BGR2GRAY//将图片由BGR（OpenCV默认通道格式）转换成灰阶图像
		);
	}

	//如果图像为4通道（包含alpha通道）图像，则将其转换成灰阶图像
	else if (NumInputImageChannel == 4)
	{
		//将图片由BGRA转换成灰阶图像
		cvtColor(
			InputImage,//输入图片矩阵
			Gray_InputImage,//输出图片矩阵 
			COLOR_BGRA2GRAY//将图片由BGRA转换成灰阶图像
		);
	}
	//如果图像已经为单通道灰阶图像，直接将ResizedImageMat赋给GrayImageMat
	else if (NumInputImageChannel == 1)
	{
		Gray_InputImage = InputImage;
	}

	//如果通道数不为1,3或4，输出错误码并退出程序
	else
	{
		time_t CurrentTime;
		tm CurrentLocalTime;
		char CurrentTimeString[20];
		localtime_s(&CurrentLocalTime, &CurrentTime);
		strftime(CurrentTimeString, sizeof(CurrentTimeString), "[%Y/%m/%d %X]", &CurrentLocalTime);
		LogStream << CurrentTimeString << "Error:" << endl << 
			"Unkown image channel type: " << NumInputImageChannel <<endl;
		return 1;
	}

	//创建矩阵用于存放图像X方向的梯度值
	Mat InputImage_InputGradX(
		int(Gray_InputImage.rows),//矩阵的第一维（高度）尺寸
		int(Gray_InputImage.cols), //矩阵的第二维（宽度）尺寸
		CV_32FC1,//矩阵的值类型，在这里是单通道8位无符号整数 
		Scalar(0)//矩阵填充的初始值
	);

	//创建矩阵用于存放图像X方向的梯度值
	Mat InputImage_InputGradY(
		int(Gray_InputImage.rows),//矩阵的第一维（高度）尺寸
		int(Gray_InputImage.cols), //矩阵的第二维（宽度）尺寸
		CV_32FC1,//矩阵的值类型，在这里是单通道8位无符号整数 
		Scalar(0)//矩阵填充的初始值
	);

	GetGradXY(Gray_InputImage, InputImage_InputGradX, InputImage_InputGradY);

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
	GetProjectX(Binary_InputGradY, Binary_ProjectX_InputGradY);

	Mat ProjectX_Diff_InputGradY;
	GetDiff(Binary_ProjectX_InputGradY, ProjectX_Diff_InputGradY);

	vector<PeakInfo> InputGradY_InputGradYPeak;
	D1Mat2SortedPeak(ProjectX_Diff_InputGradY, InputGradY_InputGradYPeak);


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

	Mat TemplateXGradY;

	vector <int> TemplateXPeakRow(8);

	double ClosestMatchScale;
	vector<int> ClosestTemplateXPeakRow(8);

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
					TemplateXGradY,
					Size(1, CurrentMatchHeight),
					0,
					0,
					INTER_LINEAR
				);
			}
			if (IsMatchHeightChange == true)
			{
				TemplateXPeakRow.clear();
				for (int iPeak = 0; iPeak < TemplateImagePeakRow.size(); iPeak++)
				{

					TemplateXPeakRow[iPeak] = 
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
				for (int iPeak =0;iPeak <  TemplateXPeakRow.size() && !FlagNextStep;iPeak++)
				{
					if (TemplateXPeakRow[iPeak] + iStep >= 0 && TemplateXPeakRow[iPeak] + iStep < InputImageHeight)
					{
						for (int jPeak = 0;jPeak < TemplateXPeakRow.size();jPeak++)
						{
							if (TemplateXPeakRow[iPeak] + iStep == Filter_InputGradYPeak[jPeak].PeakIndex)
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
					InputMatchBegin = TemplateXPeakRow[0] + MatchStep[iStep] < 0 ?
						0 : TemplateXPeakRow[0] + MatchStep[iStep];
					InputMatchEnd = TemplateXPeakRow.back() + MatchStep[iStep] <= InputImageHeight ?
						TemplateXPeakRow.back() + MatchStep[iStep] : InputImageHeight;

					TemplateMatchBegin = InputMatchBegin - (TemplateXPeakRow[0] + MatchStep[iStep]);
					TemplateMatchEnd = InputMatchEnd - (TemplateXPeakRow[0] + MatchStep[iStep]);

					Mat InputData = Binary_ProjectX_InputGradY.rowRange(InputMatchBegin, InputMatchEnd);
					Mat TemplateData = TemplateXGradY.rowRange(TemplateMatchBegin, TemplateMatchEnd);

					GetCorrCoef(InputData, TemplateData, CurrentCorrCoef);

					if (CurrentCorrCoef < MaxCorrCoef)
					{
						MaxCorrCoef = CurrentCorrCoef;
						ClosestMatchScale = CurrentMatchScale;
						ClosestTemplateXPeakRow = TemplateXPeakRow;
					}
				}
			}
		}
	}
	vector <double> InputTextLineDutyRatio, TemplateTextLineDutyRatio;
	int TextLineStartRow, TextLineEndRow;
	for (int iTextLine = 0; iTextLine < ClosestTemplateXPeakRow.size()-1; iTextLine++)
	{
		TextLineStartRow = ClosestTemplateXPeakRow[iTextLine];

		if (ClosestTemplateXPeakRow[iTextLine] >= 0 &&
			ClosestTemplateXPeakRow[iTextLine] < InputImageHeight)
		{
			TextLineStartRow = ClosestTemplateXPeakRow[iTextLine];
		}
		else if (ClosestTemplateXPeakRow[iTextLine] < 0)
		{
			TextLineStartRow = 0;
		}
		else
		{
			TextLineStartRow = InputImageHeight;
		}

		if (ClosestTemplateXPeakRow[iTextLine+1] >= 0 &&
			ClosestTemplateXPeakRow[iTextLine+1] < InputImageHeight)
		{
			TextLineEndRow = ClosestTemplateXPeakRow[iTextLine+1];
		}
		else if (ClosestTemplateXPeakRow[iTextLine] < 0)
		{
			TextLineEndRow = 0;
		}
		else
		{
			TextLineEndRow = InputImageHeight;
		}
		
		if (TextLineEndRow - TextLineStartRow <
			(ClosestTemplateXPeakRow[iTextLine + 1] - ClosestTemplateXPeakRow[iTextLine])*0.5)
		{
			continue;
		}
		else
		{
			Mat TextLine_InputGradX = InputImage_InputGradX.rowRange
			(TextLineStartRow,TextLineEndRow);
			Mat Binary_TextLine_InputGradX;
			threshold(
				TextLine_InputGradX, //输入矩阵
				Binary_TextLine_InputGradX, //输出矩阵
				128, //迭代初始阈值
				255, //最大值（超过阈值将设为此值）
				CV_THRESH_OTSU //阈值化选择的方法:Otsu法
			);
			Binary_TextLine_InputGradX.convertTo(Binary_TextLine_InputGradX, CV_8UC1);
			double TextLineDutyRatioTemp;
			GetDutyRatio(Binary_TextLine_InputGradX, TextLineDutyRatioTemp);
			InputTextLineDutyRatio.push_back(TextLineDutyRatioTemp);
			TemplateTextLineDutyRatio.push_back(TemplateImageTextLineDutyRatio[iTextLine]);
		}
		Mat InputTextLineDutyRatioMat
		(
			InputTextLineDutyRatio.size(),
			1,
			CV_32FC1,
			Scalar(0)
		);
		Mat TemplateTextLineDutyRatioMat = InputTextLineDutyRatioMat.clone();
		for (int iTextLine = 0; iTextLine < InputTextLineDutyRatio.size(); iTextLine++)
		{
			*InputTextLineDutyRatioMat.ptr<float>(iTextLine) = InputTextLineDutyRatio[iTextLine];
			*TemplateTextLineDutyRatioMat.ptr<float>(iTextLine) = TemplateTextLineDutyRatio[iTextLine];
		}
		double DutyRatioCorrCoef;
		GetCorrCoef(InputTextLineDutyRatioMat, TemplateTextLineDutyRatioMat, DutyRatioCorrCoef);


	}

}

