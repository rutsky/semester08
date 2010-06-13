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

#ifndef BRESENHAM_H
#define BRESENHAM_H

#include <cmath>

#include <Eigen/Core>

#include "util.h"

namespace bresenham
{
  USING_PART_OF_NAMESPACE_EIGEN

  // TODO: Not stadart input iterator.
  class PointsIterator
  {
  public:
    PointsIterator( Vector2i const &p0, Vector2i const &p1 )
      : valid_(true)
      , p_(p0)
      , pStart_(p0)
      , pEnd_(p1)
      , error_(0.0)
    {
      if (std::abs(p1.x() - p0.x()) >= std::abs(p1.y() - p0.y()))
      {
        // Line angle is less or equal to 45 degrees (absolute).
        // Moving along OX axis.
        baseMovingCoord_ = 0;
        slaveMovingCoord_ = 1;
        
        baseMovingStep_ = util::sign(p1.x() - p0.x());
        slaveMovingStep_ = util::sign(p1.y() - p0.y());
        
        // dError = dy / dx
        dError_ =
          static_cast<double>(p1.y() - p0.y()) / 
          static_cast<double>(p1.x() - p0.x());
      }
      else
      {
        // Line angle is greater than 45 degrees (absolute).
        // Moving along OY axis.
        baseMovingCoord_ = 1;
        slaveMovingCoord_ = 0;
        
        baseMovingStep_ = util::sign(p1.y() - p0.y());
        slaveMovingStep_ = util::sign(p1.x() - p0.x());
        
        // dError = dx / dy
        dError_ =
          static_cast<double>(p1.x() - p0.x()) / 
          static_cast<double>(p1.y() - p0.y());
      }
    }
    
    operator bool () const 
    {
      return valid_;
    }
    
    Vector2i operator * () const
    {
      return p_;
    }
    
    Vector2i const * operator -> () const
    {
      return &p_;
    }
    
    PointsIterator & operator ++ ()
    {
      if (valid_)
        step();
      
      return *this;
    }
    
    PointsIterator operator ++ ( int )
    {
      PointsIterator tmp = *this;
      ++*this;
      return tmp;
    }
    
  private:
    void step()
    {
      std::cout << "(" << pStart_.x() << "," << pStart_.y() << ") -> (" << pEnd_.x() << "," << pEnd_.y() << ")\n";
      std::cout << "cur. pos: (" << p_.x() << "," << p_.y() << ")\n";
      std::cout << "base coord: " << baseMovingCoord_ << ", slave coord: " << slaveMovingCoord_ << "\n";
      std::cout << "base step: " << baseMovingStep_ << ", slave step: " << slaveMovingStep_ << "\n";
      std::cout << "error: " << error_ << "\n";
      
      assert(valid_);
      if (p_[baseMovingCoord_] == pEnd_[baseMovingCoord_])
      {
        // At last point
        p_[slaveMovingCoord_] = pEnd_[slaveMovingCoord_];
        valid_ = false;
      }
      else
      {
        assert(baseMovingStep_ != 0);
        p_[baseMovingCoord_] += baseMovingStep_;
        if (p_[baseMovingCoord_] == pEnd_[baseMovingCoord_])
        {
          // Reach last point.
          p_[slaveMovingCoord_] = pEnd_[slaveMovingCoord_];
          valid_ = false;
        }
        else
        {
          error_ += dError_;
          if (abs(error_) >= 0.5)
          {
            p_[slaveMovingCoord_] += slaveMovingStep_;
            error_ -= slaveMovingStep_;
          }
        }
      }
    }

  private:
    bool valid_;
    
    Vector2i p_;            // current point
    Vector2i pStart_;       // start point (for debugging purposes)
    Vector2i pEnd_;         // end point
    
    int baseMovingCoord_, slaveMovingCoord_;
    int baseMovingStep_, slaveMovingStep_;
    double error_;          // current error
    double dError_;         // error increasing step
  };
}

#endif // BRESENHAM_H
