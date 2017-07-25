#include "main.h"
int D1Mat2SortedPeak(Mat & InputMat, vector<PeakInfo> &OutputPeakInfo)
{
	PeakInfo PeakInfoTemp;
	if (InputMat.cols == 1 && InputMat.channels == 1)
	{
		
		for (int iRow = 0; iRow < InputMat.rows; iRow++)
		{
			PeakInfoTemp.PeakIndex = iRow;
			PeakInfoTemp.PeakAmp = *InputMat.ptr<float>(iRow);
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
			PeakInfoTemp.PeakAmp = InputMat.ptr<float>(0)[iCol];
			if (PeakInfoTemp.PeakAmp >0)
			{
				OutputPeakInfo.push_back(PeakInfoTemp);
			}
		}
	}
	else
	{
		ofstream LogStream(LogFilePath, ios::app);
		time_t CurrentTime;
		tm CurrentLocalTime;
		char CurrentTimeString[20];
		localtime_s(&CurrentLocalTime, &CurrentTime);
		strftime(CurrentTimeString, sizeof(CurrentTimeString), "[%Y/%m/%d %X]", &CurrentLocalTime);
		LogStream << CurrentTimeString << "Error:" << endl << "Ilegal input parameter." << endl;
		return 1;
	}
	sort(OutputPeakInfo.begin(),
		OutputPeakInfo.end(),
		[](PeakInfo A, PeakInfo B) {return A.PeakAmp > B.PeakAmp; });
	return 0;
}