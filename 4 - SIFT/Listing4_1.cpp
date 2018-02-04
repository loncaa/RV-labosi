void RunSIFT(IplImage *m_pImageInput)
{
	/************** Run SIFT **************/
	CRV2DSIFT SIFT;
	SIFT.CreateSIFT(m_pImageInput);

	/**************************************/



	/***********************Get and mark SIFT features**************************/
	CvSeq* descriptors = SIFT.Descriptors;
	for( int i = 0; i < SIFT.m_nDescriptors; i++ )  
	{
		RV2DSIFT_DESCRIPTOR* desc = (RV2DSIFT_DESCRIPTOR*)cvGetSeqElem(descriptors,i);
		cvCircle( m_pImageInput, cvPoint(cvRound(desc->fu),cvRound(desc->fv)), 3, CV_RGB(0,0,0), -1, 8, 0 );

	}

	/******************************************************************/
¸}



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








/****************Compare and relate features*****************/

//We now have descriptors from both pictures ie model and scene
//run comparison

int nM = m_SIFTModel.m_nDescriptors;	//number of descriptors in model
int nS = m_SIFTScene.m_nDescriptors;	//number of descriptors in scene


CvSeq *pSiftModelDescriptors = m_SIFTModel.Descriptors;
CvSeq *pSiftSceneDescriptors = m_SIFTScene.Descriptors;


int nReliableMatches = 0;

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

	//reset relation
	pMatchS->bRelation = false;
	
	//reset model
	pMatchM = MatchM;

	for(iM = 0; iM < nM; iM++, pMatchM++)
	{
		//reset relation
		pMatchM->bRelation = false;

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
		pMatchS->bRelation = true;
		pMatchM->bRelation = true;
		nReliableMatches++;
	}
	
		

}	

/*****************************************************************/
	
	
