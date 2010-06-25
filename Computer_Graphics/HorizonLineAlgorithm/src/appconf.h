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

#ifndef APPCONF_H
#define APPCONF_H

#include <cstddef>

namespace appconf
{
  extern size_t const startFuncIdx;
  extern size_t const startXCells, startYCells;
  extern double const startXDomain, startYDomain;
  extern double const startXViewVolume, startYViewVolume;
  extern bool const startKeepAspectRatio;
  extern bool const startDrawXEdges, startDrawYEdges;
  extern double const startYaw, startPitch;
  extern double const startXDomainCenter, startYDomainCenter;
  extern double const startXViewVolumeCenter, startYViewVolumeCenter;
  extern bool const startDrawAxes;
  extern bool const startDrawFakeEdges;
}

#endif // APPCONF_H
