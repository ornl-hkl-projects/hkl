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
 * Copyright (C) 2003-2019, 2022 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 */
#define _GNU_SOURCE
#include <gsl/gsl_sf_trig.h>
#include <math.h>                       // for floor, M_PI_2, acos, asin, etc
#include <stdlib.h>                     // for free, NULL
#include "hkl-interval-private.h"       // for HklInterval
#include "hkl-macros-private.h"         // for HKL_MALLOC
#include "hkl.h"                        // for FALSE, TRUE

/**
 * hkl_interval_dup: (skip)
 * @self:
 *
 * copy an #HklInterval
 *
 * Returns:
 **/
HklInterval *hkl_interval_dup(const HklInterval *self)
{
	HklInterval *dup = g_new(HklInterval, 1);

	*dup = *self;

	return dup;
}

/**
 * hkl_interval_free: (skip)
 * @self:
 *
 * delete an #HklInterval
 **/
void hkl_interval_free(HklInterval *self)
{
        free(self);
}

/**
 * hkl_interval_cmp: (skip)
 * @self:
 * @interval:
 *
 * compare two intervals
 *
 * Returns:
 **/
int hkl_interval_cmp(const HklInterval *self, const HklInterval *interval)
{
	return (fabs(self->min - interval->min) > HKL_EPSILON)
		|| (fabs(self->max - interval->max) > HKL_EPSILON);
}

/**
 * hkl_interval_plus_interval: (skip)
 * @self:
 * @interval:
 *
 * add two ontervals
 **/
void hkl_interval_plus_interval(HklInterval *self, const HklInterval *interval)
{
	self->min += interval->min;
	self->max += interval->max;
}

/**
 * hkl_interval_plus_double: (skip)
 * @self:
 * @d:
 *
 * add to an interval a double
 **/
void hkl_interval_plus_double(HklInterval *self, double const d)
{
	self->min += d;
	self->max += d;
}


/**
 * hkl_interval_minus_interval: (skip)
 * @self:
 * @interval:
 *
 * substract two #HklInterval
 **/
void hkl_interval_minus_interval(HklInterval *self, const HklInterval *interval)
{
	self->min -= interval->max;
	self->max -= interval->min;
}


/**
 * hkl_interval_minus_double: (skip)
 * @self:
 * @d:
 *
 * subst a double to an #HklInterval
 **/
void hkl_interval_minus_double(HklInterval *self, double const d)
{
	self->min -= d;
	self->max -= d;
}

/**
 * hkl_interval_times_interval: (skip)
 * @self:
 * @interval:
 *
 * multiply two #HklInterval
 **/
void hkl_interval_times_interval(HklInterval *self, const HklInterval *interval)
{
	double min;
	double max;
	double m1 = self->min * interval->min;
	double m2 = self->min * interval->max;
	double m3 = self->max * interval->min;
	double m4 = self->max * interval->max;

	min = m1;
	if (m2 < min)
		min = m2;
	if (m3 < min)
		min = m3;
	if (m4 < min)
		min = m4;

	max = m1;
	if (m2 > max)
		max = m2;
	if (m3 > max)
		max = m3;
	if (m4 > max)
		max = m4;

	self->min = min;
	self->max = max;
}

/**
 * hkl_interval_times_double: (skip)
 * @self:
 * @d:
 *
 * multiply an #HklInterval by a double
 **/
void hkl_interval_times_double(HklInterval *self, double const d)
{
	double min;
	double max;
	if (d < 0) {
		min = self->max * d;
		max = self->min * d;
	} else {
		min = self->min * d;
		max = self->max * d;
	}
	self->min = min;
	self->max = max;
}

/**
 * hkl_interval_divides_double: (skip)
 * @self:
 * @d:
 *
 * divide an #HklInterval by a double
 **/
void hkl_interval_divides_double(HklInterval *self, double const d)
{
	double min = self->min / d;
	double max = self->max / d;
	if (min > max){
		double tmp = min;
		min = max;
		max = tmp;
	}
	self->min = min;
	self->max = max;
}

