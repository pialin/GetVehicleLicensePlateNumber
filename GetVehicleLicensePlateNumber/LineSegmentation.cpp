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
	for (int iCol = 5 ;iCol< 20;iCol++)
	{
		Output.at<float>(0, iCol) = DftMagnitude.at<float>(0, iCol);
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
		namedWindow(
			MainWindowName,//��������
			CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		);

		resizeWindow(MainWindowName, int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

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
		namedWindow(
			"ProjectYGrad_Histogram",//��������
			CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		);
		resizeWindow("ProjectYGrad_Histogram", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		imshow(
			"ProjectYGrad_Histogram",
			ProjectYGrad_Histogram
		);
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
			/*if (Diff_BinaryGradY.at<uchar>(iRow,0) == 255)
			{
			HistogramColor = 255;
			}
			else
			{
			HistogramColor = 100;
			}*/

			//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
			for (int iCol = 0; iCol < DftMagnitude.at<float>(0, iRow)*InputImageWidth; iCol++)
			{
 				DftMagnitude_Histogram.at<uchar>(iRow, iCol) = HistogramColor;
			}
		}
		
		//������Ӧ�Ĵ���
		namedWindow(
			" DftMagnitude_Histogram",//��������
			CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		);
		resizeWindow(" DftMagnitude_Histogram", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));

		imshow(
			" DftMagnitude_Histogram",
			DftMagnitude_Histogram
		);
		//waitKey(0);
		//vector<size_t> BinaryGradY_EdgeRow;
		//vector<uchar> BinaryGradY_EdgeAmp;

		//for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		//{
		//	if (abs(int(ProjectY_BinaryGrad.at<uchar>(iRow + 1, 0)) - int(ProjectY_BinaryGrad.at<uchar>(iRow, 0))) == 255)
		//	{
		//		BinaryGradY_EdgeRow.push_back(iRow);
		//		BinaryGradY_EdgeAmp.push_back(abs(int(Binary_ProjectYGrad.at<uchar>(iRow + 1, 0)) - int(Binary_ProjectYGrad.at<uchar>(iRow, 0))));

		//	}
		//}

		//vector<size_t> SortedEdgeIndex = SortIndex(BinaryGradY_EdgeAmp);
		////vector <uchar>  BinaryGradY_SortEdgeAmp;
		//vector <size_t> DiffGradY_SortEdgeRow;
		//for (vector<size_t>::iterator itEdgeIndex = SortedEdgeIndex.begin();
		//	itEdgeIndex < SortedEdgeIndex.end();
		//	itEdgeIndex++)
		//{
		//	//BinaryGradY_SortEdgeAmp.push_back(BinaryGradY_EdgeAmp[*itEdgeIndex]);
		//	DiffGradY_SortEdgeRow.push_back(BinaryGradY_EdgeRow[*itEdgeIndex]);
		//}

		//bool FlagIgnoreEdge = false;

		////vector <uchar>  Sort_FilterEdgeAmp;
		//vector <size_t> SortEdgeRow_LineRow;

		//double MinEdgeDistance = InputImageHeight / TemplateHeight * 80 * 0.6;
		//SortEdgeRow_LineRow.push_back(DiffGradY_SortEdgeRow[0]);

		//for (size_t iEdge = 1; iEdge < DiffGradY_SortEdgeRow.size(); iEdge++)
		//{
		//	for (size_t jEdge = 0; jEdge < SortEdgeRow_LineRow.size(); jEdge++)
		//	{
		//		if (abs(int(DiffGradY_SortEdgeRow[iEdge]) - int(SortEdgeRow_LineRow[jEdge])) < MinEdgeDistance)
		//		{
		//			FlagIgnoreEdge = true;
		//			break;
		//		}
		//	}
		//	if (FlagIgnoreEdge == true)
		//	{
		//		FlagIgnoreEdge = false;
		//	}
		//	else
		//	{
		//		SortEdgeRow_LineRow.push_back(DiffGradY_SortEdgeRow[iEdge]);
		//		//Sort_FilterEdgeAmp.push_back(BinaryGradY_SortEdgeAmp[iEdge]);
		//	}
		//}

		//SortEdgeRow_LineRow.push_back(0);
		//SortEdgeRow_LineRow.push_back(InputImageHeight - 1);

		//sort(SortEdgeRow_LineRow.begin(), SortEdgeRow_LineRow.end());
		//vector <int64> SortEdgeRow_LineGapWidth;
		//RawInput.row(SortEdgeRow_LineRow[0]) = Scalar(0, 0, 255);
		//for (vector<size_t>::iterator itLine = SortEdgeRow_LineRow.begin() + 1;
		//	itLine < SortEdgeRow_LineRow.end();
		//	itLine++)
		//{
		//	SortEdgeRow_LineGapWidth.push_back(*itLine - *(itLine - 1));
		//	RawInput.row(*itLine) = Scalar(0, 0, 255);
		//}



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