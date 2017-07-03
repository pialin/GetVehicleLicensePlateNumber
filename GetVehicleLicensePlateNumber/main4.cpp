//添加OpenCV相关头文件
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//添加标准库输入输出流头文件
#include <iostream>
//添加算法库头文件
#include<algorithm>//为了使用swap
//添加数学运算库头文件
//#include <cmath>

#define NUMPEAK 9

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
	for (size_t iImage = 0; iImage < ImagePathList.size(); iImage++)
	{
		//新建矩阵RawImageMat用于存储原始图片数据
		Mat RawImage;

		//根据第一个参数的文件路径进行图片读取
		RawImage = imread(
			ImagePathList[iImage],//输入图片路径
			CV_LOAD_IMAGE_UNCHANGED//以不修改通道类型的方式读取图片
		);

		//检查读入的Mat是否包含图像数据
		if (!RawImage.data)
		{
			//显示图片读取失败提示信息
			cout << " Error:  Can't read image data from" << ArgumentVector[1] << endl;
			//返回错误码并退出程序
			return -1;
		}
		//计算输入图像的宽、高、面积和宽高比
		const double InputImageWidth = RawImage.cols;
		const double InputImageHeight = RawImage.rows;

		//计算模板图片标题的宽高比
		const double TemplateRatio = TemplateWidth / TemplateHeight;


		//计算模板图片标题的宽高比和面积
		const double TemplateTitleRatio = TemplateTitleWidth / TemplateTitleHeight;

		//计算模板的车牌区域的宽高比和面积
		const double TemplatePlateRatio = TemplatePlateWidth / TemplatePlateHeight;

		//将图片转换成灰阶图像
		Mat Raw_GrayImage;

		//获取图片的通道数
		int NumRawImageChannel = RawImage.channels();

		//如果图像为3通道彩色图像
		if (NumRawImageChannel == 3)
		{
			//将图片由BGR转换成灰阶图像
			cvtColor(
				RawImage,//输入图片矩阵
				Raw_GrayImage,//输出图片矩阵 
				COLOR_BGR2GRAY//将图片由BGR（OpenCV默认通道格式）转换成灰阶图像
			);
		}

		//如果图像为4通道（包含alpha通道）图像，则将其转换成灰阶图像
		else if (NumRawImageChannel == 4)
		{
			//将图片由BGRA转换成灰阶图像
			cvtColor(
				RawImage,
				Raw_GrayImage,
				COLOR_BGRA2GRAY//将图片由BGRA转换成灰阶图像
			);
		}
		//如果图像已经为单通道灰阶图像，直接将ResizedImageMat赋给GrayImageMat
		else if (NumRawImageChannel == 1)
		{
			Raw_GrayImage = RawImage;
		}

		//如果通道数不为1,3或4，输出错误码并退出程序
		else
		{
			cout << "Unkown image channel type: " << RawImage.channels();
		}


		//指定窗口名称
		const char* MainWindowName = "get plate number from vehicle license";


		//创建相应的窗口
		namedWindow(
			MainWindowName,//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);
		//将图片窗口设置为固定高度500像素，且不改变图像的宽高比
		double WindowHeight = 500.0;
		resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		imshow(
			MainWindowName,
			Raw_GrayImage
		);

		waitKey(0);

		//创建矩阵用于存放图像X方向的梯度值
		Mat GrayImage_GradY(
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
				GrayImage_GradY.at<uchar>(iRow, iCol) = (2 * abs(Raw_GrayImage.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayImage.at<uchar>(iRow + 1, iCol - 1))
					+ abs(Raw_GrayImage.at<uchar>(iRow - 1, iCol) - Raw_GrayImage.at<uchar>(iRow + 1, iCol))
					+ abs(Raw_GrayImage.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayImage.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}

		Mat GrayImage_BinaryGradY;

		threshold(
			GrayImage_GradY, //输入矩阵
			GrayImage_BinaryGradY, //输出矩阵
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
				SumTemp += double(GrayImage_BinaryGradY.at<uchar>(iRow, iCol));
			}
			//求叠加值的均值作为水平投影后的梯度值
			Binary_ProjectYGradY.at<uchar>(iRow, 0) = unsigned char(SumTemp / InputImageWidth);
		}

		Mat ProjectY_DiffGradY(
			int(InputImageHeight),//矩阵行数
			1,//矩阵列数
			CV_32SC1,//矩阵值的类型（8位无符号整数单通道）
			Scalar(0)//矩阵填入的初始值
		);
		int TempDiff;
		int DiffGradY_TopNPeakAmp[NUMPEAK+1] = { 0 };
		int DiffGradY_TopNPeakIndex[NUMPEAK+1] = { 0 };
 		for(int iRow=1;iRow<InputImageHeight-1;iRow++)
		{
			TempDiff = Binary_ProjectYGradY.at<uchar>(iRow, 0) - Binary_ProjectYGradY.at<uchar>(iRow - 1, 0);
			if (TempDiff > 0)
			{
				ProjectY_DiffGradY.at<int>(iRow, 0) = TempDiff;
				DiffGradY_TopNPeakAmp[0] = TempDiff;
				DiffGradY_TopNPeakIndex[0] = iRow;
				for (int iPeak = 0;iPeak<NUMPEAK;iPeak++)
				{
					if(DiffGradY_TopNPeakAmp[iPeak] > DiffGradY_TopNPeakAmp[iPeak+1])
					{
						swap(DiffGradY_TopNPeakAmp[iPeak], DiffGradY_TopNPeakAmp[iPeak + 1]);
						swap(DiffGradY_TopNPeakIndex[iPeak], DiffGradY_TopNPeakIndex[iPeak + 1]);
					}
					else
					{
						break;
					}

				}
			}
		}
		
		int TopN_SortPeakAmp[NUMPEAK];
		int TopN_SortPeakIndex[NUMPEAK];
		for (int iPeak1 = 1; iPeak1 < NUMPEAK+1;iPeak1++)
		{
			for (int iPeak2 = iPeak1; iPeak2 < NUMPEAK ; iPeak2++)
			{
				if (DiffGradY_TopNPeakIndex[iPeak2] > DiffGradY_TopNPeakIndex[iPeak2+1])
				{
					swap(DiffGradY_TopNPeakIndex[iPeak2], DiffGradY_TopNPeakIndex[iPeak2 + 1]);
				}
				else
				{
					break;
				}

			}
		}
		for (int iPeak = 1; iPeak < NUMPEAK + 1; iPeak++)
		{
			TopN_SortPeakIndex[iPeak - 1] = DiffGradY_TopNPeakIndex[iPeak];
			TopN_SortPeakAmp[iPeak - 1] = ProjectY_DiffGradY.at<int>(TopN_SortPeakIndex[iPeak - 1], 0);
		}
 		Mat  DiffGradY_StemFigure(
			int(InputImageHeight),//矩阵行数
			int(InputImageWidth),//矩阵列数
			CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
			Scalar(0)//矩阵填入的初始值
		);
		//绘制Stem图时茎的颜色
		unsigned char StemColor;
		//绘制水平梯度水平投影后的Stem图
		for (int iRow = 0; iRow < InputImageHeight; iRow++)
		{
			StemColor = 100;
			for (int iPeak = 0; iPeak < NUMPEAK; iPeak++)
			{
				if (iRow == TopN_SortPeakIndex[iPeak])
				{
					StemColor = 255;
					break;
				}
			}
			////根据二值化结果决定Stem图茎的颜色，若为0，使用灰色代表该行投影值小于阈值
			//if (ProjectY_BinaryGradY.at<uchar>(iRow, 0) == 0)
			//{
			//	StemColor = 100;
			//}
			////若为255，则用白色代表该行投影值大于阈值
			//else
			//{
			//	StemColor = 255;
			//}
			//根据投影后的梯度值绘制Stem图，每一行根据Stem值大小绘制不同宽度的Stem
			if (ProjectY_DiffGradY.at<int>(iRow, 0) > 0)
			{
				for (int iCol = 0; iCol < ProjectY_DiffGradY.at<int>(iRow, 0) / 255.0*InputImageWidth; iCol++)
				{
					DiffGradY_StemFigure.at<uchar>(iRow, iCol) = StemColor;
				}
			}

		}


		//显示边缘二值图像
		namedWindow(
			"ProjectYGradY_StemFigure",//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);
		//将窗口变为高度为WindowHeight，宽高比不变
		resizeWindow("ProjectYGradY_StemFigure", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));
		imshow(
			"ProjectYGradY_StemFigure",
			DiffGradY_StemFigure
		);
		//等待键盘响应，参数0表示等待时间为无限长
		waitKey(0);

		//设置行驶证的比例限制范围
		//水平方向至少占据宽度的60%
		double LicenseProportionXMin = 0.6;
		//水平方向至多达到宽度的110%
		double LicenseProportionXMax = 1.2;
		//垂直方向至少占据高度的60%
		double LicenseProportionYMin = 0.6;
		//垂直方向至多达到宽度的110%
		double LicenseProportionYMax = 1.2;

		//根据比例限制范围计算标题区域的高度可能范围
		double TitleMinHeight = InputImageHeight*LicenseProportionYMin*(TemplateTitleHeight / TemplateHeight);
		double TitleMaxHeight = InputImageHeight*LicenseProportionYMax*(TemplateTitleHeight / TemplateHeight);
		//根据比例限制计算标题部分垂直缝隙最大高度
		double TitleMaxGapHeight = InputImageHeight*LicenseProportionYMax *(TemplateTitleGapHeight / TemplateHeight);
		//根据比例限制计算标题部分英文习题的可能范围
		double TitleMinEnglishHeight = InputImageHeight*LicenseProportionYMin *(TemplateTitleEnglishHeight / TemplateHeight);
		double TitleMaxEnglishHeight = InputImageHeight*LicenseProportionYMax *(TemplateTitleEnglishHeight / TemplateHeight);
		//根据比例限制计算标题区域的宽度可能范围
		double TitleMinWidth = InputImageWidth*LicenseProportionXMin*(TemplateTitleWidth / TemplateWidth);
		double TitleMaxWidth = InputImageWidth*LicenseProportionXMax*(TemplateTitleWidth / TemplateWidth);
		//根据比例限制计算标题部分水平缝隙最大宽度
		double TitleMaxGapWidth = InputImageWidth*LicenseProportionXMax *(TemplateTitleGapWidth / TemplateWidth);

		//创建向量存储二值化后水平投影的上升沿和下降沿所在行
		vector<int> RisingRow, FallingRow;
		//创建向量存储二值化后标题部分水平投影的上升沿和下降沿所在行
		vector<int> TitleRisingRow, TitleFallingRow, IndexTitleRisingRow, IndexTitleFallingRow;

		
	}
	//返回0并正常退出程序
	return 0;
}