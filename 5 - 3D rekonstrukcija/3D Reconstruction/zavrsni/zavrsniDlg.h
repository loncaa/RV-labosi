// zavrsniDlg.h : header file
//

#pragma once
#include "cv.h"
#include "highgui.h"
#include "rv2dsift.h"
#include "math.h"

struct MSMATCH_DATA

{
	int iBestMatch;
	int eBestMatch;
};

int MatchSIFTDescriptors(RV2DSIFT_DESCRIPTOR *pModelDescriptor,
						 RV2DSIFT_DESCRIPTOR *pSceneDescriptor);


int MatchSIFT(CRV2DSIFT ref_SIFT, CRV2DSIFT cur_SIFT, int *pMatch );


// CzavrsniDlg dialog
class CzavrsniDlg : public CDialog
{
// Construction
public:
	CzavrsniDlg(CWnd* pParent = NULL);	// standard constructor

	//member variables
	IplImage *m_pImageFrame;
	IplImage *m_pImageInput;

	int m_iCnt;

	IplImage *m_InputFirstImage;
	IplImage *m_InputSecondImage;
	IplImage *m_InputDoubleImage;
	IplImage *m_InputDoubleImage2;
	CRV2DSIFT m_FirstSIFT;
	CRV2DSIFT m_SecondSIFT;
	RV2DSIFT_DESCRIPTOR *pSObject;
	RV2DSIFT_DESCRIPTOR *pMObject;

	void ReleaseAll();


// Dialog Data
	enum { IDD = IDD_ZAVRSNI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFirstImageOpen();
	afx_msg void OnSeconImageOpen();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton3();
	
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
};
