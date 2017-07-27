/*��ӱ�׼��*/
//Ϊ��ʹ��cout��������������
#include  <iostream>
//Ϊ��ʹ��swap��
#include<algorithm>
//Ϊ��ʹ��iota
#include<numeric>
//Ϊ��ʹ��ifstream�ж��ļ��Ƿ����
#include <fstream> 

/*���OpenCV���ͷ�ļ�*/
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "tinyxml2.h"

//ʹ��C++��׼�������ռ�
using namespace std;
////ʹ��TinyXml2�����ռ�
//using namespace tinyxml2;


//ʹ��OpenCV�������ռ�
using namespace cv;

//��ģ��ͼƬ�л�õ�λ����Ϣ
//ģ��ͼƬ�Ŀ���


double WindowHeight = 700.0;

double MinMatchScale = 0.6;
double MaxMatchScale = 1.4;

template <typename SortValueType>
vector<int>  SortIndex(vector<SortValueType> &InputValueVector) {

	// initialize original index locations
	vector<int> IndexVector(InputValueVector.size());
	iota(IndexVector.begin(), IndexVector.end(), 0);

	// sort indexes based on comparing values in v
	sort(IndexVector.begin(),
		IndexVector.end(),
		[&InputValueVector](int x, int y) {return InputValueVector[x] > InputValueVector[y]; });
	return IndexVector;
}


