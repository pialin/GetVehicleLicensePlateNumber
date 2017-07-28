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
	String InputImagePath,//输入图片的读取路径
	Mat & Gray_InputImage
);


int GetPlateNumberLineRect
(
	Mat  PlateNumberLineGrad,
	TextLineInfo  PlateNumberLineInfo,
	double EstimatePlateNumberWidth,

	Rect & PlateNumberLineRect,
	double & PlateNumberLineDutyRatio
);

int GetTitleRect
(
	Mat TitleLineGrad,
	TextLineInfo TitleLineInfo,
	double EstimateTitleWidth,

	Rect & InputImageTitleRect,
	double & TitleDutyRatio
);

int  TextLineSegmentation
(
	Mat  InputImage_InputGradY,
	Mat  Binary_ProjectX_TemplateImageGradY,
	vector <int>  TemplateImageLineRow,
	Size TemplateImageSize,

	vector <TextLineInfo>  & InputImageTextLineInfo,
	double & MatchScaleY

);

int PlateNumberArea2PlateNumber
(
	Mat  PlateNumberAreaGradX,
	TextLineInfo  PlateNumberLineInfo,
	double  EstimatePlateNumberHeight,
	Size  TemplateImagePlateNumberSize,

	Rect & PlateNumberRect
);




int PlateNumberLineRect2PlateNumberAreaRect
(
	Rect  TemplateImagePlateNumberLineRect,
	Rect  TemplateImagePlateNumberAreaRect,
	Size  TemplateImageSize,
	Rect  InputImagePlateNumberLineRect,
	TextLineInfo  InputImagePlateNumberLineInfo,
	Size  InputImageSize,
	double InputImageScaleY,

	double & InputImageScaleX,
	Rect & InputImagePlateNumberAreaRect
);


int PrepareTemplateImage
(
	vector <int> & TemplateImageLineRow,
	Mat & Binary_ProjectX_TemplateImageGradY,
	double & TemplateImageMeanLineHeight,
	Size & TemplateImageSize,
	Rect & TemplateImageTitleRect,
	Rect & TemplateImagePlateNumberLineRect,
	Rect & TemplateImagePlateNumberAreaRect,
	Size & TemplateImagePlateNumberSize
);



int TitleRect2PlateNumberAreaRect
(
	Rect  TemplateImageTitleRect,
	Rect  TemplateImagePlateNumberAreaRect,
	Size  TemplateImageSize,
	Rect  InputImageTitleRect,
	TextLineInfo  PlateNumberLineInfo,
	Size  InputImageSize,
	double  InputImageScaleY,

	double  & InputImageScaleX,
	Rect & InputImagePlateNumberAreaRect
);


