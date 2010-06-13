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

#ifndef EDGE_H
#define EDGE_H

#include <vector>
#include <cmath>
#include <cassert>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "function.h"

namespace edge
{
  USING_PART_OF_NAMESPACE_EIGEN

  class baseRenderingEdge
  {
  public:
    baseRenderingEdge( Vector3d p0, Vector3d p1, bool fake, int axesIdx = -1 )
      : p0_(p0)
      , p1_(p1)
      , fake_(fake)
      , axesIdx_(axesIdx)
    {
    }
    
    Vector3d p0() const { return p0_; }
    Vector3d p1() const { return p1_; }
    
    // Fake edges shouldn't be drawed on screen (but should update horizon 
    // buffer).
    bool fake() const { return fake_; }
    
    int axesIdx() const { return axesIdx_; }
    
  private:
    Vector3d p0_, p1_;
    bool fake_;
    int axesIdx_;
  };
  
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
                    bool hEdges = true, bool vEdges = true )
    {
      addGridEdges(grid, hEdges, vEdges);
    }
    
    template< class GridType >
    void addGridEdges( GridType grid, bool hEdges = true, bool vEdges = true )
    {
      if (hEdges)
      {
        // Add horizontal edges.
        for (size_t y = 0; y < grid.ySize(); ++y)
          for (size_t x = 0; x + 1 < grid.xSize(); ++x)
          {
            edge_t edge(grid(x, y), grid(x + 1, y), false);
            edges_.push_back(edge);
          }
      }
      
      if (vEdges)
      {
        // Add vertical edges.
        for (size_t y = 0; y + 1 < grid.ySize(); ++y)
          for (size_t x = 0; x < grid.xSize(); ++x)
          {
            edge_t edge(grid(x, y), grid(x, y + 1), false);
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
            edge_t leftEdge(grid(0, y), grid(0, y + 1), true);
            edges_.push_back(leftEdge);
            
            baseRenderingEdge rightEdge(grid(grid.xSize() - 1, y), grid(grid.xSize() - 1, y + 1), true);
            edges_.push_back(rightEdge);
          }
        }
        else // vEdges
        {
          for (size_t x = 0; x + 1 < grid.xSize(); ++x)
          {
            edge_t bottomEdge(grid(x, 0), grid(x + 1, 0), true);
            edges_.push_back(bottomEdge);
            
            edge_t topEdge(grid(x, grid.ySize() - 1), grid(x + 1, grid.ySize() - 1), true);
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

#endif // EDGE_H
