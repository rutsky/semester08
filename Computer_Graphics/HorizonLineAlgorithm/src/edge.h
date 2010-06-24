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

#include <cmath>
#include <cassert>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "color.h"

namespace edge
{
  USING_PART_OF_NAMESPACE_EIGEN
  
  enum line_form_style_t
  {
    rs_none = 0,
    rs_solid,
    rs_dash,
  };
  
  struct line_style_t
  {
    line_style_t()
      : color(color::grey())
      , style(rs_solid)
      , updateHorizon(true)
    {
    }
    
    line_style_t( color::color_t newColor, 
                  line_form_style_t newStyle = rs_solid,
                  bool newUpdateHorizon = true )
      : color(newColor)
      , style(newStyle)
      , updateHorizon(newUpdateHorizon)
    {
    }
    
    color::color_t    color;
    line_form_style_t style;
    bool              updateHorizon;
  };
  
  class edge_t
  {
  public:
    edge_t( Vector3d const &p0, Vector3d const &p1 )
      : p0_(p0)
      , p1_(p1)
      , isDraw_(true)
      , isUpdateHorizon_(true)
    {
    }
    
    edge_t( Vector3d const &p0, Vector3d const &p1, color::color_t color )
      : p0_(p0)
      , p1_(p1)
      , isDraw_(true)
      , isUpdateHorizon_(true)
      , initHorizon_(color)
      , aboveHorizon_(color)
      , insideHorizon_(color)
      , belowHorizon_(color)
    {
    }
    
    edge_t( Vector3d const &p0, Vector3d const &p1, 
            line_style_t const &initHorizon, line_style_t const &aboveHorizon, 
            line_style_t const &insideHorizon, line_style_t const &belowHorizon,
            bool isDraw = true, bool isUpdateHorizon = true )
      : p0_(p0)
      , p1_(p1)
      , isDraw_(isDraw)
      , isUpdateHorizon_(isUpdateHorizon)
      , initHorizon_(initHorizon)
      , aboveHorizon_(aboveHorizon)
      , insideHorizon_(insideHorizon)
      , belowHorizon_(belowHorizon)
    {
      /*
      if (color::getA(insideHorizon.color))
        std::cout << "!!!\n";
      */
    }
    
    virtual Vector3d p0() const { return p0_; };
    virtual Vector3d p1() const { return p1_; };
    
    // If false, none of segment points will be drawed.
    virtual bool isDraw() const { return isDraw_; }
    // If false, none of segment points will update horizon.
    virtual bool isUpdateHorizon() const { return isUpdateHorizon_; }
    
    line_style_t const &initHorizon  () const { return initHorizon_;   }
    line_style_t const &aboveHorizon () const { return aboveHorizon_;  }
    line_style_t const &insideHorizon() const { return insideHorizon_; }
    line_style_t const &belowHorizon () const { return belowHorizon_;  }
    
    void transform( Eigen::Transform3d const &tf )
    {
      p0_ = tf * p0_;
      p1_ = tf * p1_;
    }
    
    void swapEnds()
    {
      std::swap(p0_, p1_);
    }
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
  protected:
    Vector3d p0_, p1_;
    bool isDraw_;
    bool isUpdateHorizon_; 
    line_style_t initHorizon_, aboveHorizon_, insideHorizon_, belowHorizon_;
  };
}

#endif // EDGE_H
