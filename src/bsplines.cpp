/*
 * bsplines.c
 *
 *  Created on: Feb 13, 2013
 *      Author: luzdora
 */

/********   B-SPLINE FUNCTIONS   *****************/

#include <stdio.h>
#include "bspline.hpp"

double cubicBSplines[16]={ -1.0/6.0,  3.0/6.0, -3.0/6.0, 1.0/6.0,
			    3.0/6.0, -1.0,      3.0/6.0, 0.0,
			   -3.0/6.0,  0.0,      3.0/6.0, 0.0,
			    1.0/6.0,  4.0/6.0,  1.0/6.0, 0.0};

/*************************************************************/
/** BSpline Structure Inicialization**/

int BSplineInit(BSpline *spline){
  //   if (spline == NULL)
  //    return -1;
    spline->PtsList   = NULL;
    spline->tmpPtsMat = NULL;
    spline->ctrlPts   = NULL;
    spline->mult4Pts  = NULL;
    spline->indPts4Span = NULL;
    spline->contour   = NULL;
    spline->contourPts = NULL;
    spline->NoCtrlPts = -1;
    spline->NoTotPts  = spline->NoSpans  = 0;
    spline->NoPts4Span= spline->NoPtsMult= 0;
    sprintf(spline->errorMessage, "BSpline not initialized ");
    return(0);
}

int BSplineInitStr(BSpline *spline,
		    int n,            /** Number of points **/
		    CvPoint *pts,      /** Contour points   **/
		    BSplineOrder ord,
		    BSplineType type,
		    int *indMult) {
    register int i, repeatPts = 0;
    BSplineOrder order  =spline->order  = ord;
    int NoCtrlPts  = spline->NoCtrlPts  = n;
    int NoPts4Span = spline->NoPts4Span = (ord+1);
    int *mult4Pts  = spline->mult4Pts;

    if (n <= 0){
      fprintf(stderr, "BSpline, Not enought control points\n");
      return(n);
    }
    //  printf ("\n Puntos %d ", n);
    spline->closure= type;
    if (NoCtrlPts > order){ /* case of multiple points */
        spline->NoPtsMult=0;
	if (indMult != NULL){
	   mult4Pts = (int *)malloc(sizeof(int)*NoCtrlPts);/* InitMulti */
	   for (i=0 ; i<NoCtrlPts ; i++) {
	     mult4Pts[i] = indMult[i];
	     if ((mult4Pts[i] > 1) && (mult4Pts[i] < NoPts4Span)){
	       repeatPts += mult4Pts[i] - 1;
	       spline->NoPtsMult++;
	     }
	   }
	}
	else
	  mult4Pts = 0;
	spline->NoSpans  = n-type*order+repeatPts;
	spline->NoTotPts = NoCtrlPts + repeatPts;

	printf ("\n Num Spans=%d, Num total de Puntos=%d ", spline->NoSpans, spline->NoTotPts );

	BSplineInitMat(spline);
	BSplineInitPtsList(spline);
	memcpy(spline->ctrlPts, pts, sizeof(CvPoint)*NoCtrlPts);/*gavina:for()*/
	BSplineInitSpanInd(spline);
	BSplineInitContour(spline);
	sprintf(spline->errorMessage,"%d control points", NoCtrlPts);
    } else {
      NoCtrlPts = -1;
      sprintf(spline->errorMessage, "Not enought control points");
    }
    return(n);
}

/*Copy Funtion*/
void BSplineCopy(BSpline *spline, BSpline *splineDat){
     register  int i, j;
     int NoCtrlPts = splineDat->NoCtrlPts;
     int order     = splineDat->order;
     CvPoint  *ctrlPts = NULL;
     double  *contour = NULL;

     printf ("copy \n");
     BSplineFree (spline);
     BSplineInit (spline);

     spline->NoCtrlPts = NoCtrlPts;
     spline->NoTotPts  = splineDat->NoTotPts;
     spline->NoSpans   = splineDat->NoSpans;
     spline->NoPts4Span= splineDat->NoPts4Span;
     spline->NoPtsMult = splineDat->NoPtsMult;

     spline->closure   = splineDat->closure;
     spline->order     = (BSplineOrder)order;

     if (NoCtrlPts>order){
         BSplineInitMat(spline);
	 ctrlPts = spline->ctrlPts;
	 contour = spline->contour;

	 for (i=0;i<NoCtrlPts;i++) {
	   ctrlPts[i] = splineDat->ctrlPts[i];
	   for(j=0;j<splineDat->NoPts4Span;j++)
	     contour[i*NoCtrlPts+j] = splineDat->contour[i*NoCtrlPts+j];
	 }
	 if (splineDat->NoPtsMult>0){
	   spline->mult4Pts = (int *)malloc(sizeof(int)*NoCtrlPts);
	   memcpy(spline->mult4Pts, splineDat->mult4Pts, sizeof(int)*NoCtrlPts);
	 }
     } else {
       spline->NoCtrlPts = -1;
       sprintf(spline->errorMessage,"Not enought control points");
     }
}

