#include "main.h"

int GetGradXY(Mat& InputMat, Mat& OutputGradXMat)
{
	if (InputMat.type() != CV_8UC1 || InputMat.channels() != 1)
	{
		AppendLog( "Error: Ilegal input parameter.");
		return 1;
	}
	Mat OutputGradXMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);



	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
		{
			OutputGradXMatTemp.ptr<uchar>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<uchar>(iRow)[iCol + 1] - InputMat.ptr<uchar>(iRow)[iCol - 1]) +
				3 * (InputMat.ptr<uchar>(iRow - 1)[iCol + 1] - InputMat.ptr<uchar>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<uchar>(iRow + 1)[iCol + 1] - InputMat.ptr<uchar>(iRow + 1)[iCol - 1])
			);
			
		}
	}

	float MinValue, MaxValue;
	minMaxLoc(OutputGradXMatTemp, MinValue, MaxValue);
	OutputGradXMatTemp.convertTo(OutputGradXMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);
	return 0;
}