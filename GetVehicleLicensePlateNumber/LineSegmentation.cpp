//添加OpenCV相关头文件
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//添加标准库输入输出流头文件
#include <iostream>
//添加算法库头文件
#include<algorithm>//为了使用swap
#include<numeric>//为了使用iota
//添加数学运算库头文件
//#include <cmath>

//使用C++标准库命名空间
using namespace std;

//使用OpenCV库命名空间
using namespace cv;

//从模板图片中获得的位置信息
//模板图片的宽、高
const double TemplateWidth = 1044;
const double TemplateHeight = 716;
//模板图片标题所在区域的宽、高及区域中心的XY坐标
const double TemplateTitleWidth = 684;
const double TemplateTitleHeight = 68;
const double TemplateTitleCenterX = 512;
const double TemplateTitleCenterY = 72;
//模板图片标题部分的水平间隙高度（水平投影时中文和英文之间出现的空隙宽度）
const double TemplateTitleGapHeight = 5;
//模板图片标题部分的垂直间隙宽度（垂直投影时文字之间出现的空隙宽度）
const double TemplateTitleGapWidth = 40;
//模板图片标题部分英文的高度
const double TemplateTitleEnglishHeight = 14;
//模板图片车牌区域的宽、高及区域中心的XY坐标
const double TemplatePlateWidth = 174;
const double TemplatePlateHeight = 41;
const double TemplatePlateCenterX = 271;
const double TemplatePlateCenterY = 166;

double WindowHeight = 500.0;

template <typename SortValueType>
vector<size_t>  SortIndex(vector<SortValueType> &InputValueVector) {

	// initialize original index locations
	vector<size_t> IndexVector(InputValueVector.size());
	iota(IndexVector.begin(), IndexVector.end(), 0);

	// sort indexes based on comparing values in v
	sort(IndexVector.begin(),
		IndexVector.end(),
		[&InputValueVector](size_t x, size_t y) {return InputValueVector[x] > InputValueVector[y]; });
	return IndexVector;
}