int main(int ArgumentCount, char** ArgumentVector)
{
	Mat TemplateImageGradY;

	vector <int> TemplateImageLineRow;
	Mat Binary_ProjectX_TemplateImageGradY;
	double TemplateImageMeanLineHeight;
	Size TemplateImageSize;
	Rect TemplateImageTitleRect, TemplateImagePlateNumberAreaRect,TemplateImagePlateNumberLineRect;
	Size TemplateImagePlateNumberSize;

	PrepareTemplateImage
	(
		TemplateImageLineRow,
		Binary_ProjectX_TemplateImageGradY,
		TemplateImageMeanLineHeight,
		TemplateImageSize,
		TemplateImageTitleRect,
		TemplateImagePlateNumberLineRect,
		TemplateImagePlateNumberAreaRect,
		TemplateImagePlateNumberSize
	);

	String  InputImagePathGlobExpression = ArgumentVector[1];

	//创建用于存储图片路径的String数组
	vector<String> InputImagePathList;

	//根据输入Glob表达式查找符合的图片
	glob(
		InputImagePathGlobExpression,//文件查找Glob表达式
		InputImagePathList, //输出图像文件列表
		false//不进行递归，即不对子文件夹进行搜索
	);


	for (int iInputImagePath = 0; iInputImagePath < InputImagePathList.size(); iInputImagePath++)
	{
		Mat Gray_InputImage;

		LoadInputImage(InputImagePathList[iInputImagePath], Gray_InputImage);

		Size InputImageSize(Gray_InputImage.rows, Gray_InputImage.cols);


		//创建矩阵用于存放图像Y方向的梯度值
		Mat InputImage_InputGradX(
			int(Gray_InputImage.rows),//矩阵的第一维（高度）尺寸
			int(Gray_InputImage.cols), //矩阵的第二维（宽度）尺寸
			CV_8UC1,//矩阵的值类型，在这里是单通道8位无符号整数 
			Scalar(0)//矩阵填充的初始值
		);

		//创建矩阵用于存放图像Y方向的梯度值
		Mat InputImage_InputGradY(
			int(Gray_InputImage.rows),//矩阵的第一维（高度）尺寸
			int(Gray_InputImage.cols), //矩阵的第二维（宽度）尺寸
			CV_8UC1,//矩阵的值类型，在这里是单通道8位无符号整数 
			Scalar(0)//矩阵填充的初始值
		);

		//创建矩阵用于存放图像Y方向的梯度值
		Mat InputImage_InputGrad(
			int(Gray_InputImage.rows),//矩阵的第一维（高度）尺寸
			int(Gray_InputImage.cols), //矩阵的第二维（宽度）尺寸
			CV_8UC1,//矩阵的值类型，在这里是单通道8位无符号整数 
			Scalar(0)//矩阵填充的初始值
		);

		GetAllGrad<uchar>(Gray_InputImage, InputImage_InputGradX,InputImage_InputGradY, InputImage_InputGrad);

		vector <TextLineInfo>  InputImageTextLineInfo;
		double InputImageScaleY;
		TextLineSegmentation
		(
			InputImage_InputGradY,
			Binary_ProjectX_TemplateImageGradY,
			TemplateImageLineRow,
			TemplateImageSize,
			
			InputImageTextLineInfo,
			InputImageScaleY
		);


		double InputImageScaleX;
		Rect InputImagePlateNumberAreaRect;
		TextLineInfo InputImageTitleLineInfo, InputImagePlateNumberLineInfo;
		
		if ((*InputImageTextLineInfo.begin()).TextLineIndex == 0)
		{
			InputImageTitleLineInfo = *InputImageTextLineInfo.begin();

			Mat InputImageTitleLineGrad = InputImage_InputGrad.rowRange(InputImageTitleLineInfo.StartRow,
				InputImageTitleLineInfo.EndRow);

			double  EstimateInputImageTitleWidth = double(InputImageTitleLineInfo.TextLineHeight) /
				double(TemplateImageLineRow[1] - TemplateImageLineRow[0])*double(TemplateImageSize.width);

			Rect InputImageTitleRect;

			double InputImageTitleDutyRatio;

			GetTitleRect
			(
				InputImageTitleLineGrad,
				InputImageTitleLineInfo,
				EstimateInputImageTitleWidth,

				InputImageTitleRect,
				InputImageTitleDutyRatio
			);

			InputImagePlateNumberLineInfo = InputImageTextLineInfo[1];

			if (InputImageTitleDutyRatio < 0.5)
			{
				TextLineInfo InputImageTitleLineInfoTemp = InputImageTextLineInfo[1];
				Mat InputImageTitleLineGradTemp = InputImage_InputGrad.rowRange(InputImageTitleLineInfoTemp.StartRow,
					InputImageTitleLineInfoTemp.EndRow);
				Rect InputImageTitleRectTemp;
				double InputImageTitleDutyRatioTemp;

				GetTitleRect
				(
					InputImageTitleLineGradTemp,
					InputImageTitleLineInfoTemp,
					EstimateInputImageTitleWidth,

					InputImageTitleRectTemp,
					InputImageTitleDutyRatioTemp
				);
				if (InputImageTitleDutyRatioTemp > 0.8)
				{
					InputImageTitleLineInfo = InputImageTitleLineInfoTemp;
					InputImageTitleRect = InputImageTitleRectTemp;
					InputImagePlateNumberLineInfo = InputImageTextLineInfo[2];
				}
			}


			TitleRect2PlateNumberAreaRect
			(
				TemplateImageTitleRect,
				TemplateImagePlateNumberAreaRect,
				TemplateImageSize,
				InputImageTitleRect,
				InputImagePlateNumberLineInfo,
				InputImageSize,
				InputImageScaleY,

				InputImageScaleX,
				InputImagePlateNumberAreaRect
			);
		}
		else if ((*InputImageTextLineInfo.begin()).TextLineIndex == 1)
		{
			InputImagePlateNumberLineInfo = *InputImageTextLineInfo.begin();
			Mat InputImagePlateNumberLineGrad = InputImage_InputGrad.rowRange(InputImagePlateNumberLineInfo.StartRow,
				InputImagePlateNumberLineInfo.EndRow);

			double  EstimateInputImagePlateNumberLineWidth = double(InputImagePlateNumberLineInfo.TextLineHeight) /
				double(TemplateImageLineRow[2]- TemplateImageLineRow[1])*double(TemplateImagePlateNumberLineRect.width);

			Rect InputImagePlateNumberLineRect;
			double InputImagePlateNumberLineDutyRatio;
			GetPlateNumberLineRect
			(
				InputImagePlateNumberLineGrad,
				InputImagePlateNumberLineInfo,
				EstimateInputImagePlateNumberLineWidth,

				InputImagePlateNumberLineRect,
				InputImagePlateNumberLineDutyRatio
			);


			PlateNumberLineRect2PlateNumberAreaRect
			(
				TemplateImagePlateNumberLineRect,
				TemplateImagePlateNumberAreaRect,
				TemplateImageSize,
				InputImagePlateNumberLineRect,
				InputImagePlateNumberLineInfo,
				InputImageSize,
				InputImageScaleY,

				InputImageScaleX,
				InputImagePlateNumberAreaRect
			);
		}

		InputImagePlateNumberAreaRect = InputImagePlateNumberAreaRect & Rect(0, 0, InputImageSize.height, InputImageSize.width);

		Mat InputImagePlateNumberAreaGradX = InputImage_InputGradX(InputImagePlateNumberAreaRect);

		Rect InputImagePlateNumberRect;

		double  EstimateInputImagePlateNumberHeight = double(InputImagePlateNumberLineInfo.TextLineHeight)
			/ double(TemplateImageLineRow[2] - TemplateImageLineRow[1])*double(TemplateImagePlateNumberSize.height);

		PlateNumberArea2PlateNumber
		(
			InputImagePlateNumberAreaGradX,
			InputImagePlateNumberLineInfo,
			EstimateInputImagePlateNumberHeight,
			TemplateImagePlateNumberSize,

			InputImagePlateNumberRect
		);

	}

}

