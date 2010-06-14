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

#ifndef HLA_H
#define HLA_H

#include <vector>
#include <limits>
#include <utility>
#include <cmath>
#include <cassert>
#include <algorithm>

#include <Eigen/Core>

#include "bresenham.h"
#include "color.h"
#include "edge.h"

namespace hla
{
  USING_PART_OF_NAMESPACE_EIGEN
  
  // FrameAdapterType is abstraction of 2d surface for drawing with such CS:
  //   O  - left-lower corner,
  //   OX - right,
  //   OY - up.
  // FrameAdapterType must provide such functions as:
  //   width(), 
  //   height(), 
  //   putPixel(x, y, color),
  //   getPixel(x, y).
  template< class FrameAdapterType >
  class FrameRenderer
  {
  public:
    typedef FrameAdapterType frame_adapter_t;
    
  private:
    struct horizon_column_t
    {
      horizon_column_t()
        : initialized(false)
        , lo(-1)
        , hi(-1)
      {
      }
      
      bool initialized;
      int lo, hi;
    };
    
    typedef std::vector<horizon_column_t> horizon_t;
    
  public:
    FrameRenderer( FrameAdapterType &frame )
      : frame_(frame)
      , horizon_(frame.width())
    {
    }
    
    void resetHorizons()
    {
      std::fill(horizon_.begin(), horizon_.end(), horizon_column_t());
    }
    
    template< class EdgeType >
    void drawEdge( EdgeType const &edge )
    {
      Vector2i const p0(std::floor(edge.p0().x()), std::floor(edge.p0().y()));
      Vector2i const p1(std::floor(edge.p1().x()), std::floor(edge.p1().y()));
      //Vector2i const p0(std::floor(0.5 + edge.p0().x()), std::floor(0.5 + edge.p0().y()));
      //Vector2i const p1(std::floor(0.5 + edge.p1().x()), std::floor(0.5 + edge.p1().y()));
      
      // TODO: Implement segment culling by render frame.
      
      // Draw current segment.
      if (edge.isDraw())
      {
        size_t idx = 0;
        for (bresenham::PointsIterator pIt(p0, p1); pIt; ++pIt, ++idx)
        {
          Vector2i const p = *pIt;
          if (isInsideRenderFrame(p))
          {
            horizon_column_t const &horizonColumn = horizon_[p.x()];
            if (!horizonColumn.initialized)
            {
              // Uninitialized column.
              edge::line_style_t const &ls = edge.initHorizon();
              drawLinePoint(p, ls.color, ls.style, idx);
            }
            else if (p.y() > horizonColumn.hi)
            {
              // Point above horizon.
              edge::line_style_t const &ls = edge.aboveHorizon();
              drawLinePoint(p, ls.color, ls.style, idx);
            }
            else if (p.y() < horizonColumn.lo)
            {
              // Point below horizon.
              edge::line_style_t const &ls = edge.belowHorizon();
              drawLinePoint(p, ls.color, ls.style, idx);
            }
            else
            {
              assert(p.y() >= horizonColumn.lo && p.y() <= horizonColumn.hi);
              // Point inside horizon.
              //std::cout << "end color:" << frame_.getPixel(p.x(), p.y()) << "\n";
              if (color::getA(frame_.getPixel(p.x(), p.y())))
              {
                //std::cout << "a: " << color::getA(frame_.getPixel(p.x(), p.y())) << "\n";
                // Found alpha label in "busy" horizon column, overwrite it.
                if (p.y() == horizonColumn.hi)
                {
                  // Near high horizon border.
                  edge::line_style_t const &ls = edge.aboveHorizon();
                  drawLinePoint(p, ls.color, ls.style, idx);
                }
                else if (p.y() == horizonColumn.lo)
                {
                  // Near low or any else horizon border.
                  edge::line_style_t const &ls = edge.belowHorizon();
                  drawLinePoint(p, ls.color, ls.style, idx);
                }
                //assert(!color::getA(frame_.getPixel(p.x(), p.y())));
              }
              else
              {
                // Draw as point inside horizon.
                edge::line_style_t const &ls = edge.insideHorizon();
                drawLinePoint(p, ls.color, ls.style, idx);
              }
            }
          }
        }
      }
      
      // Update horizon.
      if (edge.isUpdateHorizon())
      {
        for (bresenham::PointsIterator pIt(p0, p1); pIt; ++pIt)
        {
          Vector2i const p = *pIt;
          
          bresenham::PointsIterator pNextIt = pIt; // TODO: Use single iterator.
          ++pNextIt;
          
          // Don't update horizon for last point. This fix bugs like this:
          //   BC
          //   12
          //   12
          // 1  2
          // 1  2
          // A    2
          //      2
          //      2
          //      2
          //      D
          // ------ 
          //  0 1 2  --- index of horizon column
          // First segment AB (marked with "1", from (0,0) to (1,5)).
          // After it (and further from us) segment CD (marked with "2", from
          // (1,5) to (-4,2)). At point B=C they intersects.
          // In old scheme, when AB has been drawed, it updated horizon at
          // indices [0, 1]. After that, drawing of CD segment have been 
          // denies at cells (1,1), (1,2) (and (1,3), (1,4), (1,5)),
          // which leaded to "holes" in image.
          if (!pNextIt || pNextIt->x() == p.x())
            continue;
          
          if (isXInsideRenderFrame(p.x()))
          {
            horizon_column_t &horizonColumn = horizon_[p.x()];
            if (!horizonColumn.initialized)
            {
              // Uninitialized column.
              edge::line_style_t const &ls = edge.initHorizon();
              if (ls.updateHorizon)
              {
                horizonColumn.initialized = true;
                horizonColumn.hi = p.y();
                horizonColumn.lo = p.y();
              }
            }
            else if (p.y() > horizonColumn.hi)
            {
              // Point above horizon.
              edge::line_style_t const &ls = edge.aboveHorizon();
              if (ls.updateHorizon)
              {
                horizonColumn.hi = p.y();
              }
            }
            else if (p.y() < horizonColumn.lo)
            {
              // Point below horizon.
              edge::line_style_t const &ls = edge.belowHorizon();
              if (ls.updateHorizon)
              {
                horizonColumn.lo = p.y();
              }          
            }
            else
            {
              assert(p.y() >= horizonColumn.lo && p.y() <= horizonColumn.hi);
              // Point inside horizon.
              edge::line_style_t const &ls = edge.insideHorizon();
              if (ls.updateHorizon)
              {
                // Nothing to do.
              }
            }
          }
        }
      }
    }
    
