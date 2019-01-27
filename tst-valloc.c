/* Test for valloc.
   Copyright (C) 2013-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include "test.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int errors = 0;

static void merror(const char *msg) {
  ++errors;
  printf("Error: %s\n", msg);
}

TEST(valloc) {
  void *p;
  unsigned long pagesize = getpagesize();
  unsigned long ptrval;
  int save;

  errno = 0;

  /* An attempt to allocate a huge value should return NULL and set
     errno to ENOMEM.  */
  p = valloc(-1);

  save = errno;

  if (p != NULL)
    merror("valloc (-1) succeeded.");

  if (p == NULL && save != ENOMEM)
    merror("valloc (-1) errno is not set correctly");

  free(p);

  errno = 0;

  /* Test to expose integer overflow in malloc internals from BZ #15856.  */
  p = valloc(-pagesize);

  save = errno;

  if (p != NULL)
    merror("valloc (-pagesize) succeeded.");

  if (p == NULL && save != ENOMEM)
    merror("valloc (-pagesize) errno is not set correctly");

  free(p);

  /* A zero-sized allocation should succeed with glibc, returning a
     non-NULL value.  */
  p = valloc(0);

  if (p != NULL)
    merror("valloc (0) failed.");

  free(p);

  /* Check the alignment of the returned pointer is correct.  */
  p = valloc(32);

  if (p == NULL)
    merror("valloc (32) failed.");

  ptrval = (unsigned long)p;

  if ((ptrval & (pagesize - 1)) != 0)
    merror("returned pointer is not page aligned.");

  free(p);

  return errors != 0;
}
