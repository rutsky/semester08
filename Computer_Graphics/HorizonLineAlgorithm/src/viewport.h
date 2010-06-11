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

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <iostream>
#include <memory>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

class Viewport : public Fl_Box 
{
public:
  Viewport( int x, int y, int w, int h, char const *l = 0 )
    : Fl_Box(x, y, w, h, l)
  {
  }
  
  void draw()
  {
    std::cout << "draw(" << x() << ", " << y() << ", " << w() << ", " << h() << ");\n";
    size_t const size = w() * h() * 3;
    if (buffer_.size() < size)
      buffer_.resize(size);
    fl_draw_image(&buffer_[0], x(), y(), w(), h(), 3, 0);
  }
  
private:
  std::vector<uchar> buffer_;
};

#endif // VIEWPORT_H