/**
 * hkl_interval_contain_zero: (skip)
 * @self:
 *
 * check if an #HklInterval contain zero
 *
 * Returns:
 **/
int hkl_interval_contain_zero(HklInterval const *self)
{
	if (self->min <= 0 && self->max >= 0)
		return TRUE;
	else
		return FALSE;
}

/**
 * hkl_interval_cos: (skip)
 * @self:
 *
 * compute the cosinus of an #HklInterval
 **/
void hkl_interval_cos(HklInterval *self)
{
	double min = 0;
	double max = 0;
	double cmin;
	double cmax;

	cmin = cos(self->min);
	cmax = cos(self->max);

	if (self->max - self->min >= 2 * M_PI) {
		min = -1;
		max = 1;
	} else {
		int quad_min;
		int quad_max;

		quad_min = (int)floor(self->min / M_PI_2) % 4;
		if (quad_min < 0)
			quad_min += 4;

		quad_max = (int)floor(self->max / M_PI_2) % 4;
		if (quad_max < 0)
			quad_max += 4;

		switch (quad_max) {
		case 0:
			switch (quad_min) {
			case 0:
				min = cmax;
				max = cmin;
				break;
			case 1:
				min = -1;
				max = 1;
				break;
			case 2:
				min = cmin;
				max = 1;
				break;
			case 3:
				if (cmin < cmax) {
					min = cmin;
					max = 1;
				} else {
					min = cmax;
					max = 1;
				}
				break;
			}
			break;
		case 1:
			switch (quad_min) {
			case 0:
				min = cmax;
				max = cmin;
				break;
			case 1:
				min = -1;
				max = 1;
				break;
			case 2:
				if (cmin < cmax) {
					min = cmin;
					max = 1;
				} else {
					min = cmax;
					max = 1;
				}
				break;
			case 3:
				min = cmax;
				max = 1;
				break;
			}
			break;
		case 2:
			switch (quad_min) {
			case 0:
				min = -1;
				max = cmin;
				break;
			case 1:
				if (cmin < cmax) {
					min = -1;
					max = cmax;
				} else {
					min = -1;
					max = cmin;
				}
				break;
			case 2:
				if (cmin < cmax) {
					min = cmin;
					max = cmax;
				} else {
					min = -1;
					max = 1;
				}
				break;
			case 3:
				min = -1;
				max = 1;
				break;
			}
			break;
		case 3:
			switch (quad_min) {
			case 0:
				if (cmin < cmax) {
					min = -1;
					max = cmax;
				} else {
					min = -1;
					max = cmin;
				}
				break;
			case 1:
				min = -1;
				max = cmax;
				break;
			case 2:
				min = cmin;
				max = cmax;
				break;
			case 3:
				if (cmin < cmax) {
					min = cmin;
					max = cmax;
				} else {
					min = -1;
					max = 1;
				}
				break;
			}
			break;
		}
	}
	self->min = min;
	self->max = max;
}

/**
 * hkl_interval_acos: (skip)
 * @self:
 *
 * compute the arc cosinus of an #HklInterval
 **/
void hkl_interval_acos(HklInterval *self)
{
	double tmp;

	tmp = self->min;
	self->min = acos(self->max);
	self->max = acos(tmp);
}


/**
 * hkl_interval_sin: (skip)
 * @self:
 *
 * compute the sin of an #HklInterval
 **/