template <typename InputMatType>
int GetProjectY
(
	Mat InputMat,
	Mat & OutputMat
)
{
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}

	Mat OutputMatTemp = Mat::zeros(
		1,
		int(InputMat.rows),
		CV_32FC1
	);
	for (int iCol = 0; iCol < InputMat.cols; iCol++)
	{

		//叠加同一行每一列的梯度值
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			OutputMatTemp.ptr<float>(0)[iCol] = OutputMatTemp.ptr<float>(0)[iCol] +
				InputMat.ptr<InputMatType>(iRow)[iCol];
		}


	}
	double MinValue, MaxValue;
	minMaxLoc(OutputMatTemp, &MinValue, &MaxValue);
	OutputMatTemp.convertTo(OutputMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue*(MaxValue - MinValue) / 255.0);
	return 0;
}


int AppendLog(String LogInfo)
{
	char LogFilePath[] = "Log.txt";
	ofstream LogStream(LogFilePath, ios::app);
	time_t CurrentRawTime;
	time(&CurrentRawTime);
	char CurrentTimeString[22];
	tm CurrentLocalTime;
	localtime_s(&CurrentLocalTime, &CurrentRawTime);
	strftime(CurrentTimeString, sizeof(CurrentTimeString), "[%Y/%m/%d %X]", &CurrentLocalTime);
	LogStream << CurrentTimeString  << endl <<
		LogInfo << endl;
	LogStream.close();
	return 0;
}



int FilterSortedPeak
(
	vector<PeakInfo> InputPeakInfo,
	double MinPeakGap,
	vector<PeakInfo> & OutputPeakInfo
)
{
	bool FlagKeepCurrentPeak;
	vector<PeakInfo>  OutputPeakInfoTemp;
	for (int iPeak = 0; iPeak < InputPeakInfo.size(); iPeak++)
	{
		FlagKeepCurrentPeak = true;
		for (int jPeak = 0; jPeak < OutputPeakInfo.size(); jPeak++)
		{
			if (abs(InputPeakInfo[iPeak].PeakIndex - OutputPeakInfo[jPeak].PeakIndex)
				< MinPeakGap)
			{
				FlagKeepCurrentPeak = false;
				break;
			}
		}
		if (FlagKeepCurrentPeak == true)
		{
			OutputPeakInfoTemp.push_back(InputPeakInfo[iPeak]);
		}
	}
	OutputPeakInfo = OutputPeakInfoTemp;
	return 0;
}


template <typename InputMatType>
int GetAllGrad
(
	Mat InputMat,
	Mat& OutputGradXMat,
	Mat& OutputGradYMat,
	Mat& OutputGradMat
)
{

	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}
	Mat OutputGradXMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32SC1
	);

	Mat OutputGradYMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32SC1
	);

	Mat OutputGradMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32SC1
	);


	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
		{
			OutputGradXMatTemp.ptr<int>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<InputMatType>(iRow)[iCol + 1] - InputMat.ptr<InputMatType>(iRow)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow - 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow + 1)[iCol - 1])
			);
			OutputGradYMatTemp.ptr<int>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol] - InputMat.ptr<InputMatType>(iRow - 1)[iCol]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol - 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol + 1])
			);

			OutputGradMatTemp.ptr<int>(iRow)[iCol] = OutputGradXMatTemp.ptr<int>(iRow)[iCol] +
				OutputGradYMatTemp.ptr<int>(iRow)[iCol];
		}
	}

	OutputGradXMatTemp.convertTo(OutputGradXMat, CV_8UC1, 1.0 / 16.0);
	OutputGradYMatTemp.convertTo(OutputGradYMat, CV_8UC1, 1.0 / 16.0);
	OutputGradMatTemp.convertTo(OutputGradMat, CV_8UC1, 1.0 / 32.0);
	return 0;
}

