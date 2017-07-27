#include "main.h"
template <typename InputMatType>
int GetProjectY(Mat &InputMat, Mat & OutputMat)
{
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}

	OutputMat = Mat::zeros(
		1,
		int(InputMat.rows),
		CV_8UC1
	);
	double SumTemp;
	for (int iCol = 0; iCol < InputMat.cols; iCol++)
	{
		SumTemp = 0;
		//叠加同一行每一列的梯度值
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			SumTemp  = SumTemp + InputMat.ptr<InputMatType>(iRow)[iCol];
		}
		OutputMat.ptr<uchar>(0)[iCol] = SumTemp / InputMat.rows;
			
	}
	return 0;
}