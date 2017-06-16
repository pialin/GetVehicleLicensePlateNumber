#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

//ʹ��C++��׼�������ռ�
using namespace std;

//ʹ��OpenCV�������ռ�
using namespace cv;

const float TemplateWidth = 580;
const float TemplateHeight = 387;
const float TemplateTitleWidth = 340;
const float TemplateTitleHeight = 40;
const float TemplateTitleCenterX = 284;
const float TemplateTitleCenterY = 46;

const float TemplatePlateWidth = 83;
const float TemplatePlateHeight = 20;
const float TemplatePlateCenterX = 151;
const float TemplatePlateCenterY = 86;

int main(int ArgumentCount, char** ArgumentVector)
{


	//�������������������Ŀ�Ƿ���ȷ���������ȷ����ʾ�÷�˵�����˳�����
	if (ArgumentCount != 2)
	{
		//��ʾ�����÷�˵��
		cout << " Usage:  " << ArgumentVector[0] << " ImageToLoadAndDisplay" << endl;
		//���ش����벢�˳�����
		return -1;
	}

	//�½�����RawImageMat���ڴ洢ԭʼͼƬ����
	Mat RawImageMat;

	//���ݵ�һ���������ļ�·������ͼƬ��ȡ
	RawImageMat = imread(
		ArgumentVector[1],//����ͼƬ·��
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


	Mat GrayImageMat;

	//��ȡͼƬ��ͨ����
	int NumRawImageChannel = RawImageMat.channels();

	//���ͼ��Ϊ3ͨ����ɫͼ������ת���ɻҽ�ͼ��
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
	//���ͼ���Ѿ�Ϊ��ͨ���ҽ�ͼ��ֱ�Ӳ���ת����RawImageMat����GrayImageMat
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
		CV_WINDOW_NORMAL//�˴��ڸ���ͼƬ��С��ʾͼƬ������resize
	);
	resizeWindow(WindowName, GrayImageMat.cols * 600 / GrayImageMat.rows, 600);
	//��������������ʾ�ҽ�ͼ��
	imshow(
		WindowName,//ϣ������ͼƬ�Ĵ�������
		GrayImageMat//ϣ�����Ƶ�ͼƬ����
	);

	//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
	waitKey(0);


	//��ͼ�����5*5�ĸ�˹ģ��
	Mat BlurGrayImageMat;
	GaussianBlur(
		GrayImageMat, //����ͼƬ����
		BlurGrayImageMat, //���ͼƬ����
		Size(9, 9), //��˹�˳ߴ�
		0, //??��˹��X�����׼�� 
		0, //??��˹��Y�����׼��
		BORDER_DEFAULT //??���ز�ֵ��ʽ 
	);

	//��������������ʾ��˹ģ�����ͼ��
	imshow(
		WindowName,//ϣ������ͼƬ�Ĵ�������
		BlurGrayImageMat//ϣ�����Ƶ�ͼƬ����
	);

	//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
	waitKey(0);


	//ʹ��Sobel���Ӽ���X��ˮƽ����Y(��ֱ)�����ݶ�
	Mat GrayImageGradX, GrayImageGradY;
	//����ҽ�ͼ��X�����ݶȣ�
	Sobel(
		BlurGrayImageMat,
		GrayImageGradX, //����ݶȾ���
		CV_16S, //����ݶȾ��������/��ȣ�Ŀ������������Ϊ16λ�з��������������ֵ���
		1, //�����󵼽���Ϊ1
		0, //�����󵼽���Ϊ0�������󵼣�
		CV_SCHARR, //ʹ��3*3��Scharr�˲�������Sobel�˲���һ���쵫׼ȷ�ʸ��ߣ�����3*3��Sobel�˲��������ݶ�
		1, //??scaleΪ1
		0, //??deltaΪ0
		BORDER_DEFAULT //??�߽����ͣ����ز�ֵ����
	);//������൱�ڣ�Scharr( GrayImageMat, GrayImageGradX,  CV_16S , 0, 1, 1, 0, BORDER_DEFAULT);

//ͬ�����ҽ�ͼ��Y������ݶ�
	Sobel(
		BlurGrayImageMat,
		GrayImageGradY,
		CV_16S,
		0, //�����󵼽���Ϊ0�������󵼣�
		1, //�����󵼽���Ϊ1
		CV_SCHARR,
		1,
		0,
		BORDER_DEFAULT
	);


	//��X��Y������ݶ�ֵ�����ֵ��ӳ���8λ�޷�������
	Mat GrayImageGradX8U, GrayImageGradY8U;
	convertScaleAbs(
		GrayImageGradX, //�������
		GrayImageGradX8U //�������
	);

	convertScaleAbs(
		GrayImageGradY,
		GrayImageGradY8U
	);

	//��XY�����ݶȵļ�Ȩ��Ϊ���ݶ�
	Mat GrayImageEdge;
	addWeighted(
		GrayImageGradX8U, //�������1
		0.5, //����1Ȩ��
		GrayImageGradY8U, //�������2
		0.5, //����2Ȩ��
		0, //����ƫ��ֵ
		GrayImageEdge, //�������
		-1 //����������������1��ͬ
	);

	//�ݶȾ����ֵ��
	threshold(
		GrayImageGradX8U, //�������
		GrayImageEdge, //�������
		128, //������ʼ��ֵ
		255, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);

	//��ʾ��Ե��ֵͼ��
	imshow(
		WindowName,
		GrayImageEdge
	);

	//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
	waitKey(0);

	//�Իҽ�ͼ��ı�Ե����ˮƽ���������
	//�������Ͳ����ĽṹԪ
	int DilateElementWidth = 11;
	Mat DilateElement = Mat(
		1,//��һά��Y��ߴ磩
		DilateElementWidth,//�ڶ�ά��X��ߴ磩
		CV_8U,//�������ͣ�8λ�޷�������
		cvScalar(1)//������ֵ��ȫ1
	);
	Mat DilatedGrayImageEdge;
	//�������Ͳ���  
	dilate(
		GrayImageEdge,//�������
		DilatedGrayImageEdge,//�������
		DilateElement,//���ͽṹԪ
		Point(DilateElementWidth/2-1, 0), //����Ԫê��
		1 //�������͵Ĵ���
	);
	//��ʾ���ͺ�Ķ�ֵͼ��
	imshow(
		WindowName,
		DilatedGrayImageEdge
	);


	//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
	waitKey(0);

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
	//���������ȡ��������
	for (size_t iContour = 0; iContour < AllContour.size(); iContour++)
	{
		//��ȡ��������С��Ӿ��Σ�����ת��
		RotatedRect  ContourRect = minAreaRect(AllContour[iContour]);

		//��ȡ��Ӿ���ĸ߶ȡ���ȡ���ת�ǡ���������ĵ�λ��
		float ContourRectHeight = ContourRect.size.height;
		//ע�⣺��Ϊǰ������ˮƽ��������Ͳ��������Զ������ȼ�ȥ��һ������Ԫ�Ŀ��
		float ContourRectWidth = ContourRect.size.width- DilateElementWidth;
		float ContourRectRotateAngle = ContourRect.angle;
		float ContourRectArea = ContourRect.size.area();
		float ContourRectCenterX = ContourRect.center.x;
		float ContourRectCenterY = ContourRect.center.y;

		//����ģ��Ŀ�߱Ⱥ����
		const float TemplateRatio = TemplateWidth/TemplateHeight;
		const float TemplateArea = TemplateWidth*TemplateHeight;

		//����ģ��ı���Ŀ�߱Ⱥ����
		const float TemplateTitleRatio = TemplateTitleWidth/TemplateTitleHeight;
		const float TemplateTitleArea = TemplateTitleWidth*TemplateTitleHeight;

		//��������ͼ��Ŀ��ߡ�����Ϳ�߱�
		const float InputImageWidth = RawImageMat.cols;
		const float InputImageHeight = RawImageMat.rows;
		const float InputImageArea = InputImageWidth * InputImageHeight;
		const float InputImageRatio = InputImageWidth / InputImageHeight;
		
		//��ģ���߱Ƚ��в���֮�������ͼ������
		float CroppedInputImageArea;

		//��¼�Ƿ��ҵ������еı�־����
		bool FlagFoundTitle = false;

		//����ģ����Ʊ��������ĵ�������ͼ���е�λ��
		float EstimatedTitleCenterX,EstimatedTitleCenterY;


		//�������ͼ���߱ȱ�ģ��ͼ����󣨼������֣�
		if (InputImageRatio >= TemplateRatio  )
		{
			//�������֮�������ͼ������
			CroppedInputImageArea = InputImageHeight * TemplateRatio * InputImageHeight;
			//����������������Ĳ��֣����ĵ��X������Ҫ�����ƣ�
			EstimatedTitleCenterX = (InputImageWidth - InputImageHeight * TemplateRatio)/2.0 +
				TemplateTitleCenterX /TemplateWidth *  InputImageWidth;
			EstimatedTitleCenterY = TemplateTitleCenterY /TemplateHeight *  InputImageHeight;
		}
		//��������ͼ���߱ȱ�ģ��ͼ���С���������ݣ�
		else
		{
			//�������֮�������ͼ������
			CroppedInputImageArea = InputImageWidth * InputImageWidth /TemplateRatio;
			//���������������Ĳ��֣����ĵ��Y������Ҫ�����ƣ�
			EstimatedTitleCenterX = TemplateTitleCenterX /TemplateWidth *  InputImageWidth;
			EstimatedTitleCenterY = (InputImageHeight - InputImageWidth /TemplateRatio)/2.0 +
				(TemplateTitleCenterY /TemplateHeight) *  InputImageHeight;	
		}

		//�߳��������
		float LengthErrorLimit = 0.05;
		//�Ƕ��������
		float AngleErrorLimit = 5;

		//����ȡ������������ɸѡ
		if (
			//���������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ�˳ʱ��Ϊ������ת�Ƕ�Ϊ-90+AngleErrorLimit��-90�ȣ���ʱ���߱�ʶ��Ϊ��
			(fabs(ContourRectHeight / ContourRectWidth - TemplateTitleRatio ) / TemplateTitleRatio <= LengthErrorLimit && ContourRectRotateAngle <= -90+AngleErrorLimit) ||
			//��������Ӿ��εĿ�߱Ⱥ�ģ��������������LengthErrorLimit��ˮƽ��б������AngleErrorLimit�ȣ���ʱ��Ϊ������ת�Ƕ�Ϊ-0��-1*AngleErrorLimit�ȣ���ʱ��߱�ʶ��Ϊ��
			(fabs(ContourRectWidth / ContourRectHeight - TemplateTitleRatio) / TemplateTitleRatio <= LengthErrorLimit && ContourRectRotateAngle >= -1*AngleErrorLimit) &&
			//���������ռ���к�����ͼƬ����ı�����ģ��������(2+LengthErrorLimit)*LengthErrorLimit
			(fabs(ContourRectArea / CroppedInputImageArea - TemplateTitleArea/TemplateArea) / TemplateTitleArea/TemplateArea <= (2+LengthErrorLimit)*LengthErrorLimit &&
			//���������������ģ����Ƴ����ĵ���X���Y�᷽���ϵ���������������LengthErrorLimit
			(fabs(ContourRectCenterX - EstimatedTitleCenterX) / EstimatedTitleCenterY <= LengthErrorLimit) &&
			(fabs(ContourRectCenterY - EstimatedTitleCenterY) / EstimatedTitleCenterY <= LengthErrorLimit)
			)//���������������Ǳ㽫�������������������ڵ�λ��
		{
			//��FlagFoundTitle��true��ʾ�����Ѿ��ҵ��˱����ж�Ӧ������
			FlagFoundTitle = true;
			//��ԭʼͼ���ϻ��Ƴ���һ����
			drawContours (RawImageMat,//���ƶ���
				AllContour,//����������
				iContour,//��������������/���
				Scalar(255,0,0),//���Ƶ��ߵ���ɫ����ɫ
				5,//���Ƶ��߿�
				CV_AA,//���ͣ������
				ContourHierarchy,//�����ĵȼ�
				0,//ֻ���Ƶ�ǰ���������
				Point(0,0)//������ˮƽ�ʹ�ֱ�����ƫ��
				);
			//��ˮƽ�������ģ��˳ʱ����б����ʱ����б����������Ƴ��ƺ����ĵ�����

			float InputImageOffsetX =  (TemplateTitleCenterX-TemplatePlateCenterX)/TemplateTitleWidth  * ContourRectWidth
			float InputImageOffsetY = 
			//˳ʱ����б���
			if (ContourRectRotateAngle <= -90+AngleErrorLimit)
			{
				//�ӱ���������ˮƽ�ʹ�ֱ�����ƶ������Ƶĳ��ƺŶ�Ӧ�����ĵ�
				EstimatedPlateCenterX = ContourRectCenterX -
					TemplateOffsetX/TemplateTitleWidth  * ContourRectWidth * cos(-1.0*ContourRectRotateAngle/180.0*pi);
			    EstimatedPlateCenterY = ContourRectCenterY + 
					TemplateOffsetX/TemplateTitleHeight * ContourRectHeight * sin(-1.0*ContourRectRotateAngle/180.0*pi);
			}
			else if  (ContourRectRotateAngle >= -1*AngleErrorLimit)
			{
				EstimatedPlateCenterX = ContourRectCenterX +
					TemplateOffsetX/TemplateTitleWidth  * ContourRectWidth * sin(-1.0*ContourRectRotateAngle/180.0*pi);
				EstimatedPlateCenterY = ContourRectCenterY + 
					TemplateOffsetY/TemplateTitleHeight * ContourRectHeight * cos(-1.0*ContourRectRotateAngle/180.0*pi);
			}

		}

		if (FlagFoundTitle == true)
		{
			for (size_t iContour = 0; iContour < AllContour.size(); iContour++)
			{
				RotatedRect  ContourRect = minAreaRect(AllContour[iContour]);

				if (
					(fabs(PlateRectHeight / PlateRectWidth - TemplatePlateRatio ) / TemplatePlateRatio <= 0.05 && PlateRectRotateAngle <= -85) ||
					(fabs(PlateRectWidth / PlateRectHeight - TemplatePlateRatio) / TemplatePlateRatio <= 0.05 && PlateRectRotateAngle >= -5) &&
					(fabs(PlateRectArea / (DetectedImageWidth*DetectedImageHeight) - TemplatePlateArea) / TemplatePlateArea <= 0.05*0.05) &&
					(fabs(PlateRectCenterX - EstimatedPlateCenterX) / EstimatedPlateCenterY <= 0.05) &&
					(fabs(PlateRectCenterY - EstimatedPlateCenterY) / EstimatedPlateCenterY <= 0.05)
					)
				{
					drawContours (RawImageMat,//���ƶ���
						AllContour,//����������
						iContour,//��������������/���
						Scalar(0,0,255),//���Ƶ��ߵ���ɫ
						5,//���Ƶ��߿�
						CV_AA,//���ͣ������
						ContourHierarchy,//�����ĵȼ�
						0,//ֻ���Ƶ�ǰ���������
						Point(0,0)//������ˮƽ�ʹ�ֱ�����ƫ��
						);
					imshow(WindowName,RawImageMat);
				}
			}

		}

	waitKey(0);

	//����0�������˳�����
	return 0;

}

