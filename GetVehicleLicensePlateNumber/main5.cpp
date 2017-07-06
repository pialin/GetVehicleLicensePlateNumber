#include <iostream>
//����㷨��ͷ�ļ�
#include<algorithm>//Ϊ��ʹ��swap
#include<numeric>//Ϊ��ʹ��iota
//�����ѧ�����ͷ�ļ�//���OpenCV���ͷ�ļ�
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//��ӱ�׼�����������ͷ�ļ�

//���TinyXml2ͷ�ļ�
#include "tinyxml2.h" 

//#include <cmath>

//ʹ��C++��׼�������ռ�
using namespace std;
//ʹ��TinyXml2�����ռ�
using namespace tinyxml2;

//ʹ��OpenCV�������ռ�
using namespace cv;
int main(int ArgumentCount, char** ArgumentVector)
{

	//�������������������Ŀ�Ƿ���ȷ���������ȷ����ʾ�÷�˵�����˳�����
	if (ArgumentCount != 3)
	{
		//��ʾ�����÷�˵��
		cout << " Usage:  " << ArgumentVector[0] << "ImagePathGlobExpression" << "OutputPath" << endl;
		//���ش����벢�˳�����
		return -1;
	}

	//�ֱ��ȡ�����������
	String SearchGlobExpression = ArgumentVector[1];
	String OutputPath = ArgumentVector[2];

	//�������ڴ洢ͼƬ·����String����
	vector<String> ImagePathList;

	//��������Glob���ʽ���ҷ��ϵ�ͼƬ
	glob(
		SearchGlobExpression,//�ļ�����Glob���ʽ
		ImagePathList, //���ͼ���ļ��б�
		false//�����еݹ飬���������ļ��н�������
	);

	//�Է��ϵ�ͼƬ���б���
	for (size_t iInput = 0; iInput < ImagePathList.size(); iInput++)
	{
		//�½�����RawImageMat���ڴ洢ԭʼͼƬ����
		Mat RawInput;

		//���ݵ�һ���������ļ�·������ͼƬ��ȡ
		RawInput = imread(
			ImagePathList[iInput],//����ͼƬ·��
			CV_LOAD_IMAGE_UNCHANGED//�Բ��޸�ͨ�����͵ķ�ʽ��ȡͼƬ
		);

		//�������Mat�Ƿ����ͼ������
		if (!RawInput.data)
		{
			//��ʾͼƬ��ȡʧ����ʾ��Ϣ
			cout << " Error:  Can't read image data from" << ArgumentVector[1] << endl;
			//���ش����벢�˳�����
			return -1;
		}
		//��������ͼ��Ŀ��ߡ�����Ϳ�߱�
		const double InputImageWidth = RawInput.cols;
		const double InputImageHeight = RawInput.rows;


		//��ͼƬת���ɻҽ�ͼ��
		Mat Raw_GrayInput;

		//��ȡͼƬ��ͨ����
		int NumRawInputChannel = RawInput.channels();

		//���ͼ��Ϊ3ͨ����ɫͼ��
		if (NumRawInputChannel == 3)
		{
			//��ͼƬ��BGRת���ɻҽ�ͼ��
			cvtColor(
				RawInput,//����ͼƬ����
				Raw_GrayInput,//���ͼƬ���� 
				COLOR_BGR2GRAY//��ͼƬ��BGR��OpenCVĬ��ͨ����ʽ��ת���ɻҽ�ͼ��
			);
		}

		//���ͼ��Ϊ4ͨ��������alphaͨ����ͼ������ת���ɻҽ�ͼ��
		else if (NumRawInputChannel == 4)
		{
			//��ͼƬ��BGRAת���ɻҽ�ͼ��
			cvtColor(
				RawInput,
				Raw_GrayInput,
				COLOR_BGRA2GRAY//��ͼƬ��BGRAת���ɻҽ�ͼ��
			);
		}
		//���ͼ���Ѿ�Ϊ��ͨ���ҽ�ͼ��ֱ�ӽ�ResizedImageMat����GrayImageMat
		else if (NumRawInputChannel == 1)
		{
			Raw_GrayInput = RawInput;
		}

		//���ͨ������Ϊ1,3��4����������벢�˳�����
		else
		{
			cout << "Unkown image channel type: " << RawInput.channels();
		}

		//Ѱ���ļ�·�����һ����\��
		//size_t LastFileSepPos = ImagePathList[iInput].rfind('\\');//rfind �������'\'
		size_t LastDotPos = ImagePathList[iInput].rfind('.');//rfind �������'.'
		//��ȡ�ļ���·��												  
		//string FolderPath = ImagePathList[iInput].substr(0, LastFileSepPos);
		//��ȡͼƬ�ļ���
		//string ImageFileName = ImagePathList[iInput].substr(0, LastDotPos);
		//��ȡ���ͼƬ����·�����ļ���Ϊ����ͼ������ǰ����ϡ�Result_����
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
			rectangle(RawInput,//���ƾ��εĶ���
				PlateRowRect, //Ҫ���Ƶľ���
				Scalar(0,0,255), //��������ɫ
				3,//�������
				LINE_AA,//���ͣ�������� 
				0 //??������ֵ�������ƣ���С��λ��
			);
		}
		//ָ����������
		const char* MainWindowName = "get plate number from vehicle license";


		//������Ӧ�Ĵ���
		namedWindow(
			MainWindowName,//��������
			CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		);
		//��ͼƬ��������Ϊ�̶��߶�700���أ��Ҳ��ı�ͼ��Ŀ�߱�
		double WindowHeight = 700.0;
		resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		imshow(
			MainWindowName,
			RawInput
		);

		waitKey(0);
	}
}