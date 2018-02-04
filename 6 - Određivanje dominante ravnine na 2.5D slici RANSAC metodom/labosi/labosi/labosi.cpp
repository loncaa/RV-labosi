/**
 * labosiLV6
 * @Author: Antonio Lonèar
 */

#include "stdafx.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <iostream>;
using namespace std;
using namespace cv;

struct RV3DPOINT
{
	int u, v, d;
};

inline int RVRandom(int yMin, int yMax)
{
	return  (rand() % yMax) + yMin;
}

void ReadKinectPics(char* pathDepth, Mat DepthImage, int *DepthMap, RV3DPOINT *Point3DArray, int &n3DPoints)
{

	int u, v, d;
	int dmin = 2047;
	int dmax = 0;

	RV3DPOINT *pPoint3D = Point3DArray;

	n3DPoints = 0;
	FILE *fp;

	fopen_s(&fp, pathDepth, "r");

	if (fp)
	{
		bool fileReaded = true;

		//Determine max and min depth values and get Depth map
		for (v = 0; v < DepthImage.rows; v++)
		{
			for (u = 0; u < DepthImage.cols; u++)
			{
				if (!(fileReaded = (fscanf(fp, "%d ", &d) == 1)))
					break;


				if (d == 2047)
				{
					d = -1;
				}
				else
				{
					//count number of valid 3D points
					n3DPoints++;

					//determine min and max d
					if (d<dmin)
						dmin = d;

					if (d>dmax)
						dmax = d;
				}

				DepthMap[v*DepthImage.cols + u] = d;


				if (d != -1)
				{
					pPoint3D->u = u;
					pPoint3D->v = v;
					pPoint3D->d = d;

					pPoint3D++; //povecava adresu pokazivaca za 1
				}


			}
		}

		fclose(fp);
	}


	//Form grayscale pic -> Scale from 1 to 255 (reserve 0 for undefined regions)
	for (v = 0; v<DepthImage.rows; v++)
	{
		for (u = 0; u<DepthImage.cols; u++)
		{
			d = DepthMap[v*DepthImage.cols + u];

			if (d != -1)
				d = ((d - dmin) * 254 / (dmax - dmin)) + 1;
			else
				d = 0;

			// ??
			((uchar *)(DepthImage.data + v*DepthImage.step))[u] = d;
		}
	}


}

bool Plane(RV3DPOINT *pPoint3D0, RV3DPOINT *pPoint3D1, RV3DPOINT *pPoint3D2, double &a, double &b, double &c)
{
	/* matrica koordinata tri tocke na slici */
	Mat A(3, 3, CV_64FC1);
	A.at<double>(0, 0) = pPoint3D0->u;
	A.at<double>(0, 1) = pPoint3D0->v;
	A.at<double>(0, 2) = 1.0;

	A.at<double>(1, 0) = pPoint3D1->u;
	A.at<double>(1, 1) = pPoint3D1->v;
	A.at<double>(1, 2) = 1.0;

	A.at<double>(2, 0) = pPoint3D2->u;
	A.at<double>(2, 1) = pPoint3D2->v;
	A.at<double>(2, 2) = 1.0;

	Mat Z(3, 1, CV_64FC1);
	Z.at<double>(0, 0) = pPoint3D0->d;
	Z.at<double>(1, 0) = pPoint3D1->d;
	Z.at<double>(2, 0) = pPoint3D2->d;

	Mat p(3, 1, CV_64FC1);
	if (solve(A, Z, p)) //rješava linearne jednadðbe, rezultat sprema u p
	{
		a = p.at<double>(0, 0);
		b = p.at<double>(1, 0);
		c = p.at<double>(2, 0);

		A.release();
		Z.release();
		p.release();

		return true;
	}
	else
	{
		A.release();
		Z.release();
		p.release();

		return false;
	}
}

void Consensus(RV3DPOINT *Point3DArray, int n3DPoints, double &a, double &b, double &c, RV3DPOINT **ConsensusSet, int &nConsensus)
{
	RV3DPOINT *pPoint3D = Point3DArray;

	RV3DPOINT **pConsensusSet = ConsensusSet;
	double Tol = 4.0;

	int i;
	double e;

	nConsensus = 0;

	for (i = 0; i < n3DPoints; i++, pPoint3D++)
	{
		e = pPoint3D->d - (a * pPoint3D->u + b * pPoint3D->v + c); // [?] sta je pPoint3D->d

		if (e > Tol)
			continue;

		if (e >= -Tol)
			*(pConsensusSet++) = pPoint3D;


	}

	nConsensus = pConsensusSet - ConsensusSet;

}

