#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

//使用C++标准库命名空间
using namespace std;

//使用OpenCV库命名空间
using namespace cv;


int main( int ArgumentCount, char** ArgumentVector )
{


	//检查命令行所带参数数目是否正确，如果不正确则显示用法说明并退出程序
	if( ArgumentCount != 2)
	{
		//显示程序用法说明
		cout << " Usage:  "<< ArgumentVector[0] << " ImageToLoadAndDisplay" << endl;
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
	if( !RawImageMat.data )
	{ 
		//显示图片读取失败提示信息
		cout << " Error:  Can't read image data from"<< ArgumentVector[1]  << endl;
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
	//在上述窗口中显示灰阶图像
	imshow( 
		WindowName,//希望绘制图片的窗口名称
		GrayImageMat//希望绘制的图片矩阵
		);

	//等待键盘响应，参数0表示等待时间为无限长
	waitKey(0);


	//对图像进行3*3的高斯模糊
	Mat BlurGrayImageMat;
	GaussianBlur( 
		GrayImageMat, //输入图片矩阵
		BlurGrayImageMat, //输出图片矩阵
		Size(3,3), //高斯核尺寸
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
	Mat GrayImageGradX,GrayImageGradY;
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

	////convertScaleAbs( grad_x, abs_grad_x );

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
	Mat GrayImageGradX8U,GrayImageGradY8U;
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
		GrayImageEdge, //输入矩阵
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
	Mat DilateElement = Mat(
		1,//第一维（Y轴尺寸）
		19,//第二维（X轴尺寸）
		CV_8U,//矩阵类型：8位无符号整数
		cvScalar(1)//填充的数值，全1
		);  
	Mat DilatedGrayImageEdge;  
	//进行膨胀操作  
	dilate(
		GrayImageEdge,//输入矩阵
		DilatedGrayImageEdge,//输出矩阵
		DilateElement,//膨胀结构元
		Point(9,0), //膨胀元锚点
		1 //进行膨胀的次数
		);
	//显示膨胀后的二值图像
	imshow( 
		WindowName,
		DilatedGrayImageEdge
		);


	//等待键盘响应，参数0表示等待时间为无限长
	waitKey(0);

	//返回0并正常退出程序
	return 0;
}