template <typename InputMatType>
int GetCorrCoef
(
	Mat InputMatA,
	Mat InputMatB,
	double & CorrCoef
)
{
	if (InputMatA.channels() != 1 ||
		InputMatA.rows != 1 && InputMatA.cols != 1 ||
		InputMatB.channels() != 1 ||
		InputMatB.rows != 1 && InputMatB.cols != 1 ||
		InputMatA.rows != InputMatB.rows ||
		InputMatA.cols != InputMatB.cols)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}

	double SumABTemp = 0.0;
	double SumATemp = 0.0;
	double SumAATemp = 0.0;
	double SumBTemp = 0.0;
	double SumBBTemp = 0.0;

	if (InputMatA.cols == 1)
	{
		for (int iRow = 0; iRow < InputMatA.rows; iRow++)
		{

			SumATemp = SumATemp + *InputMatA.ptr<InputMatType>(iRow);
			SumBTemp = SumBTemp + *InputMatB.ptr<InputMatType>(iRow);
			SumABTemp = SumABTemp + *InputMatA.ptr<InputMatType>(iRow) *
				*InputMatB.ptr<InputMatType>(iRow);
			SumAATemp = SumAATemp + *InputMatA.ptr<InputMatType>(iRow)*
				*InputMatA.ptr<InputMatType>(iRow);
			SumBBTemp = SumBBTemp + *InputMatB.ptr<InputMatType>(iRow)*
				*InputMatB.ptr<InputMatType>(iRow);
		}
		CorrCoef = (SumABTemp - SumATemp * SumBTemp / InputMatA.rows) /
			sqrt((SumAATemp - SumATemp * SumATemp / InputMatA.rows)*
			(SumBBTemp - SumBTemp * SumBTemp / InputMatA.rows));
	}
	else
	{
		for (int iCol = 0; iCol < InputMatA.cols; iCol++)
		{
			SumATemp = SumATemp + InputMatA.ptr<InputMatType>(0)[iCol];
			SumBTemp = SumBTemp + InputMatB.ptr<InputMatType>(0)[iCol];
			SumABTemp = SumABTemp + InputMatA.ptr<InputMatType>(0)[iCol] *
				InputMatB.ptr<InputMatType>(0)[iCol];
			SumAATemp = SumAATemp + InputMatA.ptr<InputMatType>(0)[iCol] *
				InputMatA.ptr<InputMatType>(0)[iCol];
			SumBBTemp = SumBBTemp + InputMatB.ptr<InputMatType>(0)[iCol] *
				InputMatB.ptr<InputMatType>(0)[iCol];
		}
		CorrCoef = (SumABTemp - SumATemp * SumBTemp / InputMatA.cols) /
			sqrt((SumAATemp - SumATemp * SumATemp / InputMatA.cols)*
			(SumBBTemp - SumBTemp * SumBTemp / InputMatA.cols));
	}

	return 0;

}




template <typename InputMatType>
int GetDiff
(
	Mat InputMat,
	Mat & OutputMat
)
{
	Mat  OutputMatTemp;
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}

	OutputMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);

	for (int iRow = 0; iRow < InputMat.rows; iRow++)
	{
		//叠加同一行每一列的梯度值
		for (int iCol = 1; iCol < InputMat.cols; iCol++)
		{
			OutputMatTemp.ptr<float>(iRow)[iCol] = InputMat.ptr<InputMatType>(iRow)[iCol] - InputMat.ptr<float>(iRow)[iCol - 1];
		}

	}

	double MinValue, MaxValue;
	minMaxLoc(OutputMatTemp, &MinValue, &MaxValue);
	OutputMatTemp.convertTo(OutputMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);
	return 0;

}



template <typename InputMatType>
int GetDutyRatio
(
	Mat InputMat,
	double & DutyRatio
)
{
	if (InputMat.channels() != 1 ||
		InputMat.rows != 1 && InputMat.cols != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}


	double SumTemp = 0.0;
	double OneValue;
	if (InputMat.cols == 1)
	{
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			SumTemp = SumTemp + *InputMat.ptr<InputMatType>(iRow);

		}
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			if (*InputMat.ptr<InputMatType>(iRow) > 0)
			{
				OneValue = *InputMat.ptr<InputMatType>(iRow);
				break;
			}

		}
		DutyRatio = SumTemp / InputMat.rows / OneValue;
	}
	else
	{
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			SumTemp = SumTemp + InputMat.ptr<InputMatType>(0)[iCol];
		}

		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			if (InputMat.ptr<InputMatType>(0)[iCol] > 0)
			{
				OneValue = InputMat.ptr<InputMatType>(0)[iCol];
				break;
			}

		}
		DutyRatio = SumTemp / InputMat.cols / OneValue;

	}


	return 0;

}



template <typename InputMatType>
int GetGradX
(
	Mat InputMat,
	Mat& OutputGradXMat
)
{
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}
	Mat OutputGradXMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);



	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
		{
			OutputGradXMatTemp.ptr<uchar>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<InputMatType>(iRow)[iCol + 1] - InputMat.ptr<InputMatType>(iRow)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow - 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow + 1)[iCol - 1])
			);

		}
	}

	double MinValue, MaxValue;
	minMaxLoc(OutputGradXMatTemp, & MinValue, & MaxValue);
	OutputGradXMatTemp.convertTo(OutputGradXMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);
	return 0;
}


template <typename InputMatType>
int GetGradXY
(
	Mat InputMat,
	Mat& OutputGradXMat,
	Mat& OutputGradYMat
)
{
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}
	Mat OutputGradXMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);

	Mat OutputGradYMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);


	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
		{
			OutputGradXMatTemp.ptr<float>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<InputMatType>(iRow)[iCol + 1] - InputMat.ptr<InputMatType>(iRow)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow - 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow + 1)[iCol - 1])
			);
			OutputGradYMatTemp.ptr<float>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol] - InputMat.ptr<InputMatType>(iRow - 1)[iCol]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol - 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol + 1])
			);
		}
	}
	double MinValue, MaxValue;
	minMaxLoc(OutputGradXMatTemp, & MinValue, & MaxValue);
	OutputGradXMatTemp.convertTo(OutputGradXMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);

	minMaxLoc(OutputGradYMatTemp, & MinValue, & MaxValue);
	OutputGradYMatTemp.convertTo(OutputGradYMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);
	return 0;
}


