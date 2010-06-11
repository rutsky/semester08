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
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Enumerations.H>

#include "hla.h"

STATIC_ASSERT(sizeof(Fl_Color) == 4);

class RenderFrame
{
public:
  RenderFrame()
    : w_(0)
    , h_(0)
  {
  }
  
  void resize( size_t w, size_t h )
  {
    w_ = w;
    h_ = h;
    size_t const newSize = lineSize() * height();
    if (newSize > buffer_.size())
      buffer_.resize(newSize);
  }
  
  size_t width() const { return w_; }
  size_t height() const { return h_; }
  
  void putPixel( size_t x, size_t y, Fl_Color color )
  {
    (*this)(x, y) = color;
  }
  
  size_t pixelSize() const { return sizeof(Fl_Color); }
  size_t lineSize() const { return pixelSize() * width(); }
  uchar const * buffer() const { return (uchar const *)&buffer_[0]; };
  
  void clear( uchar byte = 0 )
  {
    std::memset(&buffer_[0], byte, buffer_.size());
  }
  
private:
  Fl_Color & operator () ( size_t x, size_t y )
  {
    return buffer_[lineSize() * y + x];
  }
  
  Fl_Color const & operator () ( size_t x, size_t y ) const
  {
    return buffer_[lineSize() * y + x];
  }

private:
  size_t w_, h_;
  std::vector<Fl_Color> buffer_;
};

class Viewport : public Fl_Box
{
public:
  Viewport( int x, int y, int w, int h, char const *l = 0 )
    : Fl_Box(x, y, w, h, l)
    , funcIdx_(0)
  {
  }
  
  void setFunction( int idx )
  {
    std::cout << "setFunction(" << idx << ")\n";
    funcIdx_ = idx;
  }
  
  void draw()
  {
    frame_.resize(w(), h());
    frame_.clear();
    
    std::cout << "draw(" << x() << ", " << y() << ", " << w() << ", " << h() << ");\n"; // DEBUG
    
    
    
    fl_draw_image(frame_.buffer(), x(), y(), frame_.width(), frame_.height(), frame_.pixelSize(), frame_.lineSize());
  }
  
private:
  RenderFrame frame_;
  int funcIdx_;
};

#endif // VIEWPORT_H