    template< class EdgeIt >
    void drawEdges( EdgeIt first, EdgeIt beyond )
    {
      for (EdgeIt edgeIt = first; edgeIt != beyond; ++edgeIt)
        drawEdge(*edgeIt);
    }
    
  private:
    bool isXInsideRenderFrame( double x ) const
    {
      return (x >= 0 && x < static_cast<int>(frame_.width()));
    }
    
    bool isInsideRenderFrame( Vector2i const &p ) const
    {
      return 
        (p.x() >= 0 && p.x() < static_cast<int>(frame_.width()) &&
         p.y() >= 0 && p.y() < static_cast<int>(frame_.height()));
    }
    
    void drawLinePoint( Vector2i const &p, color::color_t c, edge::line_form_style_t const &formStyle, size_t idx )
    {
      color::Converter oldColorConv(frame_.getPixel(p.x(), p.y()));
      color::Converter colorConv(c);
      if (colorConv.a)
      {
        oldColorConv.a = colorConv.a;
        c = oldColorConv;
      }
      
      //std::cout << "found alpha: (" << p.x() << ", " << p.y() << ")\n";*/
      if (formStyle == edge::rs_solid)
      {
        // Solid line.
        frame_.putPixel(p.x(), p.y(), c);
      }
      else if (formStyle == edge::rs_dash)
      {
        // Dashed line.
        if (idx % 2 != 0)
          frame_.putPixel(p.x(), p.y(), c);
      }
      else // (formStyle == edge::rs_none)
      {
        // Don't draw anything.
        assert(formStyle == edge::rs_none);
        // TODO: Or may be just ignore unknown type?
      }
    }
    
  private:
    frame_adapter_t &frame_;
    horizon_t horizon_;
  };
}

#endif // HLA_H
