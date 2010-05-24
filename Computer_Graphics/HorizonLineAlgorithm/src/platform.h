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

/* 
 * Platform specific stuff.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <CImg.h>

#if       cimg_OS == 2
#define MAIN_FUNCTION \
    int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
#else  // cimg_OS == 2
#define MAIN_FUNCTION \
    int main()
#endif // cimg_OS == 2

#endif // PLATFORM_H