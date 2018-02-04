// labosi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\calib3d\calib3d.hpp>

#include <iostream>
#include <fstream>
using namespace std;
using namespace cv;

bool isCalculated = false;
bool isPressedS = false;
bool cam = true;
int klik = 0;
const int brojTocaka = 4;

vector<Point3f> objectPoints; //tocke u stvarnome 3d prostoru, visina sirina duzina
vector<Point2f> imagePoints, newHomoPoints, objectPoints2f; //tocke na slici koju uslika kamera
Mat distortion, intrinsics;

Point3f m; // koordinate tocke na slici
Point3f p; //koordinate tocke u stvarnome svijetu



//rotacijski vektor, translacijski vektor, rotacijska matrica
Mat rvec, tvec, rmat;
Mat A, b;
Mat savedFrame, roi;


/* solvePnP - http://docs.opencv.org/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html#solvepnp */

void resetPoints()
{
	klik = 0;
	imagePoints.clear();
}

void onMouse(int event, int x, int y, int flags, void* param) {

	if (event == CV_EVENT_LBUTTONDOWN && klik < brojTocaka) {

		imagePoints.push_back(Point2f(x, y));
		//imagePoints2f[klik++] = Point2f(x, y);
		klik++;
	}
	else if (klik == brojTocaka)
	{
		Mat wrapedPerspective, h;

		cout << "Oznacene tocke su:" << endl;
		for (int i = 0; i < imagePoints.size(); i++)
			cout << imagePoints.at(i).x << " " << imagePoints.at(i).y << endl;

		/* ispravljanje slike sa matricom homografije */
		h = getPerspectiveTransform(imagePoints, objectPoints2f); //odreðivanje matrice homografije sa 4 para tocaka
		warpPerspective(savedFrame, wrapedPerspective, h, Size(640, 480));
		perspectiveTransform(imagePoints, newHomoPoints, h);

		/* odreðivanje roi-a sa nove slike koju smo dobili matricom homografije */
		roi = wrapedPerspective(Rect(Point(0, 0), newHomoPoints.at(3)));
		//roi = savedFrame(Rect(imagePoints.at(0), imagePoints.at(3))); // ne koristi se matrica homografije
		imshow("Roi", roi);
		imwrite("roi.png", roi);

		cout << "3. Pritisni desnu tipku misa za dalje!" << endl;
		klik++; // samo kako bi uvjet bio falase;
	}
	else if (event == CV_EVENT_MBUTTONDOWN){
		resetPoints();
	}
	else if (klik == (brojTocaka+1) && event == CV_EVENT_RBUTTONDOWN)
	{
		//object points - 4 tocke milimetarskoga papira
		solvePnP(objectPoints, newHomoPoints, intrinsics, distortion, rvec, tvec); //rotacijska matrica, translacijski vektor, koristi se matrica homografije
		//solvePnP(objectPoints, imagePoints, intrinsics, distortion, rvec, tvec); //rotacijska matrica, translacijski vektor

		Rodrigues(rvec, rmat); //Rodrigues - za rotacijsku matricu iz rvec, 3x1 -> 3x3
	
		/*matrice koje se mnoze moraju biti istoga tipa, mi smo uzeli intristic.type() = 5*/
		rmat.convertTo(rmat, intrinsics.type()); //rotmacijska matrica
		tvec.convertTo(tvec, intrinsics.type()); //translacijski vektro

		A = intrinsics*rmat; //[3x3]
		b = intrinsics*tvec; //[3x1]		

		resetPoints();
		isCalculated = true;
		cout << "4. Racunanje zavrseno!\nPritisni slovo 'h' za nastavak.\n" << endl;
	}
}

void releaseAll()
{
	distortion.release();
	intrinsics.release();
	rvec.release();
	tvec.release();
	rmat.release();
	A.release();
	b.release();
	savedFrame.release();
	roi.release();
}

