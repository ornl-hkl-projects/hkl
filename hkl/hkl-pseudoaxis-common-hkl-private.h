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
 * Copyright (C) 2003-2019 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 *          Maria-Teresa Nunez-Pardo-de-Verra <tnunez@mail.desy.de>
 */
#ifndef __HKL_PSEUDOAXIS_COMMON_HKL_PRIVATE__
#define __HKL_PSEUDOAXIS_COMMON_HKL_PRIVATE__

#include <gsl/gsl_vector_double.h>      // for gsl_vector
#include "hkl-pseudoaxis-auto-private.h"
#include "hkl-pseudoaxis-private.h"     // for HklModeOperations, etc
#include "hkl.h"                        // for HklEngine, HklDetector, etc
#include "hkl-pseudoaxis-common-readonly-private.h"

typedef struct _HklEngineHkl HklEngineHkl;
struct _HklEngineHkl {
	HklEngine engine;
	HklParameter *h;
	HklParameter *k;
	HklParameter *l;
};

extern int _RUBh_minus_Q_func(const gsl_vector *x, void *params, gsl_vector *f);
extern int _double_diffraction_func(const gsl_vector *x, void *params, gsl_vector *f);
extern int _psi_constant_vertical_func(const gsl_vector *x, void *params, gsl_vector *f);
extern int _emergence_fixed_func(const gsl_vector *x, void *params, gsl_vector *f);

extern int RUBh_minus_Q(double const x[], void *params, double f[]);
extern int _double_diffraction(double const x[], void *params, double f[]);

extern int hkl_is_reachable(HklEngine *engine, double wavelength, GError **error);

extern int hkl_mode_get_hkl_real(HklMode *self,
				 HklEngine *engine,
				 HklGeometry *geometry,
				 HklDetector *detector,
				 HklSample *sample,
				 GError **error);

extern int hkl_mode_set_hkl_real(HklMode *self,
				 HklEngine *engine,
				 HklGeometry *geometry,
				 HklDetector *detector,
				 HklSample *sample,
				 GError **error);

extern int hkl_mode_initialized_set_psi_constant_vertical_real(HklMode *base,
							       HklEngine *engine,
							       HklGeometry *geometry,
							       HklDetector *detector,
							       HklSample *sample,
							       int initialized,
							       GError **error);

extern HklEngine *hkl_engine_hkl_new(HklEngineList *engines);

#define HKL_MODE_OPERATIONS_HKL_DEFAULTS	\
	HKL_MODE_OPERATIONS_AUTO_DEFAULTS,	\
		.get = hkl_mode_get_hkl_real

#define HKL_MODE_OPERATIONS_HKL_FULL_DEFAULTS	\
	HKL_MODE_OPERATIONS_HKL_DEFAULTS,	\
		.set = hkl_mode_set_hkl_real

static const HklModeOperations hkl_mode_operations = {
	HKL_MODE_OPERATIONS_HKL_DEFAULTS,
};

static const HklModeOperations hkl_full_mode_operations = {
	HKL_MODE_OPERATIONS_HKL_FULL_DEFAULTS,
};

static const HklModeOperations psi_constant_vertical_mode_operations = {
	HKL_MODE_OPERATIONS_HKL_FULL_DEFAULTS,
	.capabilities = HKL_ENGINE_CAPABILITIES_READABLE | HKL_ENGINE_CAPABILITIES_WRITABLE | HKL_ENGINE_CAPABILITIES_INITIALIZABLE,
	.initialized_set = hkl_mode_initialized_set_psi_constant_vertical_real,
};

static const HklModeOperations constant_incidence_mode_operations = {
	HKL_MODE_OPERATIONS_AUTO_WITH_INIT_DEFAULTS,
	.get = hkl_mode_get_hkl_real,
	.set = hkl_mode_set_hkl_real
};

static const HklFunction RUBh_minus_Q_func = {
	.function = _RUBh_minus_Q_func,
	.size = 3,
};

static const HklFunction double_diffraction_func = {
	.function = _double_diffraction_func,
	.size = 4,
};

static const HklFunction psi_constant_vertical_func = {
	.function = _psi_constant_vertical_func,
	.size = 4,
};

