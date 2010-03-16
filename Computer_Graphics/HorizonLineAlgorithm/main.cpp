/*
 *  This file is part of HLA - Horizon Line Algorithm implementation.
 *
 *  Copyright (C) 2010  Vladimir Rutsky
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
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CImg.h"

using namespace cimg_library;

int main() 
{
  // 640x400 color image with 8 BPP.
  CImg<unsigned char> image(640, 400, 1, 3);

  // Fill with black.
  image.fill(0); 

  unsigned char const red[] = { 255, 0, 0 };

  image.draw_text(100, 100, "Hello, World!", red);

  image.display("CImg: Hello, World!");
}
