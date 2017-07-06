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
			GrayTemplate_GradY.at<uchar>(iRow, iCol) = (2 * abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol - 1))
				+ abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol))
				+ abs(Raw_GrayTemplate.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayTemplate.at<uchar>(iRow + 1, iCol + 1))) / 4;
		}
	}

	Mat GrayTemplate_BinaryGradY;/////////////////////////////////

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

	Mat ProjectY_DiffGradY(
		int(TemplateHeight),//��������
		1,//��������
		CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
		Scalar(0)//��������ĳ�ʼֵ
	);


	for (int iRow = 1; iRow < TemplateHeight - 1; iRow++)
	{
		ProjectY_DiffGradY.at<uchar>(iRow, 0) = 
			abs(Binary_ProjectYGradY.at<uchar>(iRow, 0) - Binary_ProjectYGradY.at<uchar>(iRow - 1, 0));
	}

	Mat Diff_BinaryGradY;
	threshold(
		ProjectY_DiffGradY, //�������
		Diff_BinaryGradY, //�������
		128, //������ʼ��ֵ
		255, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);
	vector<size_t> DiffGradY_PeakRow;
	vector<uchar> DiffGradY_PeakAmp;
	for (size_t iRow = 0;
		iRow < TemplateHeight;
		iRow++)
	{
		if (Diff_BinaryGradY.at<uchar>(iRow, 0) == 255)
		{

			DiffGradY_PeakAmp.push_back(ProjectY_DiffGradY.at<uchar>(iRow, 0));
			DiffGradY_PeakRow.push_back(iRow);
		}
	}
	vector<size_t> SortedPeakIndex = SortIndex(DiffGradY_PeakAmp);
	//vector <uchar>  BinaryGradY_SortPeakAmp;
	vector <size_t> DiffGradY_SortPeakRow;
	for (vector<size_t>::iterator itPeakIndex = SortedPeakIndex.begin();
		itPeakIndex < SortedPeakIndex.end();
		itPeakIndex++)
	{
		//BinaryGradY_SortPeakAmp.push_back(BinaryGradY_PeakAmp[*itPeakIndex]);
		DiffGradY_SortPeakRow.push_back(DiffGradY_PeakRow[*itPeakIndex]);
	}

	bool FlagIgnorePeak = false;

	//vector <uchar>  Sort_FilterPeakAmp;
	vector <size_t> SortPeakRow_LineRow;

	double MinPeakDistance = 80 * 0.6;
	SortPeakRow_LineRow.push_back(DiffGradY_SortPeakRow[0]);

	for (size_t iPeak = 1; iPeak < DiffGradY_SortPeakRow.size(); iPeak++)
	{
		for (size_t jPeak = 0; jPeak < SortPeakRow_LineRow.size(); jPeak++)
		{
			if (fabs(double(DiffGradY_SortPeakRow[iPeak]) - double(SortPeakRow_LineRow[jPeak])) < MinPeakDistance)
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
			SortPeakRow_LineRow.push_back(DiffGradY_SortPeakRow[iPeak]);
			//Sort_FilterPeakAmp.push_back(BinaryGradY_SortPeakAmp[iPeak]);
		}
	}

	SortPeakRow_LineRow.push_back(0);
	SortPeakRow_LineRow.push_back(TemplateHeight-1);

	sort(SortPeakRow_LineRow.begin(), SortPeakRow_LineRow.end());

	vector <size_t>  TemplateLineRow = SortPeakRow_LineRow;
	Mat TemplateDiffGradX = ProjectY_DiffGradY;
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
		vector<size_t> DiffGradY_EdgeRow;
		vector<uchar> DiffGradY_EdgeAmp;

		for (int iRow = 0; iRow < InputImageHeight-1; iRow++)
		{
			if (abs(int(ProjectY_BinaryGradY.at<uchar>(iRow + 1, 0)) - int(ProjectY_BinaryGradY.at<uchar>(iRow, 0))) == 255)
			{
				DiffGradY_EdgeRow.push_back(iRow);
				DiffGradY_EdgeAmp.push_back(abs(int(Binary_ProjectYGradY.at<uchar>(iRow + 1, 0)) - int(Binary_ProjectYGradY.at<uchar>(iRow, 0))));

			}
		}
		 
		vector<size_t> SortedEdgeIndex = SortIndex(DiffGradY_EdgeAmp);
		//vector <uchar>  BinaryGradY_SortEdgeAmp;
		vector <size_t> DiffGradY_SortEdgeRow;
		for (vector<size_t>::iterator itEdgeIndex = SortedEdgeIndex.begin();
			itEdgeIndex < SortedEdgeIndex.end();
			itEdgeIndex++)
		{
			//BinaryGradY_SortEdgeAmp.push_back(BinaryGradY_EdgeAmp[*itEdgeIndex]);
			DiffGradY_SortEdgeRow.push_back(DiffGradY_EdgeRow[*itEdgeIndex]);
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
				if (fabs(double(DiffGradY_SortEdgeRow[iEdge]) - double(SortEdgeRow_LineRow[jEdge])) < MinEdgeDistance)
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

		for (vector<size_t>::iterator itLineY = SortEdgeRow_LineRow.begin();
			itLineY < SortEdgeRow_LineRow.end();
			itLineY++)
		{
			if (*itLineY == 0)
			{
				RawInput(Range(0, 1), Range::all()) = Scalar(0, 0, 255);
			}
			else if (*itLineY == InputImageHeight)
			{
				RawInput(Range(InputImageHeight-1, InputImageHeight), Range::all()) = Scalar(0, 0, 255);
			}
			else
			{
				RawInput(Range(*itLineY - 1, *itLineY + 1), Range::all()) = Scalar(0, 0, 255);
			}
		}

		//	Mat  FilterPeakIndex_StemFigure(
			//	int(InputImageHeight),//��������
			//	int(InputImageWidth),//��������
			//	CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			//	Scalar(0)//��������ĳ�ʼֵ
			//);
			//

			//sort(PartialSort_FilterPeakIndex.begin(), PartialSort_FilterPeakIndex.end());
			//
			//vector<double> Filter_DiffPeakIndex(PartialSort_FilterPeakIndex.size()-1);
			//for (size_t iPeak = 1;
			//	iPeak < PartialSort_FilterPeakIndex.size();
			//	iPeak++)
			//{
			//	Filter_DiffPeakIndex[iPeak - 1] = PartialSort_FilterPeakIndex[iPeak] - PartialSort_FilterPeakIndex[iPeak-1];
			//}

			//sort(Filter_DiffPeakIndex.begin(), Filter_DiffPeakIndex.end());

			//double EstimatedLineGapWidth;
			//if (Filter_DiffPeakIndex.size() % 2 == 1)
			//{
			//	EstimatedLineGapWidth = Filter_DiffPeakIndex[Filter_DiffPeakIndex.size() / 2];
			//}
			//else
			//{
			//	EstimatedLineGapWidth = (Filter_DiffPeakIndex[(Filter_DiffPeakIndex.size()) / 2-1] +
			//		Filter_DiffPeakIndex[(Filter_DiffPeakIndex.size()) / 2 ])/2;
			//}
			//for (vector<size_t>::iterator itLine = PartialSort_FilterPeakIndex.begin();
			//	itLine != PartialSort_FilterPeakIndex.end();
			//	itLine++)
			//{
			//	RawImage(Range(*itLine-1,*itLine+1),Range::all()) = Scalar(0, 0, 255);
			//}
 	//	imshow(MainWindowName, RawInput);
		//waitKey(0);
  	}
	//����0�������˳�����
	return 0; 
}