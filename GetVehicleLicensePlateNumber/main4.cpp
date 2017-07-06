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

double WindowHeight = 700.0;

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


//主函数，输入命令行参数：
//第1个为待处理图片的路径：可以使用Glob表达式选中多个文件，不同文件将逐个循环处理
//第2个参数为结果输出路径,默认在该文件夹下以“Result_”+输入图像文件名的方式输出结果
int main(int ArgumentCount, char** ArgumentVector)
{

	//检查命令行所带参数数目是否正确，如果不正确则显示用法说明并退出程序
	if (ArgumentCount != 3)
	{
		//显示程序用法说明
		cout << " Usage:  " << ArgumentVector[0] << "ImagePathGlobExpression" << "OutputPath" << endl;
		//返回错误码并退出程序
		return -1;
	}
	///////////////////////////////////////////////////
	//对符合的图片进行遍历

		//新建矩阵RawImageMat用于存储原始图片数据
	Mat RawTemplate;

	//根据第一个参数的文件路径进行图片读取
	RawTemplate = imread(
		"E:\\Desktop\\TestImage\\Template2.jpg",//输入图片路径
		CV_LOAD_IMAGE_UNCHANGED//以不修改通道类型的方式读取图片
	);

	//检查读入的Mat是否包含图像数据
	if (!RawTemplate.data)
	{
		//显示图片读取失败提示信息
		cout << " Error:  Can't read image data from E:\\Desktop\\TestImage\\Template2.jpg"<< endl;
		//返回错误码并退出程序
		return -1;
	}
	//计算输入图像的宽、高、面积和宽高比
	const double TemplateWidth = RawTemplate.cols;
	const double TemplateHeight = RawTemplate.rows;
	const double TemplateRatio = TemplateWidth / TemplateHeight;

	//将图片转换成灰阶图像
	Mat Raw_GrayTemplate;

	//获取图片的通道数
	int NumTemplateChannel = RawTemplate.channels();

	//如果图像为3通道彩色图像
	if (NumTemplateChannel == 3)
	{
		//将图片由BGR转换成灰阶图像
		cvtColor(
			RawTemplate,//输入图片矩阵
			Raw_GrayTemplate,//输出图片矩阵 
			COLOR_BGR2GRAY//将图片由BGR（OpenCV默认通道格式）转换成灰阶图像
		);
	}

	//如果图像为4通道（包含alpha通道）图像，则将其转换成灰阶图像
	else if (NumTemplateChannel == 4)
	{
		//将图片由BGRA转换成灰阶图像
		cvtColor(
			RawTemplate,
			Raw_GrayTemplate,
			COLOR_BGRA2GRAY//将图片由BGRA转换成灰阶图像
		);
	}
	//如果图像已经为单通道灰阶图像，直接将ResizedImageMat赋给GrayImageMat
	else if (NumTemplateChannel == 1)
	{
		Raw_GrayTemplate = RawTemplate;
	}

	//如果通道数不为1,3或4，输出错误码并退出程序
	else
	{
		cout << "Unkown image channel type: " << RawTemplate.channels();
	}


	//创建矩阵用于存放图像X方向的梯度值
	Mat GrayTemplate_GradY(
		int(TemplateHeight),//矩阵的第一维（高度）尺寸
		int(TemplateWidth), //矩阵的第二维（宽度）尺寸
		CV_8UC1,//矩阵的值类型，在这里是单通道8位无符号整数 
		Scalar(0)//矩阵填充的初始值
	);
	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < TemplateHeight - 1; iRow++)
	{
		for (int iCol = 1; iCol < (TemplateWidth - 1); iCol++)
		{
			GrayTemplate_GradY.at<uchar>(iRow, iCol) = (2 * abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol - 1))
				+ abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol))
				+ abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol + 1))) / 4;
		}
	}

	Mat GrayTemplate_BinaryGradY;/////////////////////////////////

	threshold(
		GrayTemplate_GradY, //输入矩阵
		GrayTemplate_BinaryGradY, //输出矩阵
		128, //迭代初始阈值
		255, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);


	//创建Y梯度投影向量
	Mat  Binary_ProjectYGradY(
		int(TemplateHeight),//矩阵行数
		1,//矩阵列数
		CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
		Scalar(0)//矩阵填入的初始值
	);

	//临时加和变量
	double SumTemp;
	for (int iRow = 0; iRow < TemplateHeight; iRow++)
	{
		//每次叠加前将加和变量清零
		SumTemp = 0;

		//叠加同一行每一列的梯度值
		for (int iCol = 0; iCol < TemplateWidth; iCol++)
		{
			SumTemp += double(GrayTemplate_BinaryGradY.at<uchar>(iRow, iCol));
		}
		//求叠加值的均值作为水平投影后的梯度值
		Binary_ProjectYGradY.at<uchar>(iRow, 0) = unsigned char(SumTemp / TemplateWidth);
	}

	Mat ProjectY_DiffGradY(
		int(TemplateHeight),//矩阵行数
		1,//矩阵列数
		CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
		Scalar(0)//矩阵填入的初始值
	);


	for (int iRow = 1; iRow < TemplateHeight - 1; iRow++)
	{
		ProjectY_DiffGradY.at<uchar>(iRow, 0) = 
			abs(Binary_ProjectYGradY.at<uchar>(iRow, 0) - Binary_ProjectYGradY.at<uchar>(iRow - 1, 0));
	}

	Mat Diff_BinaryGradY;
	threshold(
		ProjectY_DiffGradY, //输入矩阵
		Diff_BinaryGradY, //输出矩阵
		128, //迭代初始阈值
		255, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);
	vector<size_t> DiffGradY_PeakRow;
	vector<uchar> DiffGradY_PeakAmp;
	for (size_t iRow = 0;
		iRow < TemplateHeight;
		iRow++)
	{
		if (Diff_BinaryGradY.at<uchar>(iRow, 0) == 255)
		{

			DiffGradY_PeakAmp.push_back(ProjectY_DiffGradY.at<uchar>(iRow, 0));
			DiffGradY_PeakRow.push_back(iRow);
		}
	}
	vector<size_t> SortedPeakIndex = SortIndex(DiffGradY_PeakAmp);
	//vector <uchar>  BinaryGradY_SortPeakAmp;
	vector <size_t> DiffGradY_SortPeakRow;
	for (vector<size_t>::iterator itPeakIndex = SortedPeakIndex.begin();
		itPeakIndex < SortedPeakIndex.end();
		itPeakIndex++)
	{
		//BinaryGradY_SortPeakAmp.push_back(BinaryGradY_PeakAmp[*itPeakIndex]);
		DiffGradY_SortPeakRow.push_back(DiffGradY_PeakRow[*itPeakIndex]);
	}

	bool FlagIgnorePeak = false;

	//vector <uchar>  Sort_FilterPeakAmp;
	vector <size_t> SortPeakRow_LineRow;

	double MinPeakDistance = 80 * 0.6;
	SortPeakRow_LineRow.push_back(DiffGradY_SortPeakRow[0]);

	for (size_t iPeak = 1; iPeak < DiffGradY_SortPeakRow.size(); iPeak++)
	{
		for (size_t jPeak = 0; jPeak < SortPeakRow_LineRow.size(); jPeak++)
		{
			if (fabs(double(DiffGradY_SortPeakRow[iPeak]) - double(SortPeakRow_LineRow[jPeak])) < MinPeakDistance)
			{
				FlagIgnorePeak = true;
				break;
			}
		}
		if (FlagIgnorePeak == true)
		{
			FlagIgnorePeak = false;
		}
		else
		{
			SortPeakRow_LineRow.push_back(DiffGradY_SortPeakRow[iPeak]);
			//Sort_FilterPeakAmp.push_back(BinaryGradY_SortPeakAmp[iPeak]);
		}
	}

	SortPeakRow_LineRow.push_back(0);
	SortPeakRow_LineRow.push_back(TemplateHeight-1);

	sort(SortPeakRow_LineRow.begin(), SortPeakRow_LineRow.end());

	vector <size_t>  TemplateLineRow = SortPeakRow_LineRow;
	Mat TemplateDiffGradX = ProjectY_DiffGradY;
	////////////////////////////////////////////////////////
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


		////创建相应的窗口
		//namedWindow(
		//	MainWindowName,//窗口名称
		//	CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		//);
		////将图片窗口设置为固定高度500像素，且不改变图像的宽高比
		//double WindowHeight = 700.0;
		//resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		//imshow(
		//	MainWindowName,
		//	Raw_GrayInput
		//);

		//waitKey(0);


		//设置行驶证的比例限制范围
		//水平方向至少占据宽度的60%
		double LicenseProportionXMin = 0.6;
		//水平方向至多达到宽度的110%
		double LicenseProportionXMax = 1.2;
		//垂直方向至少占据高度的60%
		double LicenseProportionYMin = 0.6;
		//垂直方向至多达到宽度的110%
		double LicenseProportionYMax = 1.2;


		//创建矩阵用于存放图像X方向的梯度值
		Mat GrayInput_GradY(
			int(InputImageHeight),//矩阵的第一维（高度）尺寸
			int(InputImageWidth), //矩阵的第二维（宽度）尺寸
			CV_8UC1,//矩阵的值类型，在这里是单通道8位无符号整数 
			Scalar(0)//矩阵填充的初始值
		);
		//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			for (int iCol = 1; iCol < (InputImageWidth - 1); iCol++)
			{
				GrayInput_GradY.at<uchar>(iRow, iCol) = (2 * abs(Raw_GrayInput.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayInput.at<uchar>(iRow + 1, iCol - 1))
					+ abs(Raw_GrayInput.at<uchar>(iRow - 1, iCol) - Raw_GrayInput.at<uchar>(iRow + 1, iCol))
					+ abs(Raw_GrayInput.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayInput.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}

		Mat GrayInput_BinaryGradY;

		threshold(
			GrayInput_GradY, //输入矩阵
			GrayInput_BinaryGradY, //输出矩阵
			128, //迭代初始阈值
			255, //最大值（超过阈值将设为此值）
			CV_THRESH_OTSU //阈值化选择的方法:Otsu法
		);


		//创建Y梯度投影向量
		Mat  Binary_ProjectYGradY(
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
				SumTemp += double(GrayInput_BinaryGradY.at<uchar>(iRow, iCol));
			}
			//求叠加值的均值作为水平投影后的梯度值
			Binary_ProjectYGradY.at<uchar>(iRow, 0) = unsigned char(SumTemp / InputImageWidth);
		}

		//Mat ProjectY_DiffGradY(
		//	int(InputImageHeight),//矩阵行数
		//	1,//矩阵列数
		//	CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
		//	Scalar(0)//矩阵填入的初始值
		//);


		//Mat DiffGradX_StemFigure(
		//	int(InputImageHeight),//矩阵行数
		//	int(InputImageWidth),//矩阵列数
		//	CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
		//	Scalar(0)//矩阵填入的初始值
		//);
		//for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		//{
		//	ProjectY_DiffGradY.at<uchar>(iRow, 0) =
		//		abs(Binary_ProjectYGradY.at<uchar>(iRow, 0) - Binary_ProjectYGradY.at<uchar>(iRow - 1, 0));
		//	
		//}

		

		Mat ProjectY_BinaryGradY;
		threshold(
			Binary_ProjectYGradY, //输入矩阵
			ProjectY_BinaryGradY, //输出矩阵
			128, //迭代初始阈值
			255, //最大值（超过阈值将设为此值）
			CV_THRESH_OTSU //阈值化选择的方法:Otsu法
		);

		//uchar StemColor;
		//for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		//{
		//	if (Diff_BinaryGradY.at<uchar>(iRow,0) == 255)
		//	{
		//		StemColor = 255;
		//	}
		//	else
		//	{
		//		StemColor = 100;
		//	}

		//	//根据投影后的梯度值绘制Stem图，每一行根据Stem值大小绘制不同宽度的Stem
		//	for (int iCol = 0; iCol < ProjectY_DiffGradY.at<uchar>(iRow, 0) / 255.0*InputImageWidth; iCol++)
		//	{ 
		//		DiffGradX_StemFigure.at<uchar>(iRow, iCol) = StemColor;
		//	}
		//}
		vector<size_t> DiffGradY_EdgeRow;
		vector<uchar> DiffGradY_EdgeAmp;

		for (int iRow = 0; iRow < InputImageHeight-1; iRow++)
		{
			if (abs(int(ProjectY_BinaryGradY.at<uchar>(iRow + 1, 0)) - int(ProjectY_BinaryGradY.at<uchar>(iRow, 0))) == 255)
			{
				DiffGradY_EdgeRow.push_back(iRow);
				DiffGradY_EdgeAmp.push_back(abs(int(Binary_ProjectYGradY.at<uchar>(iRow + 1, 0)) - int(Binary_ProjectYGradY.at<uchar>(iRow, 0))));

			}
		}
		 
		vector<size_t> SortedEdgeIndex = SortIndex(DiffGradY_EdgeAmp);
		//vector <uchar>  BinaryGradY_SortEdgeAmp;
		vector <size_t> DiffGradY_SortEdgeRow;
		for (vector<size_t>::iterator itEdgeIndex = SortedEdgeIndex.begin();
			itEdgeIndex < SortedEdgeIndex.end();
			itEdgeIndex++)
		{
			//BinaryGradY_SortEdgeAmp.push_back(BinaryGradY_EdgeAmp[*itEdgeIndex]);
			DiffGradY_SortEdgeRow.push_back(DiffGradY_EdgeRow[*itEdgeIndex]);
		}

		bool FlagIgnoreEdge = false;

		//vector <uchar>  Sort_FilterEdgeAmp;
		vector <size_t> SortEdgeRow_LineRow;

		double MinEdgeDistance = InputImageHeight/TemplateHeight *80 * 0.6;
		SortEdgeRow_LineRow.push_back(DiffGradY_SortEdgeRow[0]);

		for (size_t iEdge = 1; iEdge < DiffGradY_SortEdgeRow.size(); iEdge++)
		{
			for (size_t jEdge = 0; jEdge < SortEdgeRow_LineRow.size(); jEdge++)
			{
				if (fabs(double(DiffGradY_SortEdgeRow[iEdge]) - double(SortEdgeRow_LineRow[jEdge])) < MinEdgeDistance)
				{
					FlagIgnoreEdge = true;
					break;
				}
			}
			if (FlagIgnoreEdge == true)
			{
				FlagIgnoreEdge = false;
			}
			else
			{
				SortEdgeRow_LineRow.push_back(DiffGradY_SortEdgeRow[iEdge]);
				//Sort_FilterEdgeAmp.push_back(BinaryGradY_SortEdgeAmp[iEdge]);
			}
		}

		SortEdgeRow_LineRow.push_back(0);
		SortEdgeRow_LineRow.push_back(InputImageHeight - 1);

		sort(SortEdgeRow_LineRow.begin(), SortEdgeRow_LineRow.end());

		for (vector<size_t>::iterator itLineY = SortEdgeRow_LineRow.begin();
			itLineY < SortEdgeRow_LineRow.end();
			itLineY++)
		{
			if (*itLineY == 0)
			{
				RawInput(Range(0, 1), Range::all()) = Scalar(0, 0, 255);
			}
			else if (*itLineY == InputImageHeight)
			{
				RawInput(Range(InputImageHeight-1, InputImageHeight), Range::all()) = Scalar(0, 0, 255);
			}
			else
			{
				RawInput(Range(*itLineY - 1, *itLineY + 1), Range::all()) = Scalar(0, 0, 255);
			}
		}

		//	Mat  FilterPeakIndex_StemFigure(
			//	int(InputImageHeight),//矩阵行数
			//	int(InputImageWidth),//矩阵列数
			//	CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
			//	Scalar(0)//矩阵填入的初始值
			//);
			//

			//sort(PartialSort_FilterPeakIndex.begin(), PartialSort_FilterPeakIndex.end());
			//
			//vector<double> Filter_DiffPeakIndex(PartialSort_FilterPeakIndex.size()-1);
			//for (size_t iPeak = 1;
			//	iPeak < PartialSort_FilterPeakIndex.size();
			//	iPeak++)
			//{
			//	Filter_DiffPeakIndex[iPeak - 1] = PartialSort_FilterPeakIndex[iPeak] - PartialSort_FilterPeakIndex[iPeak-1];
			//}

			//sort(Filter_DiffPeakIndex.begin(), Filter_DiffPeakIndex.end());

			//double EstimatedLineGapWidth;
			//if (Filter_DiffPeakIndex.size() % 2 == 1)
			//{
			//	EstimatedLineGapWidth = Filter_DiffPeakIndex[Filter_DiffPeakIndex.size() / 2];
			//}
			//else
			//{
			//	EstimatedLineGapWidth = (Filter_DiffPeakIndex[(Filter_DiffPeakIndex.size()) / 2-1] +
			//		Filter_DiffPeakIndex[(Filter_DiffPeakIndex.size()) / 2 ])/2;
			//}
			//for (vector<size_t>::iterator itLine = PartialSort_FilterPeakIndex.begin();
			//	itLine != PartialSort_FilterPeakIndex.end();
			//	itLine++)
			//{
			//	RawImage(Range(*itLine-1,*itLine+1),Range::all()) = Scalar(0, 0, 255);
			//}
 	//	imshow(MainWindowName, RawInput);
		//waitKey(0);
  	}
	//返回0并正常退出程序
	return 0; 
}