
// CameraCalibrationDlg.h : header file
//

#pragma once
#include "highgui.h"
#include "cv.h"


// CCameraCalibrationDlg dialog
class CCameraCalibrationDlg : public CDialog
{
// Construction
public:
	CCameraCalibrationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CAMERACALIBRATION_DIALOG };

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
	int m_MaxNImages;	//max number of images taken
	int m_BoardWidth;	//board width (ie the max number of corners that can be found along the chessboard length)
	int m_BoardHeight;	//board height(ie the max number of corners that can be found along the chessboard height)
	int m_NCorners;		//number of corners = board width * board height;
	float m_SquareLength;	//the length of one of the squares on the chessboard (in mm)
	CvSize m_BoardSize;
	CvCapture* m_capture;
	IplImage *m_imageClone; 
	IplImage *m_grayImage;

	afx_msg void OnBnClickedCalibratecamera();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
