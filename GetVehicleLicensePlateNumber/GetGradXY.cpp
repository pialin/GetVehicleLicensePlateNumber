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

	uchar * SourceCurrentRowHeadPtr = 0, *SourcePreviousRowHeadPtr = 0, *SourceNextRowHeadPtr = 0;

	uchar * GradYCurrentRowHeadPtr = 0, *GradXCurrentRowHeadPtr = 0;

	//������ؼ��㴹ֱ�ݶȣ��������ұ�Ե�������㣬��ֵΪ���ĳ�ʼֵ0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{
		SourceCurrentRowHeadPtr = InputMat.ptr<uchar>(iRow);
		SourcePreviousRowHeadPtr = InputMat.ptr<uchar>(iRow - 1);
		SourceNextRowHeadPtr = InputMat.ptr<uchar>(iRow + 1);

		GradXCurrentRowHeadPtr = OutputGradXMat.ptr<uchar>(iRow);
		GradYCurrentRowHeadPtr = OutputGradYMat.ptr<uchar>(iRow);

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
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