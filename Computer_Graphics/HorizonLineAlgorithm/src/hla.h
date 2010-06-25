/*
 *  This file is part of FHA - Floating Horizon Algorithm implementation.
 *
 *  Copyright (C) 2010  Vladimir Rutsky <altsysrq@gmail.com>
 *
 *  FHA is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FHA is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FHA.  If not, see <http://www.gnu.org/licenses/>.
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
        , edgeEnd(false)
      {
      }
      
      bool initialized;
      int initializeRow;
      int lo, hi;

      // True if some edge initialized this column with its last point 
      // and no other edges updated this column.
      bool edgeEnd;
      int edgeEndRow;
      int edgeEndLo, edgeEndHi;
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
        Vector2i lastInitialized(-1, -1);
        for (bresenham::PointsIterator pIt(p0, p1); pIt; ++pIt, ++idx)
        {
          // DEBUG: Draw half-edges. Helps debug segments orientation.
          if (0)
          {
            if (idx > bresenham::pointsCount(p0, p1) / 2.0)
              break;
          }
          // END OF DEBUG
          
          Vector2i const p = *pIt;
          if (isInsideRenderFrame(p))
          {
            horizon_column_t horizonColumn = horizon_[p.x()];
            
            //bool const isLastColumn = (p.x() == p1.x());
            //bool const isFirstColumn = (p.x() == p0.x());
            
            if (!horizonColumn.initialized)
            {
              if (lastInitialized.x() != p.x())
              {
                // Uninitialized column.
                lastInitialized = p; // Virtually initialize it.
                
                edge::line_style_t const &ls = edge.initHorizon();
                drawLinePoint(p, ls.color, ls.style, idx);
                
                continue;
              }
              else
              {
                // Virtually initialized column.
                horizonColumn.lo = lastInitialized.y();
                horizonColumn.hi = lastInitialized.y();
              }
            }
            
            if (p.y() > horizonColumn.hi)
            {
              // Point above horizon (or virtual horizon).
              edge::line_style_t const &ls = edge.aboveHorizon();
              drawLinePoint(p, ls.color, ls.style, idx);
            }
            else if (p.y() < horizonColumn.lo)
            {
              // Point below horizon (or virtual horizon).
              edge::line_style_t const &ls = edge.belowHorizon();
              drawLinePoint(p, ls.color, ls.style, idx);
            }
            else
            {
              assert(p.y() >= horizonColumn.lo && p.y() <= horizonColumn.hi);
              // Point inside horizon (or at virtual horizon, which should be impossible).
              //std::cout << "end color:" << frame_.getPixel(p.x(), p.y()) << "\n";
              if (color::getA(frame_.getPixel(p.x(), p.y())))
              {
                //std::cout << "a: " << color::getA(frame_.getPixel(p.x(), p.y())) << "\n";
                // Found alpha label in "busy" horizon column, overwrite it.
                if (p.y() == horizonColumn.initializeRow)
                {
                  // Near high horizon border.
                  edge::line_style_t const &ls = edge.initHorizon();
                  drawLinePoint(p, ls.color, ls.style, idx);
                }
                else if (p.y() == horizonColumn.hi)
                {
                  // Near high horizon border.
                  edge::line_style_t const &ls = edge.aboveHorizon();
                  drawLinePoint(p, ls.color, ls.style, idx);
                }
                else if (p.y() == horizonColumn.lo)
                {
                  // Near low horizon border.
                  edge::line_style_t const &ls = edge.belowHorizon();
                  drawLinePoint(p, ls.color, ls.style, idx);
                }
                //assert(!color::getA(frame_.getPixel(p.x(), p.y())));
              }
              /*else if (horizonColumn.edgeEnd && (isFirstColumn || isLastColumn))
              {
                // Edges ends can overlap.
                if (p.y() > horizonColumn.initializeRow)
                {
                  // Point above initial horizon.
                  edge::line_style_t const &ls = edge.aboveHorizon();
                  drawLinePoint(p, ls.color, ls.style, idx);
                }
                else if (p.y() < horizonColumn.initializeRow)
                {
                  // Point below initial horizon.
                  edge::line_style_t const &ls = edge.belowHorizon();
                  drawLinePoint(p, ls.color, ls.style, idx);
                }
                else
                {
                  // Initialized point already drawed.
                }
              }*/
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
        bool markAsEdgeEnd(false);
        if (isXInsideRenderFrame(p1.x()))
        {
          if (horizon_[p1.x()].edgeEnd && horizon_[p1.x()].edgeEndRow == p1.y())
            applyEdgeEnd(p1.x());
          else
          {
            markAsEdgeEnd = true;
            if (horizon_[p1.x()].initialized)
            {
              horizon_[p1.x()].edgeEnd = true;
              horizon_[p1.x()].edgeEndRow = p1.y();
              horizon_[p1.x()].edgeEndHi = horizon_[p1.x()].hi;
              horizon_[p1.x()].edgeEndLo = horizon_[p1.x()].lo;
            }
          }
        }
        
        for (bresenham::PointsIterator pIt(p0, p1); pIt; ++pIt)
        {
          Vector2i const p = *pIt;
          
          if (isXInsideRenderFrame(p.x()))
          {
            horizon_column_t &horizonColumn = horizon_[p.x()];

            //bool const isLastPoint = (p == p1);
            bool const isLastColumn = (p.x() == p1.x());
            
            if (!isLastColumn || !markAsEdgeEnd)
            {
              applyEdgeEnd(p.x());
            
              if (!horizonColumn.initialized)
              {
                // Uninitialized column.
                edge::line_style_t const &ls = edge.initHorizon();
                if (ls.updateHorizon)
                {
                  horizonColumn.initialized = true;
                  horizonColumn.initializeRow = p.y();
                  horizonColumn.hi = p.y();
                  horizonColumn.lo = p.y();
                }
              }
              else
              {
                if (p.y() > horizonColumn.hi)
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
            else
            {
              if (!horizonColumn.initialized)
              {
                // Uninitialized column.
                edge::line_style_t const &ls = edge.initHorizon();
                if (ls.updateHorizon)
                {
                  horizonColumn.initialized = true;
                  horizonColumn.initializeRow = p.y();
                  horizonColumn.hi = p.y();
                  horizonColumn.lo = p.y();
                  
                  horizonColumn.edgeEnd = true;                  
                  horizonColumn.edgeEndRow = p.y();
                  horizonColumn.edgeEndHi = p.y();
                  horizonColumn.edgeEndLo = p.y();
                }
              }
              else
              {
                if (p.y() > horizonColumn.edgeEndHi)
                {
                  // Point above horizon.
                  edge::line_style_t const &ls = edge.aboveHorizon();
                  if (ls.updateHorizon)
                  {
                    horizonColumn.edgeEndHi = p.y();
                  }
                }
                else if (p.y() < horizonColumn.edgeEndLo)
                {
                  // Point below horizon.
                  edge::line_style_t const &ls = edge.belowHorizon();
                  if (ls.updateHorizon)
                  {
                    horizonColumn.edgeEndLo = p.y();
                  }          
                }
                else
                {
                  assert(p.y() >= horizonColumn.edgeEndLo && p.y() <= horizonColumn.edgeEndHi);
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
    
    void applyEdgeEnd( size_t x )
    {
      if (horizon_[x].edgeEnd)
      {
        util::make_min(horizon_[x].lo, horizon_[x].edgeEndLo);
        util::make_max(horizon_[x].hi, horizon_[x].edgeEndHi);
        
        horizon_[x].edgeEnd = false;
      }
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
