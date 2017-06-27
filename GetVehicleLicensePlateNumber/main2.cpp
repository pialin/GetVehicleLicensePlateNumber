//#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <cmath>

//使用C++标准库命名空间
using namespace std;

//使用OpenCV库命名空间
using namespace cv;

//从模板图片中获得的位置信息
//模板图片的宽、高
const double TemplateWidth = 550;
const double TemplateHeight = 371;
//模板图片标题所在区域的宽、高及区域中心的XY坐标
const double TemplateTitleWidth = 355;
const double TemplateTitleHeight = 36;
const double TemplateTitleCenterX = 269;
const double TemplateTitleCenterY = 39;
const double TemplateTitleGapHeight = 5;
const double TemplateTitleGapWidth = 15;
//模板图片车牌区域的宽、高及区域中心的XY坐标
const double TemplatePlateWidth = 84;
const double TemplatePlateHeight = 20;
const double TemplatePlateCenterX = 138;
const double TemplatePlateCenterY = 81;


//主函数，输入命令行参数为待处理图片的路径
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

	vector<String> ImagePathList;
	String SearchGlobExpression = ArgumentVector[1];
	String OutputPath = ArgumentVector[2];

	glob(SearchGlobExpression,//文件查找Glob表达式
		ImagePathList, //输出图像文件列表
		false//不进行递归，即不对子文件夹进行搜索
	);


	for (size_t iImage = 0; iImage < ImagePathList.size(); iImage++)
	{
		//新建矩阵RawImageMat用于存储原始图片数据
		Mat RawImageMat;

		//根据第一个参数的文件路径进行图片读取
		RawImageMat = imread(
			ImagePathList[iImage],//输入图片路径
			CV_LOAD_IMAGE_UNCHANGED//以不修改通道类型的方式读取图片
		);

		//检查读入的Mat是否包含图像数据
		if (!RawImageMat.data)
		{
			//显示图片读取失败提示信息
			cout << " Error:  Can't read image data from" << ArgumentVector[1] << endl;
			//返回错误码并退出程序
			return -1;
		}
		//计算输入图像的宽、高、面积和宽高比
		const double InputImageWidth = RawImageMat.cols;
		const double InputImageHeight = RawImageMat.rows;


		//计算模板图片标题的宽高比和面积
		const double TemplateTitleRatio = TemplateTitleWidth / TemplateTitleHeight;
		const double TemplateTitleArea = TemplateTitleWidth*TemplateTitleHeight;

		//计算模板的车牌区域的宽高比和面积
		const double TemplatePlateRatio = TemplatePlateWidth / TemplatePlateHeight;
		const double TemplatePlateArea = TemplatePlateWidth*TemplatePlateHeight;

		//将图片转换成灰阶图像
		Mat GrayImageMat;
		//获取图片的通道数
		int NumRawImageChannel = RawImageMat.channels();

		//如果图像为3通道彩色图像
		if (NumRawImageChannel == 3)
		{
			//将图片由BGR转换成灰阶图像
			cvtColor(
				RawImageMat,//输入图片矩阵
				GrayImageMat,//输出图片矩阵 
				COLOR_BGR2GRAY//将图片由BGR（OpenCV默认通道格式）转换成灰阶图像
			);
		}
		//如果图像已经为4通道（包含alpha通道）图像，则将其转换成灰阶图像
		else if (NumRawImageChannel == 4)
		{
			//将图片由BGRA转换成灰阶图像
			cvtColor(
				RawImageMat,
				GrayImageMat,
				COLOR_BGRA2GRAY//将图片由BGRA转换成灰阶图像
			);
		}
		//如果图像已经为单通道灰阶图像，直接不作转换将ResizedImageMat赋给GrayImageMat
		else if (NumRawImageChannel == 1)
		{
			GrayImageMat = RawImageMat;
		}

		//如果通道数不为1,3或4，输出错误码并退出程序
		else
		{
			cout << "Unkown image channel type: " << RawImageMat.channels();
		}


		//指定窗口名称
		const char* MainWindowName = "get plate number from vehicle license";
		//创建相应的窗口
		namedWindow(
			MainWindowName,//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);
		double WindowHeight = 500.0;
		resizeWindow(MainWindowName, int(WindowHeight * GrayImageMat.cols / GrayImageMat.rows), int(WindowHeight));
		//在上述窗口中显示灰阶图像
		imshow(
			MainWindowName,//希望绘制图片的窗口名称
			GrayImageMat//希望绘制的图片矩阵
		);

		//等待键盘响应，参数0表示等待时间为无限长
		waitKey(0);


		//对图像进行高斯模糊
		//设置高斯模糊核尺寸-----------------------------------------
		int GaussianBlurKernelXSize = 2 * int(GrayImageMat.cols / 100) + 1;
		int GaussianBlurKernelYSize = 2 * int(GrayImageMat.rows / 80) + 1;
		//-----------------------------------------------------------
		//Mat BlurGrayImageMat;
		GaussianBlur(
			GrayImageMat, //输入图片矩阵
			GrayImageMat, //输出图片矩阵
							  //Size(int(GaussianBlurKernelSize), int(GaussianBlurKernelSize)), //高斯核尺寸
			Size(GaussianBlurKernelXSize, GaussianBlurKernelYSize),//高斯核尺寸必须是正值且是奇数
			0, //??高斯核X方向标准差 
			0, //??高斯核Y方向标准差
			BORDER_DEFAULT //??像素插值方式 
		);

		//在上述窗口中显示高斯模糊后的图像
		imshow(
			MainWindowName,//希望绘制图片的窗口名称
			GrayImageMat//希望绘制的图片矩阵
		);

		//等待键盘响应（按下任意键），参数0表示等待时间为无限长
		waitKey(0);



		Mat GrayImageGradX(GrayImageMat.rows, GrayImageMat.cols, CV_8UC1, Scalar(0));
		for (int iRow = 1; iRow < GrayImageMat.rows - 1; iRow++)
		{
			for (int iCol = 1; iCol < (GrayImageMat.cols - 1); iCol++)
			{
				GrayImageGradX.at<uchar>(iRow, iCol) = (2 * abs(GrayImageMat.at<uchar>(iRow, iCol - 1) - GrayImageMat.at<uchar>(iRow, iCol + 1))
					+ abs(GrayImageMat.at<uchar>(iRow - 1, iCol - 1) - GrayImageMat.at<uchar>(iRow - 1, iCol + 1))
					+ abs(GrayImageMat.at<uchar>(iRow + 1, iCol - 1) - GrayImageMat.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}

		Mat BinaryGradX(GrayImageMat.rows, GrayImageMat.cols, CV_8UC1, Scalar(0));

		//梯度矩阵二值化
		threshold(
			GrayImageGradX, //输入矩阵
			BinaryGradX, //输出矩阵
			0, //迭代初始阈值
			255, //最大值（超过阈值将设为此值）
			CV_THRESH_OTSU //阈值化选择的方法:Otsu法
		);

		//显示边缘二值图像
		imshow(
			MainWindowName,
			BinaryGradX
		);

		//等待键盘响应，参数0表示等待时间为无限长
		waitKey(0);

		int DilateElementWidth = 2 * int(TemplateTitleGapWidth / TemplateWidth*BinaryGradX.cols / 2.0) + 1;


		//构造膨胀操作的结构元，构建一个1行DilateElementWidth列的结构元以完成水平膨胀
		Mat DilateElement = Mat(
			1,//第一维（Y轴尺寸）
			DilateElementWidth,//第二维（X轴尺寸）
			CV_8U,//矩阵类型：8位无符号整数
			cvScalar(1)//填充的数值，全0
		);
		//对边缘进行膨胀操作
		Mat DilatedGradX;
		dilate(
			BinaryGradX,//输入矩阵
			DilatedGradX,//输出矩阵
			DilateElement,//膨胀结构元
			Point(-1, -1), //膨胀元锚点，在这里取中心作为锚点
			1 //进行膨胀的次数
		);
		//显示膨胀后的二值图像
		imshow(
			MainWindowName,
			DilatedGradX
		);
		//等待键盘响应，参数0表示等待时间为无限长
		waitKey(0);

		//创建X梯度投影向量
		Mat  GradXProjectY(
			DilatedGradX.rows,//矩阵行数
			1,//矩阵列数
			CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
			Scalar(0)//矩阵填入的初始值
		);

		double SumTemp;
		for (int iRow = 0; iRow < BinaryGradX.rows; iRow++)
		{
			SumTemp = 0;
			for (int iCol = 0; iCol < BinaryGradX.cols; iCol++)
			{
				SumTemp += double(DilatedGradX.at<uchar>(iRow, iCol));
			}
			GradXProjectY.at<uchar>(iRow, 0) = unsigned char(SumTemp / BinaryGradX.cols);
		}


		Mat DilatedGradXProjectY;

		threshold(
			GradXProjectY, //输入矩阵
			DilatedGradXProjectY, //输出矩阵
			128, //迭代初始阈值
			255, //最大值（超过阈值将设为此值）
			CV_THRESH_OTSU //阈值化选择的方法:Otsu法
		);

		unsigned char StemColor;
		Mat GradXProjectYStem(
			GrayImageGradX.rows,//矩阵行数
			GrayImageGradX.cols,//矩阵列数
			CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
			Scalar(0)//矩阵填入的初始值
		);
		for (int iRow = 0; iRow < GrayImageGradX.rows; iRow++)
		{
			if (DilatedGradXProjectY.at<uchar>(iRow, 0) == 0)
			{
				StemColor = 100;
			}
			else
			{
				StemColor = 255;
			}

			for (int iCol = 0; iCol < GradXProjectY.at<uchar>(iRow, 0) / 255.0*GrayImageGradX.cols; iCol++)
			{
				GradXProjectYStem.at<uchar>(iRow, iCol) = StemColor;
			}

		}
		//显示边缘二值图像
		namedWindow(
			"GradXProjectYStem",//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);
		resizeWindow("GradXProjectYStem", int(WindowHeight * GrayImageMat.cols / GrayImageMat.rows), int(WindowHeight));
		imshow(
			"GradXProjectYStem",
			GradXProjectYStem
		);


		//等待键盘响应，参数0表示等待时间为无限长
		waitKey(0);


		double LicenseProportionXMin = 0.8;
		double LicenseProportionXMax = 1.2;
		double LicenseProportionYMin = 0.8;
		double LicenseProportionYMax = 1.2;

		double TitleMinHeight = DilatedGradX.rows*LicenseProportionYMin*(TemplateTitleHeight / TemplateHeight);
		double TitleMaxHeight = DilatedGradX.rows*LicenseProportionYMax*(TemplateTitleHeight / TemplateHeight);
		double TitleMaxGapHeight = DilatedGradX.rows*LicenseProportionYMax *(TemplateTitleGapHeight / TemplateHeight);

		double TitleMinWidth = DilatedGradX.cols*LicenseProportionXMin*(TemplateTitleWidth / TemplateWidth);
		double TitleMaxWidth = DilatedGradX.cols*LicenseProportionXMax*(TemplateTitleWidth / TemplateWidth);

		vector<int> RisingRow, FallingRow;
		vector<int> TitleRisingRow, TitleFallingRow, IndexTitleRisingRow, IndexTitileFallingRow;
		DilatedGradXProjectY.at<uchar>(0, 0) = 0;
		DilatedGradXProjectY.at<uchar>(DilatedGradXProjectY.rows - 1, 0) = 0;
		for (int iRow = 0; iRow < DilatedGradXProjectY.rows; iRow++)
		{
			if (iRow < DilatedGradXProjectY.rows - 2 && DilatedGradXProjectY.at<uchar>(iRow, 0) == 0 && DilatedGradXProjectY.at<uchar>(iRow + 1, 0) == 255)
			{
				RisingRow.push_back(iRow);
			}
			else if (iRow > 0 && iRow < DilatedGradXProjectY.rows - 1 && DilatedGradXProjectY.at<uchar>(iRow, 0) == 255 && DilatedGradXProjectY.at<uchar>(iRow + 1, 0) == 0)
			{
				FallingRow.push_back(iRow);
			}
		}
		if (RisingRow.size() != FallingRow.size() && RisingRow.size() == 0 || FallingRow.size() == 0)
		{
			cout << "Erorr: Unmatched sizes between RisingRow and FallingRow or empty vector RisingRow or FallingRow.";
			return -1;
		}

		for (int iConvexY = 1; iConvexY <= RisingRow.size(); iConvexY++)
		{
			TitleRisingRow.push_back(RisingRow[iConvexY - 1]);
			IndexTitleRisingRow.push_back(iConvexY - 1);
			while (iConvexY < RisingRow.size() && RisingRow[iConvexY] - FallingRow[iConvexY - 1] <= TitleMaxGapHeight)
			{
				iConvexY++;
			}
			if (FallingRow[iConvexY - 1] - TitleRisingRow.back() >= TitleMinHeight &&
				FallingRow[iConvexY - 1] - TitleRisingRow.back() <= TitleMaxHeight)
			{
				TitleFallingRow.push_back(FallingRow[iConvexY - 1]);
				IndexTitileFallingRow.push_back(iConvexY - 1);
			}
			else
			{
				TitleRisingRow.pop_back();
				IndexTitleRisingRow.pop_back();
			}

		}

		//Rect TitleRect;
		//vector <double> ErrorTitleSize;
		//for (int iTitleRow = 0; iTitleRow < TitleRisingRow.size();iTitleRow++)
		//{

		//	ErrorTitleSize.push_back()

		//	TitleRect.x = 0;
		//	TitleRect.y = TitleRisingRow[iTitleRow];
		//	TitleRect.width = DilatedGradX.cols;
		//	TitleRect.height = TitleFallingRow[iTitleRow] - TitleRisingRow[iTitleRow];


		//
		//}


		//Mat GrayImageGradY(GrayImageMat.rows, GrayImageMat.cols, CV_8UC1, Scalar(0));
		//for (int iRow = 1; iRow < GrayImageMat.rows - 1; iRow++)
		//{
		//	for (int iCol = 1; iCol < (GrayImageMat.cols - 1); iCol++)
		//	{
		//		GrayImageGradY.at<uchar>(iRow, iCol) = (2 * abs(GrayImageMat.at<uchar>(iRow - 1, iCol) - GrayImageMat.at<uchar>(iRow + 1, iCol))
		//			+ abs(GrayImageMat.at<uchar>(iRow - 1, iCol - 1) - GrayImageMat.at<uchar>(iRow + 1, iCol - 1))
		//			+ abs(GrayImageMat.at<uchar>(iRow - 1, iCol + 1) - GrayImageMat.at<uchar>(iRow + 1, iCol + 1))) / 4;
		//	}
		//}
		bool FlagFoundTitle = false;

		vector<Rect> EstimatedTitleRect;
		vector<double> TitleSizeError;

		for (int iConvexY = 0; iConvexY < TitleRisingRow.size(); iConvexY++)
		{
			Rect ProjectYRect;
			ProjectYRect.x = 0;
			ProjectYRect.y = TitleRisingRow[iConvexY];
			ProjectYRect.width = DilatedGradX.cols;
			ProjectYRect.height = TitleFallingRow[iConvexY] - TitleRisingRow[iConvexY];
			rectangle(DilatedGradX,//绘制矩形的对象
				ProjectYRect, //要绘制的矩形
				Scalar(100), //线条的颜色
				3,//线条宽度
				LINE_AA,//线型（抗混叠） 
				0 //??坐标数值（二进制）的小数位数
			);

			//显示边缘二值图像
			imshow(
				MainWindowName,
				DilatedGradX
			);

			//等待键盘响应，参数0表示等待时间为无限长
			waitKey(0);



			//创建X梯度X方向投影向量
			Mat  GradXProjectX(
				1,//矩阵行数
				DilatedGradX.cols,//矩阵列数
				CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
				Scalar(0)//矩阵填入的初始值
			);


			for (int iCol = 0; iCol < DilatedGradX.cols; iCol++)
			{
				SumTemp = 0;
				for (int iRow = RisingRow[iConvexY]; iRow <= FallingRow[iConvexY]; iRow++)
				{
					SumTemp += double(DilatedGradX.at<uchar>(iRow, iCol));
				}
				GradXProjectX.at<uchar>(0, iCol) = unsigned char(SumTemp / (FallingRow[iConvexY] - RisingRow[iConvexY]));
			}

			Mat BinaryGradXProjectX;
			threshold(
				GradXProjectX, //输入矩阵
				BinaryGradXProjectX, //输出矩阵
				128, //迭代初始阈值
				255, //最大值（超过阈值将设为此值）
				CV_THRESH_OTSU //阈值化选择的方法:Otsu法
			);


			Mat GradXProjectXStem(
				DilatedGradX.rows,//矩阵行数
				DilatedGradX.cols,//矩阵列数
				CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
				Scalar(0)//矩阵填入的初始值
			);
			for (int iCol = 0; iCol < DilatedGradX.cols; iCol++)
			{
				if (BinaryGradXProjectX.at<uchar>(0, iCol) == 0)
				{
					StemColor = 100;
				}
				else
				{
					StemColor = 255;
				}

				for (int iRow = 0; iRow < GradXProjectX.at<uchar>(0, iCol) / 255.0*DilatedGradX.rows; iRow++)
				{
					GradXProjectXStem.at<uchar>(iRow, iCol) = StemColor;
				}

			}
			//显示边缘二值图像
			namedWindow(
				"GradXProjectXStem",//窗口名称
				CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
			);
			resizeWindow("GradXProjectXStem", int(WindowHeight * GrayImageMat.cols / GrayImageMat.rows), int(WindowHeight));
			imshow(
				"GradXProjectXStem",
				GradXProjectXStem
			);


			//等待键盘响应，参数0表示等待时间为无限长
			waitKey(0);



			vector<int> RisingCol, FallingCol;
			BinaryGradXProjectX.at<uchar>(0, 0) = 0;
			BinaryGradXProjectX.at<uchar>(0, DilatedGradX.cols - 1) = 0;
  			for (int iCol = 0; iCol < BinaryGradXProjectX.cols; iCol++)
			{
				if (iCol < BinaryGradXProjectX.cols - 2 && BinaryGradXProjectX.at<uchar>(0, iCol) == 0 && BinaryGradXProjectX.at<uchar>(0, iCol + 1) == 255)
				{
					RisingCol.push_back(iCol);
				}
				else if (iCol < BinaryGradXProjectX.cols - 1 && BinaryGradXProjectX.at<uchar>(0, iCol) == 255 && BinaryGradXProjectX.at<uchar>(0, iCol + 1) == 0)
				{
					if (RisingCol.size() - FallingCol.size() == 1)
					{

						FallingCol.push_back(iCol);
					}
					else
					{
						RisingCol.pop_back();
					}
				}
			}

			if (RisingCol.size() != FallingCol.size() || RisingCol.size() == 0 || FallingCol.size() == 0)
			{
				cout << "Erorr: Unmatched sizes between RisingCol and FallingCol or empty vector RisingCol or FallingCol.";
				return -1;
			}
			vector<int> TitleRisingCol, TitleFallingCol, IndexTitleRisingCol, IndexTitileFallingCol;
			for (int iConvexX = 1; iConvexX <= RisingCol.size(); iConvexX++)
			{
				TitleRisingCol.push_back(RisingCol[iConvexX - 1]);
				IndexTitleRisingCol.push_back(iConvexX - 1);
				while (iConvexX < RisingCol.size() && RisingCol[iConvexX] - FallingCol[iConvexX - 1] <= TitleMaxGapHeight)
				{
					iConvexX++;
				}
				if (FallingCol[iConvexX - 1] - TitleRisingCol.back() >= TitleMinWidth &&
					FallingCol[iConvexX - 1] - TitleRisingCol.back() <= TitleMaxWidth)
				{
					TitleFallingCol.push_back(FallingCol[iConvexX - 1]);
					IndexTitileFallingCol.push_back(iConvexX - 1);
				}
				else
				{
					TitleRisingCol.pop_back();
					IndexTitleRisingCol.pop_back();
				}

			}

			Rect TempTitleRect;
			for (int iConvexX = 0; iConvexX < TitleRisingCol.size(); iConvexX++)
			{
				TempTitleRect.x = TitleRisingCol[iConvexX] + DilateElementWidth/2;
				TempTitleRect.y = TitleRisingRow[iConvexY] ;
				TempTitleRect.width = TitleFallingCol[iConvexX] - TitleRisingCol[iConvexX] - DilateElementWidth;
				TempTitleRect.height = TitleFallingRow[iConvexY] - TitleRisingRow[iConvexY];

				EstimatedTitleRect.push_back(TempTitleRect);
				TitleSizeError.push_back(fabs(TempTitleRect.width - TemplateTitleWidth*DilatedGradX.cols / TemplateWidth)*
					fabs(TempTitleRect.width - TemplateTitleWidth*DilatedGradX.cols / TemplateWidth) +
					fabs(TempTitleRect.height - TemplateTitleHeight*DilatedGradX.rows / TemplateHeight)*
					fabs(TempTitleRect.height - TemplateTitleHeight*DilatedGradX.rows / TemplateHeight));

			}

		}
		if (TitleSizeError.size() > 0)
		{
			FlagFoundTitle = true;
			vector<double>::iterator itMinTitleSizeError = min_element(TitleSizeError.begin(), TitleSizeError.end());
			int64 IndexMinTitleSizeError = distance(TitleSizeError.begin(), itMinTitleSizeError);

			Rect MinErrorTitleRect = EstimatedTitleRect[IndexMinTitleSizeError];

			rectangle(RawImageMat,//绘制矩形的对象
				MinErrorTitleRect, //要绘制的矩形
				Scalar(255, 0, 0), //线条的颜色
				3,//线条宽度
				LINE_AA,//线型（抗混叠） 
				0 //??坐标数值（二进制）的小数位数
			);
			imshow(
				MainWindowName,
				RawImageMat
			);
			//等待键盘响应，参数0表示等待时间为无限长
			waitKey(0);
		}

	}
	//返回0并正常退出程序
	return 0;
}