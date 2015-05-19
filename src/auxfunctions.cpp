/*
 * auxfunctions.c
 *
 *  Created on: Feb 13, 2013
 *      Author: luzdora
 */

/******** AUXILIAR FUNCTIONS FOR B-SPLINES CALCULATIONS***********/


#include "bspline.hpp"
#include "../../../KLT/include/boost_incl.hpp"


static CvPoint neighb[]={{ 0, 0}, { 1, 0}, { 1, 1},
		{ 0, 1}, {-1, 1}, {-1, 0},
		{-1,-1}, { 0,-1}, { 1,-1}};


void fillCloseRegionInMask( unsigned char *fm, int c, int r)
{
	int flag, i, j;
	int *ones = NULL;
	int sum=0;
	ones = (int *)calloc(c, sizeof(int));


	for (i=0; i <c; i++ ){
		for (j=0; j < r; j++)
			sum+= fm[j*c+i];
		if (sum >1)
			ones[i] = sum;
		else
			ones[i] = 0;
		sum = 0;
		//   printf(" columna %d, renglon %d, ones = %d ", i, j, ones[i]);

	}

	// printf("Renglon columna %d, %d, ones = %d ", r, c, ones[i]);

	for (i=0; i <c; i++ ){
		flag =0; // la bandera se limpia por columna
		if (ones[i] != 0){
			//   printf( "\n ones[%d] = %d " , i, ones[i]);
			if (!(ones[i]%2))
				for (j=0; j < r; j++){
					// Primero encuentra cuantos unos hay por renglon
					if (fm[j*c+i]==1 && flag==0)
						flag = 1;
					else if (fm[j*c+i]==0 && flag)
						fm[j*c+i]=1;
					else if (fm[j*c+i]==1 && flag)
						flag =0;
				}
			else
				for (j=0; j < r; j++)
				{
					// Primero encuentra cuantos unos hay por renglon
					if (fm[j*c+i]==1 && flag==0) //init
						flag = 1;
					else if (fm[j*c+i]==1 && flag==2) //stop
						flag =0;
					else if (flag == 2 || flag == 1)  //refill with 1's
						(fm[j*c+i]==1) ? (flag++) : (fm[j*c+i] = 1);
				}

		} // end diff cero
	}
	// printf("\n Before to leave");

	free(ones);
	//printf("\n Before to leave");
}


float meanInRegion(IplImage *ima, unsigned char *mask)
{
	int i,j, nd =0;
	float sum =0.0;

	for (i=0; i<ima->width; i++)
		for(j=0; j<ima->height; j++)
			if (mask[j*ima->width+i]){
				sum += ima->imageData[j*ima->widthStep+i];
				nd++;
			}
	return(sum/(float)nd);
}


int integerValuesInLine (CvPoint *P0, CvPoint *P1, int **Cd, int *index)
{

	int dx, dy;
	float m, x, y;

	dx = fabs(P1->x - P0->x);
	dy = fabs(P1->y - P0->y);

	if ((dx == 0) && (dy == 0)) // Check anormally cases
	{
		Cd[0][*(index)] = P0->x;
		Cd[1][*(index)] = P0->y;
		*(index)=*(index)+1;
		return 0;
	}

	if (dx >= dy){
		m= (P1->y - P0->y)/(P1->x - P0->x);
		if (P0->x > P1->x)
			for (x = P0->x; x >= P1->x; x--,*(index)=*(index)+1)
			{
				Cd[0][*(index)] = x;
				Cd[1][*(index)] = round(P0->y + m*(x - P0->x) + 0.5);
			}
		else
			for (x = P0->x; x <= P1->x; x++,*(index)=*(index)+1)
			{
				Cd[0][*(index)] = x;
				Cd[1][*(index)] = round(P0->y + m*(x - P0->x) + 0.5);
			}
	} else {
		m= (P1->x - P0->x)/(P1->y - P0->y);
		if (P0->y > P1->y)
			for (y = P0->y; y >= P1->y; y--,*(index)=*(index)+1)
			{
				Cd[0][*(index)] =round(P0->x + m*(y - P0->y) + 0.5) ;
				Cd[1][*(index)] = y;
			}
		else
			for (y = P0->y; y <= P1->y; y++,*(index)=*(index)+1)
			{
				Cd[0][*(index)] =round(P0->x + m*(y - P0->y) + 0.5) ;
				Cd[1][*(index)] = y;
			}
	}

	return 0;
}


