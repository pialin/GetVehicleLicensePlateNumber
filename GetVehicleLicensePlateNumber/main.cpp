/*��ӱ�׼��*/
//Ϊ��ʹ��cout��������������
//#include  <iostream>
//Ϊ�˽����ļ�����ȡ��д��
#include <fstream> 


/*ʹ�õ������ռ�*/
//ʹ��C++��׼�������ռ�
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