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
#include <Eigen/Geometry>

#include "bresenham.h"
#include "function.h"

// TODO: Divide on hla.h, grid.h end edge.h.

namespace hla
{
  USING_PART_OF_NAMESPACE_EIGEN
  
  namespace details
  {
    typedef std::vector<int> horizon_type;
  }
 
  // FrameAdapterType must provide such functions as:
  //   width(), 
  //   height(), 
  //   putPixel(x, y, color).
  template< class FrameAdapterType, class SegmentsIt, class ColorType >
  inline
  void renderFrame( FrameAdapterType &frame, 
                    SegmentsIt firstSeg, SegmentsIt beyondSeg, 
                    ColorType topEdgesColor, 
                    ColorType bottomEdgesColor )
  {
    details::horizon_type 
      topHorizon   (frame.width(), std::numeric_limits<int>::min()), 
      bottomHorizon(frame.width(), std::numeric_limits<int>::max());
      
    for (SegmentsIt segIt = firstSeg; segIt != beyondSeg; ++segIt)
    {
      Vector2i const p0(std::floor(segIt->p0().x()), std::floor(segIt->p0().y()));
      Vector2i const p1(std::floor(segIt->p1().x()), std::floor(segIt->p1().y()));
      
      // Draw current segment.
      if (!segIt->fake())
      {
        for (bresenham::PointsIterator pIt(p0, p1); pIt; ++pIt)
        {
          if (pIt->x() >= 0 && pIt->x() < static_cast<int>(frame.width()) &&
              pIt->y() >= 0 && pIt->y() < static_cast<int>(frame.height()))
          {
            if (pIt->y() > topHorizon[pIt->x()])
              frame.putPixel(pIt->x(), pIt->y(), topEdgesColor);
            else if (pIt->y() < bottomHorizon[pIt->x()])
              frame.putPixel(pIt->x(), pIt->y(), bottomEdgesColor);
          }
        }
      }
      
      // Update horizons.
      for (bresenham::PointsIterator pIt(p0, p1); pIt; ++pIt)
      {
        if (pIt->x() >= 0 && pIt->x() < static_cast<int>(frame.width()))
        {
          util::make_max(topHorizon[pIt->x()], pIt->y());
          util::make_min(bottomHorizon[pIt->x()], pIt->y());
        }
      }
    }
  }
}

#endif // HLA_H
