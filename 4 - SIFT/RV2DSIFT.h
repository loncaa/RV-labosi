// RV2DSIFT.h: interface for the CRV2DSIFT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RV2DSIFT_H__F69AC912_B347_4A4B_9161_69F25EDB974D__INCLUDED_)
#define AFX_RV2DSIFT_H__F69AC912_B347_4A4B_9161_69F25EDB974D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <direct.h>
#include "highgui.h"
#include "cv.h"

struct MSMATCH_DATA
{
	int iBestMatch;
	int eBestMatch;
	bool bRelation;
};

struct PIX_ARRAY
{
	int Width;
	int Height;
	int nPixBytes;
	unsigned char *pPix;
	BOOL bOwnData;
	BOOL bColor;
};

struct RV2DSIFT_DESCRIPTOR
{
	double fv;
	double fu;
	double Scale;
	double Phi;
	int Data[128];
};


class CRV2DSIFT  
{
public:
	CvSeq *Descriptors;
	int m_nDescriptors;
	int m_DescriptorLength;
	


public:

	void CreateSIFT(IplImage* pInputImage);
	CRV2DSIFT();
	virtual ~CRV2DSIFT();
};

#endif // !defined(AFX_RV2DSIFT_H__F69AC912_B347_4A4B_9161_69F25EDB974D__INCLUDED_)
