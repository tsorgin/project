/*
   This file is part of the Mandelbox program developed for the course
    CS/SE  Distributed Computer Systems taught by N. Nedialkov in the
    Winter of 2015 at McMaster University.

    Copyright (C) 2015 T. Gwosdz and N. Nedialkov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
#else
#include <sys/resource.h>
#endif
#include <unistd.h>

double getTime() {  
#if defined(_MSC_VER) || defined(__MINGW32__)
  return 0;
#else         
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  struct timeval time;
  time = usage.ru_utime;
  return time.tv_sec+time.tv_usec/1e6;
#endif
}
