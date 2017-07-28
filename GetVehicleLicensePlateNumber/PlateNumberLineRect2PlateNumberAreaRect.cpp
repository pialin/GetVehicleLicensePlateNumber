#include "main.h"
int PlateNumberLineRect2PlateNumberAreaRect
(
	Rect  TemplateImagePlateNumberLineRect,
	Rect  TemplateImagePlateNumberAreaRect,
	Size  TemplateImageSize,
	Rect  InputImagePlateNumberLineRect,
	TextLineInfo  InputImagePlateNumberLineInfo,
	Size  InputImageSize,
	double InputImageScaleY,

	double & InputImageScaleX,
	Rect & InputImagePlateNumberAreaRect
)
{
	double TemplateImagePlateNumberLineCenterCol = TemplateImagePlateNumberLineRect.x + 
		TemplateImagePlateNumberLineRect.width/ 2.0;

	double TemplateImagePlateNumberRectCenterCol = TemplateImagePlateNumberAreaRect.x +
		TemplateImagePlateNumberAreaRect.width / 2.0;

	double InputImagePlateNumberLineCenterCol = InputImagePlateNumberLineRect.x + 
		InputImagePlateNumberLineRect.width / 2.0;



	//Rect InputImagePlateNumberRect = Rect(0, 0, 0, 0);

	double LicenseWidth = double(InputImagePlateNumberLineRect.width) /
		double(TemplateImagePlateNumberLineRect.width)*double(TemplateImageSize.width);

	InputImageScaleX = double(LicenseWidth) / double(InputImageSize.width);

	InputImagePlateNumberAreaRect.y = InputImagePlateNumberLineInfo.StartRow;
	InputImagePlateNumberAreaRect.width = int(double(TemplateImagePlateNumberAreaRect.width) /
		double(TemplateImageSize.width)*double(InputImageSize.width)*InputImageScaleX);

	InputImagePlateNumberAreaRect.x = int( InputImagePlateNumberLineCenterCol - 
		(TemplateImagePlateNumberLineCenterCol - TemplateImagePlateNumberRectCenterCol) /
		double(TemplateImageSize.width)*double(InputImageSize.width)*InputImageScaleX - InputImagePlateNumberAreaRect.width / 2.0);


	InputImagePlateNumberAreaRect.height = int(double(TemplateImagePlateNumberAreaRect.height) / 
		double(TemplateImageSize.height)*InputImageSize.height*InputImageScaleY);

	return 0;

}