#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

//使用C++标准库命名空间
using namespace std;

//使用OpenCV库命名空间
using namespace cv;

const float TemplateWidth = 580;
const float TemplateHeight = 387;
const float TemplateTitleWidth = 340;
const float TemplateTitleHeight = 40;
const float TemplateTitleCenterX = 284;
const float TemplateTitleCenterY = 46;

const float TemplatePlateWidth = 83;
const float TemplatePlateHeight = 20;
const float TemplatePlateCenterX = 151;
const float TemplatePlateCenterY = 86;

int main(int ArgumentCount, char** ArgumentVector)
{


	//检查命令行所带参数数目是否正确，如果不正确则显示用法说明并退出程序
	if (ArgumentCount != 2)
	{
		//显示程序用法说明
		cout << " Usage:  " << ArgumentVector[0] << " ImageToLoadAndDisplay" << endl;
		//返回错误码并退出程序
		return -1;
	}

	//新建矩阵RawImageMat用于存储原始图片数据
	Mat RawImageMat;

	//根据第一个参数的文件路径进行图片读取
	RawImageMat = imread(
		ArgumentVector[1],//输入图片路径
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


	Mat GrayImageMat;

	//获取图片的通道数
	int NumRawImageChannel = RawImageMat.channels();

	//如果图像为3通道彩色图像，则将其转换成灰阶图像
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
	//如果图像已经为单通道灰阶图像，直接不作转换将RawImageMat赋给GrayImageMat
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
	const char* WindowName = "get plate number from vehicle license";
	//创建相应的窗口
	namedWindow(
		WindowName,//窗口名称
		CV_WINDOW_NORMAL//此窗口根据图片大小显示图片，不可resize
	);
	resizeWindow(WindowName, GrayImageMat.cols * 600 / GrayImageMat.rows, 600);
	//在上述窗口中显示灰阶图像
	imshow(
		WindowName,//希望绘制图片的窗口名称
		GrayImageMat//希望绘制的图片矩阵
	);

	//等待键盘响应，参数0表示等待时间为无限长
	waitKey(0);


	//对图像进行5*5的高斯模糊
	Mat BlurGrayImageMat;
	GaussianBlur(
		GrayImageMat, //输入图片矩阵
		BlurGrayImageMat, //输出图片矩阵
		Size(9, 9), //高斯核尺寸
		0, //??高斯核X方向标准差 
		0, //??高斯核Y方向标准差
		BORDER_DEFAULT //??像素插值方式 
	);

	//在上述窗口中显示高斯模糊后的图像
	imshow(
		WindowName,//希望绘制图片的窗口名称
		BlurGrayImageMat//希望绘制的图片矩阵
	);

	//等待键盘响应，参数0表示等待时间为无限长
	waitKey(0);


	//使用Sobel算子计算X（水平）和Y(垂直)方向梯度
	Mat GrayImageGradX, GrayImageGradY;
	//计算灰阶图像X方向梯度）
	Sobel(
		BlurGrayImageMat,
		GrayImageGradX, //输出梯度矩阵
		CV_16S, //输出梯度矩阵的类型/深度，目标矩阵深度设置为16位有符号整数避免计算值溢出
		1, //横向求导阶数为1
		0, //纵向求导结束为0（即不求导）
		CV_SCHARR, //使用3*3的Scharr滤波器（与Sobel滤波器一样快但准确率更高）代替3*3的Sobel滤波器计算梯度
		1, //??scale为1
		0, //??delta为0
		BORDER_DEFAULT //??边界类型，像素插值方法
	);//此语句相当于：Scharr( GrayImageMat, GrayImageGradX,  CV_16S , 0, 1, 1, 0, BORDER_DEFAULT);

//同理计算灰阶图像Y方向的梯度
	Sobel(
		BlurGrayImageMat,
		GrayImageGradY,
		CV_16S,
		0, //横向求导阶数为0（即不求导）
		1, //纵向求导结束为1
		CV_SCHARR,
		1,
		0,
		BORDER_DEFAULT
	);


	//将X和Y方向的梯度值求绝对值后映射成8位无符号整数
	Mat GrayImageGradX8U, GrayImageGradY8U;
	convertScaleAbs(
		GrayImageGradX, //输入矩阵
		GrayImageGradX8U //输出矩阵
	);

	convertScaleAbs(
		GrayImageGradY,
		GrayImageGradY8U
	);

	//将XY方向梯度的加权作为总梯度
	Mat GrayImageEdge;
	addWeighted(
		GrayImageGradX8U, //输入矩阵1
		0.5, //矩阵1权重
		GrayImageGradY8U, //输入矩阵2
		0.5, //矩阵2权重
		0, //标量偏置值
		GrayImageEdge, //输出矩阵
		-1 //输出矩阵类型与矩阵1相同
	);

	//梯度矩阵二值化
	threshold(
		GrayImageGradX8U, //输入矩阵
		GrayImageEdge, //输出矩阵
		128, //迭代初始阈值
		255, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);

	//显示边缘二值图像
	imshow(
		WindowName,
		GrayImageEdge
	);

	//等待键盘响应，参数0表示等待时间为无限长
	waitKey(0);

	//对灰阶图像的边缘进行水平方向的膨胀
	//构造膨胀操作的结构元
	int DilateElementWidth = 11;
	Mat DilateElement = Mat(
		1,//第一维（Y轴尺寸）
		DilateElementWidth,//第二维（X轴尺寸）
		CV_8U,//矩阵类型：8位无符号整数
		cvScalar(1)//填充的数值，全1
	);
	Mat DilatedGrayImageEdge;
	//进行膨胀操作  
	dilate(
		GrayImageEdge,//输入矩阵
		DilatedGrayImageEdge,//输出矩阵
		DilateElement,//膨胀结构元
		Point(DilateElementWidth/2-1, 0), //膨胀元锚点
		1 //进行膨胀的次数
	);
	//显示膨胀后的二值图像
	imshow(
		WindowName,
		DilatedGrayImageEdge
	);


	//等待键盘响应，参数0表示等待时间为无限长
	waitKey(0);

	//对水平膨胀后的图像进行轮廓提取
	vector<vector<Point> > AllContour;
	vector<Vec4i> ContourHierarchy;
	findContours(DilatedGrayImageEdge,//输入图像矩阵
		AllContour, //输出轮廓向量
		ContourHierarchy,//输出轮廓等级 
		RETR_EXTERNAL,//只提取外部轮廓（忽略其中包含的内部轮廓） 
		CHAIN_APPROX_SIMPLE//轮廓的近似办法，在此选择CHAIN_APPROX_SIMPLE，它压缩水平方向，垂直方向，对角线
						   //方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息
		);
	//逐个分析提取到的轮廓
	for (size_t iContour = 0; iContour < AllContour.size(); iContour++)
	{
		//获取轮廓的最小外接矩形（可旋转）
		RotatedRect  ContourRect = minAreaRect(AllContour[iContour]);

		//获取外接矩阵的高度、宽度、旋转角、面积和中心点位置
		float ContourRectHeight = ContourRect.size.height;
		//注意：因为前面做了水平方向的膨胀操作，所以对这里宽度减去了一个膨胀元的宽度
		float ContourRectWidth = ContourRect.size.width- DilateElementWidth;
		float ContourRectRotateAngle = ContourRect.angle;
		float ContourRectArea = ContourRect.size.area();
		float ContourRectCenterX = ContourRect.center.x;
		float ContourRectCenterY = ContourRect.center.y;

		//计算模板的宽高比和面积
		const float TemplateRatio = TemplateWidth/TemplateHeight;
		const float TemplateArea = TemplateWidth*TemplateHeight;

		//计算模板的标题的宽高比和面积
		const float TemplateTitleRatio = TemplateTitleWidth/TemplateTitleHeight;
		const float TemplateTitleArea = TemplateTitleWidth*TemplateTitleHeight;

		//计算输入图像的宽、高、面积和宽高比
		const float InputImageWidth = RawImageMat.cols;
		const float InputImageHeight = RawImageMat.rows;
		const float InputImageArea = InputImageWidth * InputImageHeight;
		const float InputImageRatio = InputImageWidth / InputImageHeight;
		
		//按模板宽高比进行裁切之后的输入图像的面积
		float CroppedInputImageArea;

		//记录是否找到标题行的标志变量
		bool FlagFoundTitle = false;

		//借助模板估计标题行中心点在输入图像中的位置
		float EstimatedTitleCenterX,EstimatedTitleCenterY;


		//如果输入图像宽高比比模板图像更大（即更矮胖）
		if (InputImageRatio >= TemplateRatio  )
		{
			//计算裁切之后的输入图像的面积
			CroppedInputImageArea = InputImageHeight * TemplateRatio * InputImageHeight;
			//则忽略左右两侧多余的部分（中心点的X坐标需要往右移）
			EstimatedTitleCenterX = (InputImageWidth - InputImageHeight * TemplateRatio)/2.0 +
				TemplateTitleCenterX /TemplateWidth *  InputImageWidth;
			EstimatedTitleCenterY = TemplateTitleCenterY /TemplateHeight *  InputImageHeight;
		}
		//否则输入图像宽高比比模板图像更小（即更高瘦）
		else
		{
			//计算裁切之后的输入图像的面积
			CroppedInputImageArea = InputImageWidth * InputImageWidth /TemplateRatio;
			//忽略上下两侧多余的部分（中心点的Y坐标需要往下移）
			EstimatedTitleCenterX = TemplateTitleCenterX /TemplateWidth *  InputImageWidth;
			EstimatedTitleCenterY = (InputImageHeight - InputImageWidth /TemplateRatio)/2.0 +
				(TemplateTitleCenterY /TemplateHeight) *  InputImageHeight;	
		}

		//边长误差限制
		float LengthErrorLimit = 0.05;
		//角度误差限制
		float AngleErrorLimit = 5;

		//对提取到的轮廓进行筛选
		if (
			//如果轮廓外接矩形的宽高比和模板相差比例不超过LengthErrorLimit，水平倾斜不超过AngleErrorLimit度（顺时针为正，旋转角度为-90+AngleErrorLimit～-90度，此时竖边被识别为宽）
			(fabs(ContourRectHeight / ContourRectWidth - TemplateTitleRatio ) / TemplateTitleRatio <= LengthErrorLimit && ContourRectRotateAngle <= -90+AngleErrorLimit) ||
			//或轮廓外接矩形的宽高比和模板相差比例不超过LengthErrorLimit，水平倾斜不超过AngleErrorLimit度（逆时针为负，旋转角度为-0～-1*AngleErrorLimit度，此时横边被识别为宽）
			(fabs(ContourRectWidth / ContourRectHeight - TemplateTitleRatio) / TemplateTitleRatio <= LengthErrorLimit && ContourRectRotateAngle >= -1*AngleErrorLimit) &&
			//且轮廓面积占裁切后输入图片面积的比例与模板相差不超过(2+LengthErrorLimit)*LengthErrorLimit
			(fabs(ContourRectArea / CroppedInputImageArea - TemplateTitleArea/TemplateArea) / TemplateTitleArea/TemplateArea <= (2+LengthErrorLimit)*LengthErrorLimit &&
			//且轮廓中心与根据模板估计出来的点在X轴和Y轴方向上的误差比例均不超过LengthErrorLimit
			(fabs(ContourRectCenterX - EstimatedTitleCenterX) / EstimatedTitleCenterY <= LengthErrorLimit) &&
			(fabs(ContourRectCenterY - EstimatedTitleCenterY) / EstimatedTitleCenterY <= LengthErrorLimit)
			)//满足上述条件我们便将此轮廓当作标题行所在的位置
		{
			//将FlagFoundTitle置true表示我们已经找到了标题行对应的轮廓
			FlagFoundTitle = true;
			//在原始图像上绘制出这一轮廓
			drawContours (RawImageMat,//绘制对象
				AllContour,//轮廓点数组
				iContour,//绘制轮廓的索引/序号
				Scalar(255,0,0),//绘制的线的颜色：蓝色
				5,//绘制的线宽
				CV_AA,//线型：抗混叠
				ContourHierarchy,//轮廓的等级
				0,//只绘制当前级别的轮廓
				Point(0,0)//轮廓在水平和垂直方向的偏置
				);
			//分水平方向相对模板顺时针倾斜和逆时针倾斜两种情况估计车牌号中心点坐标

			float InputImageOffsetX =  (TemplateTitleCenterX-TemplatePlateCenterX)/TemplateTitleWidth  * ContourRectWidth
			float InputImageOffsetY = 
			//顺时针倾斜情况
			if (ContourRectRotateAngle <= -90+AngleErrorLimit)
			{
				//从标题行中心水平和垂直方向移动至估计的车牌号对应的中心点
				EstimatedPlateCenterX = ContourRectCenterX -
					TemplateOffsetX/TemplateTitleWidth  * ContourRectWidth * cos(-1.0*ContourRectRotateAngle/180.0*pi);
			    EstimatedPlateCenterY = ContourRectCenterY + 
					TemplateOffsetX/TemplateTitleHeight * ContourRectHeight * sin(-1.0*ContourRectRotateAngle/180.0*pi);
			}
			else if  (ContourRectRotateAngle >= -1*AngleErrorLimit)
			{
				EstimatedPlateCenterX = ContourRectCenterX +
					TemplateOffsetX/TemplateTitleWidth  * ContourRectWidth * sin(-1.0*ContourRectRotateAngle/180.0*pi);
				EstimatedPlateCenterY = ContourRectCenterY + 
					TemplateOffsetY/TemplateTitleHeight * ContourRectHeight * cos(-1.0*ContourRectRotateAngle/180.0*pi);
			}

		}

		if (FlagFoundTitle == true)
		{
			for (size_t iContour = 0; iContour < AllContour.size(); iContour++)
			{
				RotatedRect  ContourRect = minAreaRect(AllContour[iContour]);

				if (
					(fabs(PlateRectHeight / PlateRectWidth - TemplatePlateRatio ) / TemplatePlateRatio <= 0.05 && PlateRectRotateAngle <= -85) ||
					(fabs(PlateRectWidth / PlateRectHeight - TemplatePlateRatio) / TemplatePlateRatio <= 0.05 && PlateRectRotateAngle >= -5) &&
					(fabs(PlateRectArea / (DetectedImageWidth*DetectedImageHeight) - TemplatePlateArea) / TemplatePlateArea <= 0.05*0.05) &&
					(fabs(PlateRectCenterX - EstimatedPlateCenterX) / EstimatedPlateCenterY <= 0.05) &&
					(fabs(PlateRectCenterY - EstimatedPlateCenterY) / EstimatedPlateCenterY <= 0.05)
					)
				{
					drawContours (RawImageMat,//绘制对象
						AllContour,//轮廓点数组
						iContour,//绘制轮廓的索引/序号
						Scalar(0,0,255),//绘制的线的颜色
						5,//绘制的线宽
						CV_AA,//线型：抗混叠
						ContourHierarchy,//轮廓的等级
						0,//只绘制当前级别的轮廓
						Point(0,0)//轮廓在水平和垂直方向的偏置
						);
					imshow(WindowName,RawImageMat);
				}
			}

		}

	waitKey(0);

	//返回0并正常退出程序
	return 0;

}

