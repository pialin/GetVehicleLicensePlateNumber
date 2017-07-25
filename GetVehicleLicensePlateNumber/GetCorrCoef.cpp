#include "main.h"
int GetCorrCoef(Mat &InputMatA, Mat & InputMatB, double & CorrCoef)
{
	if (InputMatA.channels() != 1 ||
		InputMatA.rows != 1 && InputMatA.cols != 1 ||
		InputMatB.channels() != 1 ||
		InputMatB.rows != 1 && InputMatB.cols != 1  ||
		InputMatA.rows != InputMatB.rows  ||
		InputMatA.cols != InputMatB.cols)
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

	double SumABTemp = 0.0;
	double SumATemp = 0.0;
	double SumAATemp = 0.0;
	double SumBTemp = 0.0;
	double SumBBTemp = 0.0;
	double MeanA, MeanB, StdDevA, StdDevB;
	if (InputMatA.cols == 1)
	{
		for (int iRow = 0; iRow < InputMatA.rows; iRow++)
		{
			SumATemp = SumATemp + *InputMatA.ptr<float>(iRow);
			SumBTemp = SumBTemp + *InputMatB.ptr<float>(iRow);
			SumABTemp = SumABTemp + *InputMatA.ptr<float>(iRow) *
				*InputMatB.ptr<float>(iRow);
			SumAATemp = SumAATemp + *InputMatA.ptr<float>(iRow)*
				*InputMatA.ptr<float>(iRow);
			SumBBTemp = SumBBTemp + *InputMatB.ptr<float>(iRow)*
				*InputMatB.ptr<float>(iRow);
		}
		CorrCoef = (SumABTemp - SumATemp * SumBTemp / InputMatA.rows) /
			sqrt((SumAATemp - SumATemp * SumATemp / InputMatA.rows)*
			(SumBBTemp - SumBTemp * SumBTemp / InputMatA.rows));
	}
	else
	{
		for (int iCol= 0; iCol < InputMatA.cols; iCol++)
		{
			SumATemp = SumATemp + InputMatA.ptr<float>[0](iCol);
			SumBTemp = SumBTemp + *InputMatB.ptr<float>[0](iCol);
			SumABTemp = SumABTemp + *InputMatA.ptr<float>[0](iCol) *
				*InputMatB.ptr<float>[0](iCol);
			SumAATemp = SumAATemp + *InputMatA.ptr<float>[0](iCol)*
				*InputMatA.ptr<float>[0](iCol);
			SumBBTemp = SumBBTemp + *InputMatB.ptr<float>[0](iCol)*
				*InputMatB.ptr<float>[0](iCol);
		}
		CorrCoef = (SumABTemp - SumATemp * SumBTemp / InputMatA.cols) /
			sqrt((SumAATemp - SumATemp * SumATemp / InputMatA.cols)*
			(SumBBTemp - SumBTemp * SumBTemp / InputMatA.cols));
	}

	return 0;

}