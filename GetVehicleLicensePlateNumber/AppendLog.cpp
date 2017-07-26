#include "main.h"
int AppendLog(String LogInfo)
{
	ofstream LogStream(LogFilePath, ios::app);
	time_t CurrentTime;
	tm CurrentLocalTime;
	char CurrentTimeString[20];
	localtime_s(&CurrentLocalTime, &CurrentTime);
	strftime(CurrentTimeString, sizeof(CurrentTimeString), "[%Y/%m/%d %X]", &CurrentLocalTime);
	LogStream << CurrentTimeString << __FUNCTION__  << "("  << __LINE__ << ")" << endl <<
		LogInfo << endl;
	LogStream.close();
}