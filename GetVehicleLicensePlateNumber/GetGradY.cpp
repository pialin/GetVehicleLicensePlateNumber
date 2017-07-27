#include "main.h"

int GetGradXY(Mat& InputMat, Mat& OutputGradYMat)
{
	if (InputMat.type() != CV_8UC1 || InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}
	Mat OutputGradYMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);



	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
		{
			OutputGradYMatTemp.ptr<uchar>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<uchar>(iRow + 1)[iCol] - InputMat.ptr<uchar>(iRow - 1)[iCol]) +
				3 * (InputMat.ptr<uchar>(iRow + 1)[iCol - 1] - InputMat.ptr<uchar>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<uchar>(iRow + 1)[iCol + 1] - InputMat.ptr<uchar>(iRow - 1)[iCol + 1])
			);

		}
	}
	float MinValue, MaxValue;
	minMaxLoc(OutputGradYMatTemp, MinValue, MaxValue);
	OutputGradYMatTemp.convertTo(OutputGradYMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);
	return 0;
}