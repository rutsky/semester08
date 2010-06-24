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

#ifndef GRID_H
#define GRID_H

#include <utility>
#include <cmath>
#include <cassert>

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "function.h"

namespace grid
{
  USING_PART_OF_NAMESPACE_EIGEN

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
      //std::cout << "FuncValuesGrid(" << x << "," << y << ") = (" << v.x() << ", " << v.y() << ", " << func_(v.x(), v.y()) << ")\n"; // debug
      return Vector3d(v.x(), v.y(), func_(v.x(), v.y()));
    }
  
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
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
}

#endif // GRID_H
