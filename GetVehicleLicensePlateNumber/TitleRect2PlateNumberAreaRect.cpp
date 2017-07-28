#include "main.h"
int TitleRect2PlateNumberAreaRect
(
	Rect  TemplateImageTitleRect,
	Rect  TemplateImagePlateNumberAreaRect,
	Size  TemplateImageSize,
	Rect  InputImageTitleRect,
	TextLineInfo  PlateNumberLineInfo,
	Size  InputImageSize,
	double  InputImageScaleY,
	
	double  & InputImageScaleX,
	Rect & InputImagePlateNumberAreaRect
)
{
	double TemplateImageTitleCenterCol = TemplateImageTitleRect.x + TemplateImageTitleRect.width/2.0;
	double InputImageTitleCenterCol = InputImageTitleRect.x + InputImageTitleRect.width / 2.0;
	double TemplateImagePlateNumberAreaCenterCol = TemplateImagePlateNumberAreaRect.x + TemplateImagePlateNumberAreaRect.width/2.0;

	InputImagePlateNumberAreaRect = Rect(0, 0, 0, 0);

	double LicenseWidth = double(InputImageTitleRect.width) /
		double(TemplateImageTitleRect.width)*double(TemplateImageSize.width);

	InputImageScaleX = double(LicenseWidth) / double(InputImageSize.width);


	InputImagePlateNumberAreaRect.y = PlateNumberLineInfo.StartRow;
	InputImagePlateNumberAreaRect.width = int(double(TemplateImagePlateNumberAreaRect.width) /
		double(InputImageSize.width)*double(InputImageSize.width)*InputImageScaleX);

	InputImagePlateNumberAreaRect.x = int(InputImageTitleCenterCol -
		(TemplateImageTitleCenterCol - TemplateImagePlateNumberAreaCenterCol) /
		double(TemplateImageSize.width)*double(InputImageSize.width)*InputImageScaleX -
		InputImagePlateNumberAreaRect.width / 2.0);


	InputImagePlateNumberAreaRect.height = int(double(TemplateImagePlateNumberAreaRect.height) /
		double(TemplateImageSize.height)*double(InputImageSize.height)*InputImageScaleY);

	return 0;
}