/*
 * regions.hpp
 *
 *  Created on: Feb 10, 2013
 *      Author: luzdora
 */

#ifndef REGIONS_HPP_
#define REGIONS_HPP_

//#include "boost_incl.hpp"
#include "bspline.hpp"

#include <iostream>

//#include "klt/kltTest.hpp"
  namespace snake {
  using namespace boost_incl;
    /*
   class regions {
    public:
      regions(){}
      ~regions(){}
      BSpline sp;

    };
    */
    int findContour(mat _Xp, mat &_Cp);
    mat temporalInputData();
    void setContourAndRegionInMask(mat cont,  int cols, int rows, int UpLeft_X, int UpLeft_Y,
				   unsigned char *featmap, int cfeatm, int rfeatm, int nIma);
  }


#endif /* REGIONS_HPP_ */
