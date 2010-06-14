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

#ifndef EDGE_GEN_H
#define EDGE_GEN_H

#include <vector>
#include <cmath>
#include <cassert>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "function.h"
#include "edge.h"
#include "color.h"

namespace edge_gen
{
  USING_PART_OF_NAMESPACE_EIGEN
  
  // TODO: Not standart iterator.
  template< class EdgeType > 
  class EdgesGenerator
  {
  public:
    typedef EdgeType edge_t;

  private:
    typedef std::vector<edge_t> edges_t;
    
  public:
    typedef typename edges_t::const_iterator const_iterator;
    
  public:
    EdgesGenerator()
    {
    }
    
    template< class GridType >
    EdgesGenerator( GridType grid, 
                    color::color_t aboveHorizonColor,
                    color::color_t belowHorizonColor,
                    bool hEdges = true, bool vEdges = true )
    {
      addGridEdges(grid, aboveHorizonColor, belowHorizonColor, hEdges, vEdges);
    }
    
    template< class GridType >
    void addGridEdges( GridType grid, 
                       color::color_t aboveHorizonColor,
                       color::color_t belowHorizonColor,
                       bool hEdges = true, bool vEdges = true )
    {
      if (hEdges)
      {
        // Add horizontal edges.
        for (size_t y = 0; y < grid.ySize(); ++y)
          for (size_t x = 0; x + 1 < grid.xSize(); ++x)
          {
            Vector3d const p0 = grid(x, y);
            Vector3d const p1 = grid(x + 1, y);
            edge_t edge(p0, p1, 
              edge::line_style_t(aboveHorizonColor),
              edge::line_style_t(aboveHorizonColor), 
              edge::line_style_t(color::color_t(), edge::rs_none),
              edge::line_style_t(belowHorizonColor));
            edges_.push_back(edge);
          }
      }
      
      if (vEdges)
      {
        // Add vertical edges.
        for (size_t y = 0; y + 1 < grid.ySize(); ++y)
          for (size_t x = 0; x < grid.xSize(); ++x)
          {
            Vector3d const p0 = grid(x, y);
            Vector3d const p1 = grid(x, y + 1);
            edge_t edge(p0, p1, 
              edge::line_style_t(aboveHorizonColor),
              edge::line_style_t(aboveHorizonColor), 
              edge::line_style_t(color::color_t(), edge::rs_none),
              edge::line_style_t(belowHorizonColor));
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
            Vector3d const leftEdgeP0 = grid(0, y);
            Vector3d const leftEdgeP1 = grid(0, y + 1);
            edge_t leftEdge(leftEdgeP0, leftEdgeP1, 
              edge::line_style_t(color::color_t()),
              edge::line_style_t(color::color_t()), 
              edge::line_style_t(color::color_t()),
              edge::line_style_t(color::color_t()),
              false, true);
            edges_.push_back(leftEdge);
            
            Vector3d const rightEdgeP0 = grid(grid.xSize() - 1, y);
            Vector3d const rightEdgeP1 = grid(grid.xSize() - 1, y + 1);
            edge_t rightEdge(rightEdgeP0, rightEdgeP1, 
              edge::line_style_t(color::color_t()),
              edge::line_style_t(color::color_t()), 
              edge::line_style_t(color::color_t()),
              edge::line_style_t(color::color_t()),
              false, true);
            edges_.push_back(rightEdge);
          }
        }
        else // vEdges
        {
          for (size_t x = 0; x + 1 < grid.xSize(); ++x)
          {
            Vector3d const bottomEdgeP0 = grid(x, 0);
            Vector3d const bottomEdgeP1 = grid(x + 1, 0);
            edge_t bottomEdge(bottomEdgeP0, bottomEdgeP1, 
              edge::line_style_t(color::color_t()),
              edge::line_style_t(color::color_t()), 
              edge::line_style_t(color::color_t()),
              edge::line_style_t(color::color_t()),
              false, true);
            edges_.push_back(bottomEdge);
            
            Vector3d const topEdgeP0 = grid(x, grid.ySize() - 1);
            Vector3d const topEdgeP1 = grid(x + 1, grid.ySize() - 1);
            edge_t topEdge(topEdgeP0, topEdgeP1, 
              edge::line_style_t(color::color_t()),
              edge::line_style_t(color::color_t()), 
              edge::line_style_t(color::color_t()),
              edge::line_style_t(color::color_t()),
              false, true);
            edges_.push_back(topEdge);
          }
        }
      }
    }
    
    void addEdge( edge_t const &edge )
    {
      edges_.push_back(edge);
    }
    
    void sort( Vector3d const &sortDir )
    {
      std::stable_sort(edges_.begin(), edges_.end(), RenderingEgdeLess(sortDir));
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
      
      bool operator () ( edge_t const &a, edge_t const &b )
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

#endif // EDGE_GEN_H