void hkl_interval_sin(HklInterval *self)
{
	double min = 0;
	double max = 0;
	double smin;
	double smax;

	smin = sin(self->min);
	smax = sin(self->max);

	/* if there is at least one period in b, then a = [-1, 1] */
	if ( self->max - self->min >= 2 * M_PI) {
		min = -1;
		max = 1;
	} else {
		int quad_min;
		int quad_max;

		quad_min = (int)floor(self->min / M_PI_2) % 4;
		if (quad_min < 0)
			quad_min += 4;

		quad_max = (int)floor(self->max / M_PI_2) % 4;
		if (quad_max < 0)
			quad_max += 4;

		switch (quad_max) {
		case 0:
			switch (quad_min) {
			case 0:
				if (smin < smax) {
					min = smin;
					max = smax;
				} else {
					min = -1;
					max = 1;
				}
				break;
			case 3:
				min = smin;
				max = smax;
				break;
			case 1:
				if (smin > smax) {
					min = -1;
					max = smin;
				} else {
					min = -1;
					max = smax;
				}
				break;
			case 2:
				min = -1;
				max = smax;
				break;
			}
			break;
		case 1:
			switch (quad_min) {
			case 0:
				if (smin < smax) {
					min = smin;
					max = 1;
				} else {
					min = smax;
					max = 1;
				}
				break;
			case 1:
				if (smin < smax) {
					min = -1;
					max = 1;
				} else {
					min = smax;
					max = smin;
				}
				break;
			case 2:
				min = -1;
				max = 1;
				break;
			case 3:
				min = smin;
				max = 1;
				break;
			}
			break;
		case 2:
			switch (quad_min) {
			case 0:
				min = smax;
				max = 1;
				break;
			case 1:
			case 2:
				if (smin < smax) {
					min = -1;
					max = 1;
				} else {
					min = smax;
					max = smin;
				}
				break;
			case 3:
				if (smin < smax) {
					min = smin;
					max = 1;
				} else {
					min = smax;
					max = 1;
				}
				break;
			}
			break;
		case 3:
			switch (quad_min) {
			case 0:
				min = -1;
				max = 1;
				break;
			case 1:
				min = -1;
				max = smin;
				break;
			case 2:
				if (smin < smax) {
					min = -1;
					max = smax;
				} else {
					min = -1;
					max = smin;
				}
				break;
			case 3:
				if (smin < smax) {
					min = smin;
					max = smax;
				} else {
					min = -1;
					max = 1;
				}
				break;
			}
			break;
		}
	}
	self->min = min;
	self->max = max;
}

/**
 * hkl_interval_asin: (skip)
 * @self:
 *
 * compute the arc sinus of an #HklInterval
 **/
void hkl_interval_asin(HklInterval *self)
{
	self->min = asin(self->min);
	self->max = asin(self->max);
}

/**
 * hkl_interval_tan: (skip)
 * @self:
 *
 * compute the tangente of an #HklInterval
 **/
void hkl_interval_tan(HklInterval *self)
{
	int quadrant_down = (int)floor(self->min / M_PI_2);
	int quadrant_up = (int)floor(self->max / M_PI_2);

	/* if there is at least one period in b or if b contains a Pi/2 + k*Pi, */
	/* then a = ]-oo, +oo[ */
	if ( ((quadrant_up - quadrant_down) >= 2)
	     || (!(quadrant_down % 2) && (quadrant_up % 2)) ) {
		self->min = -INFINITY;
		self->max = INFINITY;
	} else {
		self->min = tan(self->min);
		self->max = tan(self->max);
	}
}

/**
 * hkl_interval_atan: (skip)
 * @self:
 *
 * compute the arc tangente of an #HklInterval
 **/
void hkl_interval_atan(HklInterval *self)
{
	self->min = atan(self->min);
	self->max = atan(self->max);
}

/**
 * hkl_interval_length: (skip)
 * @self:
 *
 * compute the length of an #HklInterval
 *
 * Returns:
 **/
double hkl_interval_length(const HklInterval *self)
{
	return self->max - self->min;
}

/**
 * hkl_interval_angle_restrict_symm: (skip)
 * @self:
 *
 * restrict an #HklInterval into -pi, pi
 **/
void hkl_interval_angle_restrict_symm(HklInterval *self)
{
	gsl_sf_angle_restrict_symm_e(&self->min);
	gsl_sf_angle_restrict_symm_e(&self->max);
}
