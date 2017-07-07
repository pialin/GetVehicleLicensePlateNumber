//���OpenCV���ͷ�ļ�
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//��ӱ�׼�����������ͷ�ļ�
#include <iostream>
//����㷨��ͷ�ļ�
#include<algorithm>//Ϊ��ʹ��swap
#include<numeric>//Ϊ��ʹ��iota
//�����ѧ�����ͷ�ļ�
//#include <cmath>

//ʹ��C++��׼�������ռ�
using namespace std;

//ʹ��OpenCV�������ռ�
using namespace cv;

//��ģ��ͼƬ�л�õ�λ����Ϣ
//ģ��ͼƬ�Ŀ���
const double TemplateWidth = 1044;
const double TemplateHeight = 716;
//ģ��ͼƬ������������Ŀ��߼��������ĵ�XY����
const double TemplateTitleWidth = 684;
const double TemplateTitleHeight = 68;
const double TemplateTitleCenterX = 512;
const double TemplateTitleCenterY = 72;
//ģ��ͼƬ���ⲿ�ֵ�ˮƽ��϶�߶ȣ�ˮƽͶӰʱ���ĺ�Ӣ��֮����ֵĿ�϶��ȣ�
const double TemplateTitleGapHeight = 5;
//ģ��ͼƬ���ⲿ�ֵĴ�ֱ��϶��ȣ���ֱͶӰʱ����֮����ֵĿ�϶��ȣ�
const double TemplateTitleGapWidth = 40;
//ģ��ͼƬ���ⲿ��Ӣ�ĵĸ߶�
const double TemplateTitleEnglishHeight = 14;
//ģ��ͼƬ��������Ŀ��߼��������ĵ�XY����
const double TemplatePlateWidth = 174;
const double TemplatePlateHeight = 41;
const double TemplatePlateCenterX = 271;
const double TemplatePlateCenterY = 166;

double WindowHeight = 700.0;

template <typename SortValueType>
vector<size_t>  SortIndex(vector<SortValueType> &InputValueVector) {

	// initialize original index locations
	vector<size_t> IndexVector(InputValueVector.size());
	iota(IndexVector.begin(), IndexVector.end(), 0);

	// sort indexes based on comparing values in v
	sort(IndexVector.begin(),
		IndexVector.end(),
		[&InputValueVector](size_t x, size_t y) {return InputValueVector[x] > InputValueVector[y]; });
	return IndexVector;
}


