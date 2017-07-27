#include "main.h"

int PrepareTemplateImage
(
	String TemplateImagePath
)
{
	Mat Gray_TemplateImage;
	LoadInputImage(
		TemplateImagePath,//输入图片路径
		Gray_TemplateImage
	);

	Mat TemplateImageGradY;
	GetGradY<uchar>(Gray_TemplateImage, TemplateImageGradY);

	Mat Binary_TemplateImageGradY;
	threshold(
		TemplateImageGradY, //输入矩阵
		Binary_TemplateImageGradY, //输出矩阵
		128, //迭代初始阈值
		255, //最大值（超过阈值将设为此值）
		CV_THRESH_OTSU //阈值化选择的方法:Otsu法
	);
	
	Binary_TemplateImageGradY.convertTo(Binary_TemplateImageGradY, CV_8UC1);

	////创建X方向梯度投影向量
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

