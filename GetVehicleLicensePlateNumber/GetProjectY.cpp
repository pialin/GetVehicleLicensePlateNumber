#include "main.h"
int GetProjectY(Mat &InputMat, Mat & OutputMat)
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

	OutputMat = Mat::zeros(
		1,
		int(InputMat.rows),
		CV_32FC1
	);

	for (int iCol = 0; iCol < InputMat.cols; iCol++)
	{
		//叠加同一行每一列的梯度值
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			OutputMat.ptr<float>(0)[iCol] = OutputMat.ptr<float>(0)[iCol] +
				InputMat.ptr<uchar>(iRow)[iCol];
		}

	}
	return 0;
}