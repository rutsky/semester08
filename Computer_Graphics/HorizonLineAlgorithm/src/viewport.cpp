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

#include "precompiled.hpp"

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
  
  void Viewport::draw()
  {
    if (frame_.width() >= 1 && frame_.height() >= 1)
    {
      // Prepare frame to drawing.
      //frame_.clear(255);
      frame_.fill(color::white());
      
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
        if (drawAxes_)
        {
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
        }
        
        // Add all grid edges.
        edgesGen.addGridEdges(
          funcGrid,
          color::black(),
          color::crimsonGlory(),
          color::coquelicot(),
          drawXEdges_, drawYEdges_,
          drawFakeEdges_);
        
        std::cout << "sort dir: (" << sortDir.x() << ", " << sortDir.y() << ")\n";
        edgesGen.orientBy(sortDir);
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
}