//�����������������в�����
//��1��Ϊ������ͼƬ��·��������ʹ��Glob���ʽѡ�ж���ļ�����ͬ�ļ������ѭ������
//��2������Ϊ������·��,Ĭ���ڸ��ļ������ԡ�Result_��+����ͼ���ļ����ķ�ʽ������
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
	///////////////////////////////////////////////////
	//�Է��ϵ�ͼƬ���б���

		//�½�����RawImageMat���ڴ洢ԭʼͼƬ����
	Mat RawTemplate;

	//���ݵ�һ���������ļ�·������ͼƬ��ȡ
	RawTemplate = imread(
		"E:\\Desktop\\TestImage\\Template2.jpg",//����ͼƬ·��
		CV_LOAD_IMAGE_UNCHANGED//�Բ��޸�ͨ�����͵ķ�ʽ��ȡͼƬ
	);

	//�������Mat�Ƿ����ͼ������
	if (!RawTemplate.data)
	{
		//��ʾͼƬ��ȡʧ����ʾ��Ϣ
		cout << " Error:  Can't read image data from E:\\Desktop\\TestImage\\Template2.jpg"<< endl;
		//���ش����벢�˳�����
		return -1;
	}
	//��������ͼ��Ŀ��ߡ�����Ϳ�߱�
	const double TemplateWidth = RawTemplate.cols;
	const double TemplateHeight = RawTemplate.rows;
	const double TemplateRatio = TemplateWidth / TemplateHeight;

	//��ͼƬת���ɻҽ�ͼ��
	Mat Raw_GrayTemplate;

	//��ȡͼƬ��ͨ����
	int NumTemplateChannel = RawTemplate.channels();

	//���ͼ��Ϊ3ͨ����ɫͼ��
	if (NumTemplateChannel == 3)
	{
		//��ͼƬ��BGRת���ɻҽ�ͼ��
		cvtColor(
			RawTemplate,//����ͼƬ����
			Raw_GrayTemplate,//���ͼƬ���� 
			COLOR_BGR2GRAY//��ͼƬ��BGR��OpenCVĬ��ͨ����ʽ��ת���ɻҽ�ͼ��
		);
	}

	//���ͼ��Ϊ4ͨ��������alphaͨ����ͼ������ת���ɻҽ�ͼ��
	else if (NumTemplateChannel == 4)
	{
		//��ͼƬ��BGRAת���ɻҽ�ͼ��
		cvtColor(
			RawTemplate,
			Raw_GrayTemplate,
			COLOR_BGRA2GRAY//��ͼƬ��BGRAת���ɻҽ�ͼ��
		);
	}
	//���ͼ���Ѿ�Ϊ��ͨ���ҽ�ͼ��ֱ�ӽ�ResizedImageMat����GrayImageMat
	else if (NumTemplateChannel == 1)
	{
		Raw_GrayTemplate = RawTemplate;
	}

	//���ͨ������Ϊ1,3��4����������벢�˳�����
	else
	{
		cout << "Unkown image channel type: " << RawTemplate.channels();
	}


	//�����������ڴ��ͼ��X������ݶ�ֵ
	Mat GrayTemplate_GradY(
		int(TemplateHeight),//����ĵ�һά���߶ȣ��ߴ�
		int(TemplateWidth), //����ĵڶ�ά����ȣ��ߴ�
		CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
		Scalar(0)//�������ĳ�ʼֵ
	);
	//������ؼ��㴹ֱ�ݶȣ��������ұ�Ե�������㣬��ֵΪ���ĳ�ʼֵ0
	for (int iRow = 1; iRow < TemplateHeight - 1; iRow++)
	{
		for (int iCol = 1; iCol < (TemplateWidth - 1); iCol++)
		{
			//GrayTemplate_GradY.at<uchar>(iRow, iCol) = (2 * abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol - 1))
			//	+ abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol))
			//	+ abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol + 1))) / 4;
			GrayTemplate_GradY.at<uchar>(iRow, iCol) = (2 * abs(Raw_GrayTemplate.at<uchar>(iRow, iCol - 1) - Raw_GrayTemplate.at<uchar>(iRow , iCol + 1))
				+ abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol-1) - Raw_GrayTemplate.at<uchar>(iRow - 1, iCol+1))
				+ abs(Raw_GrayTemplate.at<uchar>(iRow + 1, iCol - 1) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol - 1))) / 4;
		}
	}

	Mat GrayTemplate_BinaryGradY;
	threshold(
		GrayTemplate_GradY, //�������
		GrayTemplate_BinaryGradY, //�������
		128, //������ʼ��ֵ
		255, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);


	//����Y�ݶ�ͶӰ����
	Mat  Binary_ProjectYGradY(
		int(TemplateHeight),//��������
		1,//��������
		CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
		Scalar(0)//��������ĳ�ʼֵ
	);

	//��ʱ�Ӻͱ���
	double SumTemp;
	for (int iRow = 0; iRow < TemplateHeight; iRow++)
	{
		//ÿ�ε���ǰ���Ӻͱ�������
		SumTemp = 0;

		//����ͬһ��ÿһ�е��ݶ�ֵ
		for (int iCol = 0; iCol < TemplateWidth; iCol++)
		{
			SumTemp += double(GrayTemplate_BinaryGradY.at<uchar>(iRow, iCol));
		}
		//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
		Binary_ProjectYGradY.at<uchar>(iRow, 0) = unsigned char(SumTemp / TemplateWidth);
	}

	Mat ProjectY_BinaryGradY;
	threshold(
		Binary_ProjectYGradY, //�������
		ProjectY_BinaryGradY, //�������
		128, //������ʼ��ֵ
		255, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);

	//uchar StemColor;
	//for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
	//{
	//	if (Diff_BinaryGradY.at<uchar>(iRow,0) == 255)
	//	{
	//		StemColor = 255;
	//	}
	//	else
	//	{
	//		StemColor = 100;
	//	}

	//	//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
	//	for (int iCol = 0; iCol < ProjectY_DiffGradY.at<uchar>(iRow, 0) / 255.0*InputImageWidth; iCol++)
	//	{ 
	//		DiffGradX_StemFigure.at<uchar>(iRow, iCol) = StemColor;
	//	}
	//}
	vector<size_t> BinaryGradY_EdgeRow;
	vector<uchar> BinaryGradY_EdgeAmp;

	for (int iRow = 1; iRow < TemplateHeight - 1; iRow++)
	{
		if (abs(int(ProjectY_BinaryGradY.at<uchar>(iRow + 1, 0)) - int(ProjectY_BinaryGradY.at<uchar>(iRow, 0))) == 255)
		{
			BinaryGradY_EdgeRow.push_back(iRow);
			BinaryGradY_EdgeAmp.push_back(abs(int(Binary_ProjectYGradY.at<uchar>(iRow + 1, 0)) - int(Binary_ProjectYGradY.at<uchar>(iRow, 0))));

		}
	}

	vector<size_t> SortedEdgeIndex = SortIndex(BinaryGradY_EdgeAmp);
	//vector <uchar>  BinaryGradY_SortEdgeAmp;
	vector <size_t> BinaryGradY_SortEdgeRow;
	for (vector<size_t>::iterator itEdgeIndex = SortedEdgeIndex.begin();
		itEdgeIndex < SortedEdgeIndex.end();
		itEdgeIndex++)
	{
		//BinaryGradY_SortEdgeAmp.push_back(BinaryGradY_EdgeAmp[*itEdgeIndex]);
		BinaryGradY_SortEdgeRow.push_back(BinaryGradY_EdgeRow[*itEdgeIndex]);
	}

	bool FlagIgnoreEdge = false;

	//vector <uchar>  Sort_FilterEdgeAmp;
	vector <size_t> SortEdgeRow_LineRow;

	double MinEdgeDistance = TemplateHeight / TemplateHeight * 80 * 0.6;
	SortEdgeRow_LineRow.push_back(BinaryGradY_SortEdgeRow[0]);
	uchar TempLineGapWidth;
	for (size_t iEdge = 1; iEdge < BinaryGradY_SortEdgeRow.size(); iEdge++)
	{
		for (size_t jEdge = 0; jEdge < SortEdgeRow_LineRow.size(); jEdge++)
		{
			TempLineGapWidth = uchar(abs(int(BinaryGradY_SortEdgeRow[iEdge]) - int(SortEdgeRow_LineRow[jEdge])));
			if (TempLineGapWidth < MinEdgeDistance)
			{
				FlagIgnoreEdge = true;
				break;
			}
		}
		if (FlagIgnoreEdge == true)
		{
			FlagIgnoreEdge = false;
		}
		else
		{
			SortEdgeRow_LineRow.push_back(BinaryGradY_SortEdgeRow[iEdge]);
			//Sort_FilterEdgeAmp.push_back(BinaryGradY_SortEdgeAmp[iEdge]);
		}
	}

	SortEdgeRow_LineRow.push_back(0);
	SortEdgeRow_LineRow.push_back(TemplateHeight - 1);

	sort(SortEdgeRow_LineRow.begin(), SortEdgeRow_LineRow.end());
	vector <int64> SortEdgeRow_LineGapWidth;
	RawTemplate.row(SortEdgeRow_LineRow[0]) = Scalar(0, 0, 255);
	for (vector<size_t>::iterator itLine = SortEdgeRow_LineRow.begin()+1;
		itLine < SortEdgeRow_LineRow.end();
		itLine++)
	{
		SortEdgeRow_LineGapWidth.push_back(*itLine - *(itLine - 1));
		RawTemplate.row(*itLine) = Scalar(0, 0, 255);
	}


	
	int64 TemplateMedianLineGapWidth;
	if (SortEdgeRow_LineGapWidth.size() % 2 == 1) 
	{ 
		partial_sort(SortEdgeRow_LineGapWidth.begin(),
			SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 + 1,
			SortEdgeRow_LineGapWidth.end());
		TemplateMedianLineGapWidth = *(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2);
	}
	else
	{
		partial_sort(SortEdgeRow_LineGapWidth.begin(),
			SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 + 2,
			SortEdgeRow_LineGapWidth.end());
		TemplateMedianLineGapWidth = (*(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2) + 
			*(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 +1 ))/2;
	}

	vector <size_t>  TemplateLineRow = SortEdgeRow_LineRow;
	vector <uchar> ProjectYTemplateGradY = Binary_ProjectYGradY;
	////////////////////////////////////////////////////////
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


		//ָ����������
		const char* MainWindowName = "get plate number from vehicle license";


		////������Ӧ�Ĵ���
		//namedWindow(
		//	MainWindowName,//��������
		//	CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		//);
		////��ͼƬ��������Ϊ�̶��߶�500���أ��Ҳ��ı�ͼ��Ŀ�߱�
		//double WindowHeight = 700.0;
		//resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		//imshow(
		//	MainWindowName,
		//	Raw_GrayInput
		//);

		//waitKey(0);


		//������ʻ֤�ı������Ʒ�Χ
		//ˮƽ��������ռ�ݿ�ȵ�60%
		double LicenseProportionXMin = 0.6;
		//ˮƽ��������ﵽ��ȵ�110%
		double LicenseProportionXMax = 1.2;
		//��ֱ��������ռ�ݸ߶ȵ�60%
		double LicenseProportionYMin = 0.6;
		//��ֱ��������ﵽ��ȵ�110%
		double LicenseProportionYMax = 1.2;


		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat GrayInput_GradY(
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
				GrayInput_GradY.at<uchar>(iRow, iCol) = (2 * abs(Raw_GrayInput.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayInput.at<uchar>(iRow + 1, iCol - 1))
					+ abs(Raw_GrayInput.at<uchar>(iRow - 1, iCol) - Raw_GrayInput.at<uchar>(iRow + 1, iCol))
					+ abs(Raw_GrayInput.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayInput.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}

		Mat GrayInput_BinaryGradY;

		threshold(
			GrayInput_GradY, //�������
			GrayInput_BinaryGradY, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);


		//����Y�ݶ�ͶӰ����
		Mat  Binary_ProjectYGradY(
			int(InputImageHeight),//��������
			1,//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
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
				SumTemp += double(GrayInput_BinaryGradY.at<uchar>(iRow, iCol));
			}
			//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
			Binary_ProjectYGradY.at<uchar>(iRow, 0) = unsigned char(SumTemp / InputImageWidth);
		}

		//Mat ProjectY_DiffGradY(
		//	int(InputImageHeight),//��������
		//	1,//��������
		//	CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
		//	Scalar(0)//��������ĳ�ʼֵ
		//);


		//Mat DiffGradX_StemFigure(
		//	int(InputImageHeight),//��������
		//	int(InputImageWidth),//��������
		//	CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
		//	Scalar(0)//��������ĳ�ʼֵ
		//);
		//for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		//{
		//	ProjectY_DiffGradY.at<uchar>(iRow, 0) =
		//		abs(Binary_ProjectYGradY.at<uchar>(iRow, 0) - Binary_ProjectYGradY.at<uchar>(iRow - 1, 0));
		//	
		//}

		

		Mat ProjectY_BinaryGradY;
		threshold(
			Binary_ProjectYGradY, //�������
			ProjectY_BinaryGradY, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		//uchar StemColor;
		//for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		//{
		//	if (Diff_BinaryGradY.at<uchar>(iRow,0) == 255)
		//	{
		//		StemColor = 255;
		//	}
		//	else
		//	{
		//		StemColor = 100;
		//	}

		//	//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
		//	for (int iCol = 0; iCol < ProjectY_DiffGradY.at<uchar>(iRow, 0) / 255.0*InputImageWidth; iCol++)
		//	{ 
		//		DiffGradX_StemFigure.at<uchar>(iRow, iCol) = StemColor;
		//	}
		//}
		vector<size_t> BinaryGradY_EdgeRow;
		vector<uchar> BinaryGradY_EdgeAmp;

		for (int iRow = 1; iRow < InputImageHeight-1; iRow++)
		{
			if (abs(int(ProjectY_BinaryGradY.at<uchar>(iRow + 1, 0)) - int(ProjectY_BinaryGradY.at<uchar>(iRow, 0))) == 255)
			{
				BinaryGradY_EdgeRow.push_back(iRow);
				BinaryGradY_EdgeAmp.push_back(abs(int(Binary_ProjectYGradY.at<uchar>(iRow + 1, 0)) - int(Binary_ProjectYGradY.at<uchar>(iRow, 0))));

			}
		}
		 
		vector<size_t> SortedEdgeIndex = SortIndex(BinaryGradY_EdgeAmp);
		//vector <uchar>  BinaryGradY_SortEdgeAmp;
		vector <size_t> DiffGradY_SortEdgeRow;
		for (vector<size_t>::iterator itEdgeIndex = SortedEdgeIndex.begin();
			itEdgeIndex < SortedEdgeIndex.end();
			itEdgeIndex++)
		{
			//BinaryGradY_SortEdgeAmp.push_back(BinaryGradY_EdgeAmp[*itEdgeIndex]);
			DiffGradY_SortEdgeRow.push_back(BinaryGradY_EdgeRow[*itEdgeIndex]);
		}

		bool FlagIgnoreEdge = false;

		//vector <uchar>  Sort_FilterEdgeAmp;
		vector <size_t> SortEdgeRow_LineRow;

		double MinEdgeDistance = InputImageHeight/TemplateHeight *80 * 0.6;
		SortEdgeRow_LineRow.push_back(DiffGradY_SortEdgeRow[0]);

		for (size_t iEdge = 1; iEdge < DiffGradY_SortEdgeRow.size(); iEdge++)
		{
			for (size_t jEdge = 0; jEdge < SortEdgeRow_LineRow.size(); jEdge++)
			{
				if (abs(int(DiffGradY_SortEdgeRow[iEdge]) - int(SortEdgeRow_LineRow[jEdge])) < MinEdgeDistance)
				{
					FlagIgnoreEdge = true;
					break;
				}
			}
			if (FlagIgnoreEdge == true)
			{
				FlagIgnoreEdge = false;
			}
			else
			{
				SortEdgeRow_LineRow.push_back(DiffGradY_SortEdgeRow[iEdge]);
				//Sort_FilterEdgeAmp.push_back(BinaryGradY_SortEdgeAmp[iEdge]);
			}
		}

		SortEdgeRow_LineRow.push_back(0);
		SortEdgeRow_LineRow.push_back(InputImageHeight - 1);

		sort(SortEdgeRow_LineRow.begin(), SortEdgeRow_LineRow.end());
		vector <int64> SortEdgeRow_LineGapWidth;
		RawInput.row(SortEdgeRow_LineRow[0]) = Scalar(0, 0, 255);
		for (vector<size_t>::iterator itLine = SortEdgeRow_LineRow.begin() + 1;
			itLine < SortEdgeRow_LineRow.end();
			itLine++)
		{
			SortEdgeRow_LineGapWidth.push_back(*itLine - *(itLine - 1));
			RawInput.row(*itLine) = Scalar(0, 0, 255);
		}



		//int64 InputMedianLineGapWidth;
		//if (SortEdgeRow_LineGapWidth.size() % 2 == 1)
		//{
		//	partial_sort(SortEdgeRow_LineGapWidth.begin(),
		//		SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 + 1,
		//		SortEdgeRow_LineGapWidth.end());
		//	InputMedianLineGapWidth = *(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2);
		//}
		//else
		//{
		//	partial_sort(SortEdgeRow_LineGapWidth.begin(),
		//		SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 + 2,
		//		SortEdgeRow_LineGapWidth.end());
		//	InputMedianLineGapWidth = (*(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2) +
		//		*(SortEdgeRow_LineGapWidth.begin() + SortEdgeRow_LineGapWidth.size() / 2 + 1)) / 2;
		//}

		//sort(SortEdgeRow_LineGapWidth.begin(),
		//	SortEdgeRow_LineGapWidth.end(),
		//	[InputMedianLineGapWidth](int64 x, int64 y) {return abs(x- InputMedianLineGapWidth) < abs(y - InputMedianLineGapWidth); });

		//vector <size_t>  InputLineRow = SortEdgeRow_LineRow;

		//vector <int64> InputLineGapWidth = SortEdgeRow_LineGapWidth;

		//Mat InputProjectYGradY = Binary_ProjectYGradY;

		//size_t TempInputLicenseHeight, TempInputMatchStartRow, TempInputMatchEndRow;
		//double TempInputHeightProportion;
		//vector <double> InputHeightProportion;
		//vector <size_t> InputLicenseHeight,InputMatchStartRow, InputMatchEndRow, TemplateMatchStartRow, TemplateMatchEndRow;
		//for (vector<int64>::iterator itInputGapWidth = InputLineGapWidth.begin();
		//	itInputGapWidth != InputLineGapWidth.end();
		//	itInputGapWidth++)
		//{

		//		for (vector<size_t>::iterator itCrossLine = InputLineRow.begin()+1;
		//			itCrossLine != TemplateLineRow.end();
		//			itCrossLine++)
		//		{
		//		
		//			TempInputLicenseHeight = size_t(double(*itInputGapWidth / TemplateMedianLineGapWidth) *TemplateHeight);

		//			TempInputMatchEndRow = *itCrossLine;

		//			TempInputMatchStartRow = (TempInputMatchEndRow - TempInputLicenseHeight) <0 ? 0:(TempInputMatchEndRow - TempInputLicenseHeight);
		//			
		//			TempInputHeightProportion = (TempInputMatchEndRow - TempInputMatchStartRow) / InputImageHeight;
		//			if (TempInputHeightProportion > 0.6)
		//			{
		//				InputMatchStartRow.push_back(TempInputMatchStartRow);
		//				InputMatchEndRow.push_back(TempInputMatchEndRow);
		//				InputLicenseHeight.push_back(TempInputLicenseHeight);
		//				TemplateMatchStartRow.push_back(*itTemplateLine);
		//				TemplateMatchEndRow.push_back(TemplateMatchStartRow.back() +
		//					double((TempInputMatchEndRow - TempInputMatchStartRow) / TempInputLicenseHeight)*TemplateHeight);

		//				InputHeightProportion.push_back(TempInputHeightProportion);
		//			}
		//			
		//		}

		//}

		//vector <size_t> SortedMatchIndex = SortIndex(InputHeightProportion);

		//for (vector <size_t>::iterator iMatch = SortedMatchIndex.begin();
		//	iMatch < SortedMatchIndex.end;
		//	iMatch++)
		//{
		//	Mat ResizedTemplateGradY(
		//		InputLicenseHeight[*iMatch],
		//		1,
		//		CV_8UC1,
		//		0);
		//	cv::resize(ProjectYTemplateGradY,
		//		ResizedTemplateGradY,
		//		ResizedTemplateGradY.size(),
		//		0,
		//		0,
		//		INTER_LINEAR
		//	);
		//	Mat MatchTemplateGradY = ResizedTemplateGradY(Range(0, TemplateMatchEndRow[*iMatch]),Range::all());
		//	Mat MatchInputGradY = ResizedTemplateGradY(Range(TemplateMatchStartRow[*iMatch], TemplateMatchEndRow[*iMatch]), Range::all());
		//	cv::meanStdDev(MatchTemplateGradY)
		//}
		
		
 	//	imshow(MainWindowName, RawInput);
		//waitKey(0);
  	}
	//����0�������˳�����
	return 0; 
}