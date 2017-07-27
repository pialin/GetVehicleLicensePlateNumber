#include "main.h"
int main(int ArgumentCount, char** ArgumentVector)
{

	String  InputImagePathGlobExpression = ArgumentVector[1];

	//�������ڴ洢ͼƬ·����String����
	vector<String> InputImagePathList;

	//��������Glob���ʽ���ҷ��ϵ�ͼƬ
	glob(
		InputImagePathGlobExpression,//�ļ�����Glob���ʽ
		InputImagePathList, //���ͼ���ļ��б�
		false//�����еݹ飬���������ļ��н�������
	);

	for (int iInputImagePath = 0; iInputImagePath < InputImagePathList.size(); iInputImagePath++)
	{
		Mat Gray_InputImage;

		LoadInputImage(InputImagePathList[iInputImagePath], Gray_InputImage);

		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat InputImage_InputGradX(
			int(Gray_InputImage.rows),//����ĵ�һά���߶ȣ��ߴ�
			int(Gray_InputImage.cols), //����ĵڶ�ά����ȣ��ߴ�
			CV_32FC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);

		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat InputImage_InputGradY(
			int(Gray_InputImage.rows),//����ĵ�һά���߶ȣ��ߴ�
			int(Gray_InputImage.cols), //����ĵڶ�ά����ȣ��ߴ�
			CV_32FC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);

		//�����������ڴ��ͼ��X������ݶ�ֵ
		Mat InputImage_InputGrad(
			int(Gray_InputImage.rows),//����ĵ�һά���߶ȣ��ߴ�
			int(Gray_InputImage.cols), //����ĵڶ�ά����ȣ��ߴ�
			CV_32FC1,//�����ֵ���ͣ��������ǵ�ͨ��8λ�޷������� 
			Scalar(0)//�������ĳ�ʼֵ
		);

		GetAllGrad<uchar>(Gray_InputImage, InputImage_InputGradX, InputImage_InputGradY, InputImage_InputGrad);

		vector <TextLineInfo>  InputImageTextLineInfo;
		double ClosestMatchScale;
		TextLineSegmentation
		(
			InputImage_InputGradX,
			InputImage_InputGradY,
			InputImageTextLineInfo,
			ClosestMatchScale
		);

		
		if ((*InputImageTextLineInfo.begin()).TextLineIndex == 0)
		{
			TextLineInfo TitleLineInfo = *InputImageTextLineInfo.begin();
			Mat TitleLineGrad = InputImage_InputGrad.rowRange(TitleLineInfo.StartRow,
				TitleLineInfo.EndRow);
			int TitleStartCol, TitleEndCol;
			double TitleDutyRatio;
			GetTitleCol(TitleLineGrad,TitleLineInfo, TitleStartCol, TitleEndCol,TitleDutyRatio);
			TextLineInfo PlateNumberLineInfo = InputImageTextLineInfo[1]; 
			if (TitleDutyRatio < 0.5)
			{	
				TextLineInfo TitleLineInfoTemp = InputImageTextLineInfo[1];
				Mat TitleLineGradTemp = InputImage_InputGrad.rowRange(TitleLineInfoTemp.StartRow,
					TitleLineInfoTemp.EndRow);
				int TitleStartColTemp, TitleEndColTemp;
				double TitleDutyRatioTemp;
				GetTitleCol(TitleLineGradTemp, TitleLineInfoTemp, TitleStartColTemp, TitleEndColTemp, TitleDutyRatioTemp);
				if (TitleDutyRatioTemp > 0.8)
				{
					TitleLineInfo = TitleLineInfoTemp;
					TitleStartCol = TitleStartColTemp;
					TitleEndCol = TitleEndColTemp;
					PlateNumberLineInfo = InputImageTextLineInfo[2];
				}
			}

			TitleCol2PlateNumberArea(TitleStartCol, TitleEndCol, PlateNumberLineInfo, InputImagePlateNumberAreaRect);
			

		}
		else if ((*InputImageTextLineInfo.begin()).TextLineIndex == 1)
		{
			TextLineInfo PlateNumberLineInfo = *InputImageTextLineInfo.begin();
			Mat PlateNumberLineGrad = InputImage_InputGrad.rowRange(PlateNumberLineInfo.StartRow,
				PlateNumberLineInfo.EndRow);
			int TitleStartCol, TitleEndCol;
			GetPlateNumberLineCol(PlateNumberLineGrad, PlateNumberLineInfo, TitleStartCol, TitleEndCol);
			PlateNumberLineCol2PlateNumberArea();
		}

		InputImagePlateNumberAreaRect = InputImagePlateNumberAreaRect & Rect(0, 0, InputImageHeight, InputImageWidth);
		PlateNumberAreaGradX = InputImage_InputGradX(InputImagePlateNumberAreaRect);

	}

	

}