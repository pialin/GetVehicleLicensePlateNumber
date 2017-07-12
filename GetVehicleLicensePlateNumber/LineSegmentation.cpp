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

double WindowHeight = 500.0;

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


int Dft1D(Mat & Input,Mat & Output)
{
	Mat Pad_Input;                            //expand input image to optimal size
	int NumPadInputCol = getOptimalDFTSize(Input.cols);
	copyMakeBorder(Input, Pad_Input, 0, 0, 0, NumPadInputCol - Input.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat DftPlane[] = { Mat_<float>(Pad_Input), Mat::zeros(Pad_Input.size(), CV_32F) };
	Mat DftComplexMat;
	merge(DftPlane, 2, DftComplexMat);         // Add to the expanded another plane with zeros

	dft(DftComplexMat, DftComplexMat,DFT_ROWS);            // this way the result may fit in the source matrix

										
	split(DftComplexMat, DftPlane);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(DftPlane[0], DftPlane[1], DftPlane[0]);// planes[0] = magnitude  
	Mat DftMagnitude = DftPlane[0];
	//DftMagnitude += Scalar::all(1);                    // switch to logarithmic scale
	//log(DftMagnitude, Output);
	////Output =  DftPlane[0];
	//// crop the spectrum, if it has an odd number of rows or columns
	////Output( Range::all(),Range(3, 20)) = DftMagnitude(Range::all(), Range(3, 20));
	float SumTemp = 0;
	for (int iFreq = 8 ; iFreq < 16; iFreq++)
	{
		for (int iHarmonic = 1; iHarmonic < 3; iHarmonic++)
		{
			SumTemp = SumTemp + DftMagnitude.at<float>(0, iHarmonic*iFreq);
		}
		Output.at<float>(0, iFreq) = SumTemp;
		SumTemp = 0;
	}
	normalize(Output, Output,1.0, 0.0, NORM_MINMAX);

	return 0;
}

//�����������������в�����
//��1��Ϊ������ͼƬ��·��������ʹ��Glob���ʽѡ�ж���ļ�����ͬ�ļ������ѭ������
//��2������Ϊ������·��,Ĭ���ڸ��ļ������ԡ�Result_��+����ͼ���ļ����ķ�ʽ������
int main(int ArgumentCount, char** ArgumentVector)
{


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


		//������Ӧ�Ĵ���
		//namedWindow(
		//	MainWindowName,//��������
		//	CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		//);

		//resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		//imshow(
		//	MainWindowName,
		//	Raw_GrayInput
		//);

		//waitKey(0);


		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat GrayInput_Grad(
			int(InputImageHeight),//����ĵ�һά���߶ȣ��ߴ�
			int(InputImageWidth), //����ĵڶ�ά����ȣ��ߴ�
			CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);

		double TempGradX,TempGradY;
		//������ؼ��㴹ֱ�ݶȣ��������ұ�Ե�������㣬��ֵΪ���ĳ�ʼֵ0
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			for (int iCol = 1; iCol < (InputImageWidth - 1); iCol++)
			{
				TempGradY = (10 * (Raw_GrayInput.at<uchar>(iRow + 1, iCol - 1) - Raw_GrayInput.at<uchar>(iRow - 1, iCol - 1))
					+ 3* (Raw_GrayInput.at<uchar>(iRow + 1, iCol) - Raw_GrayInput.at<uchar>(iRow - 1, iCol))
					+ 3* (Raw_GrayInput.at<uchar>(iRow + 1, iCol + 1) - Raw_GrayInput.at<uchar>(iRow - 1, iCol + 1))) / 32.0;

				TempGradX = (10 * (Raw_GrayInput.at<uchar>(iRow, iCol + 1) - Raw_GrayInput.at<uchar>(iRow, iCol - 1))
					+ 3 * (Raw_GrayInput.at<uchar>(iRow - 1, iCol + 1) - Raw_GrayInput.at<uchar>(iRow - 1, iCol - 1))
					+ 3 * (Raw_GrayInput.at<uchar>(iRow - 1, iCol - 1) - Raw_GrayInput.at<uchar>(iRow + 1, iCol - 1))) / 32.0; 
				GrayInput_Grad.at<uchar>(iRow, iCol) = uchar(fabs(TempGradY));
				//GrayInput_Grad.at<uchar>(iRow, iCol) = uchar(fabs(TempGradX) + fabs(TempGradY));
			}
		}

		Mat GrayInput_BinaryGrad;

		threshold(
			GrayInput_Grad, //�������
			GrayInput_BinaryGrad, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		imshow(
			MainWindowName,
			GrayInput_BinaryGrad
		);
		//����Y�ݶ�ͶӰ����
		Mat  Binary_ProjectYGrad(
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
				SumTemp += double(GrayInput_BinaryGrad.at<uchar>(iRow, iCol));
			}
			//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
			Binary_ProjectYGrad.at<uchar>(iRow, 0) = unsigned char(SumTemp / InputImageWidth);
		}

		Mat ProjectY_DiffGrad(
			int(InputImageHeight),//��������
			1,//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			ProjectY_DiffGrad.at<uchar>(iRow, 0) =
				abs(Binary_ProjectYGrad.at<uchar>(iRow, 0) - Binary_ProjectYGrad.at<uchar>(iRow - 1, 0));

		}
		Binary_ProjectYGrad = ProjectY_DiffGrad;
		/*int MedianBlurKernelSize = 2 * int(InputImageHeight*0.003)+1;
		medianBlur(
			ProjectY_DiffGrad,
			Binary_ProjectYGrad,
			MedianBlurKernelSize
		);*/

		Mat ProjectYGrad_Histogram(
			int(InputImageHeight),//��������
			int(InputImageWidth),//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);




		//Mat ProjectY_BinaryGrad;
		//threshold(
		//	Binary_ProjectYGrad, //�������
		//	ProjectY_BinaryGrad, //�������
		//	128, //������ʼ��ֵ
		//	255, //���ֵ��������ֵ����Ϊ��ֵ��
		//	CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		//);

		uchar HistogramColor= 255;
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			/*if (Diff_BinaryGradY.at<uchar>(iRow,0) == 255)
			{
				HistogramColor = 255; 
			}
			else
			{
				HistogramColor = 100;
			}*/

			//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
			for (int iCol = 0; iCol <  Binary_ProjectYGrad.at<uchar>(iRow, 0) / 255.0*InputImageWidth; iCol++)
			{ 
				ProjectYGrad_Histogram.at<uchar>(iRow, iCol) = HistogramColor;
			}
		}

		//������Ӧ�Ĵ���
		//namedWindow(
		//	"ProjectYGrad_Histogram",//��������
		//	CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		//);
		//resizeWindow("ProjectYGrad_Histogram", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		//imshow(
		//	"ProjectYGrad_Histogram",
		//	ProjectYGrad_Histogram
		//);
		//waitKey(0);

		Mat DftMagnitude(
			1,
			int(InputImageHeight),
			CV_32FC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0.0)//��������ĳ�ʼֵ
			);
		Binary_ProjectYGrad = Binary_ProjectYGrad.t();
		Dft1D(Binary_ProjectYGrad, DftMagnitude);

		Mat DftMagnitude_Histogram(
			int(InputImageHeight),
			int(InputImageWidth),
			CV_8UC1,
			Scalar(0)
		);

		for (int iRow = 0; iRow < InputImageHeight ; iRow++)
		{
			//if (Diff_BinaryGradY.at<uchar>(iRow, 0) == 255)
			//{
			//	HistogramColor = 255;
			//}
			//else
			//{
			//	HistogramColor = 100;
			//}

			//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
			for (int iCol = 0; iCol < DftMagnitude.at<float>(0, iRow)*InputImageWidth; iCol++)
			{
 				DftMagnitude_Histogram.at<uchar>(iRow, iCol) = HistogramColor;
			}
		}
		

   	}
	//����0�������˳�����
	return 0;
}