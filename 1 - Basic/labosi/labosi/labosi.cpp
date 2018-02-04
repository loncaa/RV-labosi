/**
 * labosiLV1
 * @Author: Antonio Lonèar
 */

#include "stdafx.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <iostream>;
using namespace std;
using namespace cv;

Mat img;
bool videoIsRunning = true;
Point p1, p2;

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

		imshow("ROI", roi);
		imwrite("roi.png", roi);
	}
	else if (event == CV_EVENT_MBUTTONDOWN)
		videoIsRunning = !videoIsRunning;

}

double* quickSort(double a[], int left, int right)
{
	int i = left, j = right;
	int temp;
	int pivot = a[(left + right) / 2];

	/*left-manji , veci-right*/
	while (i <= j)
	{
		while (a[i] < pivot)
			i++;
		while (a[i] > pivot)
			j++;

		if (i <= j)
		{
			temp = a[i];
			a[i] = a[j];
			a[j] = temp;
			i++;
			j++;
		}
	};

	if (left < j)
		quickSort(a, left, j);
	if (right > i)
		quickSort(a, i, right);

	return a;
}
double calculateMedian(Mat in)
{
	double res;
	int size = in.rows * in.cols;
	double* ary = new double[size]; //alokacija niza

	/* punjenje niza */
	for (int i = 0; i < in.rows; i++)
		for (int j = 0; j < in.cols; j++)
			ary[in.cols*i + j] = in.at<double>(i, j);

	ary = quickSort(ary, 0, size-1);

	if ((in.rows + in.cols)%2 == 0)
	{
		res = (ary[size / 2] + ary[size / 2 + 1]) / 2;
	}
	else
	{
		res = ary[size];
	}

	delete[] ary;
	return res;
}

/* RGB to canny */
void cannyImage(Mat cannyImg)
{
	Mat grayImg, outImg;
	double sigma = 0.33, v; //v - median intenziteta slike
	int lowerb, upperb, size, sum = 0;

	cvtColor(cannyImg, grayImg, CV_BGR2GRAY);
	GaussianBlur(grayImg, grayImg, Size(3, 3), 1, 1, BORDER_DEFAULT);
	size = grayImg.cols*grayImg.rows;

	/* alokacija niza */
	double* ary = new double[size]; 

	/* punjenje niza */
	for (int i = 0; i < grayImg.rows; i++)
		for (int j = 0; j < grayImg.cols; j++)
		{
		sum += grayImg.at<uchar>(i, j);
		}

	v = sum / size;

	lowerb = (int)max((double)0, (1.0 - sigma)*v);
	upperb = (int)min((double)255, (1.0 + sigma)*v);

	Canny(grayImg, outImg, lowerb, upperb);
	imshow("Canny", outImg);
}

int _tmain(int argc, _TCHAR* argv[])
{
	Mat frame;
	VideoCapture cap(0);

	namedWindow("Window");
	setMouseCallback("Window", onMouse, NULL);

	img = imread("cover.jpg");
	
	cannyImage(img);

	imshow("Window", img);

	if (!cap.isOpened())
		return -1;

	while (videoIsRunning)
	{
		cap >> frame;
		imshow("Video", frame);
		waitKey(15);
	}

	waitKey(0);
	return 0;
}

