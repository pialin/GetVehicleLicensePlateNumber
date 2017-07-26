#include "main.h"
template <typename InputMatType>
int GetProjectX(Mat &InputMat, Mat & OutputMat)
{
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}

	Mat OutputMatTemp = Mat::zeros(
		int(InputMat.rows),
		1,
		CV_32FC1
	);

	for (int iRow = 0; iRow < InputMat.rows; iRow++)
	{
		//叠加同一行每一列的梯度值
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			*OutputMatTemp.ptr<float>(iRow) = *OutputMatTemp.ptr<float>(iRow) +
				InputMat.ptr<InputMatType>(iRow)[iCol];
		}

	}
	OutputMat = OutputMaTemp.clone();
	return 0;
}