int integerValuesInLine2 (CvPoint *P0, CvPoint *P1, float **Cd, int *index)
{

	int dx;
	float m, x;
//	float y;

	//  printf(" Index al entrar %d \n ", *(index));

	dx = fabs(P1->x - P0->x);

	if (dx > 0) // Check anormally cases
	{
		m= (P1->y - P0->y)/(P1->x - P0->x);
		if (P0->x < P1->x)
			for (x = P0->x+1; x <= P1->x; x++,*(index)=*(index)+1)
			{
				Cd[0][*(index)] = x;
				Cd[1][*(index)] = P0->y + m*(x - P0->x);
			}
		else
			for (x = P0->x - 1; x >= P1->x; x--,*(index)=*(index)+1)
			{
				Cd[0][*(index)] = x;
				Cd[1][*(index)] = P0->y + m*(x - P0->x);
			}
	}

	//  printf(" Index al salir %d \n ", *(index));


	return 0;
}

void writeMascara (bool *mascara, float tamanio, int numOb)
{
	FILE *fp;
//	int i;
	char fname[40];

	sprintf(fname, "/tmp/mask%d.bin", numOb);

	fp = fopen(fname , "wb");
	if (fp == NULL)
		printf(" SNAKE: Can't open file '%s' for writing", fname);

	fwrite(mascara, tamanio, sizeof(bool), fp);
	//  printf("\n Tamanio MASCARA = %d \n ",sizeof(mascara) );
	/*  for (i = 0 ; i < 640*480 ; i++)  {
    printf(" %d ", (int)mascara[i]);
    } */
	fclose(fp);
}


float **reserve(int nr, int nc)
{
	float **m;
	int a;
	m = (float **) calloc(nr, sizeof(float*));
	m[0] = (float *) calloc((nr*nc), sizeof(float));
	for(a = 1; a < nr; a++) m[a] = m[a-1]+nc;
	return m;
}

void freeMat(float **m)
{
	free(m[0]);
	free(m);
}


CvPoint ptBetweenPts(float t, CvPoint *P0, CvPoint *P1)
{
	CvPoint     P;

	if (t==0.0)      return *P0;
	else if (t==1.0) return *P1;

	P.x = P0->x + (int)(t*((float)P1->x - P0->x));
	P.y = P0->y + (int)(t*((float)P1->y - P0->y));

	return P;
}


float fdist(CvPoint *P0, CvPoint *P1)
{
	float r, t;
	t = P1->x - P0->x;
	r = t*t;
	t = P1->y - P0->y;
	r+= t*t;
	return sqrt(r);
}

int PtInRect(CRect *pt, CvPoint *P)
{
	CvPoint *P1 = &pt->P1;
	CvPoint *P2 = &pt->P2;

	if ((P->x > P1->x) && (P->x < P2->x)
			&& (P->y > P1->y) && (P->y < P2->y))
		return VERD;
	else
		return FALSO;
}

/*Straight Line */
void Straight2DLine (Straight2DLn *line, CvPoint *P0, CvPoint *P1)
{
	double dx = P1->x - P0->x;
	if (dx == 0) {
		line->b = 0;
		line->a = 1;
		line->c = -P0->x;
	} else {
		line->b = 1;
		line->a = (double)(P0->y - P1->y)/dx;
		line->c = -line->a*P0->x - P0->y;
	}
	/*  printf("%fx + %fy + %f = 0\n",a,b,c); */
}

void Straight2DLineM(Straight2DLn *line, double m, CvPoint *P)
{
	line->b = 1;
	line->a = m;
	line->c = -line->a*P->x - P->y;
}

/*******************************************************************/

CvPoint neighbor(CvPoint *P, int No, int sigma)
{
	CvPoint Pt;
	if (No==0) return *P;

	Pt = neighb[No];
	Pt.x = P->x + Pt.x*sigma;
	Pt.y = P->y + Pt.y*sigma;
	return Pt;
}

/*******************************************************************/

