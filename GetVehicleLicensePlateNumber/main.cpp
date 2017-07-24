/*添加标准库*/
//为了使用cout添加输入输出流库
//#include  <iostream>
//为了进行文件流读取和写入
#include <fstream> 


/*使用的命名空间*/
//使用C++标准库命名空间
using namespace std;


int main()
{
	const char LogFilePath[] = "E:\\Git\\GetVehicleLicensePlateNumber\\"\
		"GetVehicleLicensePlateNumber\\Log.txt";
	ofstream LogStream(LogFilePath,ios::app);
	time_t CurrentTime;


	String InputImageXmlPath = InputImagePath.substr(0, InputImagePath.rfind('.')) + ".xml";
	ifstream XFileStream(InputImageXmlPath);
	if (!XmlFileStream)
	{

	}

}