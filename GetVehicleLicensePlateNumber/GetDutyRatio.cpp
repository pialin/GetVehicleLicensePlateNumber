#include "main.h"
int GetDutyRatio(Mat &InputMat, double & DutyRatio)
{
	if (InputMat.channels() != 1 ||
		InputMat.rows != 1 && InputMat.cols != 1)
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


	double SumTemp = 0.0;
	
	if (InputMat.cols == 1)
	{
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			SumTemp = SumTemp + *InputMat.ptr<uchar>(iRow);
			
		}
		DutyRatio = SumTemp / InputMat.rows / *InputMat.ptr<uchar>(0);
	}
	else
	{
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			SumTemp = SumTemp + InputMat.ptr<uchar>[0](iCol);
		}
		DutyRatio = SumTemp / InputMat.cols/ InputMat.ptr<uchar>[0](iCol);
	
	}

	
	return 0;

}