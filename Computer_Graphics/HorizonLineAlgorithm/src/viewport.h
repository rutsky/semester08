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

#include <boost/cstdint.hpp>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Group.H>

#include "appconf.h"
#include "function.h"
#include "hla.h"
#include "grid.h"
#include "edge.h"
#include "color.h"
#include "edge_gen.h"

class MainWindow;

namespace viewport
{
  USING_PART_OF_NAMESPACE_EIGEN
  
  namespace details
  {
    template< class ColorType >
    class RenderFrame
    {
    public:
      typedef ColorType color_t;
      
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
        size_t const newSize = w_ * h_;
        if (newSize > buffer_.size())
          buffer_.resize(newSize);
      }
      
      size_t width() const { return w_; }
      size_t height() const { return h_; }
      
      void putPixel( size_t x, size_t y, color_t color )
      {
        (*this)(x, y) = color;
      }
      
      color_t getPixel( size_t x, size_t y )
      {
        return (*this)(x, y);
      }
      
      size_t pixelSize() const { return sizeof(color_t); }
      size_t lineSize() const { return pixelSize() * width(); }
      uchar const * buffer() const { return (uchar const *)&buffer_[0]; };
      
      void clear( int byte = 0 )
      {
        std::memset(&buffer_[0], byte, buffer_.size() * sizeof(buffer_[0]));
      }
      
      void fill( color_t color )
      {
        std::fill(buffer_.begin(), buffer_.end(), color);
      }
      
    private:
      color_t & operator () ( size_t x, size_t y )
      {
        // Include correction to get around OY directed down.
        return buffer_.at(w_ * (h_ - 1 - y) + x);
      }
      
      color_t const & operator () ( size_t x, size_t y ) const
      {
        // Include correction to get around OY directed down.
        return buffer_.at(w_ * (h_ - 1 - y) + x);
      }

