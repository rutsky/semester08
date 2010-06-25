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

/* 
 * Platform specific stuff.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

// TODO: Not needed because FLTK provides such wrapper.
#if defined(WIN32)

#include <windows.h>

// Windows platform.
#define MAIN_FUNCTION \
    int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
#else  // POSIX platform.
#define MAIN_FUNCTION \
    int main()
#endif

#endif // PLATFORM_H