int _tmain(int argc, _TCHAR* argv[])
{

	Mat pRGBImage;
	Mat pDepthImage;
	int *DepthMap = 0;
	int n3DPoints = 0;

	int iWidth, iHeight;

	RV3DPOINT *Point3DArray = 0;

	char* pathRGB = "KinectPics\\sl-00242.bmp";
	char* pathDepth = "KinectPics\\sl-00242-D.txt";


	//Get RGB image
	pRGBImage = imread(pathRGB);
	iWidth = pRGBImage.rows;
	iHeight = pRGBImage.cols;

	DepthMap = new int[iWidth * iHeight];
	memset(DepthMap, 0, iWidth * iHeight*sizeof(int));

	Point3DArray = new RV3DPOINT[iWidth * iHeight];
	memset(Point3DArray, 0, iWidth * iHeight * sizeof(RV3DPOINT));

	pDepthImage = Mat(iWidth, iHeight, CV_8UC1, Scalar(0));


	ReadKinectPics(pathDepth, pDepthImage, DepthMap, Point3DArray, n3DPoints);

	namedWindow("RGB image");
	imshow("RGB image", pRGBImage);
	imwrite("rgb.png", pRGBImage);

	namedWindow("Depth image");
	imshow("Depth image", pDepthImage);
	imwrite("depth.png", pDepthImage);


	double a, b, c;
	double aBest, bBest, cBest;

	int nConsensus;

	int nBest = 0;


	int iter;

	int maxRANSACIter = 1000; //Max RANSAC iterations

	RV3DPOINT *pPoint3D, *pPoint3D0, *pPoint3D1, *pPoint3D2;

	RV3DPOINT **Point3DPtrBuff1 = new RV3DPOINT *[n3DPoints];
	RV3DPOINT **Point3DPtrBuff2 = new RV3DPOINT *[n3DPoints];

	RV3DPOINT **ConsensusSet = Point3DPtrBuff1;
	RV3DPOINT **BestConsensusSet = Point3DPtrBuff2;

	RV3DPOINT **tmp;


	int iR0, iR1, iR2;

	//main RANSAC loop
	for (iter = 0; iter<maxRANSACIter; iter++)
	{
		// randomly select 3 points from set A
		iR0 = RVRandom(0, n3DPoints - 1);
		iR1 = RVRandom(0, n3DPoints - 1);
		iR2 = RVRandom(0, n3DPoints - 1);

		pPoint3D0 = (Point3DArray + iR0);
		pPoint3D1 = (Point3DArray + iR1);
		pPoint3D2 = (Point3DArray + iR2);

		if (!Plane(pPoint3D0, pPoint3D1, pPoint3D2, a, b, c))
			continue;

		Consensus(Point3DArray, n3DPoints, a, b, c, ConsensusSet, nConsensus);

		//Test best values and consensus set
		if (nConsensus > nBest)
		{
			nBest = nConsensus;

			aBest = a;
			bBest = b;
			cBest = c;

			tmp = ConsensusSet;
			ConsensusSet = BestConsensusSet;
			BestConsensusSet = tmp;
		}

	}

	Mat pColor(pDepthImage.rows, pDepthImage.cols, CV_8UC1, 3);
	cvtColor(pDepthImage, pColor, CV_GRAY2RGB);

	/* [?] kakva je ovo sintaksa */
	for (int i = 0; i<nBest; i++)
	{
		pPoint3D = BestConsensusSet[i];
		((uchar *)(pColor.data + pPoint3D->v*pColor.step))[pPoint3D->u * 3] = 0;
		((uchar *)(pColor.data + pPoint3D->v*pColor.step))[pPoint3D->u * 3 + 1] = 255;
		((uchar *)(pColor.data + pPoint3D->v*pColor.step))[pPoint3D->u * 3 + 2] = 0;

	}

	namedWindow("Dominant plane");
	imshow("Dominant plane", pColor);
	imwrite("dom.png", pColor);

	waitKey(0);

	//exit elegantly 
	delete[] DepthMap;
	delete[] Point3DArray;
	delete[] Point3DPtrBuff1;
	delete[] Point3DPtrBuff2;

	return 0;
}

