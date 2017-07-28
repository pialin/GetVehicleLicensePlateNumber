#include "main.h"
int FilterSortedPeak
(
	vector<PeakInfo> InputPeakInfo,
	double MinPeakGap,
	vector<PeakInfo> & OutputPeakInfo
)
{
	bool FlagKeepCurrentPeak;
	vector<PeakInfo>  OutputPeakInfoTemp;
	for (int iPeak = 0; iPeak < InputPeakInfo.size(); iPeak++)
	{
		FlagKeepCurrentPeak = true;
		for (int jPeak = 0; jPeak < OutputPeakInfo.size(); jPeak++)
		{
			if (abs(InputPeakInfo[iPeak].PeakIndex - OutputPeakInfo[jPeak].PeakIndex)
				< MinPeakGap)
			{
				FlagKeepCurrentPeak = false;
				break;
			}
		}
		if (FlagKeepCurrentPeak == true)
		{
			OutputPeakInfoTemp.push_back(InputPeakInfo[iPeak]);
		}
	}
	OutputPeakInfo = OutputPeakInfoTemp;
	return 0;
}