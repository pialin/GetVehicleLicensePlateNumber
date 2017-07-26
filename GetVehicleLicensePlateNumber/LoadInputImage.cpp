#include "main.h"
int LoadInputImage
(
	String InputImagePath,//����ͼƬ�Ķ�ȡ·��
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


	//��ȡͼƬ��ͨ����
	int NumInputImageChannel = InputImage.channels();

	//���ͼ��Ϊ3ͨ����ɫͼ��
	if (NumInputImageChannel == 3)
	{
		//��ͼƬ��BGRת���ɻҽ�ͼ��
		cvtColor(
			InputImage,//����ͼƬ����
			Gray_InputImage,//���ͼƬ���� 
			COLOR_BGR2GRAY//��ͼƬ��BGR��OpenCVĬ��ͨ����ʽ��ת���ɻҽ�ͼ��
		);
	}

	//���ͼ��Ϊ4ͨ��������alphaͨ����ͼ������ת���ɻҽ�ͼ��
	else if (NumInputImageChannel == 4)
	{
		//��ͼƬ��BGRAת���ɻҽ�ͼ��
		cvtColor(
			InputImage,//����ͼƬ����
			Gray_InputImage,//���ͼƬ���� 
			COLOR_BGRA2GRAY//��ͼƬ��BGRAת���ɻҽ�ͼ��
		);
	}
	//���ͼ���Ѿ�Ϊ��ͨ���ҽ�ͼ��ֱ�ӽ�ResizedImageMat����GrayImageMat
	else if (NumInputImageChannel == 1)
	{
		Gray_InputImage = InputImage;
	}

	//���ͨ������Ϊ1,3��4����������벢�˳�����
	else
	{
		AppendLog(String("Error: Unkown image channel type: ") + NumInputImageChannel + ".");
		return 1;
	}

	return 0;
}