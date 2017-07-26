#include "main.h"
int LoadInputImage
(
	String InputImagePath,//输入图片的读取路径
	Mat Gray_InputImage
)
{
	if (InputImagePath.empty())
	{
		AppendLog("Error:Missing necessary input parameter 'InputImagePath' ");
		return 1;
	}

	Mat InputImage = imread(
		InputImagePath,
		CV_LOAD_IMAGE_UNCHANGED
	);


	if (!InputImage.data)
	{
		AppendLog("Error: Can't read image data from '" + InputImagePath +
			"'. This item would be skipped.");
		return 1;
	}

	Rect InputImageRect = { 0, 0, InputImage.rows, InputImage.cols };

	int ImagePathSuffixSep = InputImagePath.rfind('.');
	String XmlFilePath = InputImagePath.substr(0, ImagePathSuffixSep) + ".xml";
	Rect RectOfInterest;

	LoadRectOfInterest(XmlFilePath, RectOfInterest);


	if ((RectOfInterest & InputImageRect).area == 0)
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