/* Protected funtions */

void BSplineInitMat(BSpline *spline)
{
  int NoPts4Span = spline->NoPts4Span;
  int NoSpans    = spline->NoSpans;

  spline->tmpPtsMat=(double *)malloc(sizeof(double)*DIM*NoPts4Span);
  spline->contour  =(double *)malloc(sizeof(double)*DIM*NoPts4Span*NoSpans);
  spline->ctrlPts  =(CvPoint *)malloc(sizeof(CvPoint)*spline->NoCtrlPts);
  spline->indPts4Span= (int *)malloc(sizeof(int)*NoSpans*NoPts4Span);
  spline->PtsList   =  (int *)malloc(sizeof(int)*spline->NoTotPts);

  spline->contourPts = reserve(DIM, 1000);
}

/*
  void BSplineInitBSpline(BSpline *spline)
  {
  BSplineInitSpanInd(spline);
  BSplineInitContour(spline);
  }
*/

void BSplineInitPtsList(BSpline *spline)
{
  int i, k, n = 0, j = 0;
  int *PtsList = spline->PtsList ;
  int *mult4Pts= spline->mult4Pts;
  int NoCtrlPts= spline->NoCtrlPts;

  if (spline->NoPtsMult>0){
    for (i=0; i<NoCtrlPts; i++) {
      if (mult4Pts[i]>1) {
	for (k=0;k<mult4Pts[i];k++){
	  PtsList[n]=i;
	  n++;
	}
	j++;
      } else {
	PtsList[n]=i;
	n++;
      }
    }
  }else
    for (i=0; i<NoCtrlPts; i++)  PtsList[i]=i;
}

/* Initialized the indices for each span */
void BSplineInitSpanInd(BSpline *spline){
  register int i, j, s, ii;
  int     NoTotPts = spline->NoTotPts;
  int *indPts4Span = spline->indPts4Span;
  int  NoPts4Span  = spline->NoPts4Span;
  int *PtsList     = spline->PtsList;
  int NoSpans = spline->NoSpans;

  if (spline->closure == CLOSE) {
	for (i=0 ; i<NoSpans ; i++){
	  ii = i * NoPts4Span;
	  for (j=0 ; j < NoPts4Span ; j++){
		s = i + j - 1;
		if (s<0) s = NoTotPts + s;
		else if(s >= NoTotPts) s = s - NoTotPts;
		indPts4Span[ii + j] = PtsList[s];
		//	 	printf(" IndPts4Span = %d ",PtsList[s]);
	  }
	  // 	  printf("\n");
	}
  } else {
	for(i=0;i<NoSpans;i++) {
	  ii = i * NoPts4Span;
	  for (j=0;j<NoPts4Span;j++){
		s = i + j;
		indPts4Span[ii + j] = PtsList[s];
			/* 	printf(" %d",PtsList[s]); */
	  }
	  /*  printf("\n"); */
	}
  }
}

void BSplineInitContour(BSpline *spline){
    register  int i, e=0;
    int contourSize = DIM*spline->NoPts4Span;
    double *contour =  spline->contour;
    CvPoint P = {0,0};

    for (i=0;i<spline->NoSpans;i++)
      e += BSplineCurveCoef(spline, i, contour + i*contourSize, -1, &P);
}