    private:
      size_t w_, h_;
      std::vector<color_t> buffer_;
    };
  }

  class Viewport : public Fl_Box
  {
  private:
    typedef details::RenderFrame<color::color_t> render_frame_t;
    
  public:
    Viewport( int x, int y, int w, int h, char const *l = 0 )
      : Fl_Box(x, y, w, h, l)
      , funcIdx_(appconf::startFuncIdx)
      , xCells_(appconf::startXCells)
      , yCells_(appconf::startYCells)
      , xDomain_(appconf::startXDomain)
      , yDomain_(appconf::startYDomain)
      , xViewVolume_(appconf::startXViewVolume)
      , yViewVolume_(appconf::startYViewVolume)
      , keepAspectRatio_(appconf::startKeepAspectRatio)
      , yaw_(appconf::startYaw)
      , pitch_(appconf::startPitch)
      , drawXEdges_(appconf::startDrawXEdges)
      , drawYEdges_(appconf::startDrawYEdges)
      , xDomainCenter_(appconf::startXDomainCenter)
      , yDomainCenter_(appconf::startYDomainCenter)
      , xViewVolumeCenter_(appconf::startXViewVolumeCenter)
      , yViewVolumeCenter_(appconf::startYViewVolumeCenter)
      , drawAxes_(appconf::startDrawAxes)
      , drawFakeEdges_(appconf::startDrawFakeEdges)
    {
      frame_.resize(w, h);
    }
    
    void setFunction( size_t idx )
    {
      std::cout << "setFunction(" << idx << ")\n"; // debug
      if (idx < function::nFunctions)
        funcIdx_ = idx;
      else
        std::cerr << "Error: idx >= function::nFunctions!\n";
    }
    
    void setXCells( size_t xCells )
    {
      std::cout << "setXCells(" << xCells << ")\n"; // debug
      if (xCells >= 1)
        xCells_ = xCells;
      else
        std::cerr << "Error: xCells < 1\n";
    }
    
    void setYCells( size_t yCells )
    {
      std::cout << "setYCells(" << yCells << ")\n"; // debug
      if (yCells >= 1)
        yCells_ = yCells;
      else
        std::cerr << "Error: yCells < 1\n";
    }
    
    void setXDomain( double xDomain )
    {
      std::cout << "setXDomain(" << xDomain << ")\n"; // debug
      if (xDomain > 0)
        xDomain_ = xDomain;
      else
        std::cerr << "Error: xDomain <= 0\n";
    }
        
    void setYDomain( double yDomain )
    {
      std::cout << "setYDomain(" << yDomain << ")\n"; // debug
      if (yDomain > 0)
        yDomain_ = yDomain;
      else
        std::cerr << "Error: yDomain <= 0\n";
    }
    
    void setXViewVolume( double xViewVolume )
    {
      std::cout << "setXViewVolume(" << xViewVolume << ")\n"; // debug
      if (xViewVolume > 0)
        xViewVolume_ = xViewVolume;
      else
        std::cerr << "Error: xViewVolume <= 0\n";
    }
    
    void setYViewVolume( double yViewVolume )
    {
      std::cout << "setYViewVolume(" << yViewVolume << ")\n"; // debug
      if (yViewVolume > 0)
        yViewVolume_ = yViewVolume;
      else
        std::cerr << "Error: yViewVolume <= 0\n";
    }

    void setKeepAspectRatio( bool keep )
    {
      std::cout << "setKeepAspectRatio(" << keep << ")\n"; // debug
      keepAspectRatio_ = keep;
    }
    
    void setYaw( double yaw )
    {
      std::cout << "setYaw(" << yaw << ")\n"; // debug
      yaw_ = yaw;
    }
    
    void setPitch( double pitch )
    {
      std::cout << "setPitch(" << pitch << ")\n"; // debug
      pitch_ = pitch;
    }
    
    void setDrawXEdges( bool isDraw )
    {
      std::cout << "setDrawXEdges(" << isDraw << ")\n"; // debug
      drawXEdges_ = isDraw;
    }
    
    void setDrawYEdges( bool isDraw )
    {
      std::cout << "setDrawYEdges(" << isDraw << ")\n"; // debug
      drawYEdges_ = isDraw;
    }
    
    void setXDomainCenter( double xDomainCenter )
    {
      std::cout << "setXDomainCenter(" << xDomainCenter << ")\n"; // debug
      xDomainCenter_ = xDomainCenter;
    }
    
    void setYDomainCenter( double yDomainCenter )
    {
      std::cout << "setYDomainCenter(" << yDomainCenter << ")\n"; // debug
      yDomainCenter_ = yDomainCenter;
    }
    
    void setXViewVolumeCenter( double xViewVolumeCenter )
    {
      std::cout << "setXViewVolumeCenter(" << xViewVolumeCenter << ")\n"; // debug
      xViewVolumeCenter_ = xViewVolumeCenter;
    }
    
    void setYViewVolumeCenter( double yViewVolumeCenter )
    {
      std::cout << "setYViewVolumeCenter(" << yViewVolumeCenter << ")\n"; // debug
      yViewVolumeCenter_ = yViewVolumeCenter;
    }
    
    void setDrawAxes( bool draw )
    {
      std::cout << "setDrawAxes(" << draw << ")\n"; // debug
      drawAxes_ = draw;
    }
    
    void setDrawFakeEdges( bool draw )
    {
      std::cout << "setDrawFakeEdges(" << draw << ")\n"; // debug
      drawFakeEdges_ = draw;
    }
    
    void resize( int x, int y, int w, int h );

    void draw();
    
  private:
    render_frame_t frame_;
    size_t funcIdx_;
    size_t xCells_, yCells_;
    double xDomain_, yDomain_;
    double xViewVolume_, yViewVolume_;
    bool keepAspectRatio_;
    double yaw_, pitch_;
    bool drawXEdges_, drawYEdges_;
    double xDomainCenter_, yDomainCenter_;
    double xViewVolumeCenter_, yViewVolumeCenter_;
    bool drawAxes_;
    bool drawFakeEdges_;
  };
}
  
#endif // VIEWPORT_H
