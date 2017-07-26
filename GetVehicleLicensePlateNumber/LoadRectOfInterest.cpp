#include "main.h"
int LoadRectOfInterest(String InputXmlFilePath,Rect OutputRectOfInterest)
{
	ifstream XmlFileStream(InputXmlFilePath);
	if (!XmlFileStream)
	{
		AppendLog("Error: Can't find xml file:" + InputXmlFilePath + ".");
		return 1;
	}

	OutputRectOfInterest = {0,0,0,0};
	tinyxml2::XMLDocument XmlDoc;
	XmlDoc.LoadFile(InputXmlFilePath.c_str());
	tinyxml2::XMLElement *LabelElement = XmlDoc.FirstChildElement("annotation")->FirstChildElement("object");
	bool IsRectOfInterestFound = false;
	if (LabelElement == nullptr)
	{
		AppendLog("Error: Can't find  'annotation' node of  xml file:" + InputXmlFilePath + ".");
		return 1;
	}
	else {

		while (LabelElement)
		{

			tinyxml2::XMLElement *LabelNameElement = LabelElement->FirstChildElement("name");
			if (LabelNameElement == nullptr)
			{
				AppendLog("Error: Can't find  'annotation' node of  xml file:" + InputXmlFilePath + ".");
				return 1;
			}
			else
			{
				String LabelName = LabelNameElement->GetText();

				if (LabelName == "14.DetectArea")
				{
					tinyxml2::XMLElement *LabelRectElement = LabelElement->FirstChildElement("bndbox");

					OutputRectOfInterest.x = atoi(LabelRectElement->FirstChildElement("xmin")->GetText()) - 1;
					OutputRectOfInterest.y = atoi(LabelRectElement->FirstChildElement("ymin")->GetText()) - 1;
					OutputRectOfInterest.width = atoi(LabelRectElement->FirstChildElement("xmax")->GetText()) - 1 - OutputRectOfInterest.x;
					OutputRectOfInterest.height = atoi(LabelRectElement->FirstChildElement("ymax")->GetText()) - 1 - OutputRectOfInterest.y;
					IsRectOfInterestFound = true;
					break;
				}
			}

			LabelElement = LabelElement->NextSiblingElement("object");
		}

	}

	if (IsRectOfInterestFound == true)
	{
		return 0;
	}
	else
	{
		AppendLog("Error: Can't find  rect of interest in  xml file:" + InputXmlFilePath + ".");
		return 1;
	}
}