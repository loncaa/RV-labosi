/**
 * labosiLV2
 * @Author: Antonio Lonèar
 */

#include "stdafx.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <iostream>
#include <fstream>
using namespace std;
using namespace cv;

Mat img, res;
Point p1, p2;


void matchImage(Mat inputImage, Mat ROI, double thresHold)
{
	Mat resultCCORR, resultSQDIFF, resultCCOEFF; //slika na kojoj se trazi podudazranje i koja se obraðuje
	Mat imgCCORR, imgSQDIFF, imgCCOFF; //slika na kojoj se kvadratom oznaci mjesto podudaranja
	int nCCORR, nSQDIFF, nCCOEFF;

	inputImage.copyTo(imgCCORR);
	inputImage.copyTo(imgSQDIFF);
	inputImage.copyTo(imgCCOFF);


	/*
	For the methods CV_SQDIFF and CV_SQDIFF_NORMED the best match are the lowest values.
	*/
	cvtColor(inputImage, inputImage, CV_BGR2GRAY);
	cvtColor(ROI, ROI, CV_BGR2GRAY);

	matchTemplate(inputImage, ROI, resultCCORR, CV_TM_CCORR_NORMED);
	matchTemplate(inputImage, ROI, resultSQDIFF, CV_TM_SQDIFF_NORMED);
	matchTemplate(inputImage, ROI, resultCCOEFF, CV_TM_CCOEFF_NORMED);
	
	imwrite("resultCCORR.png", resultCCORR);
	imwrite("resultSQDIFF.png", resultSQDIFF);
	imwrite("resultCCOEFF.png", resultCCOEFF);
	
	/* Obrada slike pronaðena na internetu */

	//threshold(resultCCORR, resultCCORR, thresHold + (1 - thresHold) / 2, 1., CV_THRESH_TOZERO); //manje od 0.85 spusti na 0 - inverzno
	//threshold(resultSQDIFF, resultSQDIFF, 1 - thresHold, 1., CV_THRESH_BINARY_INV); //manje od 0.85 spusti na 0
	//threshold(resultCCOEFF, resultCCOEFF, thresHold, 1., CV_THRESH_TOZERO); //manje od 0.85 spusti na 0

	imshow("WindowRes resultCCOEFF", resultCCOEFF); // najbolji
	imshow("WindowRes resultSQDIFF", resultSQDIFF); // inverzan
	imshow("WindowRes resultCCORR", resultCCORR); //za iste stvari kao i drugi ima puno vece vrijednoti pojavljivanja
	//kada se oznaci svjetlije slovo CCEFF pokazuje najbolje dok drugi polude

	imwrite("resultCCORR.png", resultCCORR);
	imwrite("resultSQDIFF.png", resultSQDIFF);
	imwrite("resultCCOEFF.png", resultCCOEFF);

	/* Prolazi kroz obraðenu sliku i pronalazi tock gdje je vrijednost veca o 0.85*/
	int n = 0;
	for (int i = 0; i < resultCCORR.cols; i++) //stupci
	{
		for (int j = 0; j < resultCCORR.rows; j++)
		{
			if (resultCCORR.at<float>(j, i) >= thresHold + (1 - thresHold)/2)
			{
				rectangle(imgCCORR, Rect(i, j, ROI.cols, ROI.rows), Scalar(0, 0, 255));
				nCCORR++;
			}

			if (resultSQDIFF.at<float>(j, i) <= 1 - thresHold)
			{
				rectangle(imgSQDIFF, Rect(i, j, ROI.cols, ROI.rows), Scalar(0, 0, 255));
				nSQDIFF++;
			}

			if (resultCCOEFF.at<float>(j, i) >= thresHold)
			{
				rectangle(imgCCOFF, Rect(i, j, ROI.cols, ROI.rows), Scalar(0, 0, 255));
				nCCOFF++;
			}

		}
	}

	imwrite("imgCCORR.png", imgCCORR);
	imwrite("imgSQDIFF.png", imgSQDIFF);
	imwrite("imgCCOFF.png", imgCCOFF);

	//cout << "Broj pronadenih podudaranja: " << n << endl;

	imshow("WindowRes imgCCORR", imgCCORR);
	imshow("WindowRes imgSQDIFF", imgSQDIFF);
	imshow("WindowRes imgCCOFF", imgCCOFF);

}

void onMouse(int event, int x, int y, int flags, void* param) {


	if (event == CV_EVENT_LBUTTONDOWN) {
		p1 = Point(x, y);
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		Mat roi;
		p2 = Point(x, y);

		Rect roiRect = Rect(p1, p2);
		roi = img(roiRect);

		matchImage(img, roi, 0.88);

		
		imwrite("roi.bmp", roi);
		/*
		imshow("ROI", roi);
		*/
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	namedWindow("Window");
	setMouseCallback("Window", onMouse, NULL);

	img = imread("Pic_TemplateMatch.bmp");

	imshow("Window", img);
	waitKey(0);
	return 0;
}

