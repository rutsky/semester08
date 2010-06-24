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
  
  class EdgeLess
  {
  public:
    EdgeLess( Vector3d const &dir )
      : dir_(dir)
    {
    }
  
    template< class EdgeType >
    bool operator () ( EdgeType const &a, EdgeType const &b )
    {
      Vector2d const dir(dir_.x(), dir_.y());
      Vector2d const ap0(a.p0().x(), a.p0().y());
      Vector2d const ap1(a.p1().x(), a.p1().y());
      Vector2d const bp0(b.p0().x(), b.p0().y());
      Vector2d const bp1(b.p1().x(), b.p1().y());
      
      double const aMin = std::min(dir.dot(ap0), dir.dot(ap1));
      double const bMin = std::min(dir.dot(bp0), dir.dot(bp1));
      
      return aMin < bMin;
    }
    
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
  private:
    Vector3d dir_;
  };

  // TODO: Not standart iterator.
  template< class EdgeType > 
  class EdgesGenerator
  {
  public:
    typedef EdgeType edge_t;

  private:
    typedef std::vector<edge_t, Eigen::aligned_allocator<edge_t> > edges_t;
    
  public:
    typedef typename edges_t::const_iterator const_iterator;
    typedef typename edges_t::iterator iterator; // TODO
    
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
                       color::color_t initHorizonColor,
                       bool hEdges = true, bool vEdges = true,
                       bool fakeEdges = true )
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
              edge::line_style_t(initHorizonColor),                // init
              edge::line_style_t(aboveHorizonColor),               // above 
              edge::line_style_t(color::color_t(), edge::rs_none), // inside
              edge::line_style_t(belowHorizonColor));              // below
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
              edge::line_style_t(initHorizonColor),                // init 
              edge::line_style_t(aboveHorizonColor),               // above
              edge::line_style_t(color::color_t(), edge::rs_none), // inside
              edge::line_style_t(belowHorizonColor));              // below
            edges_.push_back(edge);
          }
      }
      
      if (fakeEdges && (hEdges || vEdges) && !(hEdges && vEdges))
      {
        color::color_t const alphaColor = color::makeRGB(0, 255, 255, 255);
        //std::cout << "alphaColor:" << alphaColor << "\n";
        // Only horizontal edges or only vertical edges.
        // Add fake edges.
        if (hEdges)
        {
          for (size_t y = 0; y + 1 < grid.ySize(); ++y)
          {
            Vector3d const leftEdgeP0 = grid(0, y);
            Vector3d const leftEdgeP1 = grid(0, y + 1);
            edge_t leftEdge(leftEdgeP0, leftEdgeP1, 
              edge::line_style_t(alphaColor),
              edge::line_style_t(alphaColor), 
              edge::line_style_t(color::color_t(), edge::rs_none), // inside
              edge::line_style_t(alphaColor),
              true, true);
            edges_.push_back(leftEdge);
            
            Vector3d const rightEdgeP0 = grid(grid.xSize() - 1, y);
            Vector3d const rightEdgeP1 = grid(grid.xSize() - 1, y + 1);
            edge_t rightEdge(rightEdgeP0, rightEdgeP1, 
              edge::line_style_t(alphaColor),
              edge::line_style_t(alphaColor), 
              edge::line_style_t(color::color_t(), edge::rs_none), // inside
              edge::line_style_t(alphaColor),
              true, true);
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
              edge::line_style_t(alphaColor),
              edge::line_style_t(alphaColor), 
              edge::line_style_t(color::color_t(), edge::rs_none), // inside
              edge::line_style_t(alphaColor),
              true, true);
            edges_.push_back(bottomEdge);
            
            Vector3d const topEdgeP0 = grid(x, grid.ySize() - 1);
            Vector3d const topEdgeP1 = grid(x + 1, grid.ySize() - 1);
            edge_t topEdge(topEdgeP0, topEdgeP1, 
              edge::line_style_t(alphaColor),
              edge::line_style_t(alphaColor), 
              edge::line_style_t(color::color_t(), edge::rs_none), // inside
              edge::line_style_t(alphaColor),
              true, true);
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
      std::stable_sort(edges_.begin(), edges_.end(), EdgeLess(sortDir));
    }
    
    void orientBy( Vector3d const &sortDir )
    {
      Vector2d sortDir2d(sortDir.x(), sortDir.y());
      
      for (typename edges_t::iterator edgeIt = edges_.begin(); edgeIt != edges_.end(); ++edgeIt)
      {
        Vector2d p0(edgeIt->p0().x(), edgeIt->p0().y());
        Vector2d p1(edgeIt->p1().x(), edgeIt->p1().y());
        
        //double const eps = 1e-8;
        if (sortDir2d.dot(p0) > sortDir2d.dot(p1))// + eps)
          edgeIt->swapEnds();
        /*
        else if (abs(sortDir2d.dot(p0) - sortDir2d.dot(p1)) <= eps)
        {
          Vector2d r(-sortDir2d.y(), sortDir2d.x());
          if (r.dot(p0) > r.dot(p1))
            edgeIt->swapEnds();
        }*/
      }
    }
    
    const_iterator begin() const { return edges_.begin(); }
    const_iterator end  () const { return edges_.end  (); }
    
    iterator begin() { return edges_.begin(); }
    iterator end  () { return edges_.end  (); }

  private:
    edges_t edges_;
  };
}

#endif // EDGE_GEN_H
