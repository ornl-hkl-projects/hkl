/* This file is part of the hkl library.
 *
 * The hkl library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The hkl library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the hkl library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2003-2023 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 */
/*
 * Copyright (C) Linus Torvalds, 2005-2006.
 * Copyright (C) Synchrotron Soleil 2007.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "xrays-macros.h"

static void report(const char *prefix, const char *err, va_list params)
{
  fputs(prefix, stderr);
  vfprintf(stderr, err, params);
  fputs("\n", stderr);
}

static NORETURN void die_builtin(const char *err, va_list params)
{
  report("fatal: ", err, params);
  exit(128);
}

/* If we are in a dlopen()ed .so write to a global variable would segfault
 * (ugh), so keep things static. */
static void (*die_routine)(const char *err, va_list params) NORETURN = die_builtin;

void set_die_routine(void (*routine)(const char *err, va_list params) NORETURN)
{
  die_routine = routine;
}

void die(const char *err, ...)
{
  va_list params;

  va_start(params, err);
  die_routine(err, params);
  va_end(params);
}
