#include "main.h"

int GetGradXY(Mat& SourceMat, Mat& OutputGradXMat, Mat& OutputGradYMat)
{
	if (SourceMat.type() != CV_8UC1 || SourceMat.channels() != 1)
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
		int(SourceMat.rows),
		int(SourceMat.cols),
		CV_32FC1
	);

	OutputGradXMat = Mat::zeros(
		int(SourceMat.rows),
		int(SourceMat.cols),
		CV_32FC1
	);

	uchar * SourceCurrentRowHeadPtr = 0, *SourcePreviousRowHeadPtr = 0, *SourceNextRowHeadPtr = 0;

	uchar * GradYCurrentRowHeadPtr = 0, *GradXCurrentRowHeadPtr = 0;

	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < SourceMat.rows - 1; iRow++)
	{
		SourceCurrentRowHeadPtr = SourceMat.ptr<uchar>(iRow);
		SourcePreviousRowHeadPtr = SourceMat.ptr<uchar>(iRow - 1);
		SourceNextRowHeadPtr = SourceMat.ptr<uchar>(iRow + 1);

		GradXCurrentRowHeadPtr = OutputGradXMat.ptr<uchar>(iRow);
		GradYCurrentRowHeadPtr = OutputGradYMat.ptr<uchar>(iRow);

		for (int iCol = 1; iCol < SourceMat.cols - 1; iCol++)
		{
			GradYCurrentRowHeadPtr[iCol] = abs(
				10 * (SourceNextRowHeadPtr[iCol] - SourcePreviousRowHeadPtr[iCol]) +
				3 * (SourceNextRowHeadPtr[iCol - 1] - SourcePreviousRowHeadPtr[iCol - 1]) +
				3 * (SourceNextRowHeadPtr[iCol + 1] - SourcePreviousRowHeadPtr[iCol + 1])
			);
			GradXCurrentRowHeadPtr[iCol] = abs(
				10 * (SourceCurrentRowHeadPtr[iCol + 1] - SourceCurrentRowHeadPtr[iCol - 1]) +
				3 * (SourcePreviousRowHeadPtr[iCol + 1] - SourcePreviousRowHeadPtr[iCol - 1]) +
				3 * (SourceNextRowHeadPtr[iCol + 1] - SourceNextRowHeadPtr[iCol - 1])
			);
		}
	}
}