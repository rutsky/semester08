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
    
    void resize( int x, int y, int w, int h );

    void draw()
    {
      if (frame_.width() >= 1 && frame_.height() >= 1)
      {
        // Prepare frame to drawing.
        frame_.clear(255);
        //frame_.fill(details::colors::black());
        
        //frame_.putPixel(0, 1, FL_WHITE);
        
        std::cout << "draw(" << x() << ", " << y() << ", " << w() << ", " << h() << ");\n"; // debug
        
        // Calculate domain.
        Vector2d const origin(xDomainCenter_ - xDomain_ / 2.0, yDomainCenter_ - yDomain_ / 2.0);
        Vector2i const extent((int)xCells_ + 1, (int)yCells_ + 1);
        Vector2d const domain(xDomain_, yDomain_);
        Vector2d const unit(
          static_cast<double>(xDomain_) / xCells_, 
          static_cast<double>(yDomain_) / yCells_);
        
        // Build grid.
        grid::FuncValuesGrid 
          funcGrid(function::functions[funcIdx_], origin, unit, extent);
                                    
        // Build view transformation.
        // Function in right-handed CS where:
        //   OX looks right,
        //   OY looks from us,
        //   OZ looks up.
        
        // Yaw transformation.
        Eigen::Transform3d const yawTf(
          Eigen::AngleAxisd(-util::deg2rad(yaw_), Vector3d::UnitZ()));
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
        // Move CS center to view volume center.
        Eigen::Transform3d const viewVolumeTranslateTf(
          Eigen::Translation3d(
            static_cast<double>(-xViewVolumeCenter_),
            static_cast<double>(-yViewVolumeCenter_),
            0.0));
        // Scale CS so that X view will contain exactly X domain (similar with Y).
        Eigen::Transform3d const scaleTf(
          Eigen::Scaling3d(
            static_cast<double>(frame_.width() - 1) / xViewVolume_,
            static_cast<double>(frame_.height() - 1) / yViewVolume_,
            1.0));
        // Move CS center to screen center.
        Eigen::Transform3d const translateTf(
          Eigen::Translation3d(
            static_cast<double>(frame_.width()) / 2.0,
            static_cast<double>(frame_.height()) / 2.0,
            0.0));
        
        Eigen::Transform3d const totalTf = 
          translateTf * scaleTf * replaceAxesTf * pitchTf * yawTf * viewVolumeTranslateTf;
        
        // Sort direction.
        Vector3d const sortDir(
          Eigen::AngleAxisd(util::deg2rad(yaw_), Vector3d::UnitZ()) * 
          Vector3d(0.0, 1.0, 0.0));
        
        // Build and sort drawing segments list.
        typedef edge_gen::EdgesGenerator<edge::edge_t> edges_gen_t;
        edges_gen_t edgesGen;
        {         
          // Add axes.
          {
            // OX.
            Vector3d const pStart(Vector3d(xViewVolumeCenter_, yViewVolumeCenter_, 0.0));
            Vector3d const pEnd(Vector3d(xViewVolumeCenter_ + xViewVolume_ / 2.0, yViewVolumeCenter_, 0.0));
            size_t const nFractions = std::max(xCells_ * 2, yCells_ * 2);
            Vector3d const step = (pEnd - pStart) / static_cast<double>(nFractions);
            Vector3d p(pStart);
            color::color_t c = color::red();
            for (size_t i = 0; i < nFractions; ++i, p += step)
            {
              Vector3d const p0(p);
              Vector3d const p1(p + step);
              
              edge::edge_t axis(
                p0,
                p1,
                edge::line_style_t(c, edge::rs_solid), // init
                edge::line_style_t(c, edge::rs_solid), // above
                edge::line_style_t(c, edge::rs_dash),  // inside
                edge::line_style_t(c, edge::rs_solid), // below
                true, false);
              edgesGen.addEdge(axis);
            }
          }
          
          {
            // OY.
            Vector3d const pStart(Vector3d(xViewVolumeCenter_, yViewVolumeCenter_, 0.0));
            Vector3d const pEnd(Vector3d(xViewVolumeCenter_, yViewVolumeCenter_ + yViewVolume_ / 2.0, 0.0));
            size_t const nFractions = std::max(xCells_ * 2, yCells_ * 2);
            Vector3d const step = (pEnd - pStart) / static_cast<double>(nFractions);
            Vector3d p(pStart);
            color::color_t c = color::green();
            for (size_t i = 0; i < nFractions; ++i, p += step)
            {
              Vector3d const p0(p);
              Vector3d const p1(p + step);
              
              edge::edge_t axis(
                p0,
                p1,
                edge::line_style_t(c, edge::rs_solid), // init
                edge::line_style_t(c, edge::rs_solid), // above
                edge::line_style_t(c, edge::rs_dash),  // inside
                edge::line_style_t(c, edge::rs_solid), // below
                true, false);
              edgesGen.addEdge(axis);
            }
          }
          
          {
            // OZ.
            Vector3d const pStart(Vector3d(xViewVolumeCenter_, yViewVolumeCenter_, 0.0));
            Vector3d const pEnd(Vector3d(xViewVolumeCenter_, yViewVolumeCenter_, (xViewVolume_ + yViewVolume_) / 4.0));
            size_t const nFractions = std::max(xCells_ * 2, yCells_ * 2);
            Vector3d const step = (pEnd - pStart) / static_cast<double>(nFractions);
            Vector3d p(pStart);
            color::color_t c = color::blue();
            for (size_t i = 0; i < nFractions; ++i, p += step)
            {
              Vector3d const p0(p);
              Vector3d const p1(p + step);
              
              edge::edge_t axis(
                p0,
                p1,
                edge::line_style_t(c, edge::rs_solid), // init
                edge::line_style_t(c, edge::rs_solid), // above
                edge::line_style_t(c, edge::rs_dash),  // inside
                edge::line_style_t(c, edge::rs_solid), // below
                true, false);
              edgesGen.addEdge(axis);
            }
          }
         
          // Add all grid edges.
          edgesGen.addGridEdges(
            funcGrid,
            color::black(),
            color::bulgarianRose(), 
            drawXEdges_, drawYEdges_);
          
          edgesGen.sort(sortDir);
        }
        
        // Transform all edges.
        for (edges_gen_t::iterator edgeIt = edgesGen.begin(); 
            edgeIt != edgesGen.end();
            ++edgeIt)
          edgeIt->transform(totalTf);
        
        // Draw to frame.
        {
          hla::FrameRenderer<render_frame_t> hlaFrameRenderer(frame_);
          
          hlaFrameRenderer.drawEdges(edgesGen.begin(), edgesGen.end());
        }
        
        // Flush frame to window.
        fl_draw_image(frame_.buffer(), x(), y(), 
                      frame_.width(), frame_.height(), 
                      frame_.pixelSize(), frame_.lineSize());
      }
    }
    
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
  };
}
  
#endif // VIEWPORT_H