int LnIntersec(Straight2DLn *L1, Straight2DLn *L2, CvPoint *P){
	int  d;
	double M[6];
	double r[2];

	if ((L1->b==1) && (L2->b == 1)){
		P->x = (int)((L2->c - L1->c)/(L1->a - L2->a));
		P->y = (int)(-L1->a*P->x - L1->c);
		return 0;
	} else {
		M[0] = L1->a;
		M[1] = L1->b;
		M[2] = L1->c;
		M[3] = L2->a;
		M[4] = L2->b;
		M[5] = L2->c;
		/* 	 	printf("=> %fx + %fy + %f = 0\n",L1->a,L1->b,L1->c); */
		/* 	 	printf("=> %fx + %fy + %f = 0\n",L2->a,L2->b,L2->c); */
		d = linearSol(2, M, r);
		if (d == 1) return (0);
		P->x = (int) r[0];
		P->y = (int) r[1];
		/*   printf("centre =(%d,%d)\n",P->x,P->y); */
	}
	return 0;
}

CvPoint quadCenter(CvPoint *P0, CvPoint *P1, CvPoint *P2, CvPoint *P3)
{
	CvPoint P;
	Straight2DLn L02, L13;

	Straight2DLine(&L02, P0, P2);
	Straight2DLine(&L13, P1, P3);

	LnIntersec(&L02, &L13, &P);

	return P;
}

int getStrLn4X(int NoPts, CvPoint *Pts, Straight2DLn *Ln)
{
	register int i;
	double Sx=0.0f,Sy=0.0f,Sxy=0.0f,Sx2=0.0f;
	double div;

	if (NoPts<2) return 1;

	for (i=0;i<NoPts;i++){
		Sx+=Pts[i].x;
		Sy+=Pts[i].y;
		Sxy+=Pts[i].x*Pts[i].y;
		Sx2+=Pts[i].x*Pts[i].x;
	}

	div=(NoPts*Sx2-Sx*Sx);

	if (div==0) return 1;

	Ln->b=1;
	Ln->c=(Sx*Sxy-Sy*Sx2)/div;
	Ln->a=(Sx*Sy-NoPts*Sxy)/div;

	/* printf("%d=> %fx + %fy + %f = 0\n",NoPts,Ln->a,Ln->b,Ln->c); */
	return 0;
}

int getStrLn4Y(int NoPts, CvPoint *Pts, Straight2DLn *Ln)
{
	register int i;
	double Sx=0.0f,Sy=0.0f,Sxy=0.0f,Sy2=0.0f;
	double div;

	if (NoPts<2) return 1;

	for (i=0;i<NoPts;i++){
		Sx+=Pts[i].x;
		Sy+=Pts[i].y;
		Sxy+=Pts[i].x*Pts[i].y;
		Sy2+=Pts[i].y*Pts[i].y;
	}

	div=(NoPts*Sy2-Sy*Sy);

	if (div==0) return 1;

	Ln->a=1;
	Ln->c=(Sy*Sxy-Sx*Sy2)/div;
	Ln->b=(Sx*Sy-NoPts*Sxy)/div;

	/*  printf("%d=> %fx + %fy + %f = 0\n",NoPts,Ln->a,Ln->b,Ln->c); */
	return 0;
}

double distPt2Ln(CvPoint *P, Straight2DLn *Ln)
{
	double d;

	d = (Ln->a*P->x + Ln->b*P->y + Ln->c)/
			sqrt(Ln->a*Ln->a + Ln->b*Ln->b);

	return d;
}

/********************************************************************/
/* Matematics funtions for matrix manipulation                      */
/********************************************************************/

/*-------------------------------------------------------------------
 *  function:  getImgDiffAbs ->
 *  param   :
 *  return  :
 *------------------------------------------------------------------*/
