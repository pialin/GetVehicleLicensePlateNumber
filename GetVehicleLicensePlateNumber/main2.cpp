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
const double TemplateWidth = 550;
const double TemplateHeight = 371;
//ģ��ͼƬ������������Ŀ��߼��������ĵ�XY����
const double TemplateTitleWidth = 340;
const double TemplateTitleHeight = 36;
const double TemplateTitleCenterX = 270;
const double TemplateTitleCenterY = 39;
//ģ��ͼƬ��������Ŀ��߼��������ĵ�XY����
const double TemplatePlateWidth = 84;
const double TemplatePlateHeight = 20;
const double TemplatePlateCenterX = 138;
const double TemplatePlateCenterY = 81;


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


	for (size_t iImage = 0; iImage<ImagePathList.size(); iImage++)
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
		const float TemplateTitleRatio = TemplateTitleWidth / TemplateTitleHeight;
		const float TemplateTitleArea = TemplateTitleWidth*TemplateTitleHeight;

		//����ģ��ĳ�������Ŀ�߱Ⱥ����
		const float TemplatePlateRatio = TemplatePlateWidth / TemplatePlateHeight;
		const float TemplatePlateArea = TemplatePlateWidth*TemplatePlateHeight;

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
		const char* WindowName = "get plate number from vehicle license";
		//������Ӧ�Ĵ���
		namedWindow(
			WindowName,//��������
			CV_WINDOW_NORMAL//����һ������ͼƬ�Զ����ô�С�Ĵ��ڣ��������ֶ��޸ĳߴ�
		);
		double WindowWidth = 600.0;
		resizeWindow(WindowName, int(GrayImageMat.cols / GrayImageMat.rows*WindowWidth), WindowWidth);
		//��������������ʾ�ҽ�ͼ��
		imshow(
			WindowName,//ϣ������ͼƬ�Ĵ�������
			GrayImageMat//ϣ�����Ƶ�ͼƬ����
		);

		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);


		////��ͼ����и�˹ģ��
		////���ø�˹ģ���˳ߴ�-----------------------------------------
		//double GaussianBlurKernelSize = 11;
		////-----------------------------------------------------------
		//Mat BlurGrayImageMat;
		//GaussianBlur(
		//	GrayImageMat, //����ͼƬ����
		//	BlurGrayImageMat, //���ͼƬ����
		//					  //Size(int(GaussianBlurKernelSize), int(GaussianBlurKernelSize)), //��˹�˳ߴ�
		//	Size(5, 5),//��˹�˳ߴ��������ֵ��������
		//	0, //??��˹��X�����׼�� 
		//	0, //??��˹��Y�����׼��
		//	BORDER_DEFAULT //??���ز�ֵ��ʽ 
		//);

		////��������������ʾ��˹ģ�����ͼ��
		//imshow(
		//	WindowName,//ϣ������ͼƬ�Ĵ�������
		//	BlurGrayImageMat//ϣ�����Ƶ�ͼƬ����
		//);

		//�ȴ�������Ӧ�������������������0��ʾ�ȴ�ʱ��Ϊ���޳�
		//waitKey(0);



		Mat GrayImageGradX(GrayImageMat.rows, GrayImageMat.cols, CV_8UC1, Scalar(0));
		for (size_t iRow = 1; iRow<GrayImageMat.rows-1; iRow++)
		{
			for (size_t iCol = 1; iCol<(GrayImageMat.cols - 1); iCol++)
			{
				GrayImageGradX.at<uchar>(iRow, iCol) = (2 * abs(GrayImageMat.at<uchar>(iRow, iCol - 1) - GrayImageMat.at<uchar>(iRow, iCol + 1))
					+ abs(GrayImageMat.at<uchar>(iRow - 1, iCol - 1) - GrayImageMat.at<uchar>(iRow - 1, iCol + 1))
					+ abs(GrayImageMat.at<uchar>(iRow + 1, iCol - 1) - GrayImageMat.at<uchar>(iRow + 1, iCol + 1)))/4;
			}
		}


		//�ݶȾ����ֵ��
		threshold(
			GrayImageGradX, //�������
			GrayImageGradX, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		//��ʾ��Ե��ֵͼ��
		imshow(
			WindowName,
			GrayImageGradX
		);

		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);

		//����X�ݶ�ͶӰ����
		Mat  GradXProject(
			GrayImageGradX.rows,//��������
			1,//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);

		double SumTemp;
		for (size_t iRow = 0; iRow<GradXProject.rows; iRow++)
		{
			SumTemp = 0;
			for (size_t iCol = 0; iCol< GrayImageGradX.cols; iCol++)
			{
				SumTemp += double(GrayImageGradX.at<uchar>(iRow , iCol));
			}
			GradXProject.at<uchar>(iRow, 0) = unsigned char(SumTemp / GrayImageGradX.cols);
		}

		threshold(
			GradXProject, //�������
			GradXProject, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);


		//��ʾ��Ե��ֵͼ��
		imshow(
			WindowName,
			GradXProject
		);

		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);

		size_t iRow = 0;
		vector<size_t> RisingRow , FallingRow;
		for (size_t iRow = 0; iRow < GradXProject.rows;iRow++)
		{
			for (; iRow < GradXProject.rows - 1 && GradXProject.at<uchar>(iRow, 0) == 0 && GradXProject.at<uchar>(iRow + 1, 0) == 255; iRow++);
			if (iRow < GradXProject.rows - 1)
			{
				RisingRow.push_back(iRow);
				iRow++;
			}
			for (; iRow < GradXProject.rows - 1 && GradXProject.at<uchar>(iRow + 1, 0) == 0; iRow++);
			if (iRow < GradXProject.rows - 1)
			{
				FallingRow.push_back(iRow);
			}
		}

		size_t TextHeightMin = 10;

		vector<size_t>::iterator itTextRowStart = RisingRow.begin(), itTextRowEnd = FallingRow.begin();

		for (;itTextRowStart != RisingRow.end() && itTextRowEnd != FallingRow.end(); )
		{
			if (*(itTextRowStart)-*(itTextRowEnd) <= TextHeightMin)
			{
				itTextRowStart = RisingRow.erase(itTextRowStart);
				itTextRowEnd = RisingRow.erase(itTextRowEnd);
			}
			else
			{
				itTextRowStart++;
				itTextRowEnd++;
			}
			
		}

		Mat GrayImageGradY(GrayImageMat.rows, GrayImageMat.cols, CV_8UC1, Scalar(0));
		for (size_t iRow = 1; iRow<GrayImageMat.rows - 1; iRow++)
		{
			for (size_t iCol = 1; iCol<(GrayImageMat.cols - 1); iCol++)
			{
				GrayImageGradY.at<uchar>(iRow, iCol) = (2 * abs(GrayImageMat.at<uchar>(iRow-1, iCol) - GrayImageMat.at<uchar>(iRow+1, iCol))
					+ abs(GrayImageMat.at<uchar>(iRow - 1 , iCol - 1) - GrayImageMat.at<uchar>(iRow + 1, iCol - 1))
					+ abs(GrayImageMat.at<uchar>(iRow - 1, iCol + 1) - GrayImageMat.at<uchar>(iRow + 1, iCol + 1))) / 4;
			}
		}


		//�ݶȾ����ֵ��
		threshold(
			GrayImageGradY, //�������
			GrayImageGradY, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);

		//��ʾ��Ե��ֵͼ��
		imshow(
			WindowName,
			GrayImageGradY
		);

		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);

		//����X�ݶ�ͶӰ����
		Mat  GradYProject(
			1,//��������
			GrayImageGradY.rows,//��������
			CV_8UC1,//����ֵ�����ͣ�8λ�޷���������ͨ����
			Scalar(0)//��������ĳ�ʼֵ
		);

		double SumTemp;
		for (size_t iCol = 0; iCol<GradYProject.cols; iCol++)
		{
			SumTemp = 0;
			for (size_t iRow = 0; iRow< GrayImageGradY.rows; iRow++)
			{
				SumTemp += double(GrayImageGradY.at<uchar>(iRow, iCol));
			}
			GradYProject.at<uchar>(0,iCol) = unsigned char(SumTemp / GrayImageGradY.rows);
		}

		threshold(
			GradYProject, //�������
			GradYProject, //�������
			128, //������ʼ��ֵ
			255, //���ֵ��������ֵ����Ϊ��ֵ��
			CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
		);


		//��ʾ��Ե��ֵͼ��
		imshow(
			WindowName,
			GradYProject
		);

		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		waitKey(0);


		for (size_t iRow = 2; iRow< EdgeXProject.rows - 2; iRow++)
		{

			if (BinaryEdgeXProject.at<uchar>(iRow - 2, 0) == 0 && BinaryEdgeXProject.at<uchar>(iRow - 1, 0) == 0 &&
				BinaryEdgeXProject.at<uchar>(iRow, 0) == 0 && BinaryEdgeXProject.at<uchar>(iRow + 1, 0) == 0 &&
				BinaryEdgeXProject.at<uchar>(iRow + 2, 0) == 0)

			{
				for (size_t iCol = CropRect.x; iCol < CropRect.x + CropRect.width; iCol++)
				{
					GrayImageEdge.at<uchar>(CropRect.y + iRow, iCol) = 0;
				}
			}
		}

		//��ʾ��Ե��ֵͼ��
		imshow(
			WindowName,
			GrayImageEdge
		);

		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		//waitKey(0);




		//�Իҽ�ͼ��ı�Ե����ˮƽ���������
		//�������Ͳ����ṹԪ�ĳߴ�-----------------------------------
		double DilateElementWidth = 9;
		//-----------------------------------------------------------
		//�������Ͳ����ĽṹԪ������һ��1��DilateElementWidth�еĽṹԪ�����ˮƽ����
		Mat DilateElement = Mat(
			1,//��һά��Y��ߴ磩
			int(DilateElementWidth),//�ڶ�ά��X��ߴ磩
			CV_8U,//�������ͣ�8λ�޷�������
			cvScalar(1)//������ֵ��ȫ0
		);
		//�Ա�Ե�������Ͳ���
		Mat DilatedGrayImageEdge;
		dilate(
			GrayImageEdge,//�������
			DilatedGrayImageEdge,//�������
			DilateElement,//���ͽṹԪ
			Point(-1, -1), //����Ԫê�㣬������ȡ������Ϊê��
			1 //�������͵Ĵ���
		);
		//��ʾ���ͺ�Ķ�ֵͼ��
		imshow(
			WindowName,
			DilatedGrayImageEdge
		);

		//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
		//waitKey(0);



		//��ˮƽ���ͺ��ͼ�����������ȡ
		vector<vector<Point> > AllContour;
		vector<Vec4i> ContourHierarchy;
		findContours(DilatedGrayImageEdge,//����ͼ�����
			AllContour, //�����������
			ContourHierarchy,//��������ȼ� 
			RETR_EXTERNAL,//ֻ��ȡ�ⲿ�������������а������ڲ������� 
			CHAIN_APPROX_SIMPLE//�����Ľ��ư취���ڴ�ѡ��CHAIN_APPROX_SIMPLE����ѹ��ˮƽ���򣬴�ֱ���򣬶Խ���
							   //�����Ԫ�أ�ֻ�����÷�����յ����꣬����һ����������ֻ��4����������������Ϣ
		);

		//sort(AllContour.begin(),AllContour.begin()+2,CompareContourCenter);
		////��ͨ����֮����ԭͼ�ϻ���ͼƬ���еĿ�
		rectangle(ResizedImageMat,//���ƾ��εĶ���
			CropRect, //Ҫ���Ƶľ���
			Scalar(255, 255, 255), //��������ɫ
			3,//�������
			CV_AA,//���ͣ�������� 
			0 //??������ֵ�������ƣ���С��λ��
		);

		//�߳��������-----------------------------------------------
		double LengthErrorLimit = 0.3;
		//�Ƕ��������
		float AngleErrorLimit = 5;
		//-----------------------------------------------------------

		//����ģ����Ʊ��������ĵ�������ͼ���е�λ��
		double EstimatedTitleCenterX = (ResizedImageWidth - CropRect.width) / 2.0 +
			TemplateTitleCenterX / TemplateWidth *  CropRect.width;
		double EstimatedTitleCenterY = (ResizedImageHeight - CropRect.height) / 2.0 +
			TemplateTitleCenterY / TemplateHeight *  CropRect.height;

		//ʹ��iContour������ȡ��������
		size_t iContour = 0;

		//��¼�Ƿ��ҵ������кͳ�������ı�־����
		bool FlagFoundTitle = false;
		bool FlagFoundPlate = false;

		//������ݱ���������Ƴ����ĳ����������ĵ����ꡢ��͸�
		double EstimatedPlateCenterX;
		double EstimatedPlateCenterY;


		//����������Ӿ���� �߶ȡ���ȡ���������ĵ�λ�ú���ת��
		double ContourRectHeight;
		double ContourRectWidth;
		double ContourRectRotateAngle;
		double ContourArea;
		double ContourRectCenterX;
		double ContourRectCenterY;
		double ContourRectAngle;

		//������������С��Ӿ��Σ�����ת��
		RotatedRect  ContourRect;

		//�����ж�������Ӿ����Ƿ���Ͽ�߱�������ȡ��߶ȡ����ĵ�XYλ��,�Լ������С�ı�־����
		bool IsWidthMatch, IsHeightMatch, IsCenterXMatch, IsCenterYMatch, IsAreaMatch;

		//�����һ�α�����Ѱ�ұ�������ʱ˳����еĳ��Ƴߴ��߱Ⱥͳߴ����з���Ҫ���������ţ��ڶ��μ��ֻ������ⲿ��������
		vector<int> PlateToBe;
		//����PlateToBe�ĵ�����
		vector<int>::iterator itContour;

		//��δ�ҵ���������ʱ�һ���δ�������ʱ����ѭ��
		while (FlagFoundTitle == false && iContour < AllContour.size())
		{
			////���Ʊ�����������
			//drawContours (ResizedImageMat,//���ƶ���
			//	AllContour,//����������
			//	iContour,//��������������/���
			//	Scalar(255,0,0),//���Ƶ��ߵ���ɫ
			//	1,//���Ƶ��߿�
			//	CV_AA,//���ͣ������
			//	ContourHierarchy,//�����ĵȼ�
			//	0,//ֻ���Ƶ�ǰ���������
			//	Point(0,0)//������ˮƽ�ʹ�ֱ�����ƫ��
			//	);

			//imshow(WindowName,ResizedImageMat);
			//waitKey(0);

			//��ȡ��������С��Ӿ��Σ�����ת��
			ContourRect = minAreaRect(AllContour[iContour]);

			//��ȡ��Ӿ���ĸ߶ȡ���ȡ���ת�ǡ���������ĵ�λ��
			ContourRectHeight = ContourRect.size.height;
			ContourRectWidth = ContourRect.size.width;
			ContourRectRotateAngle = ContourRect.angle;

			Moments ContourMoment = moments(AllContour[iContour], false);
			ContourRectCenterX = ContourMoment.m10 / ContourMoment.m00;
			ContourRectCenterY = ContourMoment.m01 / ContourMoment.m00;

			//����������Χ�����
			ContourArea = contourArea(AllContour[iContour],//��������
				false//�Ƿ������������ת�ǣ�˳ʱ�뻹����ʱ�룩��������ŵ����ֵ������ѡ��
			);

			//����Ӿ��ε���ת�ǽ�������ӳ�䣨-90��0 -> -45~45����ʹ���ֱ��
			//����ת��С��-45֤�����α�˳ʱ����ת��
			if (ContourRect.angle < -45.0)
			{
				//�����90��ת��Ϊ��ֵ
				ContourRectAngle = ContourRect.angle + 90.0;
				//�������ֵ�����Է���ֱ�������RotatedRect��angle���ԵĶ��壩
				swap(ContourRectWidth, ContourRectHeight);
			}

			ContourRectWidth = ContourArea / ContourRectHeight;

			//ע�⣺��Ϊǰ������ˮƽ��������Ͳ��������Զ����ｫ��ȼ�ȥһ������Ԫ�Ŀ��
			ContourRectWidth = ContourRectWidth - DilateElementWidth;

			//���ж��Ƿ���ϳ��ƺ�������Ŀ�߱Ⱥ�������������������Ա�����泵�����������ظ�����
			//���������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ�˳ʱ��Ϊ������ת�Ƕ�Ϊ-90+AngleErrorLimit��-90�ȣ���ʱ���߱�ʶ��Ϊ��
			//IsRatioMatch = (fabs(ContourRectWidth/ ContourRectHeight - TemplatePlateRatio ) / TemplatePlateRatio <= LengthErrorLimit && ContourRectRotateAngle <= -90+AngleErrorLimit) ||
			//��������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ���ʱ��Ϊ������ת�Ƕ�Ϊ-0��-1*AngleErrorLimit�ȣ���ʱ��߱�ʶ��Ϊ��
			//(fabs(ContourRectWidth / ContourRectHeight - TemplatePlateRatio) / TemplatePlateRatio <= LengthErrorLimit && ContourRectRotateAngle >= -1*AngleErrorLimit);

			//��͸�ռͼƬ�ı�����ģ��������LengthErrorLimit
			IsWidthMatch = fabs(ContourRectWidth / CropRect.width - TemplatePlateWidth / TemplateWidth) / (TemplatePlateWidth / TemplateWidth) <= LengthErrorLimit;
			IsHeightMatch = fabsf(ContourRectHeight / CropRect.height - TemplatePlateHeight / TemplateHeight) / (TemplatePlateHeight / TemplateHeight) <= LengthErrorLimit;
			//����Χ�ɵ������ģ��������LengthErrorLimit*(2+LengthErrorLimit)
			if (ContourArea >= TemplateTitleArea)
			{
				IsAreaMatch = fabsf(ContourArea - TemplatePlateArea) / (TemplatePlateArea) <= fabsf(LengthErrorLimit*(2.0 + LengthErrorLimit));
			}
			else
			{
				IsAreaMatch = fabsf(ContourArea - TemplatePlateArea) / (TemplatePlateArea) <= fabs(LengthErrorLimit*(LengthErrorLimit - 2.0));
			}

			//����������㳵������ĸ߶ȼ����Ҫ�󣬼�����ѹ��׼��������ʸ��PlateToBe�У��ڶ��α�����ֻ��������
			//IsRatioMatch && IsWidthMatch &&
			if (IsWidthMatch && IsHeightMatch && IsAreaMatch)
			{
				PlateToBe.push_back(iContour);
			}

			//���������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ�˳ʱ��Ϊ������ת�Ƕ�Ϊ-90+AngleErrorLimit��-90�ȣ���ʱ���߱�ʶ��Ϊ��
			//IsRatioMatch = fabs(ContourRectWidth / ContourRectHeight - TemplateTitleRatio) / TemplateTitleRatio <= LengthErrorLimit ;

			//��͸�ռͼƬ�ı�����ģ��������LengthErrorLimit
			IsWidthMatch = fabs(ContourRectWidth / CropRect.width - TemplateTitleWidth / TemplateWidth) / (TemplateTitleWidth / TemplateWidth) <= LengthErrorLimit;
			IsHeightMatch = fabsf(ContourRectHeight / CropRect.height - TemplateTitleHeight / TemplateHeight) / (TemplateTitleHeight / TemplateHeight) <= LengthErrorLimit;

			//���������������ģ����Ƴ����ĵ���X���Y�᷽���ϵ���������������LengthErrorLimit
			IsCenterXMatch = (fabsf(ContourRectCenterX - EstimatedTitleCenterX) / EstimatedTitleCenterX <= LengthErrorLimit);
			IsCenterYMatch = (fabsf(ContourRectCenterY - EstimatedTitleCenterY) / EstimatedTitleCenterY <= LengthErrorLimit);
			//����Χ�ɵ������ģ��������LengthErrorLimit*(2+LengthErrorLimit)
			if (ContourArea >= TemplateTitleArea)
			{
				IsAreaMatch = fabsf(ContourArea - TemplateTitleArea) / (TemplateTitleArea) <= fabsf(LengthErrorLimit*(2.0 + LengthErrorLimit));
			}
			else
			{
				IsAreaMatch = fabsf(ContourArea - TemplateTitleArea) / (TemplateTitleArea) <= fabs(LengthErrorLimit*(LengthErrorLimit - 2.0));
			}


			//����������������㣬����Ϊ��ǰ����Ϊ��������Ӧ������
			if (IsWidthMatch && IsHeightMatch  && IsCenterXMatch &&IsCenterYMatch && IsAreaMatch)
				////����ȡ������������ɸѡ
				//if (
				//	//���������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ�˳ʱ��Ϊ������ת�Ƕ�Ϊ-90+AngleErrorLimit��-90�ȣ���ʱ���߱�ʶ��Ϊ��
				//	(fabs(ContourRectHeight / ContourRectWidth - TemplateTitleRatio ) / TemplateTitleRatio <= LengthErrorLimit && ContourRectRotateAngle <= -90+AngleErrorLimit) ||
				//	//��������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ���ʱ��Ϊ������ת�Ƕ�Ϊ-0��-1*AngleErrorLimit�ȣ���ʱ��߱�ʶ��Ϊ��
				//	(fabs(ContourRectWidth / ContourRectHeight - TemplateTitleRatio) / TemplateTitleRatio <= LengthErrorLimit && ContourRectRotateAngle >= -1*AngleErrorLimit) &&
				//	//���������ռ���к�����ͼƬ����ı�����ģ��������(2+LengthErrorLimit)*LengthErrorLimit
				//	(fabs(ContourRectArea / CroppedInputImageArea - TemplateTitleArea/TemplateArea) / (TemplateTitleArea/TemplateArea) <= (2+LengthErrorLimit)*LengthErrorLimit) &&
				//	//���������������ģ����Ƴ����ĵ���X���Y�᷽���ϵ���������������LengthErrorLimit
				//	(fabs(ContourRectCenterX - EstimatedTitleCenterX) / EstimatedTitleCenterX <= LengthErrorLimit) &&
				//	(fabs(ContourRectCenterY - EstimatedTitleCenterY) / EstimatedTitleCenterY <= LengthErrorLimit)
				//	)//���������������Ǳ㽫�������������������ڵ�λ��
			{
				//��FlagFoundTitle��true��ʾ�����Ѿ��ҵ��˱����ж�Ӧ������
				FlagFoundTitle = true;
				//��ԭʼͼ���ϻ��Ƴ���һ����
				drawContours(ResizedImageMat,//���ƶ���
					AllContour,//����������21
					iContour,//��������������/���
					Scalar(255, 0, 0),//���Ƶ��ߵ���ɫ����ɫ
					3,//���Ƶ��߿�
					CV_AA,//���ͣ������
					ContourHierarchy,//�����ĵȼ�   00000.
					0,//ֻ���Ƶ�ǰ���������
					Point(0, 0)//������ˮƽ�ʹ�ֱ�����ƫ��
				);
				CropRect.height = int(ContourRectHeight / TemplateTitleHeight * TemplateHeight);
				CropRect.width = TemplateRatio	* CropRect.height;

				CropRect.x = int(ContourRectCenterX - (TemplateTitleCenterX / TemplateWidth)* CropRect.width);
				CropRect.y = int(ContourRectCenterY - (TemplateTitleCenterY / TemplateWidth)* CropRect.height);



				rectangle(ResizedImageMat,//���ƾ��εĶ���
					CropRect, //Ҫ���Ƶľ���
					Scalar(255, 0, 0), //��������ɫ
					3,//�������
					CV_AA,//���ͣ�������� 
					0 //??������ֵ�������ƣ���С��λ��
				);

				imshow(WindowName, ResizedImageMat);
				//waitKey(0);


				//�����ҵ��ı���������Ƴ��ƺ��������ĵ�����
				EstimatedPlateCenterX = CropRect.x + (TemplatePlateCenterX) / TemplateWidth  * CropRect.width;
				EstimatedPlateCenterY = CropRect.y + (TemplatePlateCenterY) / TemplateHeight * CropRect.height;
				//EstimatedPlateWidth = TemplatePlateHeight/TemplateWidth * CropRect.width;
				//EstimatedPlateHeight = TemplatePlateHeight/TemplateHeight * CropRect.height;

			}

			////˳ʱ����б���
			//if (ContourRectRotateAngle <= -90+AngleErrorLimit)
			//{
			//	//�ӱ���������ˮƽ�ʹ�ֱ�����ƶ������Ƶĳ��ƺŶ�Ӧ�����ĵ�
			//	EstimatedPlateCenterX = ContourRectCenterX -
			//		TemplateOffsetX/TemplateTitleWidth  * ContourRectWidth * cos(-1.0*ContourRectRotateAngle/180.0*pi);
			//    EstimatedPlateCenterY = ContourRectCenterY + 
			//		TemplateOffsetX/TemplateTitleHeight * ContourRectHeight * sin(-1.0*ContourRectRotateAngle/180.0*pi);
			//}
			//else if  (ContourRectRotateAngle >= -1*AngleErrorLimit)
			//{
			//	EstimatedPlateCenterX = ContourRectCenterX +
			//		TemplateOffsetX/TemplateTitleWidth  * ContourRectWidth * sin(-1.0*ContourRectRotateAngle/180.0*pi);
			//	EstimatedPlateCenterY = ContourRectCenterY + 
			//		TemplateOffsetY/TemplateTitleHeight * ContourRectHeight * cos(-1.0*ContourRectRotateAngle/180.0*pi);
			//}	


			//����ҵ��˱�������,��ʼ���г���������
			if (FlagFoundTitle == true)
			{
				//����������Ϊ��ѡ׼�������������������Ŀ�ͷ
				itContour = PlateToBe.begin();

				//��δ�ҵ�����������߻�������δ�����ʱ����ѭ��
				while (FlagFoundPlate == false && itContour != PlateToBe.end())
				{
					//ȡ�õ�ǰѭ�����������
					iContour = *(itContour);
					//drawContours (ResizedImageMat,//���ƶ���
					//	AllContour,//����������
					//	iContour,//��������������/���
					//	Scalar(0,0,255),//���Ƶ��ߵ���ɫ
					//	1,//���Ƶ��߿�
					//	CV_AA,//���ͣ������
					//	ContourHierarchy,//�����ĵȼ�
					//	0,//ֻ���Ƶ�ǰ���������
					//	Point(0,0)//������ˮƽ�ʹ�ֱ�����ƫ��
					//	);

					//imshow(WindowName,ResizedImageMat);
					//waitKey(0);

					ContourRect = minAreaRect(AllContour[iContour]);

					//��ȡ��Ӿ���ĸ߶ȡ���ȡ���ת�ǡ���������ĵ�λ��
					ContourRectHeight = ContourRect.size.height;
					ContourRectWidth = ContourRect.size.width;
					ContourRectRotateAngle = ContourRect.angle;
					//����������Χ�����
					ContourArea = contourArea(AllContour[iContour],//��������
						false//�Ƿ������������ת�ǣ�˳ʱ�뻹����ʱ�룩��������ŵ����ֵ������ѡ��
					);
					ContourRectCenterX = ContourRect.center.x;
					ContourRectCenterY = ContourRect.center.y;

					//�Ƕȱ任�Ϳ�߽���
					if (ContourRect.angle < -45.0)
					{
						ContourRectAngle = ContourRect.angle + 90.0;
						swap(ContourRectWidth, ContourRectHeight);
					}

					//ע�⣺��Ϊǰ������ˮƽ��������Ͳ��������Զ������ȼ�ȥ��һ������Ԫ�Ŀ��
					ContourRectWidth = ContourRectWidth - DilateElementWidth;

					//���������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ�˳ʱ��Ϊ������ת�Ƕ�Ϊ-90+AngleErrorLimit��-90�ȣ���ʱ���߱�ʶ��Ϊ��
					//IsRatioMatch = fabs(ContourRectWidth / ContourRectHeight  - TemplatePlateRatio ) / TemplatePlateRatio <= LengthErrorLimit ;
					////��������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ���ʱ��Ϊ������ת�Ƕ�Ϊ-0��-1*AngleErrorLimit�ȣ���ʱ��߱�ʶ��Ϊ��
					//(fabs(ContourRectWidth / ContourRectHeight - TemplatePlateRatio) / TemplatePlateRatio <= LengthErrorLimit && ContourRectRotateAngle >= -1*AngleErrorLimit);

					////��͸�ռͼƬ�ı�����ģ��������LengthErrorLimit
					//IsWidthMatch = fabs(ContourRectWidth  / CroppedInputImageWidth  - TemplatePlateWidth/TemplateWidth) / (TemplatePlateWidth/TemplateWidth) <= LengthErrorLimit ;
					//IsHeightMatch = fabs(ContourRectHeight / CroppedInputImageHeight  - TemplatePlateHeight/TemplateHeight) / (TemplatePlateHeight/TemplateHeight) <= LengthErrorLimit ;

					//����������������ģ����Ƴ����ĵ���X���Y�᷽���ϵ��������Ƿ��������LengthErrorLimit
					IsCenterXMatch = (fabsf(ContourRectCenterX - EstimatedPlateCenterX) / EstimatedTitleCenterX <= LengthErrorLimit);
					IsCenterYMatch = (fabsf(ContourRectCenterY - EstimatedPlateCenterY) / EstimatedTitleCenterY <= LengthErrorLimit);

					//�����������������Ϊ��ǰ������Ӧ���ƺ������ڵ�����
					if (IsCenterXMatch &&IsCenterYMatch)
						//if (
						//	//���������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ�˳ʱ��Ϊ������ת�Ƕ�Ϊ-90+AngleErrorLimit��-90�ȣ���ʱ���߱�ʶ��Ϊ��
						//	(fabs(ContourRectHeight / ContourRectWidth - TemplatePlateRatio ) / TemplatePlateRatio <= LengthErrorLimit && ContourRectRotateAngle <= -90+AngleErrorLimit) ||
						//	//��������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ���ʱ��Ϊ������ת�Ƕ�Ϊ-0��-1*AngleErrorLimit�ȣ���ʱ��߱�ʶ��Ϊ��
						//	(fabs(ContourRectWidth / ContourRectHeight - TemplatePlateRatio) / TemplatePlateRatio <= LengthErrorLimit && ContourRectRotateAngle >= -1*AngleErrorLimit) &&
						//	//���������ռ���к�����ͼƬ����ı�����ģ��������(2+LengthErrorLimit)*LengthErrorLimit
						//	(fabs(ContourRectArea / CroppedInputImageArea - TemplatePlateArea/TemplateArea) / (TemplatePlateArea/TemplateArea) <= (2+LengthErrorLimit)*LengthErrorLimit) &&
						//	//���������������ģ����Ƴ����ĵ���X���Y�᷽���ϵ���������������LengthErrorLimit
						//	(fabs(ContourRectCenterX - EstimatedPlateCenterX) / EstimatedPlateCenterX <= LengthErrorLimit) &&
						//	(fabs(ContourRectCenterY - EstimatedPlateCenterY) / EstimatedPlateCenterY <= LengthErrorLimit)
						//	)//���������������Ǳ㽫�������������ƺ������ڵ�λ��
					{
						//���ҵ���������ı�־������true�����������ҵ��˳�������
						FlagFoundPlate = true;
						drawContours(ResizedImageMat,//���ƶ���
							AllContour,//����������
							iContour,//��������������/���
							Scalar(0, 0, 255),//���Ƶ��ߵ���ɫ
							3,//���Ƶ��߿�
							CV_AA,//���ͣ������
							ContourHierarchy,//�����ĵȼ�
							0,//Sֻ���Ƶ�ǰ���������
							Point(0, 0)//������ˮƽ�ʹ�ֱ�����ƫ��
						);

						CropRect.height = int(ContourRectHeight / TemplatePlateHeight * TemplateHeight);
						CropRect.width = TemplateRatio	* CropRect.height;
						CropRect.x = int(ContourRectCenterX - (TemplatePlateCenterX / TemplateWidth)* CropRect.width);
						CropRect.y = int(ContourRectCenterY - (TemplatePlateCenterY / TemplateWidth)* CropRect.height);

						rectangle(ResizedImageMat,//���ƾ��εĶ���
							CropRect, //Ҫ���Ƶľ���
							Scalar(0, 0, 255), //��������ɫ
							3,//�������
							CV_AA,//���ͣ�������� 
							0 //??������ֵ�������ƣ���С��λ��
						);


					}
					//����������1ת�������һ������
					itContour++;
				}
			}
			//������������1�����һ������
			iContour++;
		}
		imshow(WindowName, ResizedImageMat);

		//waitKey(0);

		int SepPos = ImagePathList[iImage].rfind('\\');//rfind �������
		string FolderPath = ImagePathList[iImage].substr(0, SepPos);
		string ImageFileName = ImagePathList[iImage].substr(SepPos + 1, -1);
		string OutputImagePath = OutputPath + "\\���" + ImageFileName;
		imwrite(OutputImagePath, ResizedImageMat);
	}
	//����0�������˳�����
	return 0;
}