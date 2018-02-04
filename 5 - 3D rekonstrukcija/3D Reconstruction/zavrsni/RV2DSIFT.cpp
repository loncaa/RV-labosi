// RV2DSIFT.cpp: implementation of the CRV2DSIFT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "zavrsni.h"
//#include "RV1.h"
#include "RV2DSIFT.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRV2DSIFT::CRV2DSIFT()
{
	m_RoiXOffset=0;
	m_RoiYOffset=0;
}

CRV2DSIFT::~CRV2DSIFT()
{

}
void SavePGM(char *Filename, unsigned char *I, int Width, int Height, BOOL bFlush)
{
	int i, j;
	FILE *fpbmp;

	fpbmp=fopen(Filename,"wb");

	fprintf(fpbmp, "P5 %d %d 255\n", Width, Height);

	//unsigned char *pI = I + (Height - 1) * Width;
	unsigned char *pI = I;

	for(i = 0; i < Height; i++)
	{
		for(j = 0; j < Width; j++)
			fputc((int)(*(pI++)),fpbmp);

		//pI -= 2 * Width;
	}

	if(bFlush)
		fflush(fpbmp);

	fclose(fpbmp);
}




void CRV2DSIFT::CreateSIFT(IplImage* pInputImage)
{
	CvMemStorage* storage = cvCreateMemStorage(0);

	//char current_path[MAX_PATH];
	//_getcwd(current_path, MAX_PATH-1);
	
	char *current_path ="C:\\siftDemoV4";

	char *SIFTDetectorFileName = "C:\\siftDemoV4\\siftWin32";

	char SIFTInputImageFileNameBMP[200];
	strcpy(SIFTInputImageFileNameBMP,current_path);
	strcat(SIFTInputImageFileNameBMP,"\\SiftTemp.BMP");

	char SIFTInputImageFileName[200];
	strcpy(SIFTInputImageFileName,current_path);
	strcat(SIFTInputImageFileName,"\\SiftTemp.PGM");

	char SIFTOutputFileName[200];
	strcpy(SIFTOutputFileName,current_path);
	strcat(SIFTOutputFileName,"\\SiftTemp.key");
	

	//use this save option to BMP due to ROI
	//saving to PGM gives wrong format for sift
	cvSaveImage(SIFTInputImageFileNameBMP, pInputImage);

	//reload saved bmp, convert to PIX_ARRAY and save as PGM
	IplImage *tempImage = cvLoadImage(SIFTInputImageFileNameBMP,0);


	PIX_ARRAY *pPixArray = new PIX_ARRAY;

	pPixArray->bColor = 0;
	pPixArray->bOwnData = false;
	pPixArray->nPixBytes = 1;
	pPixArray->Height = tempImage->height;
	pPixArray->Width = tempImage->width;
	pPixArray->pPix = (unsigned char *)tempImage->imageData;

	//need to use this save option due to sift.exe error!!
	SavePGM(SIFTInputImageFileName,
				pPixArray->pPix, pPixArray->Width, pPixArray->Height, TRUE);
	

	
	//m_RoiXOffset = pInputImage->roi->xOffset;
	//m_RoiYOffset = pInputImage->roi->yOffset;
   


	char SIFTCommandLine[600];

	//prepare SIFT params
	SIFTCommandLine[0] = 0;
	
	strcat(SIFTCommandLine, SIFTDetectorFileName);
	strcat(SIFTCommandLine, " <");
	strcat(SIFTCommandLine, SIFTInputImageFileName);
	strcat(SIFTCommandLine, " >");
	strcat(SIFTCommandLine, SIFTOutputFileName);
	
	system(SIFTCommandLine);
	FILE *fp = fopen(SIFTOutputFileName, "rt");



	int i, j, k;

	fscanf(fp, "%d %d\n", &m_nDescriptors, &m_DescriptorLength);


	RV2DSIFT_DESCRIPTOR *pSIFTDescriptor; 
	pSIFTDescriptor = new RV2DSIFT_DESCRIPTOR;
	int Descript;
	int *pDescriptData;

	Descriptors = cvCreateSeq( 0, sizeof(CvSeq), sizeof(RV2DSIFT_DESCRIPTOR), storage);

	for(i = 0; i < m_nDescriptors; i++)
	{

		fscanf(fp, "%lf %lf %lf %lf\n", &(pSIFTDescriptor->fv), &(pSIFTDescriptor->fu), 
			&(pSIFTDescriptor->Scale), &(pSIFTDescriptor->Phi));

		pDescriptData = pSIFTDescriptor->Data;
		



		for(j = 0; j < 6; j++)
		{
			for(k = 0; k < 20; k++)
			{
				fscanf(fp, "%d ", &Descript);
				*(pDescriptData++) = Descript;
			}

			fscanf(fp, "\n");
		}

		for(k = 0; k < 8; k++)
		{
			fscanf(fp, "%d ", &Descript);
			*(pDescriptData++) = Descript;
		}

		fscanf(fp, "\n");


		cvSeqPush( Descriptors,(RV2DSIFT_DESCRIPTOR *)pSIFTDescriptor);

		
	}	// for every SIFT-feature

	fclose(fp);
	delete pSIFTDescriptor;
	delete pPixArray;
}



