// zavrsniDlg.cpp : implementation file
//

#include "stdafx.h"
#include "zavrsni.h"
#include "zavrsniDlg.h"
#include <string>
#include <sstream>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define GetCurrentDir _getcwd 





void ShowImage(const char *name, IplImage *image)
{
	cvNamedWindow(name);
	cvShowImage(name,image);
}

//Double Image ROI
IplImage *DoubleImageROI(IplImage *part1, IplImage *part2)
{
	IplImage *DoubleImage;
	DoubleImage=cvCreateImage(cvSize(part1->width+part2->width,part1->height),part1->depth,part1->nChannels);
	
	cvSetImageROI(DoubleImage,cvRect(0,0,part1->width,part1->height));
	cvCopy(part1,DoubleImage);
	cvResetImageROI(DoubleImage);

	cvSetImageROI(DoubleImage,cvRect(part1->width,0,part2->width,part2->height));
	cvCopy(part2,DoubleImage);
	cvResetImageROI(DoubleImage);

	return DoubleImage;

}
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


// CzavrsniDlg dialog




CzavrsniDlg::CzavrsniDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CzavrsniDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CzavrsniDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CzavrsniDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CzavrsniDlg::OnFirstImageOpen)
	ON_BN_CLICKED(IDC_BUTTON2, &CzavrsniDlg::OnSeconImageOpen)
	ON_BN_CLICKED(IDOK, &CzavrsniDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CzavrsniDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON4, &CzavrsniDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CzavrsniDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CzavrsniDlg message handlers

BOOL CzavrsniDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	m_pImageFrame = 0;
	m_pImageInput = 0;

	m_InputFirstImage = 0;
	m_InputSecondImage = 0;
	m_InputDoubleImage = 0;
	m_InputDoubleImage2 = 0;

	m_iCnt = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CzavrsniDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CzavrsniDlg::OnPaint()
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
HCURSOR CzavrsniDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CzavrsniDlg::OnFirstImageOpen()
{
	CFileDialog dlg(TRUE, _T("*.bmp"), "", OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY, "Image files (*.bmp; *.jpg; *.pgm) |*.bmp;*.jpg;*.pgm|All Files (*.*)|*.*||",NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Image");
	if (dlg.DoModal() == IDOK) {
	
		CString path = dlg.GetPathName();
		
		m_InputFirstImage = cvLoadImage(path,0);
		

		//ShowImage("FIRST IMAGE",m_InputFirstImage); //prikaz prve slike bez siftova

		//----------------------SIFT-------------------------
		//CRV2DSIFT SIFT;
	m_FirstSIFT.CreateSIFT(m_InputFirstImage);

	CvSeq* descriptors = m_FirstSIFT.Descriptors;
	for( int i = 0; i < m_FirstSIFT.m_nDescriptors; i++ )  
    {
        RV2DSIFT_DESCRIPTOR* desc = (RV2DSIFT_DESCRIPTOR*)cvGetSeqElem(descriptors,i);
		cvCircle( m_InputFirstImage, cvPoint(cvRound(desc->fu),cvRound(desc->fv)), 3, CV_RGB(0,0,0), -1, 8, 0 );
		
    }
	
	ShowImage("First image - SIFT",m_InputFirstImage);
	cvSaveImage("siftfirst.png", m_InputFirstImage);
//---------------------END SIFT---------------------
	};
}

void CzavrsniDlg::OnSeconImageOpen()
{
	CFileDialog dlg(TRUE, _T("*.bmp"), "", OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY, "Image files (*.bmp; *.jpg; *.pgm) |*.bmp;*.jpg;*.pgm|All Files (*.*)|*.*||",NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Image");
	if (dlg.DoModal() == IDOK) {
	
		CString path = dlg.GetPathName();
		m_InputSecondImage = cvLoadImage(path,0);

		//ShowImage("SECOND IMAGE",m_InputSecondImage); //prikaz druge slike
	
//----------------------SIFT-------------------------
		
	m_SecondSIFT.CreateSIFT(m_InputSecondImage);

	CvSeq* descriptors = m_SecondSIFT.Descriptors;

	for( int i = 0; i < m_SecondSIFT.m_nDescriptors; i++ )  
    {
        RV2DSIFT_DESCRIPTOR* desc = (RV2DSIFT_DESCRIPTOR*)cvGetSeqElem(descriptors,i);
		cvCircle( m_InputSecondImage, cvPoint(cvRound(desc->fu),cvRound(desc->fv)), 3, CV_RGB(0,0,0), -1, 8, 0 );
		
    }
//---------------------END SIFT---------------------

	ShowImage("Second image - SIFT",m_InputSecondImage);
	cvSaveImage("siftsecond.png", m_InputSecondImage);


	m_InputDoubleImage=DoubleImageROI(m_InputFirstImage,m_InputSecondImage);
	//ShowImage("Double Image",m_InputDoubleImage); //prikaz duple slike

	m_InputDoubleImage2=DoubleImageROI(m_InputFirstImage,m_InputSecondImage);
	}

//-----------------Matching SIFT --------------------------------------//
	//int siftArray[m_SecondSIFT.m_nDescriptors];

	int n = m_SecondSIFT.m_nDescriptors;
	int *pSiftArray = new int[m_SecondSIFT.m_nDescriptors]; //delete dshiftarray

	int nMatches = MatchSIFT(m_FirstSIFT, m_SecondSIFT, pSiftArray);
	

	int iS, iM,i;
	CvPoint pt1, pt2;  
	i=0;
	for (iS = 0; iS<m_SecondSIFT.m_nDescriptors; iS++)
	{
		if(pSiftArray[iS] != -1)
		{
			iM = pSiftArray[iS];

			pSObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(m_SecondSIFT.Descriptors,iS);	
			pMObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(m_FirstSIFT.Descriptors,iM);	

			pt1.x = cvRound(pMObject->fu + m_FirstSIFT.m_RoiXOffset);
			pt1.y = cvRound(pMObject->fv + m_FirstSIFT.m_RoiYOffset);

			pt2.x = cvRound(pSObject->fu + m_SecondSIFT.m_RoiXOffset + m_InputDoubleImage->width/2);
			pt2.y = cvRound(pSObject->fv + m_SecondSIFT.m_RoiYOffset);
	        

			cvLine( m_InputDoubleImage, pt1, pt2, CV_RGB(255,0,0), 1, 8, 0 );
		}
	
	}
	
	  ShowImage("Images (Before RANSAC)",m_InputDoubleImage ); //prikaz slike prije filtriranja tocaka
	  cvSaveImage("beforeransac.png", m_InputDoubleImage);

	//------------------ fundamental matrix ------------------
	
	CvMat* points1, *points2, *status, *fundamentalMatrix;
	points1 = cvCreateMat(2,nMatches,CV_32FC1);
	points2 = cvCreateMat(2,nMatches,CV_32FC1);
	status = cvCreateMat(1,nMatches,CV_8UC1);


	i=0;

	for (iS = 0; iS<m_SecondSIFT.m_nDescriptors; iS++)
	{
		if(pSiftArray[iS] != -1)
		{
			iM = pSiftArray[iS];

			pSObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(m_SecondSIFT.Descriptors,iS);	
			pMObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(m_FirstSIFT.Descriptors,iM);

			cvSetReal2D(points1,0,i,pMObject->fu);
			cvSetReal2D(points1,1,i,pMObject->fv);
			
			cvSetReal2D(points2,0,i,pSObject->fu);
			cvSetReal2D(points2,1,i,pSObject->fv);

			i++;
			
		}
	
	}
	
	fundamentalMatrix = cvCreateMat(3,3,CV_32FC1);
	int fm_count = cvFindFundamentalMat( points1,points2,fundamentalMatrix, CV_FM_RANSAC,1.8,0.99,status);

	//------------- end fundamental matrix -------------------

	i=0;
	int r=0;

	for (iS = 0; iS<m_SecondSIFT.m_nDescriptors; iS++)
	{
		if(pSiftArray[iS] != -1)
		{
			if(status->data.ptr[i]==1 )
			{
				iM = pSiftArray[iS];
			
				//counter r usporediti sa nmatches
				r++;
				
			pSObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(m_SecondSIFT.Descriptors,iS);	
			pMObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(m_FirstSIFT.Descriptors,iM);	

			pt1.x = cvRound(pMObject->fu + m_FirstSIFT.m_RoiXOffset);
			pt1.y = cvRound(pMObject->fv + m_FirstSIFT.m_RoiYOffset);

			pt2.x = cvRound(pSObject->fu + m_SecondSIFT.m_RoiXOffset + m_InputDoubleImage2->width/2);
			pt2.y = cvRound(pSObject->fv + m_SecondSIFT.m_RoiYOffset);
	        

			cvLine( m_InputDoubleImage2, pt1, pt2, CV_RGB(255,0,0), 1, 8, 0 );
			ShowImage("Images (After RANSAC)",m_InputDoubleImage2 );
			cvSaveImage("afterransac.png", m_InputDoubleImage2);
			
			}

			i++;
		}
	}

	CvMat* Lm, *Rm;
	
	Lm=cvCreateMat(3,1,CV_32FC1);
	Rm=cvCreateMat(3,1,CV_32FC1);
	

	
	CvMat *P,*P_invert,*essentialMatrix,*W,*Lp_i,*Rp_i,*Rp_iT,*s,*D,*U,*V,*V_T,
		  *A,*Atemp,*Ainvert,*ARp_i,*b,*bnew,*x1,*x2,*x4,*y1,*y2, *X, *Y, 
		  *P3D;


	W = cvCreateMat(3,3,CV_32FC1);
	P = cvCreateMat( 3, 3, CV_32FC1);
	P_invert = cvCreateMat( 3, 3, CV_32FC1);
	Lp_i = cvCreateMat ( 3, 1, CV_32FC1);
	Rp_i = cvCreateMat ( 3, 1, CV_32FC1);
	Rp_iT = cvCreateMat ( 1, 3, CV_32FC1);
	s = cvCreateMat ( 2, 1, CV_32FC1);
	D = cvCreateMat ( 3, 3, CV_32FC1);
	U = cvCreateMat ( 3, 3, CV_32FC1);
	V = cvCreateMat ( 3, 3, CV_32FC1);
	V_T = cvCreateMat ( 3, 3, CV_32FC1);
	b = cvCreateMat ( 3, 1, CV_32FC1);
	bnew = cvCreateMat( 3, 1, CV_32FC1);
	A = cvCreateMat ( 3, 3, CV_32FC1);
	Atemp = cvCreateMat ( 3, 3, CV_32FC1);
	Ainvert = cvCreateMat ( 3, 3, CV_32FC1);
	ARp_i = cvCreateMat (3, 1, CV_32FC1); 
	P3D = cvCreateMat ( 3, 1, CV_32FC1);
	
	
	X = cvCreateMat (2, 2, CV_32FC1);
	x1 = cvCreateMat (1, 1, CV_32FC1);
	x2 = cvCreateMat (1, 1, CV_32FC1);
	x4 = cvCreateMat (1, 1, CV_32FC1);
	float *Xdata = X->data.fl;

	Y = cvCreateMat (2, 1, CV_32FC1);
	y1 = cvCreateMat (1, 1, CV_32FC1);
	y2 = cvCreateMat (1, 1, CV_32FC1);
	float *Ydata = Y->data.fl;
	

	//Parametri projekcijske matrice//
	//float *Pdata = P->data.fl;
	//Pdata[0]= (float)346.04574585;
	//Pdata[1]= (float)0.0;
	//Pdata[2]= (float)181.64967346;
	//Pdata[3]= (float)0.0;
	//Pdata[4]= (float)341.28289795;
	//Pdata[5]= (float)140.65541077;
	//Pdata[6]= (float)0.0;
	//Pdata[7]= (float)0.0;
	//Pdata[8]= (float)1.0;

	P = (CvMat*)cvLoad("Intrinsics.xml");

	//---------------------------//

 	//init W
	float *Wdata = W->data.fl;
	Wdata[0] = 0.0;
	Wdata[1] = -1.0;
	Wdata[2] = 0.0;
	Wdata[3] = 1.0;
	Wdata[4] = 0.0;
	Wdata[5] = 0.0;
	Wdata[6] = 0.0;
	Wdata[7] = 0.0;
	Wdata[8] = 1.0;

	
	//esencijalna matrica
	essentialMatrix=cvCreateMat(3,3,CV_32FC1);
	cvInvert(P,P_invert,CV_SVD);
	cvGEMM(P, fundamentalMatrix, 1, 0, 1, Atemp, CV_GEMM_A_T); //esential matrix, transponirao P
	cvGEMM(Atemp, P, 1, 0, 1, essentialMatrix);

	cvSVD(essentialMatrix, D, U, V, CV_SVD_V_T);
	cvGEMM(U, W, 1, 0, 1, Atemp);
	cvGEMM(Atemp, V, 1, 0, 1, A);

	//init b
	float *bdata=b->data.fl;
	float *udata=U->data.fl;
	bdata[0]=udata[2];
	bdata[1]=udata[5];
	bdata[2]=udata[8];

	cvInvert( A, Ainvert, CV_SVD); // oznake RV.3 i RV.6
	cvGEMM ( Ainvert, b, 1, 0, 1, bnew); //

	FILE *points3D = fopen("points3d.txt","w");

	//odreðivanje s i t;
		i=0;
		r=0;

	for (iS = 0; iS<m_SecondSIFT.m_nDescriptors; iS++)
	{
		if(pSiftArray[iS] != -1)
		{
			if(status->data.ptr[i] == 1)
			{ 
			
				iM = pSiftArray[iS];

				r++;

			pSObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(m_SecondSIFT.Descriptors,iS);	
			pMObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(m_FirstSIFT.Descriptors,iM);	

			//brojac u vektoru
			float *data_Lm = Lm->data.fl;
			float *data_Rm = Rm->data.fl;

			data_Rm[0]=(float)(pSObject->fu);
			data_Rm[1]=(float)(pSObject->fv);
			data_Rm[2]=1;

			data_Lm[0]=(float)(pMObject->fu);
			data_Lm[1]=(float)(pMObject->fv);
			data_Lm[2]=1;
			
			cvGEMM(P_invert, Lm, 1, 0, 1, Lp_i);
			cvGEMM(P_invert, Rm, 1, 0, 1, Rp_i); //Rm je 3x1, Rp_i 3x1, P_invert 3x3

			
			//init x table
			cvGEMM(Ainvert, Rp_i, 1, 0, 1, ARp_i);
			cvGEMM(Lp_i, Lp_i, 1, 0, 1, x1, CV_GEMM_A_T);
			cvGEMM(Lp_i, ARp_i, -1, 0, 1, x2, CV_GEMM_A_T);
			cvGEMM(ARp_i, ARp_i, 1, 0, 1, x4, CV_GEMM_A_T);
			Xdata[0]=x1->data.fl[0];
			Xdata[1]=x2->data.fl[0];
			Xdata[2]=x2->data.fl[0];
			Xdata[3]=x4->data.fl[0];

			//init y table
			cvGEMM(Lp_i, bnew, -1, 0, 1, y1, CV_GEMM_A_T);
			cvGEMM(ARp_i, bnew, 1, 0, 1, y2, CV_GEMM_A_T);
			Ydata[0]=y1->data.fl[0];
			Ydata[1]=y2->data.fl[0];
			
			cvSolve(X,Y,s);

			Lp_i->data.fl[0]=s->data.fl[0]*Lp_i->data.fl[0];
			Lp_i->data.fl[1]=s->data.fl[0]*Lp_i->data.fl[1];
			Lp_i->data.fl[2]=s->data.fl[0]*Lp_i->data.fl[2];



			ARp_i->data.fl[0]=s->data.fl[1]*ARp_i->data.fl[0];
			ARp_i->data.fl[1]=s->data.fl[1]*ARp_i->data.fl[1];
			ARp_i->data.fl[2]=s->data.fl[1]*ARp_i->data.fl[2];

			
			P3D->data.fl[0]=(Lp_i->data.fl[0]+ARp_i->data.fl[0]-bnew->data.fl[0])/2;
			P3D->data.fl[1]=(Lp_i->data.fl[1]+ARp_i->data.fl[1]-bnew->data.fl[1])/2;
			P3D->data.fl[2]=(Lp_i->data.fl[2]+ARp_i->data.fl[2]-bnew->data.fl[2])/2;

			
			//upis u points3D.txt

			fprintf(points3D,"%f %f %f \n",P3D->data.fl[0],P3D->data.fl[1],P3D->data.fl[2]);
			
			
			}

			i++;
		}
	}
	fclose(points3D);
//Lm Rm, status, points1, points2

	cvReleaseMat(&points1);
	cvReleaseMat(&points2);
	cvReleaseMat(&status);
	cvReleaseMat(&fundamentalMatrix);
	cvReleaseMat(&Lm);
	cvReleaseMat(&Rm);
	cvReleaseMat(&P);
	cvReleaseMat(&P_invert);
	cvReleaseMat(&essentialMatrix);
	cvReleaseMat(&W);
	cvReleaseMat(&Lp_i);
	cvReleaseMat(&Rp_i);
	cvReleaseMat(&Rp_iT);
	cvReleaseMat(&s);
	cvReleaseMat(&D);
	cvReleaseMat(&U);
	cvReleaseMat(&V);
	cvReleaseMat(&V_T);
	cvReleaseMat(&A);
	cvReleaseMat(&Atemp);
	cvReleaseMat(&Ainvert);
	cvReleaseMat(&ARp_i);
	cvReleaseMat(&b);
	cvReleaseMat(&bnew);
	cvReleaseMat(&x1);
	cvReleaseMat(&x2);
	cvReleaseMat(&x4);
	cvReleaseMat(&y1);
	cvReleaseMat(&y2);
	cvReleaseMat(&X);
	cvReleaseMat(&Y);
	cvReleaseMat(&P3D);
	//cvReleaseMat(&I); // otkomentirati ukoliko se koriste matrice
	//cvReleaseMat(&I_T);
	//cvReleaseMat(&err);
		delete []pSiftArray;	
}
	

void CzavrsniDlg::OnBnClickedOk()
{

	ReleaseAll();
	OnOK();
}
void CzavrsniDlg::OnBnClickedCancel()
{
	ReleaseAll();
	OnCancel();
}

void CzavrsniDlg::ReleaseAll()
{
	if(m_pImageFrame)
		cvReleaseImage(&m_pImageFrame);

	if(m_pImageInput)
		cvReleaseImage(&m_pImageInput);
	
	cvReleaseImage(&m_InputFirstImage);
	cvReleaseImage(&m_InputSecondImage);
	cvReleaseImage(&m_InputDoubleImage);
	cvReleaseImage(&m_InputDoubleImage2);
	cvDestroyAllWindows();
};

int MatchSIFTDescriptors(RV2DSIFT_DESCRIPTOR *pModelDescriptor,
						 RV2DSIFT_DESCRIPTOR *pSceneDescriptor) 
{
	int *pMData;
	int *pSData;
	int *pMDataEnd;
	int e;	

	pMData = pModelDescriptor->Data;
	pSData = pSceneDescriptor->Data;
	pMDataEnd = pMData + 128;  //128 descriptorLength

	int E = 0;
	for(; pMData < pMDataEnd; pMData++, pSData++)
	{
		e = (int)(*pSData) - (int)(*pMData);
		if(e >= 0)
			E += e;
		else
			E -= e;
	}
	return E;
}

// Params:

// ref_SIFT - reference SIFT

// cur_SIFT - current SIFT

// pMatch - pointer to array of dimension cur_SIFT.m_nDescriptors

int MatchSIFT(CRV2DSIFT ref_SIFT, CRV2DSIFT cur_SIFT, int *pMatch )
{	
	int nReliableMatches = 0;

	int nM = ref_SIFT.m_nDescriptors;	//number of descriptors in model (reference)
	int nS = cur_SIFT.m_nDescriptors;	//number of descriptors in scene (current)

	CvSeq *pSiftModelDescriptors = ref_SIFT.Descriptors;
	CvSeq *pSiftSceneDescriptors = cur_SIFT.Descriptors;

	MSMATCH_DATA *MatchM = new MSMATCH_DATA[nM];
	memset(MatchM, 0x7f, nM * sizeof(MSMATCH_DATA));

	MSMATCH_DATA *MatchS = new MSMATCH_DATA[nS];
	memset(MatchS, 0x7f, nS * sizeof(MSMATCH_DATA));

	MSMATCH_DATA *pMatchS = MatchS;
	MSMATCH_DATA *pMatchM;

	RV2DSIFT_DESCRIPTOR* pSObject;
	RV2DSIFT_DESCRIPTOR* pMObject;

	int e, iS, iM;
	for(iS = 0; iS < nS; iS++, pMatchS++)
	{
		pSObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(pSiftSceneDescriptors,iS);
	
		//reset model
		pMatchM = MatchM;
		for(iM = 0; iM < nM; iM++, pMatchM++)
		{
			pMObject = (RV2DSIFT_DESCRIPTOR *)cvGetSeqElem(pSiftModelDescriptors,iM);
			e = MatchSIFTDescriptors(pMObject,pSObject);
			if(e < pMatchS->eBestMatch)
			{
				pMatchS->eBestMatch = e;
				pMatchS->iBestMatch = iM;
			}

			if(e < pMatchM->eBestMatch)
			{
				pMatchM->eBestMatch = e;
				pMatchM->iBestMatch = iS;
			}
		}	// for all MObjects
	}	// for all SObjects

	pMatchS = MatchS;

	
	for(iS = 0; iS < nS; iS++, pMatchS++)
	{
		iM = pMatchS->iBestMatch;
		pMatchM = MatchM + iM;
		
		if(iS == pMatchM->iBestMatch && pMatchS->eBestMatch == pMatchM->eBestMatch)
		{
			//Set relation
			pMatch[iS] = iM;
			nReliableMatches++;
		}
		else
			pMatch[iS] = -1;
	}
	delete[] MatchM;
	delete[] MatchS;

	return nReliableMatches;
}
void CzavrsniDlg::OnBnClickedButton4()
{
	char cCurrentPath[FILENAME_MAX]; 
	char outString[35]; 

	m_pImageInput = cvCreateImage(cvSize(m_pImageFrame->width,m_pImageFrame->height),IPL_DEPTH_8U, 1);
	//m_pImageInput = cvCloneImage(m_pImageFrame);  

	cvCvtColor(m_pImageFrame,m_pImageInput,CV_BGR2GRAY);

	if (GetCurrentDir(cCurrentPath, sizeof(cCurrentPath) / sizeof(TCHAR))) 
	{
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */ 
		 
		sprintf(outString,"\\pics\\SavedImage%d.bmp",m_iCnt);

		strcat(cCurrentPath,outString);

		m_iCnt++;

		cvSaveImage(cCurrentPath,m_pImageInput);

		cvNamedWindow ("Saved image");
		cvShowImage("Saved image",m_pImageInput );

     } 
 

	
	
	
}

void CzavrsniDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	CvCapture *capture = cvCreateCameraCapture(1);

	//capture = cvCaptureFromCAM( CV_CAP_ANY );
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 30);

	assert( capture != NULL );

	IplImage *img;
	
	cvNamedWindow( "WebCam", CV_WINDOW_AUTOSIZE );

	while((img=cvQueryFrame(capture)) != NULL)
	{
	
		m_pImageFrame = cvCloneImage(img);
		cvShowImage( "WebCam", img );
		if(( cvWaitKey(33) == 27 ))
		{
			cvDestroyWindow( "WebCam" );
			break;
		}
	}
	
	cvReleaseCapture(&capture);
}
