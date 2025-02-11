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
 * Copyright (C) 2003-2020 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 */
#ifndef __HKL_MATRIX_PRIVATE_H__
#define __HKL_MATRIX_PRIVATE_H__

#include <stdio.h>

#include "hkl.h"

G_BEGIN_DECLS

struct _HklMatrix
{
	double data[3][3];
};

extern HklMatrix *hkl_matrix_dup(const HklMatrix* self);

extern void hkl_matrix_init_from_euler(HklMatrix *self,
				       double euler_x, double euler_y, double euler_z) HKL_ARG_NONNULL(1);

extern void hkl_matrix_matrix_set(HklMatrix *self, const HklMatrix *m) HKL_ARG_NONNULL(1, 2);

extern void hkl_matrix_init_from_two_vector(HklMatrix *self,
					    const HklVector *v1, const HklVector *v2);

extern void hkl_matrix_fprintf(FILE *file, const HklMatrix *self);

extern void hkl_matrix_to_euler(const HklMatrix *self,
				double *euler_x, double *euler_y, double *euler_z);

extern void hkl_matrix_times_vector(const HklMatrix *self, HklVector *v);

extern void hkl_matrix_transpose(HklMatrix *self);

extern double hkl_matrix_det(const HklMatrix *self);

extern int hkl_matrix_solve(const HklMatrix *self,
			    HklVector *x, const HklVector *b);

extern int hkl_matrix_is_null(const HklMatrix *self);

extern void hkl_matrix_div_double(HklMatrix *self, double d);

extern int hkl_matrix_inv(const HklMatrix *self, HklMatrix *inv);

G_END_DECLS

#endif
