//添加OpenCV相关头文件
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//添加标准库输入输出流头文件
#include <iostream>
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

		//对图像进行高斯模糊
		//设置高斯模糊核尺寸(必须为正奇数)
		int GaussianBlurKernelXSize = 2 * int(InputImageWidth / 100.0) + 1;
		int GaussianBlurKernelYSize = 2 * int(InputImageHeight / 80.0) + 1;

		Mat Gray_BlurImage;
		GaussianBlur(
			Raw_GrayImage, //输入图片矩阵
			Gray_BlurImage, //输出图片矩阵
			Size(GaussianBlurKernelXSize, GaussianBlurKernelYSize),//高斯核尺寸必须是正值且是奇数
			0, //??高斯核X方向标准差 
			0, //??高斯核Y方向标准差
			BORDER_DEFAULT //??像素插值方式 
		);

		//在上述窗口中显示高斯模糊后的图像
		imshow(
			MainWindowName,//希望绘制图片的窗口名称
			Gray_BlurImage//希望绘制的图片矩阵
		);

		//等待键盘响应（按下任意键），参数0表示等待时间为无限长
		waitKey(0);


		//创建矩阵用于存放图像X方向的梯度值
		Mat BlurImage_GradX(
			int(InputImageHeight),//矩阵的第一维（高度）尺寸
			int(InputImageWidth), //矩阵的第二维（宽度）尺寸
			CV_8UC1,//矩阵的值类型，在这里是单通道8位无符号整数 
			Scalar(0)//矩阵填充的初始值
		);
		//逐个像素计算水平梯度，上下左右边缘不作计算，因为其值为填充的初始值0
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			for (int iCol = 1; iCol < (InputImageWidth - 1); iCol++)
			{
				//每一像素点的水平梯度等于0.25×abs(左上-右上）+0.25×（左下-右下）+0.5×（左-右），模仿Sobel算子计算方法
				BlurImage_GradX.at<uchar>(iRow, iCol) = (2 * abs(Gray_BlurImage.at<uchar>(iRow, iCol - 1) - Gray_BlurImage.at<uchar>(iRow, iCol + 1))
					+ abs(Gray_BlurImage.at<uchar>(iRow - 1, iCol - 1) - Gray_BlurImage.at<uchar>(iRow - 1, iCol + 1))
					+ abs(Gray_BlurImage.at<uchar>(iRow + 1, iCol - 1) - Gray_BlurImage.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}

		Mat BinaryGradX(
			int(InputImageHeight),//矩阵的第一维（高度）尺寸
			int(InputImageWidth),//矩阵的第二维（宽度）尺寸
			CV_8UC1, //矩阵的值类型，在这里是单通道8位无符号整数 
			Scalar(0)//矩阵填充的初始值
		);

		//梯度矩阵二值化
		threshold(
			BlurImage_GradX, //输入矩阵
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

		//设置水平膨胀结构元的宽度
		int DilateXElementWidth = 2 * int(TemplateTitleGapWidth / TemplateWidth*BinaryGradX.cols / 2.0) + 1;


		//构造膨胀操作的结构元，构建一个1行DilateXElementWidth列的结构元以完成水平膨胀
		Mat DilateXElement = getStructuringElement(
			MORPH_RECT,//结构元形状类型：矩形
			Size(DilateXElementWidth, 1),//结构元尺寸:1行DilateXElementWidth列
			Point(-1, -1)//锚点位置：（-1，-1）默认锚点位于中心
		);
		////对边缘进行膨胀操作
		//Mat Binary_DilateXGradX;
		//dilate(
		//	BinaryGradX,//输入矩阵
		//	Binary_DilateXGradX,//输出矩阵
		//	DilateXElement,//膨胀结构元
		//	Point(-1, -1), //膨胀元锚点，在这里取中心作为锚点
		//	1 //进行膨胀的次数
		//);
		Mat Binary_DilateXGradX = BinaryGradX;

		//显示膨胀后的二值图像
		imshow(
			MainWindowName,
			Binary_DilateXGradX
		);
		//等待键盘响应，参数0表示等待时间为无限长
		waitKey(0);


		//创建X梯度投影向量
		Mat  DilateX_ProjectYGradX(
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
				SumTemp += double(Binary_DilateXGradX.at<uchar>(iRow, iCol));
			}
			//求叠加值的均值作为水平投影后的梯度值
			DilateX_ProjectYGradX.at<uchar>(iRow, 0) = unsigned char(SumTemp / Binary_DilateXGradX.cols);
		}


		Mat ProjectY_BinaryGradX;

		threshold(
			DilateX_ProjectYGradX, //输入矩阵
			ProjectY_BinaryGradX, //输出矩阵
			128, //迭代初始阈值
			255, //最大值（超过阈值将设为此值）
			CV_THRESH_OTSU //阈值化选择的方法:Otsu法
		);


		Mat ProjectYGradX_StemFigure(
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
			//根据二值化结果决定Stem图茎的颜色，若为0，使用灰色代表该行投影值小于阈值
			if (ProjectY_BinaryGradX.at<uchar>(iRow, 0) == 0)
			{
				StemColor = 100;
			}
			//若为255，则用白色代表该行投影值大于阈值
			else
			{
				StemColor = 255;
			}
			//根据投影后的梯度值绘制Stem图，每一行根据Stem值大小绘制不同宽度的Stem
			for (int iCol = 0; iCol < DilateX_ProjectYGradX.at<uchar>(iRow, 0) / 255.0*InputImageWidth; iCol++)
			{
				ProjectYGradX_StemFigure.at<uchar>(iRow, iCol) = StemColor;
			}

		}


		//显示边缘二值图像
		namedWindow(
			"ProjectYGradX_StemFigure",//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);
		//将窗口变为高度为WindowHeight，宽高比不变
		resizeWindow("ProjectYGradX_StemFigure", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));
		imshow(
			"ProjectYGradX_StemFigure",
			ProjectYGradX_StemFigure
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

		//将投影二值化后结果的第一行和最后一行置0（不是必须的，只是为了保险）
		ProjectY_BinaryGradX.at<uchar>(0, 0) = 0;
		ProjectY_BinaryGradX.at<uchar>(int(InputImageHeight - 1), 0) = 0;
		//逐行检查上升沿和下降沿
		for (int iRow = 0; iRow < InputImageHeight; iRow++)
		{
			//上升沿只可能在第一行到倒数第三行之间出现（上升沿对应的行数值为0，下一行数值为255）
			if (iRow < InputImageHeight - 2 && ProjectY_BinaryGradX.at<uchar>(iRow, 0) == 0 && ProjectY_BinaryGradX.at<uchar>(iRow + 1, 0) == 255)
			{
				//如果符合上述条件则在上升沿向量后添加新元素
				RisingRow.push_back(iRow);
			}
			//下降沿只可能在第二行到倒数第二行之间出现（下降沿对应的行数值为255，下一行数值为0）
			else if (iRow > 0 && iRow < InputImageHeight - 1 && ProjectY_BinaryGradX.at<uchar>(iRow, 0) == 255 && ProjectY_BinaryGradX.at<uchar>(iRow + 1, 0) == 0)
			{
				//如果符合上述条件则在下降沿向量后添加新元素
				FallingRow.push_back(iRow);
			}
		}
		//判断上升沿和下降沿个数是否匹配（因为第一行和最后一行均为0，所以两者个数应该是相等的），不匹配则抛出错误信息并结束程序
		if (RisingRow.size() != FallingRow.size())
		{
			cout << "Erorr: Unmatched sizes between RisingRow and FallingRow.";
			return -1;
		}
		//对每个上升-下降对（Convex）进行处理，提取标题部分可能对应的Convex
		for (int iConvexY = 1; iConvexY <= RisingRow.size(); iConvexY++)
		{
			//填入初始上升沿行数及相对应的索引
			TitleRisingRow.push_back(RisingRow[iConvexY - 1]);
			IndexTitleRisingRow.push_back(iConvexY - 1);
			//如果发现高度小于最大缝隙高度且小于标题下面英文行高度的空隙则进行融合（去除小的空隙）
			while (iConvexY < RisingRow.size() && RisingRow[iConvexY] - FallingRow[iConvexY - 1] <= TitleMaxGapHeight &&
				FallingRow[iConvexY] - RisingRow[iConvexY] <= TitleMinEnglishHeight)
			{
				iConvexY++;
			}
			//如果Convex的高度在标题高度限制范围之内，且上升沿位于图片上半部分，则将其视为标题部分可能对应的Convex
			if (FallingRow[iConvexY - 1] - TitleRisingRow.back() >= TitleMinHeight &&
				FallingRow[iConvexY - 1] - TitleRisingRow.back() <= TitleMaxHeight &&
				TitleRisingRow.back() < InputImageHeight / 2)
			{
				//为标题行上升和下降沿矢量后添加元素
				TitleFallingRow.push_back(FallingRow[iConvexY - 1]);
				IndexTitleFallingRow.push_back(iConvexY - 1);
				//TODO：目前只处理第一个找到的（最高的）标题部分候选Convex（后续需要改进）
				break;
			}
			else
			{
				//否则删除循环开头填入的上升沿信息
				TitleRisingRow.pop_back();
				IndexTitleRisingRow.pop_back();
			}

		}


		//对每个候选的标题Convex进行遍历
		for (int iConvexY = 0; iConvexY < TitleRisingRow.size(); iConvexY++)
		{
			//计算标题Convex在图上对应的矩形区域
			Rect ProjectYPassRect;
			ProjectYPassRect.x = 0;
			ProjectYPassRect.y = TitleRisingRow[iConvexY];
			ProjectYPassRect.width = int(InputImageWidth);
			ProjectYPassRect.height = TitleFallingRow[iConvexY] - TitleRisingRow[iConvexY];

			//在Stem图上绘制上述矩形
			rectangle(
				ProjectYGradX_StemFigure,//绘制矩形的对象
				ProjectYPassRect, //要绘制的矩形
				Scalar(255), //线条的颜色:白色
				int(InputImageHeight / 200.0),//线条宽度
				LINE_AA,//线型（抗混叠） 
				0 //??坐标数值（二进制）的小数位数
			);

			Mat Gray_TitleRowImage = Raw_GrayImage(ProjectYPassRect);
			Mat TitleRowImage_GradY(
				Gray_TitleRowImage.rows,
				Gray_TitleRowImage.cols,
				CV_8UC1,
				Scalar(0)
				);
			for (int iRow = 1; iRow < Gray_TitleRowImage.rows-1; iRow++)
			{
				for (int iCol = 1; iCol < Gray_TitleRowImage.cols-1; iCol++)
				{
					//每一像素点的水平梯度等于0.25×abs(左上-右上）+0.25×（左下-右下）+0.5×（左-右），模仿Sobel算子计算方法
					TitleRowImage_GradY.at<uchar>(iRow, iCol) = (2 * abs(Gray_TitleRowImage.at<uchar>(iRow - 1, iCol - 1) - Gray_TitleRowImage.at<uchar>(iRow + 1, iCol - 1))
						+ abs(Gray_TitleRowImage.at<uchar>(iRow - 1, iCol) - Gray_TitleRowImage.at<uchar>(iRow + 1, iCol))
						+ abs(Gray_TitleRowImage.at<uchar>(iRow - 1, iCol + 1) - Gray_TitleRowImage.at<uchar>(iRow + 1, iCol + 1))) / 4;
				}
			}

			


			//创建存储X梯度X方向投影的矩阵
 			Mat DilateX_ProjectXGradX(
				1,//矩阵行数
				int(InputImageWidth),//矩阵列数
				CV_8UC1,//矩阵值的类型（单通道8位无符号整数）
				Scalar(0)//矩阵填入的初始值
			);



			for (int iCol = 1; iCol < InputImageWidth - 1; iCol++)
			{
				//每次循环前将加和变量清零
				SumTemp = 0;
				//累加标题部分候选Convex对应的行的数据
				for (int iRow = TitleRisingRow[iConvexY]; iRow <= TitleFallingRow[iConvexY]; iRow++)
				{
					SumTemp += double(Binary_DilateXGradX.at<uchar>(iRow, iCol));
				}
				//求累加值的平均值作为梯度垂直投影的梯度值
				DilateX_ProjectXGradX.at<uchar>(0, iCol) = unsigned char(SumTemp / (FallingRow[iConvexY] - RisingRow[iConvexY]));
			}

			//创建存储X梯度X方向投影二值化结果的矩阵
			Mat ProjectX_BinaryGradX;
			//对垂直投影后的梯度值进行Otsu二值化
			threshold(
				DilateX_ProjectXGradX, //输入矩阵
				ProjectX_BinaryGradX, //输出矩阵
				128, //迭代初始阈值
				255, //最大值（超过阈值将设为此值）
				CV_THRESH_OTSU //阈值化选择的方法:Otsu法
			);


			//创建用于存储标题部分对应行垂直投影的Stem图的矩阵
			Mat ProjectXGradX_StemFigure(
				int(InputImageHeight),//矩阵行数
				int(InputImageWidth),//矩阵列数
				CV_8UC1,//矩阵值的类型（8位无符号整数单通道）
				Scalar(0)//矩阵填入的初始值
			);

			//绘制垂直投影梯度的Stem图
			for (int iCol = 0; iCol < InputImageWidth; iCol++)
			{
				//根据垂直投影后二值化的梯度值选择Stem线的颜色
				if (ProjectX_BinaryGradX.at<uchar>(0, iCol) == 0)
				{
					//如果二值化的结果为0，则Stem线的颜色设置为灰色表明其低于阈值
					StemColor = 100;
				}
				else
				{
					//如果二值化的结果为255，则Stem线的颜色设置为白色表明其高于阈值
					StemColor = 255;
				}
				//根据投影后的梯度值绘制Stem图，每一列根据Stem值大小绘制不同高度的Stem，不属于候选Convex的行不作处理，默认为0（黑色）
				for (int iRow = 0; iRow < DilateX_ProjectXGradX.at<uchar>(0, iCol) / 255.0*InputImageHeight; iRow++)
				{
					ProjectXGradX_StemFigure.at<uchar>(iRow, iCol) = StemColor;
				}

			}

			//新建绘图窗口用于绘制垂直投影梯度Stem图
			namedWindow(
				"ProjectXGradX_StemFigure",//窗口名称
				CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
			);
			//设置窗口尺寸使其高度为WindowHeight，宽高比与输入图像一致
			resizeWindow("ProjectXGradX_StemFigure", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));
			imshow(
				"ProjectXGradX_StemFigure",
				ProjectXGradX_StemFigure
			);
			//等待键盘响应，参数0表示等待时间为无限长
			waitKey(0);


			//创建用于存储标题部分水平方向上升沿和下降沿所在列数据的向量
			vector<int> TitleRisingCol, TitleFallingCol;

			//将二值化的垂直投影的第一列和最后一列置0（非必须，为了保证上升沿和下降沿只可能交替出现，并且开始为上升沿结束为下降沿）
			ProjectX_BinaryGradX.at<uchar>(0, 0) = 0;
			ProjectX_BinaryGradX.at<uchar>(0, int(InputImageWidth - 1)) = 0;

			//遍历每一列找出上升沿和下降沿所在列
			for (int iCol = 0; iCol < InputImageWidth; iCol++)
			{
				//上升沿只可能在第一列到倒数第三列之间出现（上升沿对应的列数值为0，下一列数值为255）
				if (iCol < InputImageWidth - 2 && ProjectX_BinaryGradX.at<uchar>(0, iCol) == 0 && ProjectX_BinaryGradX.at<uchar>(0, iCol + 1) == 255)
				{
					TitleRisingCol.push_back(iCol);
				}
				//下降沿只可能在第二列到倒数第二列之间出现（下降沿对应的列数值为255，下一列数值为0）
				else if (iCol < InputImageWidth - 1 && ProjectX_BinaryGradX.at<uchar>(0, iCol) == 255 && ProjectX_BinaryGradX.at<uchar>(0, iCol + 1) == 0)
				{
					TitleFallingCol.push_back(iCol);
				}
			}
			//判断上升沿和下降沿个数是否相等，如不相等抛出错误信息并终止程序
			if (TitleRisingCol.size() != TitleFallingCol.size())
			{
				cout << "Erorr: Unmatched sizes between RisingCol and FallingCol.";
				return -1;
			}
			//根据标题行垂直投影Convex认为是标题行的高度，根据模板标题部分的宽高比估计标题部分的宽度
			double EstimatedTitleWidth = (TitleFallingRow[iConvexY] - TitleRisingRow[iConvexY]) * TemplateTitleRatio;

			//考虑所有上升沿和下降沿的组合，找出最适合作为标题行左右边界的一对
			//创建矢量存储所有上升下降沿组合对的索引
			vector<int> IndexTitleRisingColCouple, IndexTitleFallingColCouple;
			//创建矢量存储上述组合对的误差
			vector<double> RisingFallingColCoupleError;
			//用于存储临时误差数据
			double TempError;
			//遍历所有上升-下降沿组合情况
			for (int iTitleRisingCol = 0; iTitleRisingCol < TitleRisingCol.size(); iTitleRisingCol++)
			{

				for (int iTitleFallingCol = iTitleRisingCol; iTitleFallingCol < TitleFallingCol.size(); iTitleFallingCol++)
				{
					//将上升和下降沿对应的索引存入IndexTitleRisingColCouple和IndexTitleFallingColCouple向量中
					IndexTitleRisingColCouple.push_back(TitleRisingCol[iTitleRisingCol]);
					IndexTitleFallingColCouple.push_back(TitleFallingCol[iTitleFallingCol]);
					//计算误差（预估标题行宽度的差值与中心横坐标与图片中心横坐标差值的平方和）
					TempError = (TitleFallingCol[iTitleFallingCol] - TitleRisingCol[iTitleRisingCol] - EstimatedTitleWidth) *
						(TitleFallingCol[iTitleFallingCol] - TitleRisingCol[iTitleRisingCol] - EstimatedTitleWidth) +
						((TitleFallingCol[iTitleFallingCol] + TitleRisingCol[iTitleRisingCol]) / 2.0 - InputImageWidth / 2.0) *
						((TitleFallingCol[iTitleFallingCol] + TitleRisingCol[iTitleRisingCol]) / 2.0 - InputImageWidth / 2.0);
					//将误差存入RisingFallingColCoupleError向量中
					RisingFallingColCoupleError.push_back(TempError);
				}
			}

			//标志是否找到标题区域的Flag变量
			bool FlagFoundTitle = false;
			//如果到达这一步需要计算上升下降沿误差则表明找到了标题区域
			if (RisingFallingColCoupleError.size() > 0)
			{
				//将标志变量置true
				FlagFoundTitle = true;

				//查找最小误差对应的上升-下降沿对
				vector<double>::iterator itMinTitleSizeError = min_element(RisingFallingColCoupleError.begin(), RisingFallingColCoupleError.end());

				int64 IndexMinTitleSizeError = distance(RisingFallingColCoupleError.begin(), itMinTitleSizeError);

				//计算对应的最小误差标题区域
				Rect MinErrorTitleRect;

				MinErrorTitleRect.x = IndexTitleRisingColCouple[IndexMinTitleSizeError];
				MinErrorTitleRect.y = TitleRisingRow[iConvexY];
				MinErrorTitleRect.width = IndexTitleFallingColCouple[IndexMinTitleSizeError] - MinErrorTitleRect.x;
				MinErrorTitleRect.height = TitleFallingRow[iConvexY] - MinErrorTitleRect.y;
				//在原始图像上绘制最小误差标题区域的矩形框
				rectangle(RawImage,//绘制矩形的对象
					MinErrorTitleRect, //要绘制的矩形
					Scalar(255, 0, 0), //线条的颜色：红色
					int(InputImageHeight / 200.0),//线条宽度
					LINE_AA,//线型（抗混叠） 
					0 //??坐标数值（二进制）的小数位数
				);

				//显示带标题区域检测框原始图像
				imshow(
					MainWindowName,
					RawImage
				);
				//等待键盘响应，参数0表示等待时间为无限长
				waitKey(0);
			}

		}
		//寻找文件路径最后一个“\”
		size_t SepPos = ImagePathList[iImage].rfind('\\');//rfind 反向查找
														  //获取文件夹路径
		string FolderPath = ImagePathList[iImage].substr(0, SepPos);
		//获取图片文件名
		string ImageFileName = ImagePathList[iImage].substr(SepPos + 1, -1);
		//获取输出图片保存路径（文件名为输入图像名称前面加上“Result_”）
		string OutputImagePath = OutputPath + "\\Result_" + ImageFileName;
		//进行图像保存
		imwrite(OutputImagePath, RawImage);
	}
	//返回0并正常退出程序
	return 0;
}