template <typename InputMatType>
int GetGradY
(
	Mat InputMat,
	Mat& OutputGradYMat
)
{
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}
	Mat OutputGradYMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32SC1
	);



	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
		{
			OutputGradYMatTemp.ptr<int>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol] - InputMat.ptr<InputMatType>(iRow - 1)[iCol]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol - 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol + 1])
			);

		}
	}
	double MinValue, MaxValue;
	minMaxLoc(OutputGradYMatTemp, &MinValue, &MaxValue);
	OutputGradYMatTemp.convertTo(OutputGradYMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);
	return 0;
}


template <typename InputMatType>
int GetProjectX
(
	Mat InputMat,
	Mat & OutputMat
)
{
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}

	Mat OutputMatTemp = Mat::zeros(
		int(InputMat.rows),
		1,
		CV_32FC1
	);

	for (int iRow = 0; iRow < InputMat.rows; iRow++)
	{

		//叠加同一行每一列的梯度值
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			*OutputMatTemp.ptr<float>(iRow) = *OutputMatTemp.ptr<float>(iRow) +
				InputMat.ptr<InputMatType>(iRow)[iCol];
		}


	}
	double MinValue, MaxValue;
	minMaxLoc(OutputMatTemp, &MinValue, &MaxValue);
	OutputMatTemp.convertTo(OutputMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);
	return 0;
}






template <typename InputMatType>
int GetSortedPeak
(
	Mat InputMat,
	vector<PeakInfo> &OutputPeakInfo
)
{
	PeakInfo PeakInfoTemp;
	if (InputMat.cols == 1 && InputMat.channels() == 1)
	{

		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			PeakInfoTemp.PeakIndex = iRow;
			PeakInfoTemp.PeakAmp = *InputMat.ptr<InputMatType>(iRow);
			if (PeakInfoTemp.PeakAmp > 0)
			{
				OutputPeakInfo.push_back(PeakInfoTemp);
			}
		}
	}

	else if (InputMat.rows == 1 && InputMat.channels() == 1)
	{
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			PeakInfoTemp.PeakIndex = iCol;
			PeakInfoTemp.PeakAmp = InputMat.ptr<InputMatType>(0)[iCol];
			if (PeakInfoTemp.PeakAmp >0)
			{
				OutputPeakInfo.push_back(PeakInfoTemp);
			}
		}
	}
	else
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}
	sort(OutputPeakInfo.begin(),
		OutputPeakInfo.end(),
		[](PeakInfo A, PeakInfo B) {return A.PeakAmp > B.PeakAmp; });
	return 0;
}




int LoadRectOfInterest
(
	String InputXmlFilePath,
	Rect & OutputRectOfInterest
)
{
	ifstream XmlFileStream(InputXmlFilePath);
	if (!XmlFileStream)
	{
		AppendLog("Error: Can't find xml file:" + InputXmlFilePath + ".");
		return 1;
	}

	OutputRectOfInterest = { 0,0,0,0 };
	tinyxml2::XMLDocument XmlDoc;
	XmlDoc.LoadFile(InputXmlFilePath.c_str());
	tinyxml2::XMLElement *LabelElement = XmlDoc.FirstChildElement("annotation")->FirstChildElement("object");
	bool IsRectOfInterestFound = false;
	if (LabelElement == nullptr)
	{
		AppendLog("Error: Can't find  'annotation' node of  xml file:" + InputXmlFilePath + ".");
		return 1;
	}
	else {

		while (LabelElement)
		{

			tinyxml2::XMLElement *LabelNameElement = LabelElement->FirstChildElement("name");
			if (LabelNameElement == nullptr)
			{
				AppendLog("Error: Can't find  'annotation' node of  xml file:" + InputXmlFilePath + ".");
				return 1;
			}
			else
			{
				String LabelName = LabelNameElement->GetText();

				if (LabelName == "14.DetectArea")
				{
					tinyxml2::XMLElement *LabelRectElement = LabelElement->FirstChildElement("bndbox");

					OutputRectOfInterest.x = atoi(LabelRectElement->FirstChildElement("xmin")->GetText()) - 1;
					OutputRectOfInterest.y = atoi(LabelRectElement->FirstChildElement("ymin")->GetText()) - 1;
					OutputRectOfInterest.width = atoi(LabelRectElement->FirstChildElement("xmax")->GetText()) - 1 - OutputRectOfInterest.x;
					OutputRectOfInterest.height = atoi(LabelRectElement->FirstChildElement("ymax")->GetText()) - 1 - OutputRectOfInterest.y;
					IsRectOfInterestFound = true;
					break;
				}
			}

			LabelElement = LabelElement->NextSiblingElement("object");
		}

	}

	if (IsRectOfInterestFound == true)
	{
		return 0;
	}
	else
	{
		AppendLog("Error: Can't find  rect of interest in  xml file:" + InputXmlFilePath + ".");
		return 1;
	}
}



