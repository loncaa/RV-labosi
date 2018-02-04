
// CameraCalibrationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CameraCalibration.h"
#include "CameraCalibrationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCameraCalibrationDlg dialog




CCameraCalibrationDlg::CCameraCalibrationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraCalibrationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCameraCalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCameraCalibrationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CalibrateCamera, &CCameraCalibrationDlg::OnBnClickedCalibratecamera)
	ON_BN_CLICKED(IDOK, &CCameraCalibrationDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCameraCalibrationDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CCameraCalibrationDlg message handlers

BOOL CCameraCalibrationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_MaxNImages = 5;
	m_BoardWidth = 8;
	m_BoardHeight = 6;
	m_SquareLength = 27.0f;


	CString strTemp;

	strTemp.Format("%d",m_MaxNImages);
	SetDlgItemText(IDC_MaxNImages, strTemp);

	strTemp.Format("%d",m_BoardWidth);
	SetDlgItemText(IDC_BoardWidth, strTemp);

	strTemp.Format("%d",m_BoardHeight);
	SetDlgItemText(IDC_BoardHeight, strTemp);

	strTemp.Format("%.1f",m_SquareLength);
	SetDlgItemText(IDC_SquareLength, strTemp);


	m_capture = 0;
	m_imageClone = 0;
	m_grayImage = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCameraCalibrationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCameraCalibrationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCameraCalibrationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCameraCalibrationDlg::OnBnClickedCalibratecamera()
{
	/************************************************************/
	/* A MODIFIED VERSION OF THE EXAMPLE PROVIDED IN :          */ 
	/* Learning OpenCV Computer Vision with the OpenCV Library, */
	/* Gary Bradski & Adrian Kaehler pg. 398-401                */
	/************************************************************/

	CString strTemp;

	//Read input params
    GetDlgItemText(IDC_MaxNImages, strTemp);
	m_MaxNImages = atoi(strTemp);

	GetDlgItemText(IDC_BoardWidth, strTemp);
	m_BoardWidth = atoi(strTemp);

	GetDlgItemText(IDC_BoardHeight, strTemp);
	m_BoardHeight = atoi(strTemp);

	GetDlgItemText(IDC_SquareLength, strTemp);
	m_SquareLength =  (float)atof(strTemp);



	//Start calibration
	int m_NCorners = m_BoardWidth * m_BoardHeight;
	
	CvSize board_sz = cvSize(m_BoardWidth,m_BoardHeight);
	

	//ALLOCATE STORAGE
	CvMat* image_points = cvCreateMat(m_MaxNImages*m_NCorners,2,CV_32FC1);
	CvMat* object_points = cvCreateMat(m_MaxNImages*m_NCorners,3,CV_32FC1);
	CvMat* point_counts = cvCreateMat(m_MaxNImages,1,CV_32SC1);
	CvMat* intrinsic_matrix = cvCreateMat(3,3,CV_32FC1);
	CvMat* distortion_coeffs = cvCreateMat(4,1,CV_32FC1);   
	CvPoint2D32f* corners = new CvPoint2D32f[ m_NCorners ];
	
	int corner_count;
	int successes = 0;
	int step, frame = 0;
	
	
	//close all windows
	cvDestroyAllWindows(); 
	
	// CAPTURE CORNER VIEWS LOOP UNTIL WE'VE GOT m_MaxNImages
	// SUCCESSFUL CAPTURES (ALL CORNERS ON THE BOARD ARE FOUND)
	//
	if(m_capture==0)
	{
		m_capture = cvCreateCameraCapture(0);
		assert(m_capture);
	}

	cvNamedWindow("Camera_calibration");

	IplImage *image; 
	

	int c;


	while((image=cvQueryFrame(m_capture)) != NULL)
	{
	
		cvShowImage( "Camera_calibration", image );

		//Handle print //and ESC
		c = cvWaitKey(15);
		if(c == 'p')
		{
			if(successes < m_MaxNImages)
			{
				m_imageClone = cvCloneImage(image);

				m_grayImage = cvCreateImage(cvGetSize(m_imageClone),8,1);//subpixel
				


				//Find chessboard corners:
				int found = cvFindChessboardCorners(m_imageClone, board_sz, corners, &corner_count,
													CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
				//Get Subpixel accuracy on those corners
				cvCvtColor(m_imageClone, m_grayImage, CV_BGR2GRAY);
				cvFindCornerSubPix(m_grayImage, corners, corner_count,
								   cvSize(11,11),cvSize(-1,-1), cvTermCriteria(
								   CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
				//Draw it
				cvDrawChessboardCorners(m_imageClone, board_sz, corners,corner_count, found);

				cvNamedWindow("Calibration");
				cvShowImage( "Calibration", m_imageClone );
				// If we got a good board, add it to our data

				if( corner_count == m_NCorners ) 
				{
					step = successes*m_NCorners;
					for( int i=step, j=0; j<m_NCorners; ++i,++j ) 
					{
						CV_MAT_ELEM(*image_points, float,i,0) = corners[j].x;
						CV_MAT_ELEM(*image_points, float,i,1) = corners[j].y;
						CV_MAT_ELEM(*object_points,float,i,0) = j/m_BoardWidth * m_SquareLength;   
						CV_MAT_ELEM(*object_points,float,i,1) = j%m_BoardWidth * m_SquareLength;		
						CV_MAT_ELEM(*object_points,float,i,2) = 0.0f;
					}
				
					CV_MAT_ELEM(*point_counts, int,successes,0) = m_NCorners;
					successes++;
				}
				
			}
			else
				break;
		
		}
		else if(c==27)
			break;
				


	} //END COLLECTION WHILE LOOP.

	//close all windows
	cvDestroyAllWindows(); 

	if(successes == m_MaxNImages)
	{
		//ALLOCATE MATRICES ACCORDING TO HOW MANY CHESSBOARDS FOUND
		CvMat* object_points2 = cvCreateMat(successes*m_NCorners,3,CV_32FC1);
		CvMat* image_points2 = cvCreateMat(successes*m_NCorners,2,CV_32FC1);
		CvMat* point_counts2 = cvCreateMat(successes,1,CV_32SC1);
		
		
		//TRANSFER THE POINTS INTO THE CORRECT SIZE MATRICES
		int i;
		for(i = 0; i<successes*m_NCorners; ++i) 
		{
			CV_MAT_ELEM( *image_points2, float, i, 0) = CV_MAT_ELEM( *image_points, float, i, 0);
			CV_MAT_ELEM( *image_points2, float,i,1) = CV_MAT_ELEM( *image_points, float, i, 1);
			CV_MAT_ELEM(*object_points2, float, i, 0) =	CV_MAT_ELEM( *object_points, float, i, 0) ;
			CV_MAT_ELEM( *object_points2, float, i, 1) = CV_MAT_ELEM( *object_points, float, i, 1) ;
			CV_MAT_ELEM( *object_points2, float, i, 2) = CV_MAT_ELEM( *object_points, float, i, 2) ;
		}
		
		for(i=0; i<successes; ++i) //These are all the same number
		{ 
			CV_MAT_ELEM( *point_counts2, int, i, 0) = CV_MAT_ELEM( *point_counts, int, i, 0);
		}
		cvReleaseMat(&object_points);
		cvReleaseMat(&image_points);
		cvReleaseMat(&point_counts);
		
		// At this point we have all of the chessboard corners we need.
		// Initialize the intrinsic matrix such that the two focal
		// lengths have a ratio of 1.0
		//
		CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0f;
		CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0f;
		//CALIBRATE THE CAMERA!
		cvCalibrateCamera2(object_points2, image_points2,
						   point_counts2, cvGetSize( image ),
						   intrinsic_matrix, distortion_coeffs,
						   NULL, NULL,0 );//CV_CALIB_FIX_ASPECT_RATIO
		
		// SAVE THE INTRINSICS AND DISTORTIONS
		cvSave("Intrinsics.xml", intrinsic_matrix);
		cvSave("Distortion.xml", distortion_coeffs);
		


		// EXAMPLE OF LOADING THESE MATRICES BACK IN:
		CvMat *intrinsic = (CvMat*)cvLoad("Intrinsics.xml");
		CvMat *distortion = (CvMat*)cvLoad("Distortion.xml");
		
		// Build the undistort map that we will use for all
		// subsequent frames.
		//
		IplImage* mapx = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
		IplImage* mapy = cvCreateImage( cvGetSize(image), IPL_DEPTH_32F, 1 );
		
		cvInitUndistortMap(intrinsic,distortion,mapx,mapy);
		
		// Just run the camera to the screen, now showing the raw and
		// the undistorted image.
		//

		cvNamedWindow( "Undistorted image" );
		while((image=cvQueryFrame(m_capture)) != NULL) 
		{
			m_imageClone = cvCloneImage(image);
			cvShowImage( "Original image", image ); // Show raw image
			cvRemap( m_imageClone, image, mapx, mapy ); // Undistort image
			
			cvShowImage("Undistorted image", image); // Show corrected image
			//Handle pause/unpause and ESC
			c = cvWaitKey(15);
			if(c == 27)
			{
				cvDestroyWindow( "Original image" );
				cvDestroyWindow( "Undistorted image" );
				break;
			}

		}

		
		
	}

	cvDestroyAllWindows();

	delete corners;
	
}

void CCameraCalibrationDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	cvDestroyAllWindows(); 
	cvReleaseImage(&m_imageClone);
	cvReleaseImage(&m_grayImage);
	cvReleaseCapture(&m_capture);

	OnOK();
}

void CCameraCalibrationDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	cvDestroyAllWindows(); 
	cvReleaseImage(&m_imageClone);
	cvReleaseImage(&m_grayImage);
	cvReleaseCapture(&m_capture);

	OnCancel();
}

