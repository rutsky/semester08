/*
 *  This file is part of HLA - Horizon Line Algorithm implementation.
 *
 *  Copyright (C) 2010  Vladimir Rutsky <altsysrq@gmail.com>
 *
 *  HLA is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HLA is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HLA.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * Main.
 */

#include "precompiled.hpp"

#include <iostream>

#include <Eigen/Core>
#include <CImg.h>

#include "platform.h"

// import most common Eigen types 
USING_PART_OF_NAMESPACE_EIGEN

using namespace cimg_library;

MAIN_FUNCTION
{
  // 640x400 color image with 8 BPP.
  CImg<unsigned char> image(640, 400, 1, 3);

  // Fill with black.
  image.fill(0); 

  unsigned char const red[] = { 255, 0, 0 }, white[] = {255, 255, 255};

  image.draw_text(100, 100, "Hello, World!", red);

  //image.display("CImg: Hello, World!");
  
  CImgDisplay mainDisp(image, "HLA - Vladimir Rutsky, 2010");
  while (!mainDisp.is_closed())
  {
    mainDisp.wait();
    if (mainDisp.is_keyQ())
      break;
    
    image.draw_text(100, 130, "test", white);
    
    image.display(mainDisp);
  }
  
  {
    // Basic Eigen test (from Eigen's documentation).
    int rows=5, cols=5;
    MatrixXf m(rows,cols);
    m << (Matrix3f() << 1, 2, 3, 4, 5, 6, 7, 8, 9).finished(),
      MatrixXf::Zero(3,cols-3),
      MatrixXf::Zero(rows-3,3),
      MatrixXf::Identity(rows-3,cols-3);
    std::cout << m << "\n";
  }
}
