#include "main.h"
template <typename InputMatType>
int GetAllGrad(Mat& InputMat, Mat& OutputGradXMat, Mat& OutputGradYMat,Mat& OutputGradMat)
{

	if ( InputMat.channels() != 1)
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}
	Mat OutputGradXMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);

	Mat OutputGradYMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);

	Mat OutputGradMatTemp = Mat::zeros(
		int(InputMat.rows),
		int(InputMat.cols),
		CV_32FC1
	);


	//逐个像素计算垂直梯度，上下左右边缘不作计算，其值为填充的初始值0
	for (int iRow = 1; iRow < InputMat.rows - 1; iRow++)
	{

		for (int iCol = 1; iCol < InputMat.cols - 1; iCol++)
		{
			OutputGradXMatTemp.ptr<float>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<InputMatType>(iRow)[iCol + 1] - InputMat.ptr<InputMatType>(iRow)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow-1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow-1)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow+1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow+1)[iCol - 1])
			);
			OutputGradYMatTemp.ptr<float>(iRow)[iCol] = abs(
				10 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol] - InputMat.ptr<InputMatType>(iRow - 1)[iCol]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol - 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol - 1]) +
				3 * (InputMat.ptr<InputMatType>(iRow + 1)[iCol + 1] - InputMat.ptr<InputMatType>(iRow - 1)[iCol + 1])
			);

			OutputGradMatTemp.ptr<float>(iRow)[iCol] = abs(OutputGradXMatTemp.ptr<float>(iRow)[iCol]) + 
				abs(OutputGradYMatTemp.ptr<float>(iRow)[iCol]);
				
			);
		}
	}

	OutputGradXMat = OutputGradXMatTemp.clone();
	OutputGradYMat = OutputGradXMatTemp.clone();
	OutputGradMat = OutputGradMatTemp.clone();
	return 0;
}