//�����������������в�����
//��1��Ϊ������ͼƬ��·��������ʹ��Glob���ʽѡ�ж���ļ�����ͬ�ļ������ѭ������
//��2������Ϊ������·��,Ĭ���ڸ��ļ������ԡ�Result_��+����ͼ���ļ����ķ�ʽ������
int main(int ArgumentCount, char** ArgumentVector)
{

	//�������������������Ŀ�Ƿ���ȷ���������ȷ����ʾ�÷�˵�����˳�����
	if (ArgumentCount != 4)
	{
		//��ʾ�����÷�˵��
		cout << " Usage:  " << ArgumentVector[0] << "ImagePathGlobExpression" << "OutputPath" << endl;
		//���ش����벢�˳�����
		return -1;
	}
	//�ֱ��ȡ�����������
	String SearchGlobExpression = ArgumentVector[1];
	String SearchTemplatePath = ArgumentVector[2];
	String OutputPath = ArgumentVector[3];

	////////////////////////////////////////////////////////////////
	//�½�����RawImageMat���ڴ洢ԭʼͼƬ����
	Mat TemplateImage;

	//���ݵ�һ���������ļ�·������ͼƬ��ȡ
	TemplateImage = imread(
		SearchTemplatePath,//����ͼƬ·��
		CV_LOAD_IMAGE_UNCHANGED//�Բ��޸�ͨ�����͵ķ�ʽ��ȡͼƬ
	);

	//�������Mat�Ƿ����ͼ������
	if (!TemplateImage.data)
	{
		//��ʾͼƬ��ȡʧ����ʾ��Ϣ
		cout << " Error:  Can't read image data from" << SearchTemplatePath << endl;
		//���ش����벢�˳�����
		return -1;
	}

	int TemplateImageHeight = TemplateImage.rows;
	int TemplateImageWidth = TemplateImage.cols;
	double TemplateImageRatio = double(TemplateImageWidth) / TemplateImageHeight;

	//��ͼƬת���ɻҽ�ͼ��
	Mat Gray_TemplateImage;

	//��ȡͼƬ��ͨ����
	int NumTemplateImageChannel = TemplateImage.channels();

	//���ͼ��Ϊ3ͨ����ɫͼ��
	if (NumTemplateImageChannel == 3)
	{
		//��ͼƬ��BGRת���ɻҽ�ͼ��
		cvtColor(
			TemplateImage,//����ͼƬ����
			Gray_TemplateImage,//���ͼƬ���� 
			COLOR_BGR2GRAY//��ͼƬ��BGR��OpenCVĬ��ͨ����ʽ��ת���ɻҽ�ͼ��
		);
	}

	//���ͼ��Ϊ4ͨ��������alphaͨ����ͼ������ת���ɻҽ�ͼ��
	else if (NumTemplateImageChannel == 4)
	{
		//��ͼƬ��BGRAת���ɻҽ�ͼ��
		cvtColor(
			TemplateImage,//����ͼƬ����
			Gray_TemplateImage,//���ͼƬ���� 
			COLOR_BGRA2GRAY//��ͼƬ��BGRAת���ɻҽ�ͼ��
		);
	}
	//���ͼ���Ѿ�Ϊ��ͨ���ҽ�ͼ��ֱ�ӽ�ResizedImageMat����GrayImageMat
	else if (NumTemplateImageChannel == 1)
	{
		Gray_TemplateImage = TemplateImage;
	}

	//���ͨ������Ϊ1,3��4����������벢�˳�����
	else
	{
		cout << "Unkown image channel type: " << NumTemplateImageChannel;
	}


	//�����������ڴ��ͼ��X������ݶ�ֵ
	Mat TemplateImage_Grad(
		TemplateImageHeight,//����ĵ�һά���߶ȣ��ߴ�
		TemplateImageWidth, //����ĵڶ�ά����ȣ��ߴ�
		CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
		Scalar(0)//�������ĳ�ʼֵ
	);

	//�����������ڴ��ͼ��X������ݶ�ֵ
	Mat TemplateImage_GradX(
		TemplateImageHeight,//����ĵ�һά���߶ȣ��ߴ�
		TemplateImageWidth, //����ĵڶ�ά����ȣ��ߴ�
		CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
		Scalar(0)//�������ĳ�ʼֵ
	);

	//�����������ڴ��ͼ��X������ݶ�ֵ
	Mat TemplateImage_GradY(
		TemplateImageHeight,//����ĵ�һά���߶ȣ��ߴ�
		TemplateImageWidth, //����ĵڶ�ά����ȣ��ߴ�
		CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
		Scalar(0)//�������ĳ�ʼֵ
	);

	//������ؼ��㴹ֱ�ݶȣ��������ұ�Ե�������㣬��ֵΪ���ĳ�ʼֵ0
	for (int iRow = 1; iRow < TemplateImageHeight - 1; iRow++)
	{
		for (int iCol = 1; iCol < (TemplateImageWidth - 1); iCol++)
		{
			TemplateImage_GradY.at<uchar>(iRow, iCol) = uchar(abs(10.0 * (Gray_TemplateImage.at<uchar>(iRow + 1, iCol) - Gray_TemplateImage.at<uchar>(iRow - 1, iCol)) +
				3.0 * (Gray_TemplateImage.at<uchar>(iRow + 1, iCol - 1) - Gray_TemplateImage.at<uchar>(iRow - 1, iCol - 1)) +
				3.0 * (Gray_TemplateImage.at<uchar>(iRow + 1, iCol + 1) - Gray_TemplateImage.at<uchar>(iRow - 1, iCol + 1))) / 16);

			TemplateImage_GradX.at<uchar>(iRow, iCol) = uchar(abs(10.0 * (Gray_TemplateImage.at<uchar>(iRow, iCol + 1) - Gray_TemplateImage.at<uchar>(iRow, iCol + 1)) +
				3.0 * (Gray_TemplateImage.at<uchar>(iRow + 1, iCol + 1) - Gray_TemplateImage.at<uchar>(iRow + 1, iCol - 1)) +
				3.0 * (Gray_TemplateImage.at<uchar>(iRow - 1, iCol + 1) - Gray_TemplateImage.at<uchar>(iRow - 1, iCol - 1))) / 16);
			TemplateImage_Grad.at<uchar>(iRow, iCol) = (TemplateImage_GradY.at<uchar>(iRow, iCol) + TemplateImage_GradX.at<uchar>(iRow, iCol)) / 2;
		}
	}

	Mat Binary_GradY;
	threshold(
		TemplateImage_Grad, //�������
		Binary_GradY, //�������
		128, //������ʼ��ֵ
		255, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);


	//����X�����ݶ�ͶӰ����
	Mat  Binary_ProjectX_GradY(
		int(TemplateImageHeight),//��������
		1,//��������
		CV_32FC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
		Scalar(0)//��������ĳ�ʼֵ
	);

	//��ʱ�Ӻͱ���
	float SumTemp;
	for (int iRow = 0; iRow < TemplateImageHeight; iRow++)
	{
		//ÿ�ε���ǰ���Ӻͱ�������
		SumTemp = 0;

		//����ͬһ��ÿһ�е��ݶ�ֵ
		for (int iCol = 0; iCol < TemplateImageWidth; iCol++)
		{
			SumTemp += float(Binary_GradY.at<uchar>(iRow, iCol));
		}
		//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
		Binary_ProjectX_GradY.at<float>(iRow, 0) = float(SumTemp / TemplateImageWidth);
	}


	Mat Binary_Grad;
	threshold(
		TemplateImage_Grad, //�������
		Binary_Grad, //�������
		128, //������ʼ��ֵ
		255, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);


	//����X�����ݶ�ͶӰ����
	Mat  Binary_ProjectX_Grad(
		int(TemplateImageHeight),//��������
		1,//��������
		CV_32FC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
		Scalar(0)//��������ĳ�ʼֵ
	);

	for (int iRow = 0; iRow < TemplateImageHeight; iRow++)
	{
		//ÿ�ε���ǰ���Ӻͱ�������
		SumTemp = 0;

		//����ͬһ��ÿһ�е��ݶ�ֵ
		for (int iCol = 0; iCol < TemplateImageWidth; iCol++)
		{
			SumTemp += float(Binary_Grad.at<uchar>(iRow, iCol));
		}
		//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
		Binary_ProjectX_Grad.at<float>(iRow, 0) = float(SumTemp / TemplateImageWidth);
	}

	Mat  TemplateImageProjectXGrad = Binary_ProjectX_Grad;
	Mat  TemplateImageProjectXGradY = Binary_ProjectX_GradY;
	vector<int> TemplateLineRow = { 40,107,190,269,356,439,522,605,683 };
	int TemplateMatchHeight = *TemplateLineRow.rbegin() - *TemplateLineRow.begin();
	const double TemplateImageLineGapHeight = 83;
	const double TemplateImagePlateNumberLineHeight = TemplateImageLineGapHeight;
	Rect TemplateImagePlateNumberAreaRect = Rect(159, 106, 454 - 159, 228 - 106);
	int TemplateImagePlateNumberWidth = 175;
	int TemplateImagePlateNumberHeight = 39;
	int TemplateImageTitleStartCol = 193;
	int TemplateImageTitleEndCol = 833;
	int TemplateImageTitleWidth = TemplateImageTitleEndCol - TemplateImageTitleStartCol;
	int TemplateImageTitleLineHeight = TemplateLineRow[1] - TemplateLineRow[0];

	////////////////////////////////////////////////////////////////

	//�������ڴ洢ͼƬ·����String����
	vector<String> XmlPathList;

	//��������Glob���ʽ���ҷ��ϵ�ͼƬ
	glob(
		SearchGlobExpression,//�ļ�����Glob���ʽ
		XmlPathList, //���ͼ���ļ��б�
		false//�����еݹ飬���������ļ��н�������
	);


	int InputImageHeight;
	int InputImageWidth;

	//�Է��ϵ�ͼƬ���б���
	for (vector<String>::iterator itInputXmlPath = XmlPathList.begin();
		itInputXmlPath < XmlPathList.end();
		itInputXmlPath++)
	{

		ifstream XmlFileStream(*itInputXmlPath);
		if (!XmlFileStream)
		{
			cout << "Can't find XML file: '" << *itInputXmlPath <<
				"'. This item would be skipped." << endl;
			continue;
		}

		tinyxml2::XMLDocument XmlDoc;
		XmlDoc.LoadFile((*itInputXmlPath).c_str());
		tinyxml2::XMLElement *LabelElement = XmlDoc.FirstChildElement("annotation")->FirstChildElement("object");
		Rect DetectAreaRect;
		bool IsDetectAreaRectFound = false;
		if (LabelElement == nullptr)
		{
			cout << "Can't find \"object\" element of XML file: '" << *itInputXmlPath <<
				"'. This item would be skipped." << endl;
		}
		else {

			while (LabelElement)
			{

				tinyxml2::XMLElement *LabelNameElement = LabelElement->FirstChildElement("name");
				if (LabelNameElement != nullptr)
				{
					String LabelName = LabelNameElement->GetText();

					if (LabelName == "14.DetectArea")
					{
						tinyxml2::XMLElement *LabelRectElement = LabelElement->FirstChildElement("bndbox");

						DetectAreaRect.x = atoi(LabelRectElement->FirstChildElement("xmin")->GetText()) - 1;
						DetectAreaRect.y = atoi(LabelRectElement->FirstChildElement("ymin")->GetText()) - 1;
						DetectAreaRect.width = atoi(LabelRectElement->FirstChildElement("xmax")->GetText()) - 1 - DetectAreaRect.x;
						DetectAreaRect.height = atoi(LabelRectElement->FirstChildElement("ymax")->GetText()) - 1 - DetectAreaRect.y;
						IsDetectAreaRectFound = true;
						break;
					}
				}


				LabelElement = LabelElement->NextSiblingElement("object");
			}

		}
		if (!IsDetectAreaRectFound)
		{
			cout << "Can't not find '14.DetectedArea' element of XML file: '" << *itInputXmlPath <<
				"'. This item would be skipped." << endl;
			continue;
		}
		size_t LastDotPos = (*itInputXmlPath).rfind('.');//rfind �������'.'
														 //��ȡ���ͼƬ����·�����ļ���Ϊ����ͼ������ǰ����ϡ�Result_����
		String InputImagePath = (*itInputXmlPath).substr(0, LastDotPos) + ".png";
		//�½�����RawImageMat���ڴ洢ԭʼͼƬ����
		Mat InputImage;

		Mat InputImageSegmentResult;

		vector<int> SegmentLineRow(TemplateLineRow.size());

		//���ݵ�һ���������ļ�·������ͼƬ��ȡ
		Mat InputImageAll = imread(
			InputImagePath,//����ͼƬ·��
			CV_LOAD_IMAGE_UNCHANGED//�Բ��޸�ͨ�����͵ķ�ʽ��ȡͼƬ
		);


		//�������Mat�Ƿ����ͼ������
		if (!InputImageAll.data)
		{
			//��ʾͼƬ��ȡʧ����ʾ��Ϣ
			cout << " Error:  Can't read image data from '" << InputImagePath <<
				"'. This item would be skipped." << endl;
			//���ش����벢������ͼƬ
			continue;
		}

		if (DetectAreaRect.x >= 0 && DetectAreaRect.x < InputImageAll.cols &&
			DetectAreaRect.y >= 0 && DetectAreaRect.x < InputImageAll.rows &&
			DetectAreaRect.x + DetectAreaRect.width >= 0 && DetectAreaRect.x + DetectAreaRect.width < InputImageAll.cols &&
			DetectAreaRect.y + DetectAreaRect.height >= 0 && DetectAreaRect.y + DetectAreaRect.height < InputImageAll.rows)
		{
			InputImage = InputImageAll(DetectAreaRect);
		}
		else
		{
			//��ʾͼƬ��ȡʧ����ʾ��Ϣ
			cout << " Error:  Unmatched DetectAreaRect with size of image:'" << InputImagePath <<
				"'. This item would be skipped." << endl;
			//���ش����벢������ͼƬ
			continue;
		}
		InputImageHeight = InputImage.rows;
		InputImageWidth = InputImage.cols;

		//��ͼƬת���ɻҽ�ͼ��
		Mat Gray_InputImage;

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
			cout << "Unkown image channel type: " << NumInputImageChannel << endl;
		}

		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat InputImage_GradX(
			int(InputImageHeight),//����ĵ�һά���߶ȣ��ߴ�
			int(InputImageWidth), //����ĵڶ�ά����ȣ��ߴ�
			CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);

		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat InputImage_GradY(
			int(InputImageHeight),//����ĵ�һά���߶ȣ��ߴ�
			int(InputImageWidth), //����ĵڶ�ά����ȣ��ߴ�
			CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);

		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat InputImage_Grad(
			int(InputImageHeight),//����ĵ�һά���߶ȣ��ߴ�
			int(InputImageWidth), //����ĵڶ�ά����ȣ��ߴ�
			CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);

		//������ؼ��㴹ֱ�ݶȣ��������ұ�Ե�������㣬��ֵΪ���ĳ�ʼֵ0
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			for (int iCol = 1; iCol < (InputImageWidth - 1); iCol++)
			{
				InputImage_GradY.at<uchar>(iRow, iCol) = uchar(abs(10.0 * (Gray_InputImage.at<uchar>(iRow + 1, iCol) - Gray_InputImage.at<uchar>(iRow - 1, iCol)) +
					3.0 * (Gray_InputImage.at<uchar>(iRow + 1, iCol - 1) - Gray_InputImage.at<uchar>(iRow - 1, iCol - 1)) +
					3.0 * (Gray_InputImage.at<uchar>(iRow + 1, iCol + 1) - Gray_InputImage.at<uchar>(iRow - 1, iCol + 1))) / 16);

				InputImage_GradX.at<uchar>(iRow, iCol) = uchar(abs(10.0 * (Gray_InputImage.at<uchar>(iRow, iCol + 1) - Gray_InputImage.at<uchar>(iRow, iCol + 1)) +
					3.0 * (Gray_InputImage.at<uchar>(iRow + 1, iCol + 1) - Gray_InputImage.at<uchar>(iRow + 1, iCol - 1)) +
					3.0 * (Gray_InputImage.at<uchar>(iRow - 1, iCol + 1) - Gray_InputImage.at<uchar>(iRow - 1, iCol - 1))) / 16);
				InputImage_Grad.at<uchar>(iRow, iCol) = (InputImage_GradX.at<uchar>(iRow, iCol) + InputImage_GradY.at<uchar>(iRow, iCol)) / 2;
			}
		}

		Mat Binary_GradY;
		threshold(
			InputImage_GradY, //�������
			Binary_GradY, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		////����X�����ݶ�ͶӰ����
		Mat  Binary_ProjectX_GradY(
			int(InputImageHeight),//��������
			1,//��������
			CV_32FC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);

		//��ʱ�Ӻͱ���
		double SumTemp;
		for (int iRow = 0; iRow < InputImageHeight; iRow++)
		{
			//ÿ�ε���ǰ���Ӻͱ�������
			SumTemp = 0;

			//����ͬһ��ÿһ�е��ݶ�ֵ
			for (int iCol = 0; iCol < InputImageWidth; iCol++)
			{
				SumTemp += float(Binary_GradY.at<uchar>(iRow, iCol));
			}
			//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
			Binary_ProjectX_GradY.at<float>(iRow, 0) = float(SumTemp / InputImageWidth);
		}



		Mat Binary_Grad;
		threshold(
			InputImage_Grad, //�������
			Binary_Grad, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);



		////����X�����ݶ�ͶӰ����
		Mat  Binary_ProjectX_Grad(
			int(InputImageHeight),//��������
			1,//��������
			CV_32FC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);

		//��ʱ�Ӻͱ���
		for (int iRow = 0; iRow < InputImageHeight; iRow++)
		{
			//ÿ�ε���ǰ���Ӻͱ�������
			SumTemp = 0;

			//����ͬһ��ÿһ�е��ݶ�ֵ
			for (int iCol = 0; iCol < InputImageWidth; iCol++)
			{
				SumTemp += float(Binary_Grad.at<uchar>(iRow, iCol));
			}
			//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
			Binary_ProjectX_Grad.at<float>(iRow, 0) = float(SumTemp / InputImageWidth);
		}



		//����X�����ݶ�ͶӰ�������
		Mat  GradY_DiffGradY(
			int(InputImageHeight),//��������
			1,//��������
			CV_32FC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);

		for (int iRow = 1; iRow < InputImageHeight; iRow++)
		{
			GradY_DiffGradY.at<float>(iRow, 0) = fabsf(Binary_ProjectX_GradY.at<float>(iRow, 0) -
				Binary_ProjectX_GradY.at<float>(iRow - 1, 0));
		}

		Mat Histogram_DiffGradY(
			InputImageHeight,
			InputImageWidth,
			CV_8UC1,
			Scalar(0)
		);
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
			Histogram_DiffGradY(Range(iRow, iRow + 1), Range(0, int(GradY_DiffGradY.at<float>(iRow, 0) / 255.0*InputImageWidth))) = 100;

		}

		vector<int> DiffGradY_PeakRow;
		vector<float> DiffGradY_PeakAmp;

		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			if (GradY_DiffGradY.at<float>(iRow, 0) >= GradY_DiffGradY.at<float>(iRow - 1, 0) &&
				GradY_DiffGradY.at<float>(iRow, 0) >= GradY_DiffGradY.at<float>(iRow + 1, 0))
			{

				DiffGradY_PeakRow.push_back(iRow);
				DiffGradY_PeakAmp.push_back(GradY_DiffGradY.at<float>(iRow, 0));

			}
		}


		vector<int> SortPeakIndex = SortIndex(DiffGradY_PeakAmp);
		vector <int> Sort_PeakRow;
		for (vector<int>::iterator itPeakIndex = SortPeakIndex.begin();
			itPeakIndex < SortPeakIndex.end();
			itPeakIndex++)
		{
			Sort_PeakRow.push_back(DiffGradY_PeakRow[*itPeakIndex]);
		}

		bool FlagIgnorePeak = false;
		vector <int>  PeakRow_LineRowTemp;

		double MinPeakDistance;

		Mat InputImageTemp;
		Mat Histogram_DiffGradYTemp;
		float TemplateScaleStep = float(0.01);
		int NumTemplateScale = int((MaxMatchScale - MinMatchScale) / TemplateScaleStep);
		float CurrentMatchScale;

		bool IsLineRowChanged = true;
		bool IsResizedTemplateMatchHeightChanged = true;


		int ResizedTemplateMatchHeight;
		int ResizedTemplateMatchHeightTemp = -1;

		double MaxCorrCoef = 0.0;
		double ClosestMatchScale = 0.0;


		Mat TemplateGrad = TemplateImageProjectXGrad(Range(*TemplateLineRow.begin(), *TemplateLineRow.rbegin()), Range::all());
		for (int iScale = 0; iScale < NumTemplateScale; iScale++)
		{
			CurrentMatchScale = float(MinMatchScale + iScale*TemplateScaleStep);
			InputImageTemp = InputImage.clone();
			Histogram_DiffGradYTemp = Histogram_DiffGradY.clone();
			MinPeakDistance = InputImageHeight * CurrentMatchScale * (TemplateImageLineGapHeight / TemplateImageHeight) * 0.8;
			ResizedTemplateMatchHeight = int(InputImageHeight * CurrentMatchScale * TemplateMatchHeight / TemplateImageHeight);
			if (ResizedTemplateMatchHeightTemp == -1 || ResizedTemplateMatchHeight != ResizedTemplateMatchHeightTemp)
			{
				IsResizedTemplateMatchHeightChanged = true;
				ResizedTemplateMatchHeightTemp = ResizedTemplateMatchHeight;
			}
			else
			{
				IsResizedTemplateMatchHeightChanged = false;
			}
			vector<int> PeakRow_LineRow;

			for (size_t iPeak = 0; iPeak < Sort_PeakRow.size(); iPeak++)
			{
				for (size_t iLine = 0; iLine < PeakRow_LineRow.size(); iLine++)
				{
					if (abs(int(Sort_PeakRow[iPeak]) - int(PeakRow_LineRow[iLine])) < MinPeakDistance)
					{
						FlagIgnorePeak = true;
						break;
					}
				}
				if (FlagIgnorePeak == true)
				{
					FlagIgnorePeak = false;
				}
				else
				{
					PeakRow_LineRow.push_back(Sort_PeakRow[iPeak]);
					//InputImageTemp.row(Sort_PeakRow[iPeak]) = Scalar(0, 0, 255);

					//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
					Histogram_DiffGradYTemp(Range(Sort_PeakRow[iPeak], Sort_PeakRow[iPeak] + 1),
						Range(0, int(GradY_DiffGradY.at<float>(Sort_PeakRow[iPeak], 0) / 255.0*InputImageWidth))) = 255;
				}
			}
			sort(PeakRow_LineRow.begin(), PeakRow_LineRow.end());
			if (PeakRow_LineRowTemp.empty() || PeakRow_LineRow != PeakRow_LineRowTemp)
			{
				IsLineRowChanged = true;
				PeakRow_LineRowTemp = PeakRow_LineRow;
			}
			else
			{
				IsLineRowChanged = false;
			}



			if (IsLineRowChanged || IsResizedTemplateMatchHeightChanged)
			{
				int InputMatchHeight = *(PeakRow_LineRow.rbegin()) - *PeakRow_LineRow.begin();

				Mat Resize_TemplateGrad;
				resize(
					TemplateGrad,
					Resize_TemplateGrad,
					Size(1, ResizedTemplateMatchHeight),
					0,
					0,
					INTER_LINEAR
				);

				vector <int> ResizeShift_TemplateLineRow;
				ResizeShift_TemplateLineRow.push_back(-1 * ResizedTemplateMatchHeight + 1);

				for (vector<int>::iterator itLine = TemplateLineRow.begin() + 1; itLine != TemplateLineRow.end() - 1; itLine++)
				{

					ResizeShift_TemplateLineRow.push_back(int((*itLine - TemplateLineRow.back())*ResizedTemplateMatchHeight / TemplateMatchHeight));
				}
				ResizeShift_TemplateLineRow.push_back(0);
				//vector <int> Resize_Shift_TemplateLineRow;
				//for (vector<int>::iterator itLine = TemplateLineRow.begin(); itLine != TemplateLineRow.end(); itLine++)
				//{
				//	Resize_Shift_TemplateLineRow.push_back(int((*itLine - *TemplateLineRow.begin())*CurrentMatchScale));
				//}

				bool FlagNextStep = false;
				vector<int> TemplateMatchStep;
				for (int iStep = *PeakRow_LineRow.begin(); iStep < InputImageHeight + ResizedTemplateMatchHeight; iStep++)
				{
					for (vector<int> ::iterator itTemplateLine = ResizeShift_TemplateLineRow.begin();
						itTemplateLine != ResizeShift_TemplateLineRow.end();
						itTemplateLine++)
					{
						if (*itTemplateLine + iStep >= 0 && *itTemplateLine + iStep < InputImageHeight)
						{
							for (vector<int> ::iterator itInputLine = PeakRow_LineRow.begin();
								itInputLine != PeakRow_LineRow.end();
								itInputLine++)
							{
								if (*itTemplateLine + iStep == *itInputLine)
								{
									FlagNextStep = true;
									break;
								}
							}
						}
						if (FlagNextStep == true) break;
					}
					if (FlagNextStep == true)
					{
						FlagNextStep = false;
						TemplateMatchStep.push_back(iStep);
						continue;
					}
				}
				int InputMatchBegin, InputMatchEnd, TemplateMatchBegin, TemplateMatchEnd;

				for (vector<int>::iterator itStep = TemplateMatchStep.begin();
					itStep != TemplateMatchStep.end();
					itStep++)
				{
					InputMatchBegin = *ResizeShift_TemplateLineRow.begin() + *itStep < 0 ?
						0 : *ResizeShift_TemplateLineRow.begin() + *itStep;
					InputMatchEnd = *ResizeShift_TemplateLineRow.rbegin() + *itStep < InputImageHeight ?
						*ResizeShift_TemplateLineRow.rbegin() + *itStep : InputImageHeight;

					TemplateMatchBegin = InputMatchBegin - (*ResizeShift_TemplateLineRow.begin() + *itStep);
					TemplateMatchEnd = InputMatchEnd - (*ResizeShift_TemplateLineRow.begin() + *itStep);


					if (double(TemplateMatchEnd - TemplateMatchBegin + 1) / ResizedTemplateMatchHeight < 1.0 / MaxMatchScale)
					{
						continue;
					}

					Mat InputData = Binary_ProjectX_Grad(Range(InputMatchBegin, InputMatchEnd), Range::all());
					Mat TemplateData = Resize_TemplateGrad(Range(TemplateMatchBegin, TemplateMatchEnd), Range::all());

					Mat  DataMeanMat, DataStdDevMat;
					cv::meanStdDev(InputData, DataMeanMat, DataStdDevMat);
					float InputDataMean = float(DataMeanMat.at<double>(0, 0));
					float InputDataStdDev = float(DataStdDevMat.at<double>(0, 0));
					cv::meanStdDev(TemplateData, DataMeanMat, DataStdDevMat);
					float TemplateDataMean = float(DataMeanMat.at<double>(0, 0));
					float TemplateDataStdDev = float(DataStdDevMat.at<double>(0, 0));

					double CorrCoef = 0.0;
					for (int iRow = 0; iRow < InputData.rows; iRow++)
					{
						CorrCoef = CorrCoef + InputData.at<float>(iRow, 0) * TemplateData.at<float>(iRow, 0);
					}

					CorrCoef = (CorrCoef - InputData.rows*InputDataMean*TemplateDataMean) /
						(InputData.rows * InputDataStdDev * TemplateDataStdDev);

					if (CorrCoef > MaxCorrCoef)
					{
						MaxCorrCoef = CorrCoef;
						ClosestMatchScale = CurrentMatchScale;

						for (int  iLineRow = 0;
							iLineRow < ResizeShift_TemplateLineRow.size();
							iLineRow++)
						{
							SegmentLineRow[iLineRow] = (ResizeShift_TemplateLineRow[iLineRow] + *itStep);
						}
					}

				}
			}

		}

		InputImageSegmentResult = InputImageTemp.clone();
		for (int iLineRow = 0; iLineRow < SegmentLineRow.size(); iLineRow++)
		{
			if (SegmentLineRow[iLineRow] >= 0 && SegmentLineRow[iLineRow] < InputImageHeight)
			{
				if (iLineRow == 1)
				{
					InputImageSegmentResult.row(SegmentLineRow[iLineRow]) = Scalar(0, 255, 0);
				}
				else
				{
					InputImageSegmentResult.row(SegmentLineRow[iLineRow]) = Scalar(255, 0, 0);
				}
			}

		}
		//�ĵ����� ������Ҫ�ж����Ƿ����� ��������Ļ����ݶȵ���Y�᷽���ͶӰ �Ƚϴ�С
		vector <double> LineDutyRatio;
		double MinDutyRatio = 1.0;


		int TemplateXWidth = int(ClosestMatchScale*InputImageHeight*TemplateImageRatio);
		Mat Binary_LineGrad;
		Mat  Binary_ProjectY_LineGrad(
			int(SegmentLineRow.size()),
			InputImageWidth,
			CV_8UC1,
			Scalar(0.0)
		);
		Mat  ProjectY_Binary_LineGrad(
			int(SegmentLineRow.size()),
			InputImageWidth,
			CV_8UC1,
			Scalar(0.0)
		);

		Mat Binary_TitleLineGrad;
		bool IsTitleLineFound = false;
		bool IsPlateNumberLineFound = false;
		int TitleLineIndex;
		int PlateNumberLineIndex;

		int TemplateYTitleLineHeight;
		int TemplateYPlateNumberLineHeight;


		for (int iLine = 0; iLine < SegmentLineRow.size() - 1; iLine++)
		{
			if (iLine == 0)
			{
				TemplateYTitleLineHeight = SegmentLineRow[iLine + 1] - SegmentLineRow[iLine];
			}
			else if (iLine == 1)
			{
				TemplateYPlateNumberLineHeight = SegmentLineRow[iLine + 1] - SegmentLineRow[iLine];
			}
			if (SegmentLineRow[iLine] < 0)
			{
				SegmentLineRow[iLine] = 0;
			}
			else if (SegmentLineRow[iLine] >= InputImageHeight)
			{
				SegmentLineRow[iLine] = InputImageHeight - 1;
			}

			if (SegmentLineRow[iLine + 1] < 0)
			{
				SegmentLineRow[iLine + 1] = 0;
			}
			else if (SegmentLineRow[iLine + 1] >= InputImageHeight)
			{
				SegmentLineRow[iLine + 1] = InputImageHeight - 1;
			}


			if (SegmentLineRow[iLine] < SegmentLineRow[iLine + 1])
			{
				threshold(
					InputImage_Grad(Range(SegmentLineRow[iLine], SegmentLineRow[iLine + 1]), Range::all()),
					Binary_LineGrad,
					128, //������ʼ��ֵ
					255, //���ֵ��������ֵ����Ϊ��ֵ��
					CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
				);

				if (iLine == 0)
				{
					IsTitleLineFound = true;
					Binary_TitleLineGrad = Binary_LineGrad.clone();
					TitleLineIndex = iLine;

				}
				if (iLine == 1)
				{
					IsPlateNumberLineFound = true;
					//Binary_TitleLineGrad = Binary_LineGrad.clone();
					PlateNumberLineIndex = iLine;

				}


				for (int iCol = 0; iCol < InputImageWidth; iCol++)
				{
					SumTemp = 0;
					for (int iRow = 0; iRow < SegmentLineRow[iLine + 1] - SegmentLineRow[iLine]; iRow++)
					{
						SumTemp = SumTemp + Binary_LineGrad.at<uchar>(iRow, iCol);
					}
					Binary_ProjectY_LineGrad.at<uchar>(iLine, iCol) = uchar(SumTemp / (SegmentLineRow[iLine + 1] - SegmentLineRow[iLine]));
				}

				threshold(
					Binary_ProjectY_LineGrad.row(iLine),
					ProjectY_Binary_LineGrad.row(iLine),
					128, //������ʼ��ֵ
					1, //���ֵ��������ֵ����Ϊ��ֵ��
					CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
				);



				SumTemp = 0;
				for (int iCol = 0;
					iCol < InputImageWidth;
					iCol++)
				{

					SumTemp = SumTemp + ProjectY_Binary_LineGrad.row(iLine).at<uchar>(0, iCol) / 255.0;
				}
				LineDutyRatio.push_back(SumTemp / TemplateXWidth);
				//if (LineDutyRatio.back()< MinDutyRatio)
				//{
				//	MinDutyRatio = LineDutyRatio.back();
				//	MinDutyRatioLineIndex = iLine;
				//}

			}
			else
			{
				LineDutyRatio.push_back(NAN);
			}

		}


		double TitleLineDutyRatio = 0.458213270;
		double PlateNumberLineDutyRatio = 0.412103742;
		double OwnerLineDutyRatio = 0.139289141;
		vector<double>::iterator itMinLineDutyRatio = min_element(LineDutyRatio.begin(), LineDutyRatio.end());
		if (distance(LineDutyRatio.begin(), itMinLineDutyRatio) > 2)
		{
			if (*(itMinLineDutyRatio - 2) / *(itMinLineDutyRatio - 1) >= 0.6*TitleLineDutyRatio / PlateNumberLineDutyRatio  &&
				*(itMinLineDutyRatio - 2) / *(itMinLineDutyRatio - 1) < 1.4*TitleLineDutyRatio / PlateNumberLineDutyRatio  &&
				*(itMinLineDutyRatio) / *(itMinLineDutyRatio - 1) >= 0.6*PlateNumberLineDutyRatio / OwnerLineDutyRatio  &&
				*(itMinLineDutyRatio) / *(itMinLineDutyRatio - 1) < 1.4*PlateNumberLineDutyRatio / OwnerLineDutyRatio)
			{
				PlateNumberLineIndex = int(distance(LineDutyRatio.begin(), itMinLineDutyRatio) - 1);
				TitleLineIndex = PlateNumberLineIndex - 1;
				IsTitleLineFound = true;
				IsPlateNumberLineFound = true;

			}

		}
		else if (distance(LineDutyRatio.begin(), itMinLineDutyRatio) == 0)
		{
			LineDutyRatio.erase(itMinLineDutyRatio);
			itMinLineDutyRatio = min_element(LineDutyRatio.begin(), LineDutyRatio.end());
			if (distance(LineDutyRatio.begin(), itMinLineDutyRatio) >= 2)
			{
				if (*(itMinLineDutyRatio - 2) / *(itMinLineDutyRatio - 1) >= 0.6*TitleLineDutyRatio / PlateNumberLineDutyRatio  &&
					*(itMinLineDutyRatio - 2) / *(itMinLineDutyRatio - 1) < 1.4*TitleLineDutyRatio / PlateNumberLineDutyRatio  &&
					*(itMinLineDutyRatio) / *(itMinLineDutyRatio - 1) >= 0.6*PlateNumberLineDutyRatio / OwnerLineDutyRatio  &&
					*(itMinLineDutyRatio) / *(itMinLineDutyRatio - 1) < 1.4*PlateNumberLineDutyRatio / OwnerLineDutyRatio)
				{
					PlateNumberLineIndex = int(distance(LineDutyRatio.begin(), itMinLineDutyRatio) - 1);
					IsPlateNumberLineFound = true;

					TitleLineIndex = PlateNumberLineIndex - 1;
					IsTitleLineFound = true;

				}

			}
		}

		if (IsTitleLineFound == true && IsPlateNumberLineFound == true)
		{



			double  EstimateTitleWidth = double(TemplateYTitleLineHeight) / double(TemplateImageTitleLineHeight)*double(TemplateImageTitleWidth);
			double EstimateTitleCharWidth = EstimateTitleWidth / 13.0;


			Mat Binary_TitleLineGrad;

			threshold(
				InputImage_Grad.rowRange(SegmentLineRow[TitleLineIndex], SegmentLineRow[TitleLineIndex + 1]),
				Binary_TitleLineGrad,
				128,
				255,
				CV_THRESH_OTSU);
			int ElementSize = int(EstimateTitleCharWidth / 2) * 2 + 1;
			Mat DilateStructingElement = getStructuringElement(MORPH_RECT, Size(ElementSize, 1));

			Mat Binary_Dilate_TitleLineGrad;
			morphologyEx(Binary_TitleLineGrad, Binary_Dilate_TitleLineGrad, MORPH_DILATE, DilateStructingElement);

			Mat  Dilate_ProjectY_TitleLineGrad
			(
				1,
				Binary_Dilate_TitleLineGrad.cols,
				CV_8UC1,
				Scalar(0)
			);
			for (int iCol = 0; iCol < Binary_Dilate_TitleLineGrad.cols; iCol++)
			{
				SumTemp = 0;
				for (int iRow = 0; iRow < Binary_Dilate_TitleLineGrad.rows; iRow++)
				{
					SumTemp = SumTemp + Binary_Dilate_TitleLineGrad.ptr<uchar>(iRow)[iCol];
				}
				Dilate_ProjectY_TitleLineGrad.ptr<uchar>(0)[iCol] = uchar(SumTemp/ Binary_Dilate_TitleLineGrad.rows);
							
			}
			Mat ProjectY_Binary_TitleLineGrad = Dilate_ProjectY_TitleLineGrad.clone();

			threshold(
				Dilate_ProjectY_TitleLineGrad,
				ProjectY_Binary_TitleLineGrad,
				0,
				1,
				CV_THRESH_OTSU);
			ProjectY_Binary_TitleLineGrad.convertTo(ProjectY_Binary_TitleLineGrad, CV_8UC1);

			int MatchStartCol, MatchEndCol;
			if (InputImageWidth <= EstimateTitleWidth)
			{
				MatchStartCol = 0;
				MatchEndCol = InputImageWidth;
			}
			else
			{
				int iStartCol = 0;
				int SumTemp = 0;
				vector <int> NumTitleOne;
				for (int iCol = iStartCol; iCol < iStartCol + EstimateTitleWidth + ElementSize - 1; iCol++)
				{
					SumTemp = SumTemp + ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[iCol];

				}
				NumTitleOne.push_back(SumTemp);
				for (iStartCol = 1; iStartCol + EstimateTitleWidth + ElementSize < InputImageWidth; iStartCol++)
				{
					NumTitleOne.push_back(NumTitleOne.back() -
						ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[iStartCol - 1] +
						ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[int(iStartCol + EstimateTitleWidth + ElementSize)]);
				}
				vector <int>::iterator itMaxNumTitleOne = max_element(NumTitleOne.begin(), NumTitleOne.end());
				MatchStartCol = int(distance(NumTitleOne.begin(), itMaxNumTitleOne));
				MatchEndCol = int(MatchStartCol + EstimateTitleWidth + ElementSize - 1);
			}
			if (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchStartCol] == 1)
			{
				while (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchStartCol] == 0 && MatchStartCol >= 0)
				{
					MatchStartCol--;
				}
			}
			else if (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchStartCol] == 0)
			{
				while (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchStartCol] == 1 && MatchStartCol < InputImageWidth)
				{
					MatchStartCol++;
				}

			}

			if (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchEndCol] == 1)
			{
				while (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchEndCol] == 0 && MatchEndCol < InputImageWidth)
				{
					MatchEndCol++;
				}
			}
			else if (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchEndCol] == 0)
			{
				while (ProjectY_Binary_TitleLineGrad.ptr<uchar>(0)[MatchEndCol] == 1 && MatchEndCol >= 0)
				{
					MatchEndCol--;
				}

			}

			MatchStartCol = MatchStartCol + int(EstimateTitleCharWidth / 2);
			MatchEndCol = MatchEndCol - int(EstimateTitleCharWidth / 2);

			rectangle(
				InputImageSegmentResult,
				Point(MatchStartCol, SegmentLineRow[TitleLineIndex]),
				Point(MatchEndCol, SegmentLineRow[TitleLineIndex + 1]),
				Scalar(0, 0, 0),
				1,
				LINE_AA,
				0
			);
		}
		////////////////////////////////////////////////////////////////////////////////////////////////

		//Ѱ���ļ�·�����һ����\��
		size_t SepPos = InputImagePath.rfind('\\');//rfind �������
												   //��ȡ�ļ���·��
		string FolderPath = InputImagePath.substr(0, SepPos);
		//��ȡͼƬ�ļ���
		string ImageFileName = InputImagePath.substr(SepPos + 1, -1);
		//��ȡ���ͼƬ����·�����ļ���Ϊ����ͼ������ǰ����ϡ�Result_����
		string OutputImagePath = OutputPath + "Result_" + ImageFileName;
		//����ͼ�񱣴�
		imwrite(OutputImagePath, InputImageSegmentResult);
	}
	//����0�������˳�����
	return 0;
}