#include "main.h"
int main(int ArgumentCount, char** ArgumentVector)
{

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

		//创建矩阵用于存放图像X方向的梯度值
		Mat InputImage_InputGrad(
			int(Gray_InputImage.rows),//矩阵的第一维（高度）尺寸
			int(Gray_InputImage.cols), //矩阵的第二维（宽度）尺寸
			CV_32FC1,//矩阵的值类型，在这里是单通道8位无符号整数 
			Scalar(0)//矩阵填充的初始值
		);

		GetAllGrad<uchar>(Gray_InputImage, InputImage_InputGradX, InputImage_InputGradY, InputImage_InputGrad);

		vector <TextLineInfo>  InputImageTextLineInfo;
		double ClosestMatchScale;
		TextLineSegmentation
		(
			InputImage_InputGradX,
			InputImage_InputGradY,
			InputImageTextLineInfo,
			ClosestMatchScale
		);

		
		if ((*InputImageTextLineInfo.begin()).TextLineIndex == 0)
		{
			TextLineInfo TitleLineInfo = *InputImageTextLineInfo.begin();
			Mat TitleLineGrad = InputImage_InputGrad.rowRange(TitleLineInfo.StartRow,
				TitleLineInfo.EndRow);
			int TitleStartCol, TitleEndCol;
			double TitleDutyRatio;
			GetTitleCol(TitleLineGrad,TitleLineInfo, TitleStartCol, TitleEndCol,TitleDutyRatio);
			TextLineInfo PlateNumberLineInfo = InputImageTextLineInfo[1]; 
			if (TitleDutyRatio < 0.5)
			{	
				TextLineInfo TitleLineInfoTemp = InputImageTextLineInfo[1];
				Mat TitleLineGradTemp = InputImage_InputGrad.rowRange(TitleLineInfoTemp.StartRow,
					TitleLineInfoTemp.EndRow);
				int TitleStartColTemp, TitleEndColTemp;
				double TitleDutyRatioTemp;
				GetTitleCol(TitleLineGradTemp, TitleLineInfoTemp, TitleStartColTemp, TitleEndColTemp, TitleDutyRatioTemp);
				if (TitleDutyRatioTemp > 0.8)
				{
					TitleLineInfo = TitleLineInfoTemp;
					TitleStartCol = TitleStartColTemp;
					TitleEndCol = TitleEndColTemp;
					PlateNumberLineInfo = InputImageTextLineInfo[2];
				}
			}

			TitleCol2PlateNumberArea(TitleStartCol, TitleEndCol, PlateNumberLineInfo, InputImagePlateNumberAreaRect);
			

		}
		else if ((*InputImageTextLineInfo.begin()).TextLineIndex == 1)
		{
			TextLineInfo PlateNumberLineInfo = *InputImageTextLineInfo.begin();
			Mat PlateNumberLineGrad = InputImage_InputGrad.rowRange(PlateNumberLineInfo.StartRow,
				PlateNumberLineInfo.EndRow);
			int TitleStartCol, TitleEndCol;
			GetPlateNumberLineCol(PlateNumberLineGrad, PlateNumberLineInfo, TitleStartCol, TitleEndCol);
			PlateNumberLineCol2PlateNumberArea();
		}

		InputImagePlateNumberAreaRect = InputImagePlateNumberAreaRect & Rect(0, 0, InputImageHeight, InputImageWidth);
		PlateNumberAreaGradX = InputImage_InputGradX(InputImagePlateNumberAreaRect);

	}

	

}