/* mode parameters */

#define CONSTANT_PARAMETER(_name)				\
	{							\
		HKL_PARAMETER_DEFAULTS_ANGLE, .name = #_name,	\
			.description = "the freezed value",	\
			}

#define PSI_CONSTANT_PARAMETERS(_h2, _k2, _l2, _psi)			\
	{								\
		HKL_PARAMETER_DEFAULTS, .name = "h2", ._value = _h2,	\
			.description = "h coordinate of the reference plan", \
			.range = { .min=-1, .max=1 },			\
			},						\
	{								\
		HKL_PARAMETER_DEFAULTS, .name = "k2", ._value = _k2,	\
			.description = "k coordinate of the reference plan", \
			.range = { .min=-1, .max=1 },			\
			},						\
	{								\
		HKL_PARAMETER_DEFAULTS, .name = "l2", ._value = _l2,	\
			.description = "l coordinate of the reference plan", \
			.range = { .min=-1, .max=1 },			\
			},						\
	{								\
		HKL_PARAMETER_DEFAULTS_ANGLE, .name = "psi", ._value = _psi, \
			.description = "expected angle between the reference and the diffraction plans", \
			}

static const HklParameter constant_omega_parameters[] = { CONSTANT_PARAMETER(omega) };
static const HklParameter constant_chi_parameters[] = { CONSTANT_PARAMETER(chi) };
static const HklParameter constant_phi_parameters[] = { CONSTANT_PARAMETER(phi) };

/* outside the mode because used in more than one mode */
static const HklParameter double_diffraction_parameters[] = {
	{
		HKL_PARAMETER_DEFAULTS, .name = "h2", ._value = 1,
		.description = "h coordinate of the second diffracting plan",
		.range = {.min=-1, .max=1},
	},
	{
		HKL_PARAMETER_DEFAULTS, .name = "k2", ._value = 1,
		.description = "k coordinate of the second diffracting plan",
		.range = {.min=-1, .max=1},
	},
	{
		HKL_PARAMETER_DEFAULTS, .name = "l2", ._value = 1,
		.description = "l coordinate of the second diffracting plan",
		.range = {.min=-1, .max=1},
	},
};

static const HklParameter psi_constant_parameters[] = { PSI_CONSTANT_PARAMETERS(1, 1, 1, 0) };

static const HklParameter constant_incidence_parameters[] = {
	SURFACE_PARAMETERS(1, 1, 1),
	{
		HKL_PARAMETER_DEFAULTS_ANGLE, .name = "incidence",
		.description = "expected incidence of the incoming beam $\\vec{k_i}$ on the surface."
	},
	{
		HKL_PARAMETER_DEFAULTS_ANGLE, .name = "azimuth", ._value = M_PI_2,
		.description = "expected azimuth",
	},
};

#define HKL_PARAMETER_EMERGENCE_DEFAULTS				\
	HKL_PARAMETER_DEFAULTS_ANGLE,					\
		.name="emergence",					\
		.description = "expected emergence of the outgoing beam $\\vec{k_f}$ from the surface $\\vec{n}$."

#define HKL_PARAMETER_EMERGENCE_AZIMUTH_DEFAULTS			\
	HKL_PARAMETER_DEFAULTS_ANGLE,					\
		.name="emergence_azimuth",				\
		._value = M_PI_2,					\
		.description = "expected azimuth of the outgoing beam $\\vec{k_f}$ from the surface $\\vec{n}$ projected into the yOz plan."

/*******************/
/* Emergence fixed */
/*******************/

static const HklFunction emergence_fixed_func = {
	.function = _emergence_fixed_func,
	.size = 4,
};

#define HKL_MODE_HKL_EMERGENCE_FIXED_PARAMETERS_DEFAULTS(_x, _y, _z, _emergence) \
	SURFACE_PARAMETERS(_x, _y, _z),					\
	{								\
		HKL_PARAMETER_EMERGENCE_DEFAULTS,			\
			._value = _emergence,				\
			}

extern HklMode *hkl_mode_hkl_emergence_fixed_new(const HklModeAutoInfo *info);

#endif
