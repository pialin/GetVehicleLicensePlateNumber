//���OpenCV���ͷ�ļ�
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//��ӱ�׼�����������ͷ�ļ�
#include <iostream>
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

		//��ͼ����и�˹ģ��
		//���ø�˹ģ���˳ߴ�(����Ϊ������)
		int GaussianBlurKernelXSize = 2 * int(InputImageWidth / 100.0) + 1;
		int GaussianBlurKernelYSize = 2 * int(InputImageHeight / 80.0) + 1;

		Mat Gray_BlurImage;
		GaussianBlur(
			Raw_GrayImage, //����ͼƬ����
			Gray_BlurImage, //���ͼƬ����
			Size(GaussianBlurKernelXSize, GaussianBlurKernelYSize),//��˹�˳ߴ��������ֵ��������
			0, //??��˹��X�����׼�� 
			0, //??��˹��Y�����׼��
			BORDER_DEFAULT //??���ز�ֵ��ʽ 
		);

		//��������������ʾ��˹ģ�����ͼ��
		imshow(
			MainWindowName,//ϣ������ͼƬ�Ĵ�������
			Gray_BlurImage//ϣ�����Ƶ�ͼƬ����
		);

		//�ȴ�������Ӧ�������������������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);


		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat BlurImage_GradX(
			int(InputImageHeight),//����ĵ�һά���߶ȣ��ߴ�
			int(InputImageWidth), //����ĵڶ�ά����ȣ��ߴ�
			CV_8UC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);
		//������ؼ���ˮƽ�ݶȣ��������ұ�Ե�������㣬��Ϊ��ֵΪ���ĳ�ʼֵ0
		for (int iRow = 1; iRow < InputImageHeight - 1; iRow++)
		{
			for (int iCol = 1; iCol < (InputImageWidth - 1); iCol++)
			{
				//ÿһ���ص��ˮƽ�ݶȵ���0.25��abs(����-���ϣ�+0.25��������-���£�+0.5������-�ң���ģ��Sobel���Ӽ��㷽��
				BlurImage_GradX.at<uchar>(iRow, iCol) = (2 * abs(Gray_BlurImage.at<uchar>(iRow, iCol - 1) - Gray_BlurImage.at<uchar>(iRow, iCol + 1))
					+ abs(Gray_BlurImage.at<uchar>(iRow - 1, iCol - 1) - Gray_BlurImage.at<uchar>(iRow - 1, iCol + 1))
					+ abs(Gray_BlurImage.at<uchar>(iRow + 1, iCol - 1) - Gray_BlurImage.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}

		Mat BinaryGradX(
			int(InputImageHeight),//����ĵ�һά���߶ȣ��ߴ�
			int(InputImageWidth),//����ĵڶ�ά����ȣ��ߴ�
			CV_8UC1, //�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);

		//�ݶȾ����ֵ��
		threshold(
			BlurImage_GradX, //�������
			BinaryGradX, //�������
			0, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		//��ʾ��Ե��ֵͼ��
		imshow(
			MainWindowName,
			BinaryGradX
		);

		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);

		//����ˮƽ���ͽṹԪ�Ŀ��
		int DilateXElementWidth = 2 * int(TemplateTitleGapWidth / TemplateWidth*BinaryGradX.cols / 2.0) + 1;


		//�������Ͳ����ĽṹԪ������һ��1��DilateXElementWidth�еĽṹԪ�����ˮƽ����
		Mat DilateXElement = getStructuringElement(
			MORPH_RECT,//�ṹԪ��״���ͣ�����
			Size(DilateXElementWidth, 1),//�ṹԪ�ߴ�:1��DilateXElementWidth��
			Point(-1, -1)//ê��λ�ã���-1��-1��Ĭ��ê��λ������
		);
		////�Ա�Ե�������Ͳ���
		//Mat Binary_DilateXGradX;
		//dilate(
		//	BinaryGradX,//�������
		//	Binary_DilateXGradX,//�������
		//	DilateXElement,//���ͽṹԪ
		//	Point(-1, -1), //����Ԫê�㣬������ȡ������Ϊê��
		//	1 //�������͵Ĵ���
		//);
		Mat Binary_DilateXGradX = BinaryGradX;

		//��ʾ���ͺ�Ķ�ֵͼ��
		imshow(
			MainWindowName,
			Binary_DilateXGradX
		);
		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);


		//����X�ݶ�ͶӰ����
		Mat  DilateX_ProjectYGradX(
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
				SumTemp += double(Binary_DilateXGradX.at<uchar>(iRow, iCol));
			}
			//�����ֵ�ľ�ֵ��ΪˮƽͶӰ����ݶ�ֵ
			DilateX_ProjectYGradX.at<uchar>(iRow, 0) = unsigned char(SumTemp / Binary_DilateXGradX.cols);
		}


		Mat ProjectY_BinaryGradX;

		threshold(
			DilateX_ProjectYGradX, //�������
			ProjectY_BinaryGradX, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);


		Mat ProjectYGradX_StemFigure(
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
			//���ݶ�ֵ���������Stemͼ������ɫ����Ϊ0��ʹ�û�ɫ�������ͶӰֵС����ֵ
			if (ProjectY_BinaryGradX.at<uchar>(iRow, 0) == 0)
			{
				StemColor = 100;
			}
			//��Ϊ255�����ð�ɫ�������ͶӰֵ������ֵ
			else
			{
				StemColor = 255;
			}
			//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ��ȵ�Stem
			for (int iCol = 0; iCol < DilateX_ProjectYGradX.at<uchar>(iRow, 0) / 255.0*InputImageWidth; iCol++)
			{
				ProjectYGradX_StemFigure.at<uchar>(iRow, iCol) = StemColor;
			}

		}


		//��ʾ��Ե��ֵͼ��
		namedWindow(
			"ProjectYGradX_StemFigure",//��������
			CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		);
		//�����ڱ�Ϊ�߶�ΪWindowHeight����߱Ȳ���
		resizeWindow("ProjectYGradX_StemFigure", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));
		imshow(
			"ProjectYGradX_StemFigure",
			ProjectYGradX_StemFigure
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

		//��ͶӰ��ֵ�������ĵ�һ�к����һ����0�����Ǳ���ģ�ֻ��Ϊ�˱��գ�
		ProjectY_BinaryGradX.at<uchar>(0, 0) = 0;
		ProjectY_BinaryGradX.at<uchar>(int(InputImageHeight - 1), 0) = 0;
		//���м�������غ��½���
		for (int iRow = 0; iRow < InputImageHeight; iRow++)
		{
			//������ֻ�����ڵ�һ�е�����������֮����֣������ض�Ӧ������ֵΪ0����һ����ֵΪ255��
			if (iRow < InputImageHeight - 2 && ProjectY_BinaryGradX.at<uchar>(iRow, 0) == 0 && ProjectY_BinaryGradX.at<uchar>(iRow + 1, 0) == 255)
			{
				//������������������������������������Ԫ��
				RisingRow.push_back(iRow);
			}
			//�½���ֻ�����ڵڶ��е������ڶ���֮����֣��½��ض�Ӧ������ֵΪ255����һ����ֵΪ0��
			else if (iRow > 0 && iRow < InputImageHeight - 1 && ProjectY_BinaryGradX.at<uchar>(iRow, 0) == 255 && ProjectY_BinaryGradX.at<uchar>(iRow + 1, 0) == 0)
			{
				//��������������������½��������������Ԫ��
				FallingRow.push_back(iRow);
			}
		}
		//�ж������غ��½��ظ����Ƿ�ƥ�䣨��Ϊ��һ�к����һ�о�Ϊ0���������߸���Ӧ������ȵģ�����ƥ�����׳�������Ϣ����������
		if (RisingRow.size() != FallingRow.size())
		{
			cout << "Erorr: Unmatched sizes between RisingRow and FallingRow.";
			return -1;
		}
		//��ÿ������-�½��ԣ�Convex�����д�����ȡ���ⲿ�ֿ��ܶ�Ӧ��Convex
		for (int iConvexY = 1; iConvexY <= RisingRow.size(); iConvexY++)
		{
			//�����ʼ���������������Ӧ������
			TitleRisingRow.push_back(RisingRow[iConvexY - 1]);
			IndexTitleRisingRow.push_back(iConvexY - 1);
			//������ָ߶�С������϶�߶���С�ڱ�������Ӣ���и߶ȵĿ�϶������ںϣ�ȥ��С�Ŀ�϶��
			while (iConvexY < RisingRow.size() && RisingRow[iConvexY] - FallingRow[iConvexY - 1] <= TitleMaxGapHeight &&
				FallingRow[iConvexY] - RisingRow[iConvexY] <= TitleMinEnglishHeight)
			{
				iConvexY++;
			}
			//���Convex�ĸ߶��ڱ���߶����Ʒ�Χ֮�ڣ���������λ��ͼƬ�ϰ벿�֣�������Ϊ���ⲿ�ֿ��ܶ�Ӧ��Convex
			if (FallingRow[iConvexY - 1] - TitleRisingRow.back() >= TitleMinHeight &&
				FallingRow[iConvexY - 1] - TitleRisingRow.back() <= TitleMaxHeight &&
				TitleRisingRow.back() < InputImageHeight / 2)
			{
				//Ϊ�������������½���ʸ�������Ԫ��
				TitleFallingRow.push_back(FallingRow[iConvexY - 1]);
				IndexTitleFallingRow.push_back(iConvexY - 1);
				//TODO��Ŀǰֻ�����һ���ҵ��ģ���ߵģ����ⲿ�ֺ�ѡConvex��������Ҫ�Ľ���
				break;
			}
			else
			{
				//����ɾ��ѭ����ͷ�������������Ϣ
				TitleRisingRow.pop_back();
				IndexTitleRisingRow.pop_back();
			}

		}


		//��ÿ����ѡ�ı���Convex���б���
		for (int iConvexY = 0; iConvexY < TitleRisingRow.size(); iConvexY++)
		{
			//�������Convex��ͼ�϶�Ӧ�ľ�������
			Rect ProjectYPassRect;
			ProjectYPassRect.x = 0;
			ProjectYPassRect.y = TitleRisingRow[iConvexY];
			ProjectYPassRect.width = int(InputImageWidth);
			ProjectYPassRect.height = TitleFallingRow[iConvexY] - TitleRisingRow[iConvexY];

			//��Stemͼ�ϻ�����������
			rectangle(
				ProjectYGradX_StemFigure,//���ƾ��εĶ���
				ProjectYPassRect, //Ҫ���Ƶľ���
				Scalar(255), //��������ɫ:��ɫ
				int(InputImageHeight / 200.0),//�������
				LINE_AA,//���ͣ�������� 
				0 //??������ֵ�������ƣ���С��λ��
			);

			Mat Gray_TitleRowImage = Raw_GrayImage(ProjectYPassRect);
			Mat TitleRowImage_GradY(
				Gray_TitleRowImage.rows,
				Gray_TitleRowImage.cols,
				CV_8UC1,
				Scalar(0)
				);
			for (int iRow = 1; iRow < Gray_TitleRowImage.rows-1; iRow++)
			{
				for (int iCol = 1; iCol < Gray_TitleRowImage.cols-1; iCol++)
				{
					//ÿһ���ص��ˮƽ�ݶȵ���0.25��abs(����-���ϣ�+0.25��������-���£�+0.5������-�ң���ģ��Sobel���Ӽ��㷽��
					TitleRowImage_GradY.at<uchar>(iRow, iCol) = (2 * abs(Gray_TitleRowImage.at<uchar>(iRow - 1, iCol - 1) - Gray_TitleRowImage.at<uchar>(iRow + 1, iCol - 1))
						+ abs(Gray_TitleRowImage.at<uchar>(iRow - 1, iCol) - Gray_TitleRowImage.at<uchar>(iRow + 1, iCol))
						+ abs(Gray_TitleRowImage.at<uchar>(iRow - 1, iCol + 1) - Gray_TitleRowImage.at<uchar>(iRow + 1, iCol + 1))) / 4;
				}
			}

			


			//�����洢X�ݶ�X����ͶӰ�ľ���
 			Mat DilateX_ProjectXGradX(
				1,//��������
				int(InputImageWidth),//��������
				CV_8UC1,//����ֵ�����ͣ���ͨ��8λ�޷���������
				Scalar(0)//��������ĳ�ʼֵ
			);



			for (int iCol = 1; iCol < InputImageWidth - 1; iCol++)
			{
				//ÿ��ѭ��ǰ���Ӻͱ�������
				SumTemp = 0;
				//�ۼӱ��ⲿ�ֺ�ѡConvex��Ӧ���е�����
				for (int iRow = TitleRisingRow[iConvexY]; iRow <= TitleFallingRow[iConvexY]; iRow++)
				{
					SumTemp += double(Binary_DilateXGradX.at<uchar>(iRow, iCol));
				}
				//���ۼ�ֵ��ƽ��ֵ��Ϊ�ݶȴ�ֱͶӰ���ݶ�ֵ
				DilateX_ProjectXGradX.at<uchar>(0, iCol) = unsigned char(SumTemp / (FallingRow[iConvexY] - RisingRow[iConvexY]));
			}

			//�����洢X�ݶ�X����ͶӰ��ֵ������ľ���
			Mat ProjectX_BinaryGradX;
			//�Դ�ֱͶӰ����ݶ�ֵ����Otsu��ֵ��
			threshold(
				DilateX_ProjectXGradX, //�������
				ProjectX_BinaryGradX, //�������
				128, //������ʼ��ֵ
				255, //���ֵ��������ֵ����Ϊ��ֵ��
				CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
			);


			//�������ڴ洢���ⲿ�ֶ�Ӧ�д�ֱͶӰ��Stemͼ�ľ���
			Mat ProjectXGradX_StemFigure(
				int(InputImageHeight),//��������
				int(InputImageWidth),//��������
				CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
				Scalar(0)//��������ĳ�ʼֵ
			);

			//���ƴ�ֱͶӰ�ݶȵ�Stemͼ
			for (int iCol = 0; iCol < InputImageWidth; iCol++)
			{
				//���ݴ�ֱͶӰ���ֵ�����ݶ�ֵѡ��Stem�ߵ���ɫ
				if (ProjectX_BinaryGradX.at<uchar>(0, iCol) == 0)
				{
					//�����ֵ���Ľ��Ϊ0����Stem�ߵ���ɫ����Ϊ��ɫ�����������ֵ
					StemColor = 100;
				}
				else
				{
					//�����ֵ���Ľ��Ϊ255����Stem�ߵ���ɫ����Ϊ��ɫ�����������ֵ
					StemColor = 255;
				}
				//����ͶӰ����ݶ�ֵ����Stemͼ��ÿһ�и���Stemֵ��С���Ʋ�ͬ�߶ȵ�Stem�������ں�ѡConvex���в�������Ĭ��Ϊ0����ɫ��
				for (int iRow = 0; iRow < DilateX_ProjectXGradX.at<uchar>(0, iCol) / 255.0*InputImageHeight; iRow++)
				{
					ProjectXGradX_StemFigure.at<uchar>(iRow, iCol) = StemColor;
				}

			}

			//�½���ͼ�������ڻ��ƴ�ֱͶӰ�ݶ�Stemͼ
			namedWindow(
				"ProjectXGradX_StemFigure",//��������
				CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
			);
			//���ô��ڳߴ�ʹ��߶�ΪWindowHeight����߱�������ͼ��һ��
			resizeWindow("ProjectXGradX_StemFigure", int(WindowHeight * InputImageWidth / InputImageHeight), int(WindowHeight));
			imshow(
				"ProjectXGradX_StemFigure",
				ProjectXGradX_StemFigure
			);
			//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
			waitKey(0);


			//�������ڴ洢���ⲿ��ˮƽ���������غ��½������������ݵ�����
			vector<int> TitleRisingCol, TitleFallingCol;

			//����ֵ���Ĵ�ֱͶӰ�ĵ�һ�к����һ����0���Ǳ��룬Ϊ�˱�֤�����غ��½���ֻ���ܽ�����֣����ҿ�ʼΪ�����ؽ���Ϊ�½��أ�
			ProjectX_BinaryGradX.at<uchar>(0, 0) = 0;
			ProjectX_BinaryGradX.at<uchar>(0, int(InputImageWidth - 1)) = 0;

			//����ÿһ���ҳ������غ��½���������
			for (int iCol = 0; iCol < InputImageWidth; iCol++)
			{
				//������ֻ�����ڵ�һ�е�����������֮����֣������ض�Ӧ������ֵΪ0����һ����ֵΪ255��
				if (iCol < InputImageWidth - 2 && ProjectX_BinaryGradX.at<uchar>(0, iCol) == 0 && ProjectX_BinaryGradX.at<uchar>(0, iCol + 1) == 255)
				{
					TitleRisingCol.push_back(iCol);
				}
				//�½���ֻ�����ڵڶ��е������ڶ���֮����֣��½��ض�Ӧ������ֵΪ255����һ����ֵΪ0��
				else if (iCol < InputImageWidth - 1 && ProjectX_BinaryGradX.at<uchar>(0, iCol) == 255 && ProjectX_BinaryGradX.at<uchar>(0, iCol + 1) == 0)
				{
					TitleFallingCol.push_back(iCol);
				}
			}
			//�ж������غ��½��ظ����Ƿ���ȣ��粻����׳�������Ϣ����ֹ����
			if (TitleRisingCol.size() != TitleFallingCol.size())
			{
				cout << "Erorr: Unmatched sizes between RisingCol and FallingCol.";
				return -1;
			}
			//���ݱ����д�ֱͶӰConvex��Ϊ�Ǳ����еĸ߶ȣ�����ģ����ⲿ�ֵĿ�߱ȹ��Ʊ��ⲿ�ֵĿ��
			double EstimatedTitleWidth = (TitleFallingRow[iConvexY] - TitleRisingRow[iConvexY]) * TemplateTitleRatio;

			//�������������غ��½��ص���ϣ��ҳ����ʺ���Ϊ���������ұ߽��һ��
			//����ʸ���洢���������½�����϶Ե�����
			vector<int> IndexTitleRisingColCouple, IndexTitleFallingColCouple;
			//����ʸ���洢������϶Ե����
			vector<double> RisingFallingColCoupleError;
			//���ڴ洢��ʱ�������
			double TempError;
			//������������-�½���������
			for (int iTitleRisingCol = 0; iTitleRisingCol < TitleRisingCol.size(); iTitleRisingCol++)
			{

				for (int iTitleFallingCol = iTitleRisingCol; iTitleFallingCol < TitleFallingCol.size(); iTitleFallingCol++)
				{
					//���������½��ض�Ӧ����������IndexTitleRisingColCouple��IndexTitleFallingColCouple������
					IndexTitleRisingColCouple.push_back(TitleRisingCol[iTitleRisingCol]);
					IndexTitleFallingColCouple.push_back(TitleFallingCol[iTitleFallingCol]);
					//������Ԥ�������п�ȵĲ�ֵ�����ĺ�������ͼƬ���ĺ������ֵ��ƽ���ͣ�
					TempError = (TitleFallingCol[iTitleFallingCol] - TitleRisingCol[iTitleRisingCol] - EstimatedTitleWidth) *
						(TitleFallingCol[iTitleFallingCol] - TitleRisingCol[iTitleRisingCol] - EstimatedTitleWidth) +
						((TitleFallingCol[iTitleFallingCol] + TitleRisingCol[iTitleRisingCol]) / 2.0 - InputImageWidth / 2.0) *
						((TitleFallingCol[iTitleFallingCol] + TitleRisingCol[iTitleRisingCol]) / 2.0 - InputImageWidth / 2.0);
					//��������RisingFallingColCoupleError������
					RisingFallingColCoupleError.push_back(TempError);
				}
			}

			//��־�Ƿ��ҵ����������Flag����
			bool FlagFoundTitle = false;
			//���������һ����Ҫ���������½������������ҵ��˱�������
			if (RisingFallingColCoupleError.size() > 0)
			{
				//����־������true
				FlagFoundTitle = true;

				//������С����Ӧ������-�½��ض�
				vector<double>::iterator itMinTitleSizeError = min_element(RisingFallingColCoupleError.begin(), RisingFallingColCoupleError.end());

				int64 IndexMinTitleSizeError = distance(RisingFallingColCoupleError.begin(), itMinTitleSizeError);

				//�����Ӧ����С����������
				Rect MinErrorTitleRect;

				MinErrorTitleRect.x = IndexTitleRisingColCouple[IndexMinTitleSizeError];
				MinErrorTitleRect.y = TitleRisingRow[iConvexY];
				MinErrorTitleRect.width = IndexTitleFallingColCouple[IndexMinTitleSizeError] - MinErrorTitleRect.x;
				MinErrorTitleRect.height = TitleFallingRow[iConvexY] - MinErrorTitleRect.y;
				//��ԭʼͼ���ϻ�����С����������ľ��ο�
				rectangle(RawImage,//���ƾ��εĶ���
					MinErrorTitleRect, //Ҫ���Ƶľ���
					Scalar(255, 0, 0), //��������ɫ����ɫ
					int(InputImageHeight / 200.0),//�������
					LINE_AA,//���ͣ�������� 
					0 //??������ֵ�������ƣ���С��λ��
				);

				//��ʾ�������������ԭʼͼ��
				imshow(
					MainWindowName,
					RawImage
				);
				//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
				waitKey(0);
			}

		}
		//Ѱ���ļ�·�����һ����\��
		size_t SepPos = ImagePathList[iImage].rfind('\\');//rfind �������
														  //��ȡ�ļ���·��
		string FolderPath = ImagePathList[iImage].substr(0, SepPos);
		//��ȡͼƬ�ļ���
		string ImageFileName = ImagePathList[iImage].substr(SepPos + 1, -1);
		//��ȡ���ͼƬ����·�����ļ���Ϊ����ͼ������ǰ����ϡ�Result_����
		string OutputImagePath = OutputPath + "\\Result_" + ImageFileName;
		//����ͼ�񱣴�
		imwrite(OutputImagePath, RawImage);
	}
	//����0�������˳�����
	return 0;
}