/* Public functions */
void BSplineFree(BSpline *spline)
{
  if (spline->NoCtrlPts > 0){
	if (spline->PtsList  !=NULL)   free(spline->PtsList);
	if (spline->tmpPtsMat!=NULL)   free(spline->tmpPtsMat);
	if (spline->ctrlPts  !=NULL)   free(spline->ctrlPts);
	if (spline->mult4Pts !=NULL)   free(spline->mult4Pts);
	if (spline->indPts4Span!=NULL) free(spline->indPts4Span);
	if (spline->contour  !=NULL)   free(spline->contour);
	if (spline->contourPts  !=NULL) {
	  free(spline->contourPts[0]);
	  free(spline->contourPts);
	}
  }
  BSplineInit(spline);
  sprintf(spline->errorMessage,"BSpline memory free");
  fprintf(stderr, "BSpline memory free\n");
}

/* Funcion a modificar .. */
int BSplineCurveCoef(BSpline *spline, int Span, double *coefPolys, int indPt, CvPoint *P)
{
  int i, NoPts4Span = spline->NoPts4Span;
  int    indSpanMat = Span*NoPts4Span;
  int  *indPts4Span = spline->indPts4Span;
  CvPoint PtSust, *ctrlPts = spline->ctrlPts;
  double *tmpPtsMat = spline->tmpPtsMat;

  if ((Span >= spline->NoSpans) || (Span<0)) {
    sprintf(spline->errorMessage,"Wrong span number!");
    return 0;
  }

  for(i=0;i<NoPts4Span;i++){
    if ((indPt>=0) && (indPts4Span[indSpanMat+i]==indPt)) PtSust = *P;
    else PtSust = ctrlPts[indPts4Span[indSpanMat+i]];

    tmpPtsMat[i]=(double) PtSust.x;
    tmpPtsMat[i+NoPts4Span]=(double) PtSust.y;
  }

  matmatd(DIM,NoPts4Span,NoPts4Span,tmpPtsMat,cubicBSplines,coefPolys);
  return 1;
}

int BSplineCurveCoefsust2Pt(BSpline *spline, int Span, double *coefPolys, CvPoint *P1, CvPoint *P2)
{
  int i, NoPts4Span = spline->NoPts4Span;
  int    indSpanMat = Span*NoPts4Span;
  int  *indPts4Span = spline->indPts4Span;
  CvPoint PtSust, *ctrlPts = spline->ctrlPts;
  double *tmpPtsMat = spline->tmpPtsMat;

  if ((Span>=spline->NoSpans) || (Span<0)) {
    sprintf(spline->errorMessage, "Wrong span number!");
    return 0;
  }

  for(i=0;i<NoPts4Span;i++){

    if (i==1) PtSust = *P1;
    else if (i==2) PtSust = *P2;
    else PtSust = ctrlPts[indPts4Span[indSpanMat+i]];

    tmpPtsMat[i]=(double) PtSust.x;
    tmpPtsMat[i+NoPts4Span]=(double) PtSust.y;
  }

  matmatd(DIM,NoPts4Span,NoPts4Span,tmpPtsMat, cubicBSplines,coefPolys);
  return 1;
}
/*****************************************************************************
*****************  Evaluation of Splines           ***************************
********   t => [0,1]
******************************************************************************/
int BSplineEvalBSpline(BSpline *spline, int spanNo, double t, double *x, double *y){
    int  NoPts4Span = spline->NoPts4Span;
    double *contour = spline->contour;
    int   i, j, pos = 2*spanNo*NoPts4Span;/** 2 = DIM  **/

    if (t<=0){
      *x = contour[pos +   NoPts4Span - 1];
      *y = contour[pos + 2*NoPts4Span - 1];
    }else if (t>=1.0){/** � verifier -> gavina   t=0 <=> t=1.0 **/
      *x = contour[pos];
      *y = contour[pos + NoPts4Span];
      for (i=1, j=NoPts4Span+1 ; i<NoPts4Span ; i++,j++){
	*x += contour[pos+i];
	*y += contour[pos+j];
      }
    } else {
      *x = contour[pos];
      *y = contour[pos + NoPts4Span];
      for (i=1, j=NoPts4Span + 1 ; i<NoPts4Span ; i++,j++){
	*x = (*x)*t + contour[pos+i];
	*y = (*y)*t + contour[pos+j];
      }
    }
    if ((t<0)||(t>1.0)){
      sprintf(spline->errorMessage, "Evaluation parameter out of range");
      return -1;
    }

    return (0);
}
