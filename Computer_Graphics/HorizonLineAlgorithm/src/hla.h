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
  //   putPixel(x, y, color).
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
              edge::line_style_t const &ls = edge.insideHorizon();
              drawLinePoint(p, ls.color, ls.style, idx);
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
    
    void drawLinePoint( Vector2i const &p, color::color_t color, edge::line_form_style_t const &formStyle, size_t idx )
    {
      if (formStyle == edge::rs_solid)
      {
        // Solid line.
        frame_.putPixel(p.x(), p.y(), color);
      }
      else if (formStyle == edge::rs_dash)
      {
        // Dashed line.
        if (idx % 2 != 0)
          frame_.putPixel(p.x(), p.y(), color);
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
