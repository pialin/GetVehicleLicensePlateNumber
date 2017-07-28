#include "main.h"

int PrepareTemplateImage
(
	vector <int> & TemplateImageLineRow,
	Mat & Binary_ProjectX_TemplateImageGradY,
	double & TemplateImageMeanLineHeight,
	Size & TemplateImageSize,
	Rect & TemplateImageTitleRect,
	Rect & TemplateImagePlateNumberLineRect,
	Rect & TemplateImagePlateNumberAreaRect,
	Size & TemplateImagePlateNumberSize
)
{
	String TemplateImagePath = ".\\TemplateImage\\TemplateImage.png";

	Mat Gray_TemplateImage;
	LoadInputImage(
		TemplateImagePath,//����ͼƬ·��
		Gray_TemplateImage
	);
	TemplateImageLineRow = { 40,107,190,269,356,439,522,605,683 };

	for (int iLine = 2; iLine < TemplateImageLineRow.size(); iLine++)
	{
		TemplateImageMeanLineHeight = TemplateImageMeanLineHeight +
			TemplateImageLineRow[iLine] - TemplateImageLineRow[iLine - 1];
	}
	TemplateImageMeanLineHeight = TemplateImageMeanLineHeight / (TemplateImageLineRow.size() - 1);

	Gray_TemplateImage = Gray_TemplateImage.rowRange(TemplateImageLineRow[0],
		TemplateImageLineRow.back());
	TemplateImageSize.width = Gray_TemplateImage.cols;
	TemplateImageSize.height = Gray_TemplateImage.rows;
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

	Binary_ProjectX_TemplateImageGradY;
	GetProjectX<uchar>(Binary_TemplateImageGradY, Binary_ProjectX_TemplateImageGradY);

	TemplateImageTitleRect.x = 193;
	TemplateImageTitleRect.width = 833 - 193;
	TemplateImageTitleRect.y = TemplateImageLineRow[0];
	TemplateImageTitleRect.height= TemplateImageLineRow[1] -  TemplateImageLineRow[0];


	TemplateImagePlateNumberAreaRect = Rect(159, 106, 454 - 159, 228 - 106);

	TemplateImagePlateNumberSize;
	TemplateImagePlateNumberSize.width = 175;
	TemplateImagePlateNumberSize.height = 39;

	TemplateImagePlateNumberLineRect.x = 43;
	TemplateImagePlateNumberLineRect.width = 766;
	TemplateImagePlateNumberLineRect.y = TemplateImageLineRow[0];
	TemplateImagePlateNumberLineRect.height = TemplateImageLineRow[1] - TemplateImageLineRow[0];

	return 0;
}

