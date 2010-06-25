/*
 *  This file is part of FHA - Floating Horizon Algorithm implementation.
 *
 *  Copyright (C) 2010  Vladimir Rutsky <altsysrq@gmail.com>
 *
 *  FHA is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FHA is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FHA.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Main.
 */

#include "precompiled.hpp"

#include <iostream>
#include <memory>

#include <Eigen/Core>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "main_window.h"

#include "platform.h"

// import most common Eigen types 
USING_PART_OF_NAMESPACE_EIGEN

MAIN_FUNCTION
{
  {
    std::auto_ptr<MainWindow> mainWindow(
      new MainWindow(745, 515, "Floating Horizon Algorithm. 4057/2  Rutsky Vladimir, 2010"));
    
    Fl::visual(FL_DOUBLE | FL_INDEX | FL_RGB);

    mainWindow->show();

    Fl::run();
  }
}
