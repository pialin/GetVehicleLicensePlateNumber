//#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <cmath>

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
const double TemplateTitleGapHeight = 5;
const double TemplateTitleGapWidth = 40;
const double TemplateTitleEnglishHeight = 14;
//ģ��ͼƬ��������Ŀ��߼��������ĵ�XY����
const double TemplatePlateWidth = 174;
const double TemplatePlateHeight = 41;
const double TemplatePlateCenterX = 271;
const double TemplatePlateCenterY = 166;


//�����������������в���Ϊ������ͼƬ��·��
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

	vector<String> ImagePathList;
	String SearchGlobExpression = ArgumentVector[1];
	String OutputPath = ArgumentVector[2];

	glob(SearchGlobExpression,//�ļ�����Glob���ʽ
		ImagePathList, //���ͼ���ļ��б�
		false//�����еݹ飬���������ļ��н�������
	);


	for (size_t iImage = 0; iImage < ImagePathList.size(); iImage++)
	{
		//�½�����RawImageMat���ڴ洢ԭʼͼƬ����
		Mat RawImageMat;

		//���ݵ�һ���������ļ�·������ͼƬ��ȡ
		RawImageMat = imread(
			ImagePathList[iImage],//����ͼƬ·��
			CV_LOAD_IMAGE_UNCHANGED//�Բ��޸�ͨ�����͵ķ�ʽ��ȡͼƬ
		);

		//�������Mat�Ƿ����ͼ������
		if (!RawImageMat.data)
		{
			//��ʾͼƬ��ȡʧ����ʾ��Ϣ
			cout << " Error:  Can't read image data from" << ArgumentVector[1] << endl;
			//���ش����벢�˳�����
			return -1;
		}
		//��������ͼ��Ŀ��ߡ�����Ϳ�߱�
		const double InputImageWidth = RawImageMat.cols;
		const double InputImageHeight = RawImageMat.rows;

		//����ģ��ͼƬ����Ŀ�߱Ⱥ����
		const double TemplateRatio = TemplateWidth / TemplateHeight;
		//const double TemplateArea = TemplateWidth*TemplateHeight;

		//����ģ��ͼƬ����Ŀ�߱Ⱥ����
		const double TemplateTitleRatio = TemplateTitleWidth / TemplateTitleHeight;
		//const double TemplateTitleArea = TemplateTitleWidth*TemplateTitleHeight;

		//����ģ��ĳ�������Ŀ�߱Ⱥ����
		const double TemplatePlateRatio = TemplatePlateWidth / TemplatePlateHeight;
		//const double TemplatePlateArea = TemplatePlateWidth*TemplatePlateHeight;

		//��ͼƬת���ɻҽ�ͼ��
		Mat GrayImageMat;
		//��ȡͼƬ��ͨ����
		int NumRawImageChannel = RawImageMat.channels();

		//���ͼ��Ϊ3ͨ����ɫͼ��
		if (NumRawImageChannel == 3)
		{
			//��ͼƬ��BGRת���ɻҽ�ͼ��
			cvtColor(
				RawImageMat,//����ͼƬ����
				GrayImageMat,//���ͼƬ���� 
				COLOR_BGR2GRAY//��ͼƬ��BGR��OpenCVĬ��ͨ����ʽ��ת���ɻҽ�ͼ��
			);
		}
		//���ͼ���Ѿ�Ϊ4ͨ��������alphaͨ����ͼ������ת���ɻҽ�ͼ��
		else if (NumRawImageChannel == 4)
		{
			//��ͼƬ��BGRAת���ɻҽ�ͼ��
			cvtColor(
				RawImageMat,
				GrayImageMat,
				COLOR_BGRA2GRAY//��ͼƬ��BGRAת���ɻҽ�ͼ��
			);
		}
		//���ͼ���Ѿ�Ϊ��ͨ���ҽ�ͼ��ֱ�Ӳ���ת����ResizedImageMat����GrayImageMat
		else if (NumRawImageChannel == 1)
		{
			GrayImageMat = RawImageMat;
		}

		//���ͨ������Ϊ1,3��4����������벢�˳�����
		else
		{
			cout << "Unkown image channel type: " << RawImageMat.channels();
		}


		//ָ����������
		const char* MainWindowName = "get plate number from vehicle license";
		////������Ӧ�Ĵ���
		//namedWindow(
		//	MainWindowName,//��������
		//	CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		//);
		//double WindowHeight = 500.0;
		//resizeWindow(MainWindowName, int(WindowHeight * GrayImageMat.cols / GrayImageMat.rows), int(WindowHeight));


		//��ͼ����и�˹ģ��
		//���ø�˹ģ���˳ߴ�-----------------------------------------
		int GaussianBlurKernelXSize = 2 * int(GrayImageMat.cols / 100) + 1;
		int GaussianBlurKernelYSize = 2 * int(GrayImageMat.rows / 80) + 1;
		//-----------------------------------------------------------
		//Mat BlurGrayImageMat;
		GaussianBlur(
			GrayImageMat, //����ͼƬ����
			GrayImageMat, //���ͼƬ����
						  //Size(int(GaussianBlurKernelSize), int(GaussianBlurKernelSize)), //��˹�˳ߴ�
			Size(GaussianBlurKernelXSize, GaussianBlurKernelYSize),//��˹�˳ߴ��������ֵ��������
			0, //??��˹��X�����׼�� 
			0, //??��˹��Y�����׼��
			BORDER_DEFAULT //??���ز�ֵ��ʽ 
		);

		//��������������ʾ��˹ģ�����ͼ��
		//imshow(
		//	MainWindowName,//ϣ������ͼƬ�Ĵ�������
		//	GrayImageMat//ϣ�����Ƶ�ͼƬ����
		//);

		////�ȴ�������Ӧ�������������������0��ʾ�ȴ�ʱ��Ϊ���޳�
		//waitKey(0);



		Mat GrayImageGradX(GrayImageMat.rows, GrayImageMat.cols, CV_8UC1, Scalar(0));
		for (int iRow = 1; iRow < GrayImageMat.rows - 1; iRow++)
		{
			for (int iCol = 1; iCol < (GrayImageMat.cols - 1); iCol++)
			{
				GrayImageGradX.at<uchar>(iRow, iCol) = (2 * abs(GrayImageMat.at<uchar>(iRow, iCol - 1) - GrayImageMat.at<uchar>(iRow, iCol + 1))
					+ abs(GrayImageMat.at<uchar>(iRow - 1, iCol - 1) - GrayImageMat.at<uchar>(iRow - 1, iCol + 1))
					+ abs(GrayImageMat.at<uchar>(iRow + 1, iCol - 1) - GrayImageMat.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}

		Mat BinaryGradX(GrayImageMat.rows, GrayImageMat.cols, CV_8UC1, Scalar(0));

		//�ݶȾ����ֵ��
		threshold(
			GrayImageGradX, //�������
			BinaryGradX, //�������
			0, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		//��ʾ��Ե��ֵͼ��
		//imshow(
		//	MainWindowName,
		//	BinaryGradX
		//);

		////�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		//waitKey(0);

		int DilateElementWidth = 2 * int(TemplateTitleGapWidth / TemplateWidth*BinaryGradX.cols / 2.0) + 1;


		//�������Ͳ����ĽṹԪ������һ��1��DilateElementWidth�еĽṹԪ�����ˮƽ����
		Mat DilateElement = Mat(
			1,//��һά��Y��ߴ磩
			DilateElementWidth,//�ڶ�ά��X��ߴ磩
			CV_8U,//�������ͣ�8λ�޷�������
			cvScalar(1)//������ֵ��ȫ0
		);
		//�Ա�Ե�������Ͳ���
		Mat DilatedGradX;
		dilate(
			BinaryGradX,//�������
			DilatedGradX,//�������
			DilateElement,//���ͽṹԪ
			Point(-1, -1), //����Ԫê�㣬������ȡ������Ϊê��
			1 //�������͵Ĵ���
		);
		//��ʾ���ͺ�Ķ�ֵͼ��
		//imshow(
		//	MainWindowName,
		//	DilatedGradX
		//);
		////�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		//waitKey(0);

		//����X�ݶ�ͶӰ����
		Mat  GradXProjectY(
			DilatedGradX.rows,//��������
			1,//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);

		double SumTemp;
		for (int iRow = 0; iRow < BinaryGradX.rows; iRow++)
		{
			SumTemp = 0;
			for (int iCol = 0; iCol < BinaryGradX.cols; iCol++)
			{
				SumTemp += double(DilatedGradX.at<uchar>(iRow, iCol));
			}
			GradXProjectY.at<uchar>(iRow, 0) = unsigned char(SumTemp / BinaryGradX.cols);
		}


		Mat BinaryGradXProjectY;

		threshold(
			GradXProjectY, //�������
			BinaryGradXProjectY, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		unsigned char StemColor;
		Mat GradXProjectYStem(
			GrayImageGradX.rows,//��������
			GrayImageGradX.cols,//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);
		for (int iRow = 0; iRow < GrayImageGradX.rows; iRow++)
		{
			if (BinaryGradXProjectY.at<uchar>(iRow, 0) == 0)
			{
				StemColor = 100;
			}
			else
			{
				StemColor = 255;
			}

			for (int iCol = 0; iCol < GradXProjectY.at<uchar>(iRow, 0) / 255.0*GrayImageGradX.cols; iCol++)
			{
				GradXProjectYStem.at<uchar>(iRow, iCol) = StemColor;
			}

		}
		//��ʾ��Ե��ֵͼ��
		//namedWindow(
		//	"GradXProjectYStem",//��������
		//	CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		//);
		//resizeWindow("GradXProjectYStem", int(WindowHeight * GrayImageMat.cols / GrayImageMat.rows), int(WindowHeight));
		//imshow(
		//	"GradXProjectYStem",
		//	GradXProjectYStem
		//);


		////�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		//waitKey(0);


		double LicenseProportionXMin = 0.6;
		double LicenseProportionXMax = 1.1;
		double LicenseProportionYMin = 0.6;
		double LicenseProportionYMax = 1.1;

		double TitleMinHeight = DilatedGradX.rows*LicenseProportionYMin*(TemplateTitleHeight / TemplateHeight);
		double TitleMaxHeight = DilatedGradX.rows*LicenseProportionYMax*(TemplateTitleHeight / TemplateHeight);
		double TitleMaxGapHeight = DilatedGradX.rows*LicenseProportionYMax *(TemplateTitleGapHeight / TemplateHeight);

		double TitleMinEnglishHeight = DilatedGradX.rows*LicenseProportionYMin *(TemplateTitleEnglishHeight / TemplateHeight);
		double TitleMaxEnglishHeight = DilatedGradX.rows*LicenseProportionYMax *(TemplateTitleEnglishHeight / TemplateHeight);

		double TitleMaxGapWidth = DilatedGradX.cols*LicenseProportionXMax *(TemplateTitleGapWidth / TemplateWidth);
		double TitleMinWidth = DilatedGradX.cols*LicenseProportionXMin*(TemplateTitleWidth / TemplateWidth);
		double TitleMaxWidth = DilatedGradX.cols*LicenseProportionXMax*(TemplateTitleWidth / TemplateWidth);

		vector<int> RisingRow, FallingRow;
		vector<int> TitleRisingRow, TitleFallingRow, IndexTitleRisingRow, IndexTitleFallingRow;
		BinaryGradXProjectY.at<uchar>(0, 0) = 0;
		BinaryGradXProjectY.at<uchar>(BinaryGradXProjectY.rows - 1, 0) = 0;
		for (int iRow = 0; iRow < BinaryGradXProjectY.rows; iRow++)
		{
			if (iRow < BinaryGradXProjectY.rows - 2 && BinaryGradXProjectY.at<uchar>(iRow, 0) == 0 && BinaryGradXProjectY.at<uchar>(iRow + 1, 0) == 255)
			{
				RisingRow.push_back(iRow);
			}
			else if (iRow > 0 && iRow < BinaryGradXProjectY.rows - 1 && BinaryGradXProjectY.at<uchar>(iRow, 0) == 255 && BinaryGradXProjectY.at<uchar>(iRow + 1, 0) == 0)
			{
				FallingRow.push_back(iRow);
			}
		}
		if (RisingRow.size() != FallingRow.size())
		{
			cout << "Erorr: Unmatched sizes between RisingRow and FallingRow.";
			return -1;
		}

		for (int iConvexY = 1; iConvexY <= RisingRow.size(); iConvexY++)
		{
			TitleRisingRow.push_back(RisingRow[iConvexY - 1]);
			IndexTitleRisingRow.push_back(iConvexY - 1);
			while (iConvexY < RisingRow.size() && RisingRow[iConvexY] - FallingRow[iConvexY - 1] <= TitleMaxGapHeight &&
				FallingRow[iConvexY] - RisingRow[iConvexY] <= TitleMinEnglishHeight)
			{
				iConvexY++;
			}
			if (FallingRow[iConvexY - 1] - TitleRisingRow.back() >= TitleMinHeight &&
				FallingRow[iConvexY - 1] - TitleRisingRow.back() <= TitleMaxHeight &&
				TitleRisingRow.back() < DilatedGradX.rows/2)
			{
				TitleFallingRow.push_back(FallingRow[iConvexY - 1]);
				IndexTitleFallingRow.push_back(iConvexY - 1);
				break;
			}
			else
			{
				TitleRisingRow.pop_back();
				IndexTitleRisingRow.pop_back();
			}

		}

		Mat GradXProjectXStem(
			DilatedGradX.rows,//��������
			DilatedGradX.cols,//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);

		for (int iConvexY = 0; iConvexY < TitleRisingRow.size(); iConvexY++)
		{
			Rect ProjectYPassRect;
			ProjectYPassRect.x = 0;
			ProjectYPassRect.y = TitleRisingRow[iConvexY];
			ProjectYPassRect.width = DilatedGradX.cols;
			ProjectYPassRect.height = TitleFallingRow[iConvexY] - TitleRisingRow[iConvexY];
			//rectangle(GradXProjectYStem,//���ƾ��εĶ���
			//	ProjectYPassRect, //Ҫ���Ƶľ���
			//	Scalar(255), //��������ɫ
			//	3,//�������
			//	LINE_AA,//���ͣ�������� 
			//	0 //??������ֵ�������ƣ���С��λ��
			//);


			//����X�ݶ�X����ͶӰ����
			Mat  GradXProjectX(
				1,//��������
				DilatedGradX.cols,//��������
				CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
				Scalar(0)//��������ĳ�ʼֵ
			);
			Mat BinaryGradXProjectX;


			for (int iCol = 0; iCol < DilatedGradX.cols; iCol++)
			{
				SumTemp = 0;
				for (int iRow = TitleRisingRow[iConvexY]; iRow <= TitleFallingRow[iConvexY]; iRow++)
				{
					SumTemp += double(DilatedGradX.at<uchar>(iRow, iCol));
				}
				GradXProjectX.at<uchar>(0, iCol) = unsigned char(SumTemp / (FallingRow[iConvexY] - RisingRow[iConvexY]));
			}


			threshold(
				GradXProjectX, //�������
				BinaryGradXProjectX, //�������
				128, //������ʼ��ֵ
				255, //���ֵ��������ֵ����Ϊ��ֵ��
				CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
			);



			for (int iCol = 0; iCol < DilatedGradX.cols; iCol++)
			{
				if (BinaryGradXProjectX.at<uchar>(0, iCol) == 0)
				{
					StemColor = 100;
				}
				else
				{
					StemColor = 255;
				}

				for (int iRow = 0; iRow < GradXProjectX.at<uchar>(0, iCol) / 255.0*DilatedGradX.rows; iRow++)
				{
					GradXProjectXStem.at<uchar>(iRow, iCol) = StemColor;
				}

			}
			//��ʾ��Ե��ֵͼ��
			//namedWindow(
			//	"GradXProjectXStem",//��������
			//	CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
			//);
			//resizeWindow("GradXProjectXStem", int(WindowHeight * GrayImageMat.cols / GrayImageMat.rows), int(WindowHeight));
			//imshow(
			//	"GradXProjectXStem",
			//	GradXProjectXStem
			//);


			////�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
			//waitKey(0);



			vector<int> RisingCol, FallingCol;
			BinaryGradXProjectX.at<uchar>(0, 0) = 0;
			BinaryGradXProjectX.at<uchar>(0, DilatedGradX.cols - 1) = 0;
			for (int iCol = 0; iCol < BinaryGradXProjectX.cols; iCol++)
			{
				if (iCol < BinaryGradXProjectX.cols - 2 && BinaryGradXProjectX.at<uchar>(0, iCol) == 0 && BinaryGradXProjectX.at<uchar>(0, iCol + 1) == 255)
				{
					RisingCol.push_back(iCol);
				}
				else if (iCol < BinaryGradXProjectX.cols - 1 && BinaryGradXProjectX.at<uchar>(0, iCol) == 255 && BinaryGradXProjectX.at<uchar>(0, iCol + 1) == 0)
				{
					if (RisingCol.size() - FallingCol.size() == 1)
					{

						FallingCol.push_back(iCol);
					}
					else
					{
						RisingCol.pop_back();
					}
				}
			}

			if (RisingCol.size() != FallingCol.size())
			{
				cout << "Erorr: Unmatched sizes between RisingCol and FallingCol.";
				return -1;
			}
			double EstimatedTitleWidth = (TitleFallingRow[iConvexY] - TitleRisingRow[iConvexY]) * TemplateTitleRatio;
			vector<int> IndexConvexXCoupleRising, IndexConvexXCoupleFalling;
			vector<double> ConvexXCoupleError;
			double TempError;
			for (int iConvexXCoupleRising = 0; iConvexXCoupleRising < RisingCol.size(); iConvexXCoupleRising++)
			{
				for (int iConvexXCoupleFalling = iConvexXCoupleRising; iConvexXCoupleFalling < FallingCol.size(); iConvexXCoupleFalling++)
				{
					IndexConvexXCoupleRising.push_back(RisingCol[iConvexXCoupleRising]);
					IndexConvexXCoupleFalling.push_back(FallingCol[iConvexXCoupleFalling]);
					TempError = (FallingCol[iConvexXCoupleFalling] - RisingCol[iConvexXCoupleRising] - EstimatedTitleWidth) *
						(FallingCol[iConvexXCoupleFalling] - RisingCol[iConvexXCoupleRising] - EstimatedTitleWidth) +
						((FallingCol[iConvexXCoupleFalling] + RisingCol[iConvexXCoupleRising]) / 2.0 - DilatedGradX.cols / 2.0) *
						((FallingCol[iConvexXCoupleFalling] + RisingCol[iConvexXCoupleRising]) / 2.0 - DilatedGradX.cols / 2.0);
					ConvexXCoupleError.push_back(TempError);
				}
			}


			bool FlagFoundTitle = false;
			if (ConvexXCoupleError.size() > 0)
			{
				FlagFoundTitle = true;

				vector<double>::iterator itMinTitleSizeError = min_element(ConvexXCoupleError.begin(), ConvexXCoupleError.end());
				int64 IndexMinTitleSizeError = distance(ConvexXCoupleError.begin(), itMinTitleSizeError);


				Rect MinErrorTitleRect;

				MinErrorTitleRect.x = IndexConvexXCoupleRising[IndexMinTitleSizeError];
				MinErrorTitleRect.y = TitleRisingRow[iConvexY];
				MinErrorTitleRect.width = IndexConvexXCoupleFalling[IndexMinTitleSizeError] - MinErrorTitleRect.x;
				MinErrorTitleRect.height = TitleFallingRow[iConvexY] - MinErrorTitleRect.y;

				rectangle(RawImageMat,//���ƾ��εĶ���
					MinErrorTitleRect, //Ҫ���Ƶľ���
					Scalar(255, 0, 0), //��������ɫ
					3,//�������
					LINE_AA,//���ͣ�������� 
					0 //??������ֵ�������ƣ���С��λ��
				);
				//imshow(
				//	MainWindowName,
				//	RawImageMat
				//);
				//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
				//waitKey(0);
			}
			
		}
		size_t SepPos = ImagePathList[iImage].rfind('\\');//rfind �������
		string FolderPath = ImagePathList[iImage].substr(0, SepPos);
		string ImageFileName = ImagePathList[iImage].substr(SepPos + 1, -1);
		string OutputImagePath = OutputPath + "\\Result_" + ImageFileName;
		imwrite(OutputImagePath, RawImageMat);
	}
	//����0�������˳�����
	return 0;
}