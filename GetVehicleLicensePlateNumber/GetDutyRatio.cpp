#include "main.h"
template <typename InputMatType>
int GetDutyRatio
(
	Mat InputMat, 
	double & DutyRatio
)
{
	if (InputMat.channels() != 1 ||
		InputMat.rows != 1 && InputMat.cols != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}


	double SumTemp = 0.0;
	double OneValue;
	if (InputMat.cols == 1)
	{
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			SumTemp = SumTemp + *InputMat.ptr<InputMatType>(iRow);
			
		}
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			if (*InputMat.ptr<InputMatType>(iRow) > 0)
			{
				OneValue = *InputMat.ptr<InputMatType>(iRow);
				break;
			}

		}
		DutyRatio = SumTemp / InputMat.rows / OneValue;
	}
	else
	{
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			SumTemp = SumTemp + InputMat.ptr<InputMatType>[0](iCol);
		}

		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			if (InputMat.ptr<InputMatType>[0](iCol) > 0)
			{
				OneValue = InputMat.ptr<InputMatType>[0](iCol);
				break;
			}

		}
		DutyRatio = SumTemp / InputMat.cols/ OneValue;
	
	}

	
	return 0;

}