/* èitanje Distortion.xml i Intrinsics.xml fajla, dugacak naziv samo da se lakse skonta sta radi*/
void loadDistortionAndIntrinsicsXml()
{
	FileStorage fs;
	fs.open("Distortion.xml", FileStorage::READ);
	fs["Distortion"] >> distortion;
	fs.release();

	fs.open("Intrinsics.xml", FileStorage::READ);
	fs["Intrinsics"] >> intrinsics;
	fs.release();
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "~~Manual<<\n1.pritisni slovo c (capture)\n2.odredi 4 tocke koje se nalaze narubovima milimetarskoga papira\n3.stisni desnu tipku misa\n4.stisni slovo h\n\n\n1. Pritisni slovo c!" << endl;
	VideoCapture cap(1);
	if (!cap.isOpened())
		return -1;

	char c;
	double width = cap.get(CV_CAP_PROP_FRAME_WIDTH), height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	double rho, theta;
	Mat frame, map1, map2;
	Size imageSize(width, height);
	vector<Vec2f> houghLines;

	namedWindow("SavedImage");
	setMouseCallback("SavedImage", onMouse, NULL);

	/*  hardcodane tocke milimetarskoga papira u 3d prostoru, dimenzije A4 papira */
	objectPoints.push_back(Point3f(0, 0, 0)); //[*][][][][][][][*]
	objectPoints.push_back(Point3f(297, 0, 0));
	objectPoints.push_back(Point3f(0, 210, 0));
	objectPoints.push_back(Point3f(297, 210, 0)); //[*][][][][][][][*]

	/* hardcodane tocke milimetarskoga papira ali u 2d prostoru za izracun matrice homografije */
	objectPoints2f.push_back(Point2f(0, 0));
	objectPoints2f.push_back(Point2f(297, 0));
	objectPoints2f.push_back(Point2f(0, 210));
	objectPoints2f.push_back(Point2f(297, 210));

	loadDistortionAndIntrinsicsXml();

	/* pravi map1, map2 koje kasnije koristimo za ispravljanje slike */
	initUndistortRectifyMap(
		intrinsics, distortion, Mat(),
		getOptimalNewCameraMatrix(intrinsics, distortion, imageSize, 1, imageSize, 0), 
		imageSize, CV_16SC2, map1, map2
		);	

	namedWindow("Cam");
	while (cam)
	{
		cap >> frame;

		/* funckija za ispravljanje slike*/
		//remap(frame, frame, map1, map2, INTER_LINEAR);
		imshow("Cam", frame);

		c = waitKey(1);
		if (c == 'c') // c kao capture
		{
			resetPoints();
			savedFrame = frame.clone();
			imshow("SavedImage", savedFrame);
			cout << "2. Slika uslikana! Odredi cetiri tocke." << endl;
		}
		else if (isCalculated && c == 'h') // h kao hough
		{
			double alfaX, alfaY, alfaP;
			double rhoCrtica, thetaCrtica;
			Mat canny;
			
			/*ako ne uzme roi, onda je najdominantnija linija, linija papira*/
			Canny(roi, canny, 20, 200);
			imshow("canny", canny);

			/* vraca tocku sjecista linije i pravca iz ishodišta */
			HoughLines(canny, houghLines, 1, CV_PI / 180, 100);
			if (houghLines.size() > 0)
			{

				rho = houghLines[0][0]; // rho od linije 0 (najdominantnija)
				theta = houghLines[0][1]; // theta od linije 0 (najdominantnija)

				/* racunanje alfi */
				alfaX = A.at<float>(0, 0)*cos(theta) + A.at<float>(1, 0)*sin(theta) - rho*A.at<float>(2, 0);
				alfaY = A.at<float>(0, 1)*cos(theta) + A.at<float>(1, 1)*sin(theta) - rho*A.at<float>(2, 1);
				alfaP = b.at<float>(2, 0)*rho - b.at<float>(0, 0)*cos(theta) - b.at<float>(1, 0)*sin(theta);

				/* rho' i theta' za k.s. u milimetarskog papira */
				rhoCrtica = (alfaP) / sqrt(pow(alfaX, 2) + pow(alfaY, 2));
				thetaCrtica = atan2(alfaY, alfaX);
				cout << "\n\nrho: " << rho << " rho': " << rhoCrtica << "\ntheta: " << theta << " theta'': " << thetaCrtica << endl;
				
				{
					/* crtanje dominantne linije na roi-u */
					double xcos = cos(theta), ysin = sin(theta); //p = x*cos(theta) + y*cos(theta)
					double x0 = rho*xcos, y0 = rho*ysin;

					line(roi,
						Point(x0 + 1000 * (-ysin), y0 + 1000 * (xcos)),
						Point(x0 - 1000 * (-ysin), y0 - 1000 * (xcos)),
						Scalar(0, 255, 0), 1); /* polarne koordinate u kartezijev koordinatni sustav */

					/* crtanje paralelnih linija*/
					int paralLines = 0;
					for (int i = 1; i < houghLines.size(); i++)
					{

						double _theta = houghLines[i][1];
						/* linije koje imaju istu thetu su paralelne */
						if (_theta == theta)
						{
							double _rho = houghLines[i][0];
							double _xcos = cos(_theta), _ysin = sin(_theta);

							double _x0 = _rho*_xcos;
							double _y0 = _rho*_ysin;

							line(roi,
								Point(_x0 + 500 * (-ysin), _y0 + 500 * (xcos)),
								Point(_x0 - 500 * (-ysin), _y0 - 500 * (xcos)),
								Scalar(0, 0, 255), 1);

							paralLines++;
						}
					}

					if (paralLines > 0)
						cout << "Broj paralelnih linija sa dominantnim pravcem je: " << paralLines << endl;
					else
						cout << "Ne moze pronaci linije paralelne sa dominantnim pravcem!" << endl;


					/* Odreðivanje tocaka dominantne linije na milimetarskome papiru.
						p = x*cos(theta) + y*cos(theta) - koordinate dominantnoga prvaca u stvarnome prostoru */
					xcos = cos(thetaCrtica);
					ysin = sin(thetaCrtica);
					x0 = rhoCrtica*xcos;
					y0 = rhoCrtica*ysin;

					/* ispisti tocke koje se nalaze unutar x: 0 - 297 y:0 - 210*/
					cout << "\nTocke na papiru: " << endl;
					for (int i = 0; i < 1000; i++)
					{
						double y1 = y0 + i * (xcos);
						double y2 = y0 - i * (xcos);

						if (y1 >= 0 && y1 <= 210)
						{
							double x1 = x0 + i * (-ysin);
							if (x1 >= 0 && x1 <= 297)
								cout << "x:" << x1 << " " << y1 << endl;
						}
						if (y2 >= 0 && y2 <= 210)
						{
							double x2 = x0 - i * (-ysin);
							if (x2 >= 0 && x2 <= 297)
								cout << "x:" << x2 << " " << y2 << endl;
						}
					}
				}

				
				imshow("SavedImage", roi);
				isCalculated = false;

			}
			else
				cout << "Hough Transformation cant find lines! Try again, press 'c' " << endl;

			canny.release();
		}
	}

	waitKey(0);
	releaseAll();
	frame.release(); 
	map1.release();
	map2.release();

	return 0;
}

