#include "main.h"
template <typename InputMatType>
int GetProjectY
(
	Mat InputMat, 
	Mat & OutputMat
)
{
	if (InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}

	Mat OutputMatTemp = Mat::zeros(
		1,
		int(InputMat.rows),
		CV_32FC1
	);
	for (int iCol = 0; iCol < InputMat.cols; iCol++)
	{

		//叠加同一行每一列的梯度值
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			OutputMatTemp.ptr<float>(0)[iCol] = OutputMatTemp.ptr<float>(0)[iCol] +
				InputMat.ptr<InputMatType>(iRow)[iCol];
		}
		
			
	}
	float MinValue, MaxValue;
	minMaxLoc(OutputMatTemp, MinValue, MaxValue);
	OutputMatTemp.convertTo(OutputMat, CV_8UC1, (MaxValue - MinValue) / 255.0,
		-1.0*MinValue*(MaxValue - MinValue) / 255.0);
	return 0;
}