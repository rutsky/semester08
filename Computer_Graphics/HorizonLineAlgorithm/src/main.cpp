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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "platform.h"
#include "hla.h"

// import most common Eigen types 
USING_PART_OF_NAMESPACE_EIGEN

MAIN_FUNCTION
{
  {
    Fl_Window *window = new Fl_Window(300,180);
    Fl_Box *box = new Fl_Box(20,40,260,100,"Hello, World!");
    box->box(FL_UP_BOX);
    box->labelsize(36);
    box->labelfont(FL_BOLD+FL_ITALIC);
    box->labeltype(FL_SHADOW_LABEL);
    window->end();
    //window->show(argc, argv);
    window->show();
    std::cout << Fl::run() << "\n";
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
