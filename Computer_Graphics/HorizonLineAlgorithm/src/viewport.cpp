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

#include "viewport.h"

#include "main_window.h"

namespace viewport
{
  void Viewport::resize( int x, int y, int w, int h )
  {
    Fl_Box::resize(x, y, w, h);
    frame_.resize(w, h);
    
    // TODO
    static_cast<MainWindow *>(parent()->parent()->parent()->parent())->updateYViewVolume();
    // debug
    //std::cout << "parent 4: " << parent()->parent()->parent()->parent() << "\n";
  }
}
