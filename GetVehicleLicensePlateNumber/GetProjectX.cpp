#include "main.h"
int GetProjectX(Mat &InputMat, Mat & OutputMat)
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
		int(InputMat.rows),
		1,
		CV_32FC1
	);

	double SumTemp;

	uchar * InputCurrentRowHeadPtr = 0;
	float * OutputCurrentRowHeadPtr = 0;


	for (int iRow = 0; iRow < InputMat.rows; iRow++)
	{
		//每次叠加前将加和变量清零
		SumTemp = 0;
		InputCurrentRowHeadPtr = InputMat.ptr<uchar>(iRow);
		OutputCurrentRowHeadPtr = OutputMat.ptr<float>(iRow);
		//叠加同一行每一列的梯度值
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			OutputCurrentRowHeadPtr[0] += InputCurrentRowHeadPtr[iCol];
		}

	}

}