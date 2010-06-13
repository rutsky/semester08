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
#include <FL/Fl_Group.H>

#include "appconf.h"
#include "function.h"
#include "hla.h"

class MainWindow;

namespace viewport
{
  USING_PART_OF_NAMESPACE_EIGEN
  
  STATIC_ASSERT(sizeof(Fl_Color) == 4); // TODO

  namespace details
  {
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
        size_t const newSize = w_ * h_;
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
        std::memset(&buffer_[0], byte, buffer_.size() * sizeof(buffer_[0]));
      }
      
    private:
      Fl_Color & operator () ( size_t x, size_t y )
      {
        return buffer_[w_ * y + x];
      }
      
      Fl_Color const & operator () ( size_t x, size_t y ) const
      {
        return buffer_[w_ * y + x];
      }

    private:
      size_t w_, h_;
      std::vector<Fl_Color> buffer_;
    };
  }

  class Viewport : public Fl_Box
  {
  public:
    Viewport( int x, int y, int w, int h, char const *l = 0 )
      : Fl_Box(x, y, w, h, l)
      , funcIdx_(appconf::startFuncIdx)
      , xCells_(appconf::startXCells)
      , yCells_(appconf::startYCells)
      , xDomain_(appconf::startXDomain)
      , yDomain_(appconf::startYDomain)
      , yaw_(appconf::startYaw)
      , pitch_(appconf::startPitch)
      , drawXEdges_(appconf::startDrawXEdges)
      , drawYEdges_(appconf::startDrawYEdges)
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
    
    void resize( int x, int y, int w, int h );

    void draw()
    {
      // Prepare frame to drawing.
      frame_.clear(255);
      
      frame_.putPixel(0, 1, FL_WHITE);
      
      std::cout << "draw(" << x() << ", " << y() << ", " << w() << ", " << h() << ");\n"; // debug
      
      // Calcule domain.
      Vector2d const origin(0.0, 0.0); // TODO: Set origin center from GUI.
      Vector2i const extent((int)xCells_ + 1, (int)yCells_ + 1);
      Vector2d const domain(xDomain_, yDomain_);
      //std::cout << domain << "\n" << extent << "\n";
      Vector2d const unit = domain.cwise() / extent.cast<double>();
      
      // Build grid.
      hla::FuncValuesGrid 
        funcGrid(function::functions[funcIdx_], origin, unit, extent);
                                   
      // Build view transformation.
      // Function in right-handed CS where:
      //   OX looks right,
      //   OY looks from us,
      //   OZ looks up.
      
      // Yaw transformation.
      Eigen::Transform3d const yawTf(
        Eigen::AngleAxisd(util::deg2rad(yaw_), Vector3d::UnitZ()));
      // Pitch transformation.
      Eigen::Transform3d const pitchTf(
        Eigen::AngleAxisd(util::deg2rad(pitch_), Vector3d::UnitX()));
      // Rename axes so
      //   OX looks right,
      //   OY looks up,
      //   OZ looks on us.
      Eigen::Transform3d const replaceAxesTf(
        Eigen::AngleAxisd(
          (Matrix3d() << 
            1,  0,  0,
            0,  0,  1,
            0, -1,  0).finished()));
      // Scale CS so that X view will contain exactly X domain (similar with Y).
      Eigen::Transform3d const scaleTf(
        Eigen::Scaling3d(
          static_cast<double>(xViewVolume_) / xDomain_ * frame_.width(),
          static_cast<double>(yViewVolume_) / yDomain_ * frame_.height(),
          1.0));
      
      // 
      
      Eigen::Transform3d const totalTf = 
        yawTf * pitchTf * replaceAxesTf * scaleTf;
      
      // Build transformed grid.
      hla::TransformedFuncValuesGrid transformedFuncGrid(funcGrid, totalTf);
      
      // Sort direction.
      Vector3d sortDir(0.0, 0.0, 1.0);
      
      // Build and sort drawing segments list.
      typedef hla::EdgesGenerator<hla::TransformedFuncValuesGrid> edges_gen_t;
      edges_gen_t edgesGen(transformedFuncGrid, 
                    drawXEdges_, drawYEdges_);
      edgesGen.sort(sortDir);
      
      // Draw to frame.
      hla::renderFrame(frame_, 
        edgesGen.begin(), edgesGen.end(), 
        FL_GREEN, 
        FL_RED);
      
      // Flush frame to window.
      fl_draw_image(frame_.buffer(), x(), y(), 
                    frame_.width(), frame_.height(), 
                    frame_.pixelSize(), frame_.lineSize());
    }
    
  private:
    details::RenderFrame frame_;
    size_t funcIdx_;
    size_t xCells_, yCells_;
    double xDomain_, yDomain_;
    double xViewVolume_, yViewVolume_;
    bool keepAspectRatio_;
    double yaw_, pitch_;
    bool drawXEdges_, drawYEdges_;
  };
}
  
#endif // VIEWPORT_H
