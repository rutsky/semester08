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

#include "appconf.h"

namespace appconf
{
  size_t const startFuncIdx = 15;
  size_t const startXCells = 10, startYCells = 10;
  double const startXDomain = 5.0, startYDomain = 5.0;
  double const startXViewVolume = 7.0, startYViewVolume = 7.0;
  bool const startKeepAspectRatio = true;
  bool const startDrawXEdges = true, startDrawYEdges = true;
  double const startYaw = 30.0, startPitch = 45.0;
  double const startXDomainCenter = 0.0, startYDomainCenter = 0.0;
  double const startXViewVolumeCenter = 0.0, startYViewVolumeCenter = 0.0;
  bool const startDrawAxes = true;
  bool const startDrawFakeEdges = true;
}
