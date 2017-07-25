#include "main.h"

int GetGradXY(Mat& InputMat, Mat& OutputGradXMat, Mat& OutputGradYMat)
{
	if (InputMat.type() != CV_8UC1 || InputMat.channels() != 1)
	{
		ofstream LogStream(LogFilePath, ios::app);
		time_t CurrentTime;
		tm CurrentLocalTime;
		char CurrentTimeString[20];
		localtime_s(&CurrentLocalTime, &CurrentTime);
		strftime(CurrentTimeString, sizeof(CurrentTimeString), "[%Y/%m/%d %X]", &CurrentLocalTime);
		LogStream << CurrentTimeString << "Error:" << endl << "Ilegal input parameter." << endl;
		return 1;
	}
	OutputGradXMat = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);

	OutputGradXMat = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);


	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
		{
			OutputGradXMat.ptr<uchar>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<uchar>(iRow)[iCol + 1] - InputMat.ptr<uchar>(iRow)[iCol - 1]) +
				3 * (InputMat.ptr<uchar>(iRow-1)[iCol + 1] - InputMat.ptr<uchar>(iRow-1)[iCol - 1]) +
				3 * (InputMat.ptr<uchar>(iRow+1)[iCol + 1] - InputMat.ptr<uchar>(iRow+1)[iCol - 1])
			);
			OutputGradYMat.ptr<uchar>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<uchar>(iRow + 1)[iCol] - InputMat.ptr<uchar>(iRow - 1)[iCol]) +
				3 * (InputMat.ptr<uchar>(iRow + 1)[iCol - 1] - InputMat.ptr<uchar>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<uchar>(iRow + 1)[iCol + 1] - InputMat.ptr<uchar>(iRow - 1)[iCol + 1])
			);
		}
	}
	return 0;
}