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
#include <cstdlib>

#include <FL/Fl.H>

#define HAVE_GL 1

#if HAVE_GL
#  include <FL/Fl_Gl_Window.H>
#  include <FL/gl.h>
#else
#  include <FL/Fl_Box.H>
#endif // HAVE_GL

#include <FL/fl_draw.H>

#if HAVE_GL
class Viewport : public Fl_Gl_Window
#else
class Viewport : public Fl_Box
#endif // HAVE_GL
{
public:
  Viewport( int x, int y, int w, int h, char const *l = 0 )
#if HAVE_GL
    : Fl_Gl_Window(x, y, w, h, l)
#else
    : Fl_Box(x, y, w, h, l)
#endif // HAVE_GL
  {
  }
  
  void draw()
  {
#if HAVE_GL
    std::cout << "Have GL\n";
#else
    std::cout << "Don't have GL\n";
#endif // HAVE_GL
    
    std::cout << "draw(" << x() << ", " << y() << ", " << w() << ", " << h() << ");\n";
    size_t const size = w() * h() * 3;
    if (buffer_.size() < size)
      buffer_.resize(size);
    
    std::cout << buffer_.size() << "\n";
    for (size_t i = 0; i < size; ++i)
    {
      //std::cout << i << "\n";
      buffer_[i] = (uchar)std::rand(); 
    }
    
    fl_draw_image(&buffer_[0], x(), y(), w(), h(), 3, 0);
  }
  
private:
  std::vector<uchar> buffer_;
};

#endif // VIEWPORT_H