int LoadInputImage
(
	String InputImagePath,//输入图片的读取路径
	Mat & Gray_InputImage
)
{
	if (InputImagePath.empty())
	{
		AppendLog(String(__FUNCTION__) + "(" + to_string(__LINE__) + ")" + "Error:Missing necessary input parameter 'InputImagePath'");
		return 1;
	}

	Mat InputImage = imread(
		InputImagePath,
		CV_LOAD_IMAGE_UNCHANGED
	);


	if (!InputImage.data)
	{
		AppendLog("Error: Can't read image data from '" + InputImagePath + ".");
		return 1;
	}

	Rect InputImageRect = { 0, 0, InputImage.rows, InputImage.cols };

	String XmlFilePath = InputImagePath.substr(0, InputImagePath.rfind('.')) + ".xml";
	Rect RectOfInterest;

	LoadRectOfInterest(XmlFilePath, RectOfInterest);


	if ((RectOfInterest & InputImageRect).area() == 0)
	{
		AppendLog("Warning: RectOfInterest may not be an proper range and would be ignore."\
			"The whole image would be processed.");
		RectOfInterest = InputImageRect;
	}
	else
	{
		InputImage = InputImage(RectOfInterest);
	}

	int InputImageHeight = InputImage.rows;
	int InputImageWidth = InputImage.cols;


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
		AppendLog(String("Error: Unkown image channel type: ") + NumInputImageChannel + ".");
		return 1;
	}

	return 0;
}



