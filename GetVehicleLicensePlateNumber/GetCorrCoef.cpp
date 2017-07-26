#include "main.h"
template <typename InputMatType>
int GetCorrCoef(Mat &InputMatA, Mat & InputMatB, double & CorrCoef)
{
	if (InputMatA.channels() != 1 ||
		InputMatA.rows != 1 && InputMatA.cols != 1 ||
		InputMatB.channels() != 1 ||
		InputMatB.rows != 1 && InputMatB.cols != 1  ||
		InputMatA.rows != InputMatB.rows  ||
		InputMatA.cols != InputMatB.cols)
	{
		AppendLog("Error: Ilegal input parameter.");
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

			SumATemp = SumATemp + *InputMatA.ptr<InputMatType>(iRow);
			SumBTemp = SumBTemp + *InputMatB.ptr<InputMatType>(iRow);
			SumABTemp = SumABTemp + *InputMatA.ptr<InputMatType>(iRow) *
				*InputMatB.ptr<InputMatType>(iRow);
			SumAATemp = SumAATemp + *InputMatA.ptr<InputMatType>(iRow)*
				*InputMatA.ptr<InputMatType>(iRow);
			SumBBTemp = SumBBTemp + *InputMatB.ptr<InputMatType>(iRow)*
				*InputMatB.ptr<InputMatType>(iRow);
		}
		CorrCoef = (SumABTemp - SumATemp * SumBTemp / InputMatA.rows) /
			sqrt((SumAATemp - SumATemp * SumATemp / InputMatA.rows)*
			(SumBBTemp - SumBTemp * SumBTemp / InputMatA.rows));
	}
	else
	{
		for (int iCol= 0; iCol < InputMatA.cols; iCol++)
		{
			SumATemp = SumATemp + InputMatA.ptr<InputMatType>[0](iCol);
			SumBTemp = SumBTemp + *InputMatB.ptr<InputMatType>[0](iCol);
			SumABTemp = SumABTemp + *InputMatA.ptr<InputMatType>[0](iCol) *
				*InputMatB.ptr<InputMatType>[0](iCol);
			SumAATemp = SumAATemp + *InputMatA.ptr<InputMatType>[0](iCol)*
				*InputMatA.ptr<InputMatType>[0](iCol);
			SumBBTemp = SumBBTemp + *InputMatB.ptr<InputMatType>[0](iCol)*
				*InputMatB.ptr<InputMatType>[0](iCol);
		}
		CorrCoef = (SumABTemp - SumATemp * SumBTemp / InputMatA.cols) /
			sqrt((SumAATemp - SumATemp * SumATemp / InputMatA.cols)*
			(SumBBTemp - SumBTemp * SumBTemp / InputMatA.cols));
	}

	return 0;

}