/***********************************************************************************
   Copyright (C) 2022 by Sven Kochmann

   This file is part of Beryllium¹º.

   Beryllium¹º is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Beryllium¹º is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Beryllium¹º.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************************/

#pragma once

/*  wxWidget defines */
#define wxUSE_GUI 1

/* MacOS defines */

/* Windows defines */
#ifdef WIN32
    /* Windows 7 (= Version 6.1 internally) is the minimum requirement for Beryllium */
    #define WINVER 0x0601
    #define _WIN32_WINNT 0x0601

    /* To speed up compilation time let's not include everything from Windows' header files */
	#define WIN32_MEAN_AND_LEAN
	#include <windows.h>
#endif

/* some WxWidget includes for convenience */
#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
