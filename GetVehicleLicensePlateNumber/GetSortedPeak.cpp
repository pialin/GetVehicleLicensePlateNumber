#include "main.h"
template <typename InputMatType>
int GetSortedPeak(Mat & InputMat, vector<PeakInfo> &OutputPeakInfo)
{
	PeakInfo PeakInfoTemp;
	if (InputMat.cols == 1 && InputMat.channels == 1)
	{
		
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			PeakInfoTemp.PeakIndex = iRow;
			PeakInfoTemp.PeakAmp = *InputMat.ptr<InputMatType>(iRow);
			if (PeakInfoTemp.PeakAmp > 0)
			{
				OutputPeakInfo.push_back(PeakInfoTemp);
			}
		}
	}
	else if (InputMat.rows == 1 && InputMat.channels == 1)
	{
		for (int iCol = 0; iCol < InputMat.cols; iCol++)
		{
			PeakInfoTemp.PeakIndex = iCol;
			PeakInfoTemp.PeakAmp = InputMat.ptr<InputMatType>(0)[iCol];
			if (PeakInfoTemp.PeakAmp >0)
			{
				OutputPeakInfo.push_back(PeakInfoTemp);
			}
		}
	}
	else
	{
		AppendLog("Error: Ilegal input parameter.");
		return 1;
	}
	sort(OutputPeakInfo.begin(),
		OutputPeakInfo.end(),
		[](PeakInfo A, PeakInfo B) {return A.PeakAmp > B.PeakAmp; });
	return 0;
}