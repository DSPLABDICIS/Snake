/*
 * bspline.h
 *
 *  Created on: Feb 13, 2013
 *      Author: luzdora
 */

#ifndef BSPLINE_H_
#define BSPLINE_H_

//#include "jafarConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <sys/stat.h>
#include <stdbool.h>


// Opencv libraries
#include <cv.h>
#include <highgui.h>
#include "../../../KLT/include/boost_incl.hpp"

#ifdef __cplusplus
extern "C" {
#endif

  //#include "klt/kltTest.hpp"

/*******************************************************************
 **** ****** Cpoint Structs -> Points 2D, Splines  *****************
*******************************************************************/
//typedef struct CPoint{int x, y;} CPoint;


/*** Valeurs boolean ***/

typedef enum BOOLEAN {
     FALSO = 0,
     VERD  = 1
}BOOLEAN;


/*********
 *********   TYPES   *********
 *********/

/** Class Straight line expressed as f(x,y)= ax+by+c=0 */
typedef struct Straight2DLn{
  double a; /* slope of the straight line */
  double b; /* coef. for y (1 or 0) */
  double c; /* absise */
} Straight2DLn;

//int TRACK_TARGET=1;

typedef struct CRect{CvPoint P1, P2;}CRect;

typedef struct CSize{int cx, cy;}CSize;


/******* B- SPLINES *******/
/*Bsplines types" */

typedef enum BSplineType   {
        CLOSE = 0,
        OPEN  = 1
}BSplineType;

typedef enum BSplineOrder   {
  BS_LINEAR =1,
  BS_QUADRATIC,
  BS_CUBIC     /*Road*/
} BSplineOrder;

#define DIM 2
typedef struct BSpline {
  /*   int DIM=2;    */      /* dimension (temp fixe) */
  int NoCtrlPts;        /* Number of control points */
  int NoTotPts;         /* Number total of points */
  int NoSpans;          /* Number of spans */
  int NoPts4Span;       /* Number of points for each span */
  int NoPtsMult;        /* Number of discontinuities */
  int *PtsList;         /* Ordered List of Points */

  BSplineType closure;  /* closure of BSpline */
  BSplineOrder order;   /*  order of BSpline */

  double *tmpPtsMat;    /* temporal Points Matrix */
  CvPoint *ctrlPts;      /* Positions of control Points */
  /* int *ind4PtMult; */  /*  indices of CtrlPts for discontinuities */
  int *mult4Pts;        /* Multiplicity */
  int *indPts4Span;     /* indices of points for each Span */
  double *contour;      /* coeficients of contour */
  float **contourPts;      /* contour discretization */
  char errorMessage[64]; /* error message */
}BSpline;



  CvPoint ptBetweenPts ( float t, CvPoint *P0, CvPoint *P1 );
  float fdist ( CvPoint *P0, CvPoint *P1 );
  int PtInRect ( CRect *pt, CvPoint *P );
  void Straight2DLine ( Straight2DLn *line, CvPoint *P0, CvPoint *P1 );
  void Straight2DLineM ( Straight2DLn *line, double m, CvPoint *P );
  CvPoint neighbor ( CvPoint *P, int No, int sigma );
  int LnIntersec ( Straight2DLn *L1, Straight2DLn *L2, CvPoint *P );
  CvPoint quadCenter ( CvPoint *P0, CvPoint *P1, CvPoint *P2, CvPoint *P3 );
  int getStrLn4X ( int NoPts, CvPoint *Pts, Straight2DLn *Ln );
  int getStrLn4Y ( int NoPts, CvPoint *Pts, Straight2DLn *Ln );
  double distPt2Ln ( CvPoint *P, Straight2DLn *Ln );
  int deter_Of_2D_Pts ( int *Pt0, int *Pt1, int *Pt2 );
  void matmat ( int m, int n, int o, float *A, float *B, float *R );
  void matmatd ( int m, int n, int o, double *A, double *B, double *R );
  void changeLnInMat ( int dim, int lnNo, int rpLn, double *M );
  int linearSol ( int dim, double *M, double *R );
  int SaveContours ( char *file, CvPoint *contours, int No );
  int BSplineInit ( BSpline *spline );
  int BSplineInitStr ( BSpline *spline, int n, CvPoint *pts, BSplineOrder ord, BSplineType type, int *indMult );
  void BSplineCopy ( BSpline *spline, BSpline *splineDat );
  void BSplineInitMat ( BSpline *spline );
  void BSplineInitPtsList ( BSpline *spline );
  void BSplineInitSpanInd ( BSpline *spline );
  void BSplineInitContour ( BSpline *spline );
  void BSplineFree ( BSpline *spline );
  int BSplineCurveCoef ( BSpline *spline, int Span, double *coefPolys, int indPt, CvPoint *P );
  int BSplineCurveCoefsust2Pt ( BSpline *spline, int Span, double *coefPolys, CvPoint *P1, CvPoint *P2 );
  int BSplineEvalBSpline ( BSpline *spline, int spanNo, double t, double *x, double *y );

  int integerValuesInLine (CvPoint *P0, CvPoint *P1, int **Cd, int *index);
 int integerValuesInLine2 (CvPoint *P0, CvPoint *P1, float **Cd, int *index);

  void fillCloseRegionInMask( unsigned char *fm, int c, int r);
  //  void setContourInMask(int x, int y, unsigned char *fm, int cols, int rows);
   float meanInRegion(IplImage *ima, unsigned char *mask);

  void writeMascara(bool *mascara, float tamanio, int numOb);
 // void readMascara (bool *mascara, float tamanio);
  float **reserve(int nr, int nc);
  void freeMat(float **m);


#ifdef __cplusplus
}

#endif
#endif /* BSPLINE_H_ */
