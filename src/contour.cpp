/*
 * contour.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: luzdora
 */

#include "regions.hpp"

namespace snake {

boost_incl::mat temporalInputData()
{
	int i;
	boost_incl::mat _Xp(8,2) ;
	_Xp(0,0)= 195; _Xp(0,1)=168;
	_Xp(1,0)=202; _Xp(1,1)=187;
	_Xp(2,0)=208; _Xp(2,1)=197;
	_Xp(3,0)= 252 ; _Xp(3,1)= 191;
	_Xp(4,0)= 231 ; _Xp(4,1)= 120 ;
	_Xp(5,0)= 228 ; _Xp(5,1)=117 ;
	_Xp(6,0)= 213  ; _Xp(6,1)= 129;
	_Xp(7,0)= 198 ; _Xp(7,1)=148 ;


	for (i = 0 ; i < 8 ; i++)
	{
		_Xp(i,0) -= 189;
		_Xp(i,1) -= 110;
	}

	return (_Xp);
}


int findContour(boost_incl::mat _Xp, boost_incl::mat &_Cp )
{
	/*      // Display
      IplImage *m_Ipl;
      IplImage *IplTmp1;
      m_Ipl = cvLoadImage("/home/luzdora/Grossis/pruebasC/Programs/images/centroC.jpg", CV_LOAD_IMAGE_COLOR);
      IplTmp1 = cvCreateImage( cvSize(m_Ipl->width,m_Ipl->height), IPL_DEPTH_8U, 3 );
      cvCopyImage( m_Ipl,IplTmp1);
	 */

	int n = _Xp.size1(); // number of points
	int i,j, maxi, MaxPix;
	BSpline sp;
	int noCtrlPts =0;
	double cx, cy, s;
	CvPoint *PointArray;
	PointArray = (CvPoint *)malloc(n * sizeof(CvPoint));

	for (i = 0 ; i < n ; i++)
	{
		PointArray[i].x = _Xp(i,0);
		PointArray[i].y = _Xp(i,1);
	}

	/****************************/
	BSplineInit(&sp);

	// Ese valor de NULL es para que no haya multiplicidad
	noCtrlPts = BSplineInitStr(&sp, n, PointArray, BS_CUBIC, CLOSE, NULL);

	// Para ver que hace el contorno
	maxi = sp.NoSpans;
	MaxPix = 5; // aun no se porque 3
	int indx=0;
	CvPoint p[maxi*(MaxPix+1)];

	for (i=0 ; i<maxi ; i++){
		for(j=0 ; j<=MaxPix ; j++) {
			s = (double) j/MaxPix;
			BSplineEvalBSpline(&sp, i, s, &cx, &cy);
			p[i*(MaxPix+1)+j].x = cx;
			p[i*(MaxPix+1)+j].y = cy;
		}
		//	std::cout << "Evalua Bspline del span= " << i << std::endl;
	}

	// Busca todos los puntos entre dos puntos unidos por una recta
	sp.contourPts[0][0]= p[0].x;
	sp.contourPts[1][0]= p[0].y;
	indx = 1;
	for(i=0 ; i<maxi*(MaxPix+1)-1; i++)
		integerValuesInLine2(&p[i], &p[i+1], sp.contourPts, &indx);

	//       printf(" Puntos discretizados en el contorno: %d \n", indx);

	_Cp.resize(indx,2);

	// puntos restantes sobre el contorno
	for(i=0 ; i<indx; i++){
		_Cp(i,0) = sp.contourPts[0][i];
		_Cp(i,1) = sp.contourPts[1][i];
		//	setContourInMask(pc.x, pc.y, mascara,w,h);
	}
	//    fillCloseRegionInMask(mascara,w,h);
	/*
      for (i=0; i <m_Ipl->width; i++ )
	for (j=0; j < m_Ipl->height; j++)
	  if (mascara[j*m_Ipl->width+i]==1)
	    {
	      pc.x= i;
	      pc.y= j;
	      cvCircle(IplTmp1, pc, 2, CV_RGB(0,0,255), CV_FILLED );
	    }
	 */
	/*******     Promedio ********/
	//  float media = meanInRegion( IplTmp1, mascara);
	//  printf("\n media en la region %f", media);


	/****** Para Display *********/
	/*
      for (int j=0; j<n; j++)
	cvCircle(IplTmp1, PointArray[j], 2, CV_RGB(255,255,255), CV_FILLED );

      cvNamedWindow( "image with contour", 1 );
      cvShowImage("image with contour", IplTmp1);
      cvWaitKey(0);
      cvDestroyWindow("image with puntos");
        cvReleaseImage(&m_Ipl);
	 */
	// Clean up
	free(PointArray);
	//    cvReleaseImage(&IplTmp1);
	//   if (mascara != NULL)
	//	free(mascara);

	BSplineFree(&sp);
	return 0;
}


void setContourAndRegionInMask(boost_incl::mat cont,  int cols, int rows, int UpLeft_X, int UpLeft_Y,
		unsigned char *featmap, int cfeatm, int rfeatm, int nIma)
{
	int i,j,x,y, corner;
	unsigned char *fm = (unsigned char *)calloc(cols * rows, sizeof(unsigned char));
	// TAMAÑO ORIGINAL DE LA IMAGEN (cfeatm, rfeatm)
	bool *temporal = (bool *) calloc(cfeatm * rfeatm , sizeof(bool));

	for (i = 0 ; i < (int)cont.size1(); i++)
	{
		x = cont(i,0) - UpLeft_X;
		y = cont(i,1) - UpLeft_Y;
		if (x >= 0 && x < cols && y >= 0 && y < rows)
			fm[y*cols+x] = 1;
		//	  printf(" \n Punto %d , %d  ", x, y);      // posicion =  y*cols+x ,fm[y*cols+x]);
	}

	fillCloseRegionInMask(fm, cols, rows);
	//  printf ("\n Despues de llenar la region cerrada ");
	corner = UpLeft_Y*cfeatm + UpLeft_X;
	//     printf ("\n Corner = %d ",corner );
	for (i=0; i <cols; i++ )
		for (j=0; j < rows; j++)
			if (fm[j*cols+i]==1){
				featmap[corner + j*cfeatm + i ] = 255;
				temporal[corner + j*cfeatm + i] = 1;
			}
	writeMascara(temporal, cfeatm*rfeatm, nIma);
	free(fm);
	free(temporal);
}
}// end namespace snake


//int main()
//{
//	using namespace boost_incl;
//
////	mat XP;
//	int c;
//	c=20;
//	std::cout << "Numero de clusters " << c << std::endl;
//	return 0;
//}

