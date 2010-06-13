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

namespace hla
{
  USING_PART_OF_NAMESPACE_EIGEN

  class baseRenderingEdge
  {
  public:
    baseRenderingEdge( Vector3d p0, Vector3d p1, bool fake )
      : p0_(p0)
      , p1_(p1)
      , fake_(fake)
    {
    }
    
    Vector3d p0() const { return p0_; }
    Vector3d p1() const { return p1_; }
    
    // Fake edges shouldn't be drawed on screen (but should update horizon 
    // buffer).
    bool fake() const { return fake_; }
    
  private:
    Vector3d p0_, p1_;
    bool fake_;
  };

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
    
  class FuncValuesGrid
  {
  public:
    FuncValuesGrid( function::func_t func, 
                    Vector2d const &origin, Vector2d const &unit, 
                    Vector2i const &extent )
      : func_(func)
      , origin_(origin)
      , unit_(unit)
      , extent_(extent)
    {
      assert(unit_.x() > 0.0 && unit_.y() > 0.0);
      assert(extent_.x() >= 2 && extent_.y() >= 2);
    }
    
    size_t xSize() const { return extent_.x(); }
    size_t ySize() const { return extent_.y(); }
    
    Vector3d operator () ( size_t x, size_t y ) const
    {
      assert(x < xSize() && y < ySize());
      Vector2d const v = origin_ + 
        unit_.cwise() * Vector2d(static_cast<double>(x), static_cast<double>(y));
      std::cout << "FuncValuesGrid(" << x << "," << y << ") = (" << v.x() << ", " << v.y() << ", " << func_(v.x(), v.y()) << ")\n";
      return Vector3d(v.x(), v.y(), func_(v.x(), v.y()));
    }
    
  private:
    function::func_t func_; 
    Vector2d origin_;
    Vector2d unit_;
    Vector2i extent_;
  };
  
  class TransformedFuncValuesGrid
  {
  public:
    TransformedFuncValuesGrid( FuncValuesGrid const &grid, Eigen::Transform3d const &transform )
      : grid_(grid)
      , transform_(transform)
    {
    }
    
    size_t xSize() const { return grid_.xSize(); }
    size_t ySize() const { return grid_.ySize(); }
    
    Vector3d operator () ( size_t x, size_t y ) const
    {
      return transform_ * grid_(x, y);
    }
    
  private:
    FuncValuesGrid grid_;
    Eigen::Transform3d transform_;
  };
  
  // TODO: Not standart iterator.
  template< class GridType >
  class EdgesGenerator
  {
  private:
    typedef std::vector<baseRenderingEdge> edges_t;
    
  public:
    typedef edges_t::const_iterator const_iterator;
    
  public:
    EdgesGenerator( GridType grid, 
                    bool hEdges = true, bool vEdges = true )
    {
      if (hEdges)
      {
        // Add horizontal edges.
        for (size_t y = 0; y < grid.ySize(); ++y)
          for (size_t x = 0; x + 1 < grid.xSize(); ++x)
          {
            baseRenderingEdge edge(grid(x, y), grid(x + 1, y), false);
            edges_.push_back(edge);
          }
      }
      
      if (vEdges)
      {
        // Add vertical edges.
        for (size_t y = 0; y + 1 < grid.ySize(); ++y)
          for (size_t x = 0; x < grid.xSize(); ++x)
          {
            baseRenderingEdge edge(grid(x, y), grid(x, y + 1), false);
            edges_.push_back(edge);
          }
      }
      
      if ((hEdges || vEdges) && !(hEdges && vEdges))
      {
        // Only horizontal edges or only vertical edges.
        // Add fake edges.
        if (hEdges)
        {
          for (size_t y = 0; y + 1 < grid.ySize(); ++y)
          {
            baseRenderingEdge leftEdge(grid(0, y), grid(0, y + 1), true);
            edges_.push_back(leftEdge);
            
            baseRenderingEdge rightEdge(grid(grid.xSize() - 1, y), grid(grid.xSize() - 1, y + 1), true);
            edges_.push_back(rightEdge);
          }
        }
        else // vEdges
        {
          for (size_t x = 0; x + 1 < grid.xSize(); ++x)
          {
            baseRenderingEdge bottomEdge(grid(x, 0), grid(x + 1, 0), true);
            edges_.push_back(bottomEdge);
            
            baseRenderingEdge topEdge(grid(x, grid.ySize() - 1), grid(x + 1, grid.ySize() - 1), true);
            edges_.push_back(topEdge);
          }
        }
      }
    }
    
    void sort( Vector3d const &sortDir )
    {
      std::sort(edges_.begin(), edges_.end(), RenderingEgdeLess(sortDir));
    }
    
    const_iterator begin() const { return edges_.begin(); }
    const_iterator end  () const { return edges_.end  (); }
    
  private:
    class RenderingEgdeLess
    {
    public:
      RenderingEgdeLess( Vector3d const &dir )
        : dir_(dir)
      {
      }
      
      template< class EdgeType >
      bool operator () ( EdgeType const &a, EdgeType const &b )
      {
        double const aMin = std::min(dir_.dot(a.p0()), dir_.dot(a.p1()));
        double const bMin = std::min(dir_.dot(b.p0()), dir_.dot(b.p1()));
        return aMin < bMin;
      }
      
    private:
      Vector3d dir_;
    };
    
  private:
    edges_t edges_;
  };
}

#endif // HLA_H
