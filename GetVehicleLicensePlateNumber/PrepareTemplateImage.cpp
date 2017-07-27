#include "main.h"

int PrepareTemplateImage
(
	String TemplateImagePath
)
{
	Mat Gray_TemplateImage;
	LoadInputImage(
		TemplateImagePath,//����ͼƬ·��
		Gray_TemplateImage
	);

	Mat TemplateImageGradY;
	GetGradY<uchar>(Gray_TemplateImage, TemplateImageGradY);

	Mat Binary_TemplateImageGradY;
	threshold(
		TemplateImageGradY, //�������
		Binary_TemplateImageGradY, //�������
		128, //������ʼ��ֵ
		255, //���ֵ��������ֵ����Ϊ��ֵ��
		CV_THRESH_OTSU //��ֵ��ѡ��ķ���:Otsu��
	);
	
	Binary_TemplateImageGradY.convertTo(Binary_TemplateImageGradY, CV_8UC1);

	////����X�����ݶ�ͶӰ����
	Mat  Binary_ProjectX_TemplateImageGradY;
	GetProjectX<uchar>(Binary_TemplateImageGradY, Binary_ProjectX_TemplateImageGradY);

	vector<int> TemplateLineRow = {40,107,190,269,356,439,522,605,683};
	int TemplateMatchHeight = *TemplateLineRow.rbegin() - *TemplateLineRow.begin();
	const double TemplateImageLineGapHeight = 83;
	const double TemplateImagePlateNumberLineHeight = TemplateImageLineGapHeight;
	Rect TemplateImagePlateNumberAreaRect = Rect(159, 106, 454 - 159, 228 - 106);
	int TemplateImagePlateNumberWidth = 175;
	int TemplateImagePlateNumberHeight = 39;
	int TemplateImageTitleStartCol = 193;
	int TemplateImageTitleEndCol = 833;
	int TemplateImageTitleWidth = TemplateImageTitleEndCol - TemplateImageTitleStartCol;
	int TemplateImageTitleLineHeight = TemplateLineRow[1] - TemplateLineRow[0];
}

