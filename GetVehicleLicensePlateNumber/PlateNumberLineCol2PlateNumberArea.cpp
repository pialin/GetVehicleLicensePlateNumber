#include "main.h"
int PlateNumberLineCol2PlateNumberArea
(
	int PLateNumberLineStartCol,
	int PlateNumberEndCol,
	TextLineInfo  PlateNumberLineInfo,
	Rect InputImagePlateNumberAreaRect
)
{
	double TemplateImageTitleCenterCol = int((TemplateImageTitleStartCol + TemplateImageTitleEndCol - 1.0) / 2.0);

	double TemplateImagePlateNumberRectCenterCol = TemplateImagePlateNumberRect.x + (TemplateImagePlateNumberRect.width - 1.0) / 2.0;

	//Rect InputImagePlateNumberRect = Rect(0, 0, 0, 0);

	double LicenseWidth = double(PlateNumberEndCol - PLateNumberLineStartCol) /
		double(TemplateImageTitleEndCol - TemplateImageTitleStartCol)*double(TemplateImageWidth);

	double InputImageHorizonScale = double(LicenseWidth) / double(InputImageWidth);

	if (InputImageHorizonScale >= MinMatchScale &&
		InputImageHorizonScale < MaxMatchScale)
	{
		InputImagePlateNumberAreaRect.y = PlateNumberLineInfo.StartRow;
		InputImagePlateNumberAreaRect.width = int(double(TemplateImagePlateNumberAreaRect.width) /
			double(TemplateImageWidth)*double(InputImageWidth)*InputImageHorizonScale);
		InputImagePlateNumberAreaRect.x = int((PLateNumberLineStartCol + PlateNumberEndCol - 1) / 2.0 -
			(TemplateImageTitleCenterCol - TemplateImagePlateNumberAreaCenterCol) /
			double(TemplateImageWidth)*double(InputImageWidth)*InputImageHorizonScale - InputImagePlateNumberAreaRect.width / 2.0);


		InputImagePlateNumberAreaRect.height = int(double(TemplateImagePlateNumberAreaRect.height) / double(TemplateImageHeight)*InputImageHeight*ClosestMatchScale);


	}
}