int PlateNumberArea2PlateNumber
(
	Mat  PlateNumberAreaGradX,
	TextLineInfo  PlateNumberLineInfo,
	double  EstimatePlateNumberHeight,
	Size  TemplateImagePlateNumberSize,

	Rect & PlateNumberRect
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
	ProjectX_Binary_PlateNumberAreaGradX.convertTo(ProjectX_Binary_PlateNumberAreaGradX, CV_8UC1);


	int PlateNumberStartRow, PlateNumberEndRow;

	if (ProjectX_Binary_PlateNumberAreaGradX.rows <= EstimatePlateNumberHeight)
	{
		PlateNumberStartRow = 0;
		PlateNumberEndRow = ProjectX_Binary_PlateNumberAreaGradX.rows;
	}
	else
	{
		int MatchStartRow = 0, MatchEndRow = int(MatchStartRow + EstimatePlateNumberHeight);
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
			PlateNumberEndRow >= 0)
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
		double(TemplateImagePlateNumberSize.height)*double(TemplateImagePlateNumberSize.width);

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



	int MatchStartCol, MatchEndCol;
	int PlateNumberStartCol, PlateNumberEndCol;

	if (ProjectY_Binary_PlateNumberAreaGradX.cols <= EstimatePlateNumberWidth + ElementSize - 1)
	{
		PlateNumberStartCol = 0;
		PlateNumberEndCol = ProjectY_Binary_PlateNumberAreaGradX.cols;
	}
	else
	{
		MatchStartCol = 0;
		MatchEndCol = int(MatchStartCol + EstimatePlateNumberWidth + ElementSize - 1);

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

	if ((PlateNumberEndCol - PlateNumberStartCol) / EstimatePlateNumberWidth <0.8)
	{
		if ((PlateNumberStartCol + PlateNumberEndCol) / 2.0 < PlateNumberAreaGradX.cols / 2.0)
		{
			PlateNumberStartCol = int(PlateNumberEndCol - EstimatePlateNumberWidth);
		}
		else
		{
			PlateNumberEndCol = int(PlateNumberStartCol + EstimatePlateNumberWidth);
		}

	}

	PlateNumberRect.x = PlateNumberStartCol;
	PlateNumberRect.y = PlateNumberStartRow;
	PlateNumberRect.width = PlateNumberEndCol - PlateNumberStartCol;
	PlateNumberRect.height = PlateNumberEndRow - PlateNumberStartRow;

	return 0;
}


int PlateNumberLineRect2PlateNumberAreaRect
(
	Rect  TemplateImagePlateNumberLineRect,
	Rect  TemplateImagePlateNumberAreaRect,
	Size  TemplateImageSize,
	Rect  InputImagePlateNumberLineRect,
	TextLineInfo  InputImagePlateNumberLineInfo,
	Size  InputImageSize,
	double InputImageScaleY,

	double & InputImageScaleX,
	Rect & InputImagePlateNumberAreaRect
)
{
	double TemplateImagePlateNumberLineCenterCol = TemplateImagePlateNumberLineRect.x +
		TemplateImagePlateNumberLineRect.width / 2.0;

	double TemplateImagePlateNumberRectCenterCol = TemplateImagePlateNumberAreaRect.x +
		TemplateImagePlateNumberAreaRect.width / 2.0;

	double InputImagePlateNumberLineCenterCol = InputImagePlateNumberLineRect.x +
		InputImagePlateNumberLineRect.width / 2.0;



	//Rect InputImagePlateNumberRect = Rect(0, 0, 0, 0);

	double LicenseWidth = double(InputImagePlateNumberLineRect.width) /
		double(TemplateImagePlateNumberLineRect.width)*double(TemplateImageSize.width);

	InputImageScaleX = double(LicenseWidth) / double(InputImageSize.width);

	InputImagePlateNumberAreaRect.y = InputImagePlateNumberLineInfo.StartRow;
	InputImagePlateNumberAreaRect.width = int(double(TemplateImagePlateNumberAreaRect.width) /
		double(TemplateImageSize.width)*double(InputImageSize.width)*InputImageScaleX);

	InputImagePlateNumberAreaRect.x = int(InputImagePlateNumberLineCenterCol -
		(TemplateImagePlateNumberLineCenterCol - TemplateImagePlateNumberRectCenterCol) /
		double(TemplateImageSize.width)*double(InputImageSize.width)*InputImageScaleX - InputImagePlateNumberAreaRect.width / 2.0);


	InputImagePlateNumberAreaRect.height = int(double(TemplateImagePlateNumberAreaRect.height) /
		double(TemplateImageSize.height)*InputImageSize.height*InputImageScaleY);

	return 0;

}




int PrepareTemplateImage
(
	vector <int> & TemplateImageLineRow,
	Mat & Binary_ProjectX_TemplateImageGradY,
	double & TemplateImageMeanLineHeight,
	Size & TemplateImageSize,
	Rect & TemplateImageTitleRect,
	Rect & TemplateImagePlateNumberLineRect,
	Rect & TemplateImagePlateNumberAreaRect,
	Size & TemplateImagePlateNumberSize
)
{
	String TemplateImagePath = "TemplateImage\\TemplateImage.png";

	Mat Gray_TemplateImage;
	LoadInputImage(
		TemplateImagePath,//输入图片路径
		Gray_TemplateImage
	);
	TemplateImageLineRow = { 40,107,190,269,356,439,522,605,683 };

	for (int iLine = 2; iLine < TemplateImageLineRow.size(); iLine++)
	{
		TemplateImageMeanLineHeight = TemplateImageMeanLineHeight +
			TemplateImageLineRow[iLine] - TemplateImageLineRow[iLine - 1];
	}
	TemplateImageMeanLineHeight = TemplateImageMeanLineHeight / (TemplateImageLineRow.size() - 1);

	Gray_TemplateImage = Gray_TemplateImage.rowRange(TemplateImageLineRow[0],
		TemplateImageLineRow.back());
	TemplateImageSize.width = Gray_TemplateImage.cols;
	TemplateImageSize.height = Gray_TemplateImage.rows;
	Mat TemplateImageGradY;
	GetGradY<uchar>(Gray_TemplateImage, TemplateImageGradY);

	Mat Binary_TemplateImageGradY;
	threshold(
		TemplateImageGradY, //输入矩阵
		Binary_TemplateImageGradY, //输出矩阵
		128, //迭代初始阈值
		255, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);
	Binary_TemplateImageGradY.convertTo(Binary_TemplateImageGradY, CV_8UC1);

	Binary_ProjectX_TemplateImageGradY;
	GetProjectX<uchar>(Binary_TemplateImageGradY, Binary_ProjectX_TemplateImageGradY);

	TemplateImageTitleRect.x = 193;
	TemplateImageTitleRect.width = 833 - 193;
	TemplateImageTitleRect.y = TemplateImageLineRow[0];
	TemplateImageTitleRect.height = TemplateImageLineRow[1] - TemplateImageLineRow[0];


	TemplateImagePlateNumberAreaRect = Rect(159, 106, 454 - 159, 228 - 106);

	TemplateImagePlateNumberSize;
	TemplateImagePlateNumberSize.width = 175;
	TemplateImagePlateNumberSize.height = 39;

	TemplateImagePlateNumberLineRect.x = 43;
	TemplateImagePlateNumberLineRect.width = 766;
	TemplateImagePlateNumberLineRect.y = TemplateImageLineRow[0];
	TemplateImagePlateNumberLineRect.height = TemplateImageLineRow[1] - TemplateImageLineRow[0];

	return 0;
}


int TitleRect2PlateNumberAreaRect
(
	Rect  TemplateImageTitleRect,
	Rect  TemplateImagePlateNumberAreaRect,
	Size  TemplateImageSize,
	Rect  InputImageTitleRect,
	TextLineInfo  PlateNumberLineInfo,
	Size  InputImageSize,
	double  InputImageScaleY,

	double  & InputImageScaleX,
	Rect & InputImagePlateNumberAreaRect
)
{
	double TemplateImageTitleCenterCol = TemplateImageTitleRect.x + TemplateImageTitleRect.width / 2.0;
	double InputImageTitleCenterCol = InputImageTitleRect.x + InputImageTitleRect.width / 2.0;
	double TemplateImagePlateNumberAreaCenterCol = TemplateImagePlateNumberAreaRect.x + TemplateImagePlateNumberAreaRect.width / 2.0;

	InputImagePlateNumberAreaRect = Rect(0, 0, 0, 0);

	double LicenseWidth = double(InputImageTitleRect.width) /
		double(TemplateImageTitleRect.width)*double(TemplateImageSize.width);

	InputImageScaleX = double(LicenseWidth) / double(InputImageSize.width);


	InputImagePlateNumberAreaRect.y = PlateNumberLineInfo.StartRow;
	InputImagePlateNumberAreaRect.width = int(double(TemplateImagePlateNumberAreaRect.width) /
		double(InputImageSize.width)*double(InputImageSize.width)*InputImageScaleX);

	InputImagePlateNumberAreaRect.x = int(InputImageTitleCenterCol -
		(TemplateImageTitleCenterCol - TemplateImagePlateNumberAreaCenterCol) /
		double(TemplateImageSize.width)*double(InputImageSize.width)*InputImageScaleX -
		InputImagePlateNumberAreaRect.width / 2.0);


	InputImagePlateNumberAreaRect.height = int(double(TemplateImagePlateNumberAreaRect.height) /
		double(TemplateImageSize.height)*double(InputImageSize.height)*InputImageScaleY);

	return 0;
}


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
		PlateNumberLineGrad, //输入矩阵
		Binary_PlateNumberLineGrad, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
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
		Dilate_ProjectY_PlateNumberLineGrad, //输入矩阵
		ProjectY_Binary_PlateNumberLineGrad, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);




	int PlateNumberLineStartCol, PlateNumberLineEndCol;

	if (ProjectY_Binary_PlateNumberLineGrad.cols <= EstimatePlateNumberWidth + ElementSize - 1)
	{
		PlateNumberLineStartCol = 0;
		PlateNumberLineEndCol = ProjectY_Binary_PlateNumberLineGrad.cols;
	}
	else
	{
		int MatchStartCol, MatchEndCol;
		MatchStartCol = 0;
		MatchEndCol = int(MatchStartCol + EstimatePlateNumberWidth + ElementSize - 1);

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
		TitleLineGrad, //输入矩阵
		Binary_TitleLineGrad, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
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
		Dilate_ProjectY_TitleLineGrad, //输入矩阵
		ProjectY_Binary_TitleLineGrad, //输出矩阵
		128, //迭代初始阈值
		1, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
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
		int MatchEndCol = int(MatchStartCol + EstimateTitleWidth + ElementSize - 1);

		vector <int> NumTitleOne;
		NumTitleOne.push_back(0);

		for (int iCol = MatchStartCol; iCol < MatchEndCol; iCol++)
		{
			NumTitleOne.back() = NumTitleOne.back() + ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[iCol];
		}

		for (MatchStartCol++, MatchEndCol++;
			MatchEndCol <= ProjectY_Binary_TitleLineGrad.cols;
			MatchStartCol++, MatchEndCol++)
		{
			NumTitleOne.push_back(NumTitleOne.back() -
				ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchStartCol - 1] +
				ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchEndCol - 2]);
		}
		vector <int>::iterator itMaxNumTitleOne = max_element(NumTitleOne.begin(), NumTitleOne.end());
		TitleStartCol = int(distance(NumTitleOne.begin(), itMaxNumTitleOne));
		TitleEndCol = int(TitleStartCol + EstimateTitleWidth + ElementSize - 1);
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


	bool IsMatchHeightChange, IsPeakChange;

	int CurrentMatchHeight = 0;
	int PreviousMatchHeight = 0;

	double MaxCorrCoef = 0.0;
	double ClosestMatchScale = 0.0;

	const double MinMatchScale = 0.6;
	const double MaxMatchScale = 1.2;

	const double MatchScaleStep = 0.01;
	double CurrentMatchScale = MinMatchScale;

	double MinPeakGap;

	vector<PeakInfo> Filter_InputGradYPeak, Filter_InputGradYPeakTemp;

	Mat TemplateYGradY;

	vector <int> TemplateYPeakRow(TemplateImageLineRow.size());


	vector<int> ClosestTemplateYPeakRow(TemplateImageLineRow.size());

	for (int iScale = 0; CurrentMatchScale < MaxMatchScale; iScale++)
	{
		CurrentMatchScale = MinMatchScale + iScale * MatchScaleStep;

		IsMatchHeightChange = false;
		IsPeakChange = false;

		MinPeakGap = ProjectX_Diff_InputGradY.rows * CurrentMatchScale *
			((TemplateImageLineRow[1] - TemplateImageLineRow[0]) / TemplateImageSize.height);
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

			for (int iStep = (*(Filter_InputGradYPeak.end() - 1)).PeakIndex -
				(*(Filter_InputGradYPeak.end() - 2)).PeakIndex;
				iStep < InputImageHeight + (*(Filter_InputGradYPeak.end() - 1)).PeakIndex -
				(*(Filter_InputGradYPeak.end() - 2)).PeakIndex;
				iStep++)
			{
				FlagNextStep = false;
				for (int iPeak = 0; iPeak < TemplateYPeakRow.size() && !FlagNextStep; iPeak++)
				{
					if (TemplateYPeakRow[iPeak] + iStep >= 0 && TemplateYPeakRow[iPeak] + iStep < InputImageHeight)
					{
						for (int jPeak = 0; jPeak < TemplateYPeakRow.size(); jPeak++)
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

					GetCorrCoef<uchar>(InputData, TemplateData, CurrentCorrCoef);

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