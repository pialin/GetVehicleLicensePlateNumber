#include "main.h"
template <typename InputMatType>
int GetDiff
(
	Mat InputMat,
	Mat & OutputMat
)
{
	Mat  OutputMatTemp;
	if ( InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}

	OutputMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);

	for (int iRow = 0; iRow < InputMat.rows; iRow++)
	{
		//叠加同一行每一列的梯度值
		for (int iCol = 1; iCol < InputMat.cols; iCol++)
		{
			OutputMatTemp.ptr<float>(iRow)[iCol] = InputMat.ptr<InputMatType>(iRow)[iCol] - InputMat.ptr<float>(iRow)[iCol-1];
		}

	}

	float MinValue, MaxValue;
	minMaxLoc(OutputMatTemp, MinValue, MaxValue);
	OutputMatTemp.convertTo(OutputMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue* (MaxValue - MinValue) / 255.0);
	return 0;

}