int deter_Of_2D_Pts(int *Pt0, int *Pt1, int *Pt2)
{
	int r;
	r = Pt0[0]*(Pt1[1]-Pt2[1]) -
			Pt1[0]*(Pt0[1]-Pt2[1]) +
			Pt2[0]*(Pt0[1]-Pt1[1]);
	return r;
}
/********************************************************************/
/*   Matematic funtion for the multiplication of two matrix         */
/*                                                                  */
/*                      (A x B) = R                                 */
/*   where:                                                         */
/*           A = m*n                                                */
/*           B = n*o                                                */
/*                                                                  */
/*   and the results is a matrix R of:                              */
/*                                                                  */
/*           R = m*o                                                */
/********************************************************************/
void matmat(int m, int n, int o, float *A, float *B,float *R)
{
	register int i, j, k;
	int index = 0;
	for (i=0;i<m;i++){
		for (j=0;j<o;j++){
			index = i*o+j;
			R[index] = 0;
			for(k=0;k<n;k++)  /* ToDo - Optimization gavina*/
				R[index] += A[k + i*n]*B[k + j*o];
		}
	}
}

void matmatd(int m, int n, int o, double *A, double *B,double *R)
{
	register int i, j, k;
	int index = 0;

	for (i=0; i<m; i++){
		for (j=0; j<o; j++){
			index = i*o + j;
			R[index] = 0;
			for(k=0; k<n; k++)
				R[index] += A[k+i*n] * B[k+j*o];
		}
	}
}

/****************************************************************/
/* Function:                                         */
/*                                                              */
/* Description:   Solution of Linear Algebraic Equations (n x m)       */
/*                                                              */
/* Parameters:     int      n       dimension                   */
/*                 double   *mat    addres of the matrix        */
/*                                                              */
/* Returns:        double                                       */
/*                                                              */
/****************************************************************/
void changeLnInMat(int dim, int lnNo, int rpLn, double *M)
{
	register int i,diml;
	int lnNodim,rpLndim;
	double  temp;

	diml = dim+1;
	lnNodim = lnNo*diml;
	rpLndim = rpLn*diml;
	for (i=0; i<=dim; i++){
		temp = M[lnNodim + i];
		M[lnNodim + i] = M[rpLndim + i];
		M[rpLndim + i] = temp;
	}
}

/****************************************************************/
/******************** Method gavina ?? **************************/
/****************************************************************/
int linearSol(int dim, double *M, double *R)
{
	int i, j, k, chng=0, pivot=0; /*gavina: pivot ????*/
	int   idim, kdim, diml=dim+1;
	double          max, det=1.0;

	for (i=0;i<dim;i++){
		idim=i*diml;
		max=0.0;
		for (j=i;j<dim;j++){
			if (fabs(M[idim+j])>max) {
				pivot=j;   /* finding pivots */
				max = fabs(M[idim+j]);
			}
		}
		if (max==0.0) return 1;
		else if (pivot>i) {
			changeLnInMat(dim, i, pivot, M);/*remplacing lines w/higher pivots*/
			chng++;
		}
		det *= M[idim+i];
		for (k = i+1 ; k < dim ; k++){
			kdim = k*diml;
			for (j=i;j<=dim;j++){
				M[kdim+j] = M[kdim+j]-M[kdim+i]*M[idim+j]/M[idim+i];
			}
		}
	}
	i--;
	idim=i*diml;
	R[i]=-M[idim+dim]/M[idim+i];
	for (i--;i>=0;i--){
		idim=i*diml;
		R[i]=-M[idim+dim];
		for(j=i+1;j<dim;j++){
			R[i]-=M[idim+j]*R[j];
		}
		R[i]=R[i]/M[idim+i];
	}
	return 0;
}

/***************************************************************
 ************* Saving a vector of points into a file *****************
 ***************************************************************/
int SaveContours(char *file, CvPoint *contours, int No){
	register int     i;
	FILE    *fp = NULL;
	CvPoint *ptr = NULL;

	if (file != NULL){/*Saving into a file*/
		if (strlen(file)< 4){
			fprintf(stderr, "SaveContours: %s isnot a valid contour filename...\n", file);
			return -1;
		}
		if ((fp = fopen(file,"w+"))==NULL){
			fprintf(stderr, "Error Open File %s...\n", file);
			return -1;
		}
		for (ptr= contours, i= No; i--; ptr++)
			fprintf(fp, "%d %d\n", ptr->x, ptr->y);/**  y|^   ->x **/
		fclose(fp);
		/* Someone else may use this file*/
		chmod(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	}

	return No;
}
