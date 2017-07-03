//���OpenCV���ͷ�ļ�
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//��ӱ�׼�����������ͷ�ļ�
#include <iostream>
//����㷨��ͷ�ļ�
#include<algorithm>//Ϊ��ʹ��swap
//�����ѧ�����ͷ�ļ�
//#include <cmath>

#define NUMPEAK 9

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
	for (size_t iImage = 0; iImage < ImagePathList.size(); iImage++)
	{
		//�½�����RawImageMat���ڴ洢ԭʼͼƬ����
		Mat RawImage;

		//���ݵ�һ���������ļ�·������ͼƬ��ȡ
		RawImage = imread(
			ImagePathList[iImage],//����ͼƬ·��
			CV_LOAD_IMAGE_UNCHANGED//�Բ��޸�ͨ�����͵ķ�ʽ��ȡͼƬ
		);

		//�������Mat�Ƿ����ͼ������
		if (!RawImage.data)
		{
			//��ʾͼƬ��ȡʧ����ʾ��Ϣ
			cout << " Error:  Can't read image data from" << ArgumentVector[1] << endl;
			//���ش����벢�˳�����
			return -1;
		}
		//��������ͼ��Ŀ��ߡ�����Ϳ�߱�
		const double InputImageWidth = RawImage.cols;
		const double InputImageHeight = RawImage.rows;

		//����ģ��ͼƬ����Ŀ�߱�
		const double TemplateRatio = TemplateWidth / TemplateHeight;


		//����ģ��ͼƬ����Ŀ�߱Ⱥ����
		const double TemplateTitleRatio = TemplateTitleWidth / TemplateTitleHeight;

		//����ģ��ĳ�������Ŀ�߱Ⱥ����
		const double TemplatePlateRatio = TemplatePlateWidth / TemplatePlateHeight;

		//��ͼƬת���ɻҽ�ͼ��
		Mat Raw_GrayImage;

		//��ȡͼƬ��ͨ����
		int NumRawImageChannel = RawImage.channels();

		//���ͼ��Ϊ3ͨ����ɫͼ��
		if (NumRawImageChannel == 3)
		{
			//��ͼƬ��BGRת���ɻҽ�ͼ��
			cvtColor(
				RawImage,//����ͼƬ����
				Raw_GrayImage,//���ͼƬ���� 
				COLOR_BGR2GRAY//��ͼƬ��BGR��OpenCVĬ��ͨ����ʽ��ת���ɻҽ�ͼ��
			);
		}

		//���ͼ��Ϊ4ͨ��������alphaͨ����ͼ������ת���ɻҽ�ͼ��
		else if (NumRawImageChannel == 4)
		{
			//��ͼƬ��BGRAת���ɻҽ�ͼ��
			cvtColor(
				RawImage,
				Raw_GrayImage,
				COLOR_BGRA2GRAY//��ͼƬ��BGRAת���ɻҽ�ͼ��
			);
		}
		//���ͼ���Ѿ�Ϊ��ͨ���ҽ�ͼ��ֱ�ӽ�ResizedImageMat����GrayImageMat
		else if (NumRawImageChannel == 1)
		{
			Raw_GrayImage = RawImage;
		}

		//���ͨ������Ϊ1,3��4����������벢�˳�����
		else
		{
			cout << "Unkown image channel type: " << RawImage.channels();
		}


		//ָ����������
		const char* MainWindowName = "get plate number from vehicle license";


		//������Ӧ�Ĵ���
		namedWindow(
			MainWindowName,//��������
			CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		);
		//��ͼƬ��������Ϊ�̶��߶�500���أ��Ҳ��ı�ͼ��Ŀ�߱�
		double WindowHeight = 500.0;
		resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		imshow(
			MainWindowName,
			Raw_GrayImage
		);

		waitKey(0);

		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat GrayImage_GradY(
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
				GrayImage_GradY.at<uchar>(iRow, iCol) = (2 * abs(Raw_GrayImage.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayImage.at<uchar>(iRow + 1, iCol - 1))
					+ abs(Raw_GrayImage.at<uchar>(iRow - 1, iCol) - Raw_GrayImage.at<uchar>(iRow + 1, iCol))
					+ abs(Raw_GrayImage.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayImage.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}

		Mat GrayImage_BinaryGradY;

		threshold(
			GrayImage_GradY, //�������
			GrayImage_BinaryGradY, //�������
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
				SumTemp += double(GrayImage_BinaryGradY.at<uchar>(iRow, iCol));
			}
			//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
			Binary_ProjectYGradY.at<uchar>(iRow, 0) = unsigned char(SumTemp / InputImageWidth);
		}

		Mat ProjectY_DiffGradY(
			int(InputImageHeight),//��������
			1,//��������
			CV_32SC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);
		int TempDiff;
		int DiffGradY_TopNPeakAmp[NUMPEAK+1] = { 0 };
		int DiffGradY_TopNPeakIndex[NUMPEAK+1] = { 0 };
 		for(int iRow=1;iRow<InputImageHeight-1;iRow++)
		{
			TempDiff = Binary_ProjectYGradY.at<uchar>(iRow, 0) - Binary_ProjectYGradY.at<uchar>(iRow - 1, 0);
			if (TempDiff > 0)
			{
				ProjectY_DiffGradY.at<int>(iRow, 0) = TempDiff;
				DiffGradY_TopNPeakAmp[0] = TempDiff;
				DiffGradY_TopNPeakIndex[0] = iRow;
				for (int iPeak = 0;iPeak<NUMPEAK;iPeak++)
				{
					if(DiffGradY_TopNPeakAmp[iPeak] > DiffGradY_TopNPeakAmp[iPeak+1])
					{
						swap(DiffGradY_TopNPeakAmp[iPeak], DiffGradY_TopNPeakAmp[iPeak + 1]);
						swap(DiffGradY_TopNPeakIndex[iPeak], DiffGradY_TopNPeakIndex[iPeak + 1]);
					}
					else
					{
						break;
					}

				}
			}
		}
		
		int TopN_SortPeakAmp[NUMPEAK];
		int TopN_SortPeakIndex[NUMPEAK];
		for (int iPeak1 = 1; iPeak1 < NUMPEAK+1;iPeak1++)
		{
			for (int iPeak2 = iPeak1; iPeak2 < NUMPEAK ; iPeak2++)
			{
				if (DiffGradY_TopNPeakIndex[iPeak2] > DiffGradY_TopNPeakIndex[iPeak2+1])
				{
					swap(DiffGradY_TopNPeakIndex[iPeak2], DiffGradY_TopNPeakIndex[iPeak2 + 1]);
				}
				else
				{
					break;
				}

			}
		}
		for (int iPeak = 1; iPeak < NUMPEAK + 1; iPeak++)
		{
			TopN_SortPeakIndex[iPeak - 1] = DiffGradY_TopNPeakIndex[iPeak];
			TopN_SortPeakAmp[iPeak - 1] = ProjectY_DiffGradY.at<int>(TopN_SortPeakIndex[iPeak - 1], 0);
		}
 		Mat  DiffGradY_StemFigure(
			int(InputImageHeight),//��������
			int(InputImageWidth),//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);
		//����Stemͼʱ������ɫ
		unsigned char StemColor;
		//����ˮƽ�ݶ�ˮƽͶӰ���Stemͼ
		for (int iRow = 0; iRow < InputImageHeight; iRow++)
		{
			StemColor = 100;
			for (int iPeak = 0; iPeak < NUMPEAK; iPeak++)
			{
				if (iRow == TopN_SortPeakIndex[iPeak])
				{
					StemColor = 255;
					break;
				}
			}
			////���ݶ�ֵ���������Stemͼ������ɫ����Ϊ0��ʹ�û�ɫ�������ͶӰֵС����ֵ
			//if (ProjectY_BinaryGradY.at<uchar>(iRow, 0) == 0)
			//{
			//	StemColor = 100;
			//}
			////��Ϊ255�����ð�ɫ�������ͶӰֵ������ֵ
			//else
			//{
			//	StemColor = 255;
			//}
			//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
			if (ProjectY_DiffGradY.at<int>(iRow, 0) > 0)
			{
				for (int iCol = 0; iCol < ProjectY_DiffGradY.at<int>(iRow, 0) / 255.0*InputImageWidth; iCol++)
				{
					DiffGradY_StemFigure.at<uchar>(iRow, iCol) = StemColor;
				}
			}

		}


		//��ʾ��Ե��ֵͼ��
		namedWindow(
			"ProjectYGradY_StemFigure",//��������
			CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		);
		//�����ڱ�Ϊ�߶�ΪWindowHeight����߱Ȳ���
		resizeWindow("ProjectYGradY_StemFigure", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));
		imshow(
			"ProjectYGradY_StemFigure",
			DiffGradY_StemFigure
		);
		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);

		//������ʻ֤�ı������Ʒ�Χ
		//ˮƽ��������ռ�ݿ�ȵ�60%
		double LicenseProportionXMin = 0.6;
		//ˮƽ��������ﵽ��ȵ�110%
		double LicenseProportionXMax = 1.2;
		//��ֱ��������ռ�ݸ߶ȵ�60%
		double LicenseProportionYMin = 0.6;
		//��ֱ��������ﵽ��ȵ�110%
		double LicenseProportionYMax = 1.2;

		//���ݱ������Ʒ�Χ�����������ĸ߶ȿ��ܷ�Χ
		double TitleMinHeight = InputImageHeight*LicenseProportionYMin*(TemplateTitleHeight / TemplateHeight);
		double TitleMaxHeight = InputImageHeight*LicenseProportionYMax*(TemplateTitleHeight / TemplateHeight);
		//���ݱ������Ƽ�����ⲿ�ִ�ֱ��϶���߶�
		double TitleMaxGapHeight = InputImageHeight*LicenseProportionYMax *(TemplateTitleGapHeight / TemplateHeight);
		//���ݱ������Ƽ�����ⲿ��Ӣ��ϰ��Ŀ��ܷ�Χ
		double TitleMinEnglishHeight = InputImageHeight*LicenseProportionYMin *(TemplateTitleEnglishHeight / TemplateHeight);
		double TitleMaxEnglishHeight = InputImageHeight*LicenseProportionYMax *(TemplateTitleEnglishHeight / TemplateHeight);
		//���ݱ������Ƽ����������Ŀ�ȿ��ܷ�Χ
		double TitleMinWidth = InputImageWidth*LicenseProportionXMin*(TemplateTitleWidth / TemplateWidth);
		double TitleMaxWidth = InputImageWidth*LicenseProportionXMax*(TemplateTitleWidth / TemplateWidth);
		//���ݱ������Ƽ�����ⲿ��ˮƽ��϶�����
		double TitleMaxGapWidth = InputImageWidth*LicenseProportionXMax *(TemplateTitleGapWidth / TemplateWidth);

		//���������洢��ֵ����ˮƽͶӰ�������غ��½���������
		vector<int> RisingRow, FallingRow;
		//���������洢��ֵ������ⲿ��ˮƽͶӰ�������غ��½���������
		vector<int> TitleRisingRow, TitleFallingRow, IndexTitleRisingRow, IndexTitleFallingRow;

		
	}
	//����0�������˳�����
	return 0;
}