int Dft1D(Mat & Input,Mat & Output)
{
	Mat Pad_Input;                            //expand input image to optimal size
	int NumPadInputCol = getOptimalDFTSize(Input.cols);
	copyMakeBorder(Input, Pad_Input, 0, 0, 0, NumPadInputCol - Input.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat DftPlane[] = { Mat_<float>(Pad_Input), Mat::zeros(Pad_Input.size(), CV_32F) };
	Mat DftComplexMat;
	merge(DftPlane, 2, DftComplexMat);         // Add to the expanded another plane with zeros

	dft(DftComplexMat, DftComplexMat,DFT_ROWS);            // this way the result may fit in the source matrix

										
	split(DftComplexMat, DftPlane);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(DftPlane[0], DftPlane[1], DftPlane[0]);// planes[0] = magnitude  
	Mat DftMagnitude = DftPlane[0];
	//DftMagnitude += Scalar::all(1);                    // switch to logarithmic scale
	//log(DftMagnitude, Output);
	////Output =  DftPlane[0];
	//// crop the spectrum, if it has an odd number of rows or columns
	////Output( Range::all(),Range(3, 20)) = DftMagnitude(Range::all(), Range(3, 20));
	for (int iCol = 5 ;iCol< 20;iCol++)
	{
		Output.at<float>(0, iCol) = DftMagnitude.at<float>(0, iCol);
	}
	normalize(Output, Output,1.0, 0.0, NORM_MINMAX);

	return 0;
}

//主函数，输入命令行参数：
//第1个为待处理图片的路径：可以使用Glob表达式选中多个文件，不同文件将逐个循环处理
//第2个参数为结果输出路径,默认在该文件夹下以“Result_”+输入图像文件名的方式输出结果
int main(int ArgumentCount, char** ArgumentVector)
{


	//分别读取两个命令参数
	String SearchGlobExpression = ArgumentVector[1];
	String OutputPath = ArgumentVector[2];

	//创建用于存储图片路径的String数组
	vector<String> ImagePathList;

	//根据输入Glob表达式查找符合的图片
	glob(
		SearchGlobExpression,//文件查找Glob表达式
		ImagePathList, //输出图像文件列表
		false//不进行递归，即不对子文件夹进行搜索
	);

	//对符合的图片进行遍历
	for (size_t iInput = 0; iInput < ImagePathList.size(); iInput++)
	{
		//新建矩阵RawImageMat用于存储原始图片数据
		Mat RawInput;

		//根据第一个参数的文件路径进行图片读取
		RawInput = imread(
			ImagePathList[iInput],//输入图片路径
			CV_LOAD_IMAGE_UNCHANGED//以不修改通道类型的方式读取图片
		);

		//检查读入的Mat是否包含图像数据
		if (!RawInput.data)
		{
			//显示图片读取失败提示信息
			cout << " Error:  Can't read image data from" << ArgumentVector[1] << endl;
			//返回错误码并退出程序
			return -1;
		}
		//计算输入图像的宽、高、面积和宽高比
		const double InputImageWidth = RawInput.cols;
		const double InputImageHeight = RawInput.rows;


		//将图片转换成灰阶图像
		Mat Raw_GrayInput;

		//获取图片的通道数
		int NumRawInputChannel = RawInput.channels();

		//如果图像为3通道彩色图像
		if (NumRawInputChannel == 3)
		{
			//将图片由BGR转换成灰阶图像
			cvtColor(
				RawInput,//输入图片矩阵
				Raw_GrayInput,//输出图片矩阵 
				COLOR_BGR2GRAY//将图片由BGR（OpenCV默认通道格式）转换成灰阶图像
			);
		}

		//如果图像为4通道（包含alpha通道）图像，则将其转换成灰阶图像
		else if (NumRawInputChannel == 4)
		{
			//将图片由BGRA转换成灰阶图像
			cvtColor(
				RawInput,
				Raw_GrayInput,
				COLOR_BGRA2GRAY//将图片由BGRA转换成灰阶图像
			);
		}
		//如果图像已经为单通道灰阶图像，直接将ResizedImageMat赋给GrayImageMat
		else if (NumRawInputChannel == 1)
		{
			Raw_GrayInput = RawInput;
		}

		//如果通道数不为1,3或4，输出错误码并退出程序
		else
		{
			cout << "Unkown image channel type: " << RawInput.channels();
		}


		//指定窗口名称
		const char* MainWindowName = "get plate number from vehicle license";


		//创建相应的窗口
		namedWindow(
			MainWindowName,//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);

		resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		//imshow(
		//	MainWindowName,
		//	Raw_GrayInput
		//);

		//waitKey(0);


		//创建矩阵用于存放图像X方向的梯度值
		Mat GrayInput_Grad(
			int(InputImageHeight),//矩阵的第一维（高度）尺寸
			int(InputImageWidth), //矩阵的第二维（宽度）尺寸
			CV_8UC1,//矩阵的值类型，在这里是单通道8位无符号整数 
			Scalar(0)//矩阵填充的初始值
		);

		double TempGradX,TempGradY;
		//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			for (int iCol = 1; iCol < (InputImageWidth - 1); iCol++)
			{
				TempGradY = (10 * (Raw_GrayInput.at<uchar>(iRow + 1, iCol - 1) - Raw_GrayInput.at<uchar>(iRow - 1, iCol - 1))
					+ 3* (Raw_GrayInput.at<uchar>(iRow + 1, iCol) - Raw_GrayInput.at<uchar>(iRow - 1, iCol))
					+ 3* (Raw_GrayInput.at<uchar>(iRow + 1, iCol + 1) - Raw_GrayInput.at<uchar>(iRow - 1, iCol + 1))) / 32.0;

				TempGradX = (10 * (Raw_GrayInput.at<uchar>(iRow, iCol + 1) - Raw_GrayInput.at<uchar>(iRow, iCol - 1))
					+ 3 * (Raw_GrayInput.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayInput.at<uchar>(iRow - 1, iCol - 1))
					+ 3 * (Raw_GrayInput.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayInput.at<uchar>(iRow + 1, iCol - 1))) / 32.0; 
				GrayInput_Grad.at<uchar>(iRow, iCol) = uchar(fabs(TempGradY));
				//GrayInput_Grad.at<uchar>(iRow, iCol) = uchar(fabs(TempGradX) + fabs(TempGradY));
			}
		}

		Mat GrayInput_BinaryGrad;

		threshold(
			GrayInput_Grad, //输入矩阵
			GrayInput_BinaryGrad, //输出矩阵
			128, //迭代初始阈值
			255, //最大值（超过阈值将设为此值）
			CV_THRESH_OTSU //阈值化选择的方法:Otsu法
		);

		imshow(
			MainWindowName,
			GrayInput_BinaryGrad
		);
		//创建Y梯度投影向量
		Mat  Binary_ProjectYGrad(
			int(InputImageHeight),//矩阵行数
			1,//矩阵列数
			CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
			Scalar(0)//矩阵填入的初始值
		);

		//临时加和变量
		double SumTemp;
		for (int iRow = 0; iRow < InputImageHeight; iRow++)
		{
			//每次叠加前将加和变量清零
			SumTemp = 0;

			//叠加同一行每一列的梯度值
			for (int iCol = 0; iCol < InputImageWidth; iCol++)
			{
				SumTemp += double(GrayInput_BinaryGrad.at<uchar>(iRow, iCol));
			}
			//求叠加值的均值作为水平投影后的梯度值
			Binary_ProjectYGrad.at<uchar>(iRow, 0) = unsigned char(SumTemp / InputImageWidth);
		}

		Mat ProjectY_DiffGrad(
			int(InputImageHeight),//矩阵行数
			1,//矩阵列数
			CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
			Scalar(0)//矩阵填入的初始值
		);
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			ProjectY_DiffGrad.at<uchar>(iRow, 0) =
				abs(Binary_ProjectYGrad.at<uchar>(iRow, 0) - Binary_ProjectYGrad.at<uchar>(iRow - 1, 0));

		}
		Binary_ProjectYGrad = ProjectY_DiffGrad;

		Mat ProjectYGrad_Histogram(
			int(InputImageHeight),//矩阵行数
			int(InputImageWidth),//矩阵列数
			CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
			Scalar(0)//矩阵填入的初始值
		);




		//Mat ProjectY_BinaryGrad;
		//threshold(
		//	Binary_ProjectYGrad, //输入矩阵
		//	ProjectY_BinaryGrad, //输出矩阵
		//	128, //迭代初始阈值
		//	255, //最大值（超过阈值将设为此值）
		//	CV_THRESH_OTSU //阈值化选择的方法:Otsu法
		//);

		uchar HistogramColor= 255;
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			/*if (Diff_BinaryGradY.at<uchar>(iRow,0) == 255)
			{
				HistogramColor = 255; 
			}
			else
			{
				HistogramColor = 100;
			}*/

			//根据投影后的梯度值绘制Stem图，每一行根据Stem值大小绘制不同宽度的Stem
			for (int iCol = 0; iCol <  Binary_ProjectYGrad.at<uchar>(iRow, 0) / 255.0*InputImageWidth; iCol++)
			{ 
				ProjectYGrad_Histogram.at<uchar>(iRow, iCol) = HistogramColor;
			}
		}

		//创建相应的窗口
		namedWindow(
			"ProjectYGrad_Histogram",//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);
		resizeWindow("ProjectYGrad_Histogram", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		imshow(
			"ProjectYGrad_Histogram",
			ProjectYGrad_Histogram
		);
		//waitKey(0);

		Mat DftMagnitude(
			1,
			int(InputImageHeight),
			CV_32FC1,//矩阵值的类型（8位无符号整数单通道）
			Scalar(0.0)//矩阵填入的初始值
			);
		Binary_ProjectYGrad = Binary_ProjectYGrad.t();
		Dft1D(Binary_ProjectYGrad, DftMagnitude);

		Mat DftMagnitude_Histogram(
			int(InputImageHeight),
			int(InputImageWidth),
			CV_8UC1,
			Scalar(0)
		);

		for (int iRow = 0; iRow < InputImageHeight ; iRow++)
		{
			/*if (Diff_BinaryGradY.at<uchar>(iRow,0) == 255)
			{
			HistogramColor = 255;
			}
			else
			{
			HistogramColor = 100;
			}*/

			//根据投影后的梯度值绘制Stem图，每一行根据Stem值大小绘制不同宽度的Stem
			for (int iCol = 0; iCol < DftMagnitude.at<float>(0, iRow)*InputImageWidth; iCol++)
			{
 				DftMagnitude_Histogram.at<uchar>(iRow, iCol) = HistogramColor;
			}
		}
		
		//创建相应的窗口
		namedWindow(
			" DftMagnitude_Histogram",//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);
		resizeWindow(" DftMagnitude_Histogram", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		imshow(
			" DftMagnitude_Histogram",
			DftMagnitude_Histogram
		);
		//waitKey(0);
		//vector<size_t> BinaryGradY_EdgeRow;
		//vector<uchar> BinaryGradY_EdgeAmp;

		//for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		//{
		//	if (abs(int(ProjectY_BinaryGrad.at<uchar>(iRow + 1, 0)) - int(ProjectY_BinaryGrad.at<uchar>(iRow, 0))) == 255)
		//	{
		//		BinaryGradY_EdgeRow.push_back(iRow);
		//		BinaryGradY_EdgeAmp.push_back(abs(int(Binary_ProjectYGrad.at<uchar>(iRow + 1, 0)) - int(Binary_ProjectYGrad.at<uchar>(iRow, 0))));

		//	}
		//}

		//vector<size_t> SortedEdgeIndex = SortIndex(BinaryGradY_EdgeAmp);
		////vector <uchar>  BinaryGradY_SortEdgeAmp;
		//vector <size_t> DiffGradY_SortEdgeRow;
		//for (vector<size_t>::iterator itEdgeIndex = SortedEdgeIndex.begin();
		//	itEdgeIndex < SortedEdgeIndex.end();
		//	itEdgeIndex++)
		//{
		//	//BinaryGradY_SortEdgeAmp.push_back(BinaryGradY_EdgeAmp[*itEdgeIndex]);
		//	DiffGradY_SortEdgeRow.push_back(BinaryGradY_EdgeRow[*itEdgeIndex]);
		//}

		//bool FlagIgnoreEdge = false;

		////vector <uchar>  Sort_FilterEdgeAmp;
		//vector <size_t> SortEdgeRow_LineRow;

		//double MinEdgeDistance = InputImageHeight / TemplateHeight * 80 * 0.6;
		//SortEdgeRow_LineRow.push_back(DiffGradY_SortEdgeRow[0]);

		//for (size_t iEdge = 1; iEdge < DiffGradY_SortEdgeRow.size(); iEdge++)
		//{
		//	for (size_t jEdge = 0; jEdge < SortEdgeRow_LineRow.size(); jEdge++)
		//	{
		//		if (abs(int(DiffGradY_SortEdgeRow[iEdge]) - int(SortEdgeRow_LineRow[jEdge])) < MinEdgeDistance)
		//		{
		//			FlagIgnoreEdge = true;
		//			break;
		//		}
		//	}
		//	if (FlagIgnoreEdge == true)
		//	{
		//		FlagIgnoreEdge = false;
		//	}
		//	else
		//	{
		//		SortEdgeRow_LineRow.push_back(DiffGradY_SortEdgeRow[iEdge]);
		//		//Sort_FilterEdgeAmp.push_back(BinaryGradY_SortEdgeAmp[iEdge]);
		//	}
		//}

		//SortEdgeRow_LineRow.push_back(0);
		//SortEdgeRow_LineRow.push_back(InputImageHeight - 1);

		//sort(SortEdgeRow_LineRow.begin(), SortEdgeRow_LineRow.end());
		//vector <int64> SortEdgeRow_LineGapWidth;
		//RawInput.row(SortEdgeRow_LineRow[0]) = Scalar(0, 0, 255);
		//for (vector<size_t>::iterator itLine = SortEdgeRow_LineRow.begin() + 1;
		//	itLine < SortEdgeRow_LineRow.end();
		//	itLine++)
		//{
		//	SortEdgeRow_LineGapWidth.push_back(*itLine - *(itLine - 1));
		//	RawInput.row(*itLine) = Scalar(0, 0, 255);
		//}



		//int64 InputMedianLineGapWidth;
		//if (SortEdgeRow_LineGapWidth.size() % 2 == 1)
		//{
		//	partial_sort(SortEdgeRow_LineGapWidth.begin(),
		//		SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 + 1,
		//		SortEdgeRow_LineGapWidth.end());
		//	InputMedianLineGapWidth = *(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2);
		//}
		//else
		//{
		//	partial_sort(SortEdgeRow_LineGapWidth.begin(),
		//		SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 + 2,
		//		SortEdgeRow_LineGapWidth.end());
		//	InputMedianLineGapWidth = (*(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2) +
		//		*(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 + 1)) / 2;
		//}

		//sort(SortEdgeRow_LineGapWidth.begin(),
		//	SortEdgeRow_LineGapWidth.end(),
		//	[InputMedianLineGapWidth](int64 x, int64 y) {return abs(x- InputMedianLineGapWidth) < abs(y - InputMedianLineGapWidth); });

		//vector <size_t>  InputLineRow = SortEdgeRow_LineRow;

		//vector <int64> InputLineGapWidth = SortEdgeRow_LineGapWidth;

		//Mat InputProjectYGradY = Binary_ProjectYGradY;

		//size_t TempInputLicenseHeight, TempInputMatchStartRow, TempInputMatchEndRow;
		//double TempInputHeightProportion;
		//vector <double> InputHeightProportion;
		//vector <size_t> InputLicenseHeight,InputMatchStartRow, InputMatchEndRow, TemplateMatchStartRow, TemplateMatchEndRow;
		//for (vector<int64>::iterator itInputGapWidth = InputLineGapWidth.begin();
		//	itInputGapWidth != InputLineGapWidth.end();
		//	itInputGapWidth++)
		//{

		//		for (vector<size_t>::iterator itCrossLine = InputLineRow.begin()+1;
		//			itCrossLine != TemplateLineRow.end();
		//			itCrossLine++)
		//		{
		//		
		//			TempInputLicenseHeight = size_t(double(*itInputGapWidth / TemplateMedianLineGapWidth) *TemplateHeight);

		//			TempInputMatchEndRow = *itCrossLine;

		//			TempInputMatchStartRow = (TempInputMatchEndRow - TempInputLicenseHeight) <0 ? 0:(TempInputMatchEndRow - TempInputLicenseHeight);
		//			
		//			TempInputHeightProportion = (TempInputMatchEndRow - TempInputMatchStartRow) / InputImageHeight;
		//			if (TempInputHeightProportion > 0.6)
		//			{
		//				InputMatchStartRow.push_back(TempInputMatchStartRow);
		//				InputMatchEndRow.push_back(TempInputMatchEndRow);
		//				InputLicenseHeight.push_back(TempInputLicenseHeight);
		//				TemplateMatchStartRow.push_back(*itTemplateLine);
		//				TemplateMatchEndRow.push_back(TemplateMatchStartRow.back() +
		//					double((TempInputMatchEndRow - TempInputMatchStartRow) / TempInputLicenseHeight)*TemplateHeight);

		//				InputHeightProportion.push_back(TempInputHeightProportion);
		//			}
		//			
		//		}

		//}

		//vector <size_t> SortedMatchIndex = SortIndex(InputHeightProportion);

		//for (vector <size_t>::iterator iMatch = SortedMatchIndex.begin();
		//	iMatch < SortedMatchIndex.end;
		//	iMatch++)
		//{
		//	Mat ResizedTemplateGradY(
		//		InputLicenseHeight[*iMatch],
		//		1,
		//		CV_8UC1,
		//		0);
		//	cv::resize(ProjectYTemplateGradY,
		//		ResizedTemplateGradY,
		//		ResizedTemplateGradY.size(),
		//		0,
		//		0,
		//		INTER_LINEAR
		//	);
		//	Mat MatchTemplateGradY = ResizedTemplateGradY(Range(0, TemplateMatchEndRow[*iMatch]),Range::all());
		//	Mat MatchInputGradY = ResizedTemplateGradY(Range(TemplateMatchStartRow[*iMatch], TemplateMatchEndRow[*iMatch]), Range::all());
		//	cv::meanStdDev(MatchTemplateGradY)
		//}

		
		//	imshow(MainWindowName, RawInput);
		//waitKey(0);
  	}
	//返回0并正常退出程序
	return 0;
}