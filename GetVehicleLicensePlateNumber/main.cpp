#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

//ʹ��C++��׼�������ռ�
using namespace std;

//ʹ��OpenCV�������ռ�
using namespace cv;


int main( int ArgumentCount, char** ArgumentVector )
{


	//�������������������Ŀ�Ƿ���ȷ���������ȷ����ʾ�÷�˵�����˳�����
	if( ArgumentCount != 2)
	{
		//��ʾ�����÷�˵��
		cout << " Usage:  "<< ArgumentVector[0] << " ImageToLoadAndDisplay" << endl;
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
	if( !RawImageMat.data )
	{ 
		//��ʾͼƬ��ȡʧ����ʾ��Ϣ
		cout << " Error:  Can't read image data from"<< ArgumentVector[1]  << endl;
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
	//��������������ʾ�ҽ�ͼ��
	imshow( 
		WindowName,//ϣ������ͼƬ�Ĵ�������
		GrayImageMat//ϣ�����Ƶ�ͼƬ����
		);

	//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
	waitKey(0);


	//��ͼ�����3*3�ĸ�˹ģ��
	Mat BlurGrayImageMat;
	GaussianBlur( 
		GrayImageMat, //����ͼƬ����
		BlurGrayImageMat, //���ͼƬ����
		Size(3,3), //��˹�˳ߴ�
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
	Mat GrayImageGradX,GrayImageGradY;
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

	////convertScaleAbs( grad_x, abs_grad_x );

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
	Mat GrayImageGradX8U,GrayImageGradY8U;
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
		GrayImageEdge, //�������
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
	Mat DilateElement = Mat(
		1,//��һά��Y��ߴ磩
		19,//�ڶ�ά��X��ߴ磩
		CV_8U,//�������ͣ�8λ�޷�������
		cvScalar(1)//������ֵ��ȫ1
		);  
	Mat DilatedGrayImageEdge;  
	//�������Ͳ���  
	dilate(
		GrayImageEdge,//�������
		DilatedGrayImageEdge,//�������
		DilateElement,//���ͽṹԪ
		Point(9,0), //����Ԫê��
		1 //�������͵Ĵ���
		);
	//��ʾ���ͺ�Ķ�ֵͼ��
	imshow( 
		WindowName,
		DilatedGrayImageEdge
		);


	//�ȴ�������Ӧ������0��ʾ�ȴ�ʱ��Ϊ���޳�
	waitKey(0);

	//����0�������˳�����
	return 0;
}