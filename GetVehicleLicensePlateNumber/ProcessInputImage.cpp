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
	Mat  Binary_ProjectX_GradY;
	GetProjectX(Binary_InputGradY, Binary_ProjectX_GradY);



}

