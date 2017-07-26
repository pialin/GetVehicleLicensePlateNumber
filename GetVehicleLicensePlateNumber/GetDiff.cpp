#include "main.h"
template <typename InputMatType>
int GetDiff(Mat &InputMat, Mat & OutputMat)
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

	OutputMatTemp = OutputMatTemp.clone();
	return 0;

}