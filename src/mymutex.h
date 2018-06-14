/*  mymutex.h
 *
 *  Copyright (C) 2017 Josh Fisher
 *
 *  This program is free software. You may redistribute it and/or modify
 *  it under the terms of the GNU General Public License, as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  See the file "COPYING".  If not,
 *  see <http://www.gnu.org/licenses/>.
*/

#ifndef _MYMUTEX_H_
#define _MYMUTEX_H_ 1

#ifndef HAVE_TIME_H
#include <time.h>
#endif

int mymutex_create(const char *storage_name, time_t max_wait);
int mymutex_destroy(const char *storage_name, int fd);

#endif /* _MYPOPEN_H_ */
