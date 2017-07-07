#include <iostream>
//����㷨��ͷ�ļ�
#include<algorithm>//Ϊ��ʹ��swap
#include<numeric>//Ϊ��ʹ��iota

#include <fstream> 

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
		Mat Raw_InputImage;

		//���ݵ�һ���������ļ�·������ͼƬ��ȡ
		Raw_InputImage = imread(
			ImagePathList[iInput],//����ͼƬ·��
			CV_LOAD_IMAGE_UNCHANGED//�Բ��޸�ͨ�����͵ķ�ʽ��ȡͼƬ
		);

		//�������Mat�Ƿ����ͼ������
		if (!Raw_InputImage.data)
		{
			//��ʾͼƬ��ȡʧ����ʾ��Ϣ
			cout << " Error:  Can't read image data from" << ArgumentVector[1] << endl;
			//���ش����벢�˳�����
			return -1;
		}
		//��������ͼ��Ŀ��ߡ�����Ϳ�߱�
		const double InputImageWidth = Raw_InputImage.cols;
		const double InputImageHeight = Raw_InputImage.rows;

		//Ѱ���ļ�·�����һ����\��
		//size_t LastFileSepPos = ImagePathList[iInput].rfind('\\');//rfind �������'\'
		size_t LastDotPos = ImagePathList[iInput].rfind('.');//rfind �������'.'
		//��ȡ�ļ���·��												  
		//string FolderPath = ImagePathList[iInput].substr(0, LastFileSepPos);
		//��ȡͼƬ�ļ���
		//string ImageFileName = ImagePathList[iInput].substr(0, LastDotPos);
		//��ȡ���ͼƬ����·�����ļ���Ϊ����ͼ������ǰ����ϡ�Result_����
		String XmlFilePath = ImagePathList[iInput].substr(0, LastDotPos) + ".xml";
		ifstream XmlFileStream(XmlFilePath);
		if (!XmlFileStream)
		{
			cout << "Can't find XML file: " << XmlFilePath <<endl;
			return -1;
		}
		XMLDocument XmlDoc;
		XmlDoc.LoadFile(XmlFilePath.c_str());
		XMLElement *LabelElement = XmlDoc.FirstChildElement("annotation")->FirstChildElement("object");
		Rect PlateAreaRect;
		bool IsPlateAreaRectFound = false;
		if (LabelElement == nullptr)
		{
			cout << "Can't find \"object\" element of XML file: " << XmlFilePath << endl;
			return -1;
		}
		else{

			while (LabelElement)
			{

				XMLElement *LabelNameElement = LabelElement->FirstChildElement("name");
				if (LabelNameElement != nullptr)
				{
					String LabelName = LabelNameElement->GetText();

					if (LabelName == "5.PlateNumberRow")
					{
						XMLElement *LabelRectElement = LabelElement->FirstChildElement("bndbox");

						PlateAreaRect.x = atoi(LabelRectElement->FirstChildElement("xmin")->GetText());
						PlateAreaRect.y = atoi(LabelRectElement->FirstChildElement("ymin")->GetText());
						PlateAreaRect.width = atoi(LabelRectElement->FirstChildElement("xmax")->GetText()) - PlateAreaRect.x;
						PlateAreaRect.height = atoi(LabelRectElement->FirstChildElement("ymax")->GetText()) - PlateAreaRect.y;
						IsPlateAreaRectFound = true;
						break;
					}
				}


				LabelElement = LabelElement->NextSiblingElement("object");
			}
		
		}

		if (IsPlateAreaRectFound == true)
		{
			rectangle(Raw_InputImage,//���ƾ��εĶ���
				PlateAreaRect, //Ҫ���Ƶľ���
				Scalar(0, 0, 255), //��������ɫ
				3,//�������
				LINE_AA,//���ͣ������)
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
			Raw_InputImage
		);

		waitKey(0);

		Mat InputImage_PlateArea = Raw_InputImage(PlateAreaRect);

		//��ͼƬת���ɻҽ�ͼ��
		Mat Gray_PlateArea;

		//��ȡͼƬ��ͨ����
		int NumInputImageChannel = Raw_InputImage.channels();

		//���ͼ��Ϊ3ͨ����ɫͼ��
		if (NumInputImageChannel == 3)
		{
			//��ͼƬ��BGRת���ɻҽ�ͼ��
			cvtColor(
				InputImage_PlateArea,//����ͼƬ����
				Gray_PlateArea,//���ͼƬ���� 
				COLOR_BGR2GRAY//��ͼƬ��BGR��OpenCVĬ��ͨ����ʽ��ת���ɻҽ�ͼ��
			);
		}

		//���ͼ��Ϊ4ͨ��������alphaͨ����ͼ������ת���ɻҽ�ͼ��
		else if (NumInputImageChannel == 4)
		{
			//��ͼƬ��BGRAת���ɻҽ�ͼ��
			cvtColor(
				InputImage_PlateArea,//����ͼƬ����
				Gray_PlateArea,//���ͼƬ���� 
				COLOR_BGRA2GRAY//��ͼƬ��BGRAת���ɻҽ�ͼ��
			);
		}
		//���ͼ���Ѿ�Ϊ��ͨ���ҽ�ͼ��ֱ�ӽ�ResizedImageMat����GrayImageMat
		else if (NumInputImageChannel == 1)
		{
			Gray_PlateArea = InputImage_PlateArea;
		}

		//���ͨ������Ϊ1,3��4����������벢�˳�����
		else
		{
			cout << "Unkown image channel type: " << NumInputImageChannel << endl;
		}

		int  PlateAreaWidth = Gray_PlateArea.cols;
		int PlateAreaHeight = Gray_PlateArea.rows;

		Mat PlateAreaGradY
		(
			PlateAreaHeight,
			PlateAreaWidth,
			CV_8UC1,
			Scalar(0)
		);
		for (size_t iRow = 1; iRow < PlateAreaHeight-1; iRow++)
		{
			for (size_t iCol = 1; iCol < PlateAreaWidth-1; iCol++)
			{
				PlateAreaGradY.at<uchar>(iRow,iCol) = uchar ( 0.25 *
					(2* abs(Gray_PlateArea.at<uchar>(iRow - 1,iCol ) - Gray_PlateArea.at<uchar>(iRow + 1, iCol)) +
						 abs(Gray_PlateArea.at<uchar>(iRow - 1,iCol - 1) - Gray_PlateArea.at<uchar>(iRow + 1,iCol - 1)) +
						 abs(Gray_PlateArea.at<uchar>(iRow - 1 ,iCol + 1) - Gray_PlateArea.at<uchar>(iRow + 1,iCol + 1))));
			}
		}

		Mat Binary_PlateAreaGradY;
		threshold(
			PlateAreaGradY, //�������
			Binary_PlateAreaGradY, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		Mat Binary_ProjectY_PlateAreaGradY
		(
			1,
			PlateAreaWidth,
			CV_8UC1,
			Scalar(0)
		);
		double RowSumTemp=0;
		for (size_t iCol = 1; iCol < PlateAreaWidth - 1; iCol++)
		{
			for (size_t iRow = 1; iRow < PlateAreaHeight - 1; iRow++)
			{
				RowSumTemp = RowSumTemp + Binary_PlateAreaGradY.at<uchar>(iRow,iCol);
			}
			Binary_ProjectY_PlateAreaGradY.at<uchar>(0, iCol) = uchar(RowSumTemp / (PlateAreaWidth - 2));
			RowSumTemp = 0;
		}

		Mat PlateAreaGradY_Histogram
		(
			PlateAreaHeight,
			PlateAreaWidth,
			CV_8UC1,
			Scalar(0)
		);
		uchar BarHeightTemp;
		for (size_t iCol = 0; iCol < PlateAreaHeight; iCol++)
		{
			BarHeightTemp = uchar(Binary_ProjectY_PlateAreaGradY.at<uchar>(0,iCol) / 255.0 * PlateAreaHeight);
			PlateAreaGradY_Histogram(Range(0, BarHeightTemp), Range(iCol,iCol+1)) = 255;
		}

	}
}