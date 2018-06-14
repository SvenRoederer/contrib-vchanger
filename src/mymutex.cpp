/* mymutex.cpp
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

#include "config.h"
#ifdef HAVE_WINDOWS_H
#include "targetver.h"
#include <windows.h>
#include "win32_util.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#include "loghandler.h"
#include "mypopen.h"



#ifdef HAVE_WINDOWS_H

union whandle
{
   HANDLE hval;
   int ival;
};
typedef union whandle whandle_t;

static HANDLE win32_int_to_handle(int fd)
{
   whandle_t h;
   h.hval = NULL;
   h.ival = fd;
   return h.hval;
}

static int win32_handle_to_int(HANDLE fd)
{
   whandle_t h;
   h.hval = fd;
   return h.ival;
}

/*
 *  Function to create a win32 named mutex object.
 *  On success, returns the mutex handle as an int. On error, returns -1 and
 *  sets errno appropriately.
 */
static int win32_mutex_create(const char *name)
{
   HANDLE mut;
   wchar_t *p16, *path = NULL;
   size_t path_sz = 0;

    /* Convert path string to UTF16 encoding */
    if (!AnsiToUTF16(name, &path, &path_sz)) {
       errno = w32errno(ERROR_BAD_PATHNAME);
       return -1;
    }
   mut = CreateMutexW(NULL, TRUE, path);
   if (mut == NULL) {
      errno = w32errno(GetLastError());
      free(path);
      return -1;
   }
   free(path);
   return win32_handle_to_int(mut);
}

#endif


/*
 *  Function to create a mutex owned by the caller. Waits up to max_wait seconds
 *  for the mutex to be created. If max_wait is negative, waits indefinitely. If
 *  max_wait is zero, tries once to create mutex and does not block.
 *  On success, returns the descriptor of a new named mutex. On error, returns -1 and
 *  sets errno appropriately.
 */
int mymutex_create(const char *storage_name, time_t max_wait)
{
   int mut = -1;
   time_t timeout = time(NULL) + max_wait;
   char lockname[4096];

   if (!storage_name || !storage_name[0]) {
      /* Only create named mutex */
      errno = EINVAL;
      return -1;
   }
#ifdef HAVE_WINDOWS_H
      snprintf(lockname, sizeof(lockname), "vchanger-%s", storage_name);
#else
      snprintf(lockname, sizeof(lockname), "/vchanger-%s", storage_name);
#endif
      while (mut < 0) {
#ifdef HAVE_WINDOWS_H
      mut = win32_mutex_create(lockname);
#else
      mut = shm_open(lockname, O_CREAT|O_EXCL, 0777);
#endif
      if (mut < 0) {
         if (errno != EEXIST) return -1;
         if (time(NULL) > timeout) {
            errno = EBUSY;
            return -1;
         }
         sleep(1);
      }
   }
   return mut;
}


/*
 *  Function to destroy a mutex owned by the caller.
 *  On success, returns zero. On error, returns -1 and
 *  sets errno appropriately.
 */
int mymutex_destroy(const char *storage_name, int fd)
{
   char lockname[256];

   if (!storage_name || !storage_name[0] || fd < 0) {
      /* Only destroy named mutex */
      errno = EINVAL;
      return -1;
   }
#ifdef HAVE_WINDOWS_H
   if (CloseHandle(win32_int_to_handle(fd)) == FALSE) {
      errno = w32errno(GetLastError());
      return -1;
   }
#else
   snprintf(lockname, sizeof(lockname), "/vchanger-%s", storage_name);
   close(fd);
   if (shm_unlink(lockname)) return -1;
#endif
   return 0;
}

