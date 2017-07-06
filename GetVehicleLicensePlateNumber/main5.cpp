#include <iostream>
//添加算法库头文件
#include<algorithm>//为了使用swap
#include<numeric>//为了使用iota
//添加数学运算库头文件//添加OpenCV相关头文件
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//添加标准库输入输出流头文件

//添加TinyXml2头文件
#include "tinyxml2.h" 

//#include <cmath>

//使用C++标准库命名空间
using namespace std;
//使用TinyXml2命名空间
using namespace tinyxml2;

//使用OpenCV库命名空间
using namespace cv;
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

		//寻找文件路径最后一个“\”
		//size_t LastFileSepPos = ImagePathList[iInput].rfind('\\');//rfind 反向查找'\'
		size_t LastDotPos = ImagePathList[iInput].rfind('.');//rfind 反向查找'.'
		//获取文件夹路径												  
		//string FolderPath = ImagePathList[iInput].substr(0, LastFileSepPos);
		//获取图片文件名
		//string ImageFileName = ImagePathList[iInput].substr(0, LastDotPos);
		//获取输出图片保存路径（文件名为输入图像名称前面加上“Result_”）
		String XmlFilePath = ImagePathList[iInput].substr(0, LastDotPos) + ".xml";

		XMLDocument XmlDoc;
		XmlDoc.LoadFile(XmlFilePath.c_str());
		XMLElement *LabelElement = XmlDoc.FirstChildElement("annotation")->FirstChildElement("object");
		bool IsPlateRowRectFound = false;
		Rect PlateRowRect;
		while (LabelElement)
		{ 

			XMLElement *LabelNameElement = LabelElement->FirstChildElement("name");
			String LabelName = LabelNameElement->GetText();
			
			if (LabelName == "5.PlateNumberRow")
			{
				XMLElement *LabelRectElement = LabelElement->FirstChildElement("bndbox");
				
				PlateRowRect.x = atoi(LabelRectElement->FirstChildElement("xmin")->GetText());
				PlateRowRect.y = atoi(LabelRectElement->FirstChildElement("ymin")->GetText());
				PlateRowRect.width = atoi(LabelRectElement->FirstChildElement("xmax")->GetText()) - PlateRowRect.x;
				PlateRowRect.height = atoi(LabelRectElement->FirstChildElement("ymax")->GetText()) - PlateRowRect.y;
				IsPlateRowRectFound = true;
				break;
			}
			

			LabelElement = LabelElement->NextSiblingElement("object");
		}
 		if (IsPlateRowRectFound ==true)
		{
			rectangle(RawInput,//绘制矩形的对象
				PlateRowRect, //要绘制的矩形
				Scalar(0,0,255), //线条的颜色
				3,//线条宽度
				LINE_AA,//线型（抗混叠） 
				0 //??坐标数值（二进制）的小数位数
			);
		}
		//指定窗口名称
		const char* MainWindowName = "get plate number from vehicle license";


		//创建相应的窗口
		namedWindow(
			MainWindowName,//窗口名称
			CV_WINDOW_NORMAL//建立一个根据图片自动设置大小的窗口，但不能手动修改尺寸
		);
		//将图片窗口设置为固定高度700像素，且不改变图像的宽高比
		double WindowHeight = 700.0;
		resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		imshow(
			MainWindowName,
			RawInput
		);

		waitKey(0);
	}
}