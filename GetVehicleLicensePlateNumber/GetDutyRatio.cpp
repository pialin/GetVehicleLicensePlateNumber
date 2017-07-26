#include "main.h"
template <typename InputMatType>
int GetDutyRatio(Mat &InputMat, double & DutyRatio)
{
	if (InputMat.channels() != 1 ||
		InputMat.rows != 1 && InputMat.cols != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}


	double SumTemp = 0.0;
	
	if (InputMat.cols == 1)
	{
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			SumTemp = SumTemp + *InputMat.ptr<InputMatType>(iRow);
			
		}
		DutyRatio = SumTemp / InputMat.rows / *InputMat.ptr<InputMatType>(0);
	}
	else
	{
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			SumTemp = SumTemp + InputMat.ptr<InputMatType>[0](iCol);
		}
		DutyRatio = SumTemp / InputMat.cols/ InputMat.ptr<InputMatType>[0](iCol);
	
	}

	
	return 0;

}