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
 */
#include <gsl/gsl_sys.h>                // for gsl_isnan
#include "hkl-factory-private.h"        // for autodata_factories_, etc
#include "hkl-pseudoaxis-common-hkl-private.h"  // for hkl_mode_operations, etc
#include "hkl-pseudoaxis-common-psi-private.h"  // for hkl_engine_psi_new, etc
#include "hkl-pseudoaxis-common-q-private.h"  // for hkl_engine_q2_new, etc
#include "hkl-pseudoaxis-common-tth-private.h"  // for hkl_engine_tth2_new, etc
#include "hkl-pseudoaxis-common-readonly-private.h"  // for hkl_engine_tth2_new, etc

#define MU "mu"
#define OMEGA "omega"
#define CHI "chi"
#define PHI "phi"
#define GAMMA "gamma"
#define DELTA "delta"

/***********************/
/* numerical functions */
/***********************/

static int _bissector_horizontal_func(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double mu = x->data[0];
	const double omega = x->data[1];
	const double gamma = x->data[4];

	CHECK_NAN(x->data, x->size);

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = fmod(omega, M_PI);
	f->data[4] = gamma - 2 * fmod(mu, M_PI);

	return  GSL_SUCCESS;
}

static const HklFunction bissector_horizontal_func = {
	.function = _bissector_horizontal_func,
	.size = 5,
};

static int _bissector_vertical_func(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double omega = x->data[0];
	const double tth = x->data[3];

	CHECK_NAN(x->data, x->size);

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = tth - 2 * fmod(omega,M_PI);

	return  GSL_SUCCESS;
}

static const HklFunction bissector_vertical_func = {
	.function = _bissector_vertical_func,
	.size = 4,
};

/************/
/* mode hkl */
/************/

static HklMode *bissector_vertical(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {OMEGA, CHI, PHI, DELTA};
	static const HklFunction *functions[] = {&bissector_vertical_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_omega_vertical(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {CHI, PHI, DELTA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_chi_vertical(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {OMEGA, PHI, DELTA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_phi_vertical(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {OMEGA, CHI, DELTA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *lifting_detector_phi(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {PHI, GAMMA, DELTA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *lifting_detector_omega(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {OMEGA, GAMMA, DELTA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *lifting_detector_mu(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {MU, GAMMA, DELTA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *double_diffraction_vertical(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {OMEGA, CHI, PHI, DELTA};
	static const HklFunction *functions[] = {&double_diffraction_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, double_diffraction_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *bissector_horizontal(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {MU, OMEGA, CHI, PHI, GAMMA};
	static const HklFunction *functions[] = {&bissector_horizontal_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *double_diffraction_horizontal(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {MU, CHI, PHI, GAMMA};
	static const HklFunction *functions[] = {&double_diffraction_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, double_diffraction_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *psi_constant_vertical(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {OMEGA, CHI, PHI, DELTA};
	static const HklFunction *functions[] = {&psi_constant_vertical_func};
	static const HklParameter parameters[] = { PSI_CONSTANT_PARAMETERS(1, 0, 0, 0) };
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w, functions, parameters),
	};

	return hkl_mode_auto_new(&info,
				 &psi_constant_vertical_mode_operations,
				 TRUE);
}

static HklMode *psi_constant_horizontal(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {OMEGA, CHI, PHI, GAMMA};
	static const HklFunction *functions[] = {&psi_constant_vertical_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, psi_constant_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &psi_constant_vertical_mode_operations,
				 TRUE);
}

static HklMode *constant_mu_horizontal(void)
{
	static const char* axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char* axes_w[] = {CHI, PHI, GAMMA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_full_mode_operations,
				 TRUE);
}

static HklEngine *hkl_engine_e6c_hkl_new(HklEngineList *engines)
{
	HklEngine *self;
	HklMode *default_mode;

	self = hkl_engine_hkl_new(engines);

	default_mode = bissector_vertical();
	hkl_engine_add_mode(self, default_mode);
	hkl_engine_mode_set(self, default_mode);

	hkl_engine_add_mode(self, constant_omega_vertical());
	hkl_engine_add_mode(self, constant_chi_vertical());
	hkl_engine_add_mode(self, constant_phi_vertical());
	hkl_engine_add_mode(self, lifting_detector_phi());
	hkl_engine_add_mode(self, lifting_detector_omega());
	hkl_engine_add_mode(self, lifting_detector_mu());
	hkl_engine_add_mode(self, double_diffraction_vertical());
	hkl_engine_add_mode(self, bissector_horizontal());
	hkl_engine_add_mode(self, double_diffraction_horizontal());
	hkl_engine_add_mode(self, psi_constant_vertical());
	hkl_engine_add_mode(self, psi_constant_horizontal());
	hkl_engine_add_mode(self, constant_mu_horizontal());

	return self;
}

/************/
/* mode psi */
/************/

static HklMode* psi_vertical()
{
	static const char *axes_r[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};
	static const char *axes_w[] = {OMEGA, CHI, PHI, DELTA};
	static const HklFunction *functions[] = {&psi_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, psi_parameters),
	};

	return hkl_mode_psi_new(&info);
}

static HklEngine *hkl_engine_e6c_psi_new(HklEngineList *engines)
{
	HklEngine *self;
	HklMode *default_mode;

	self = hkl_engine_psi_new(engines);

	default_mode = psi_vertical();
	hkl_engine_add_mode(self, default_mode);
	hkl_engine_mode_set(self, default_mode);

	return self;
}

/*****************/
/* mode readonly */
/*****************/

REGISTER_READONLY_INCIDENCE(hkl_engine_e6c_incidence_new,
			    P99_PROTECT({MU, OMEGA, CHI, PHI}),
			    surface_parameters_y);

REGISTER_READONLY_EMERGENCE(hkl_engine_e6c_emergence_new,
			    P99_PROTECT({MU, OMEGA, CHI, PHI, GAMMA, DELTA}),
			    surface_parameters_y);

/*******/
/* E6C */
/*******/

#define HKL_GEOMETRY_EULERIAN6C_DESCRIPTION				\
	"+ xrays source fix allong the :math:`\\vec{x}` direction (1, 0, 0)\n" \
	"+ 4 axes for the sample\n"					\
	"\n"								\
	"  + **" MU "** : rotating around the :math:`\\vec{z}` direction (0, 0, 1)\n" \
	"  + **" OMEGA "** : rotating around the :math:`-\\vec{y}` direction (0, -1, 0)\n" \
	"  + **" CHI "** : rotating around the :math:`\\vec{x}` direction (1, 0, 0)\n" \
	"  + **" PHI "** : rotating around the :math:`-\\vec{y}` direction (0, -1, 0)\n" \
	"\n"								\
	"+ 2 axes for the detector\n"					\
	"\n"								\
	"  + **" GAMMA "** : rotation around the :math:`\\vec{z}` direction (0, 0, 1)\n" \
	"  + **" DELTA "** : rotation around the :math:`-\\vec{y}` direction (0, -1, 0)\n"

static const char* hkl_geometry_eulerian6C_axes[] = {MU, OMEGA, CHI, PHI, GAMMA, DELTA};

static HklGeometry *hkl_geometry_new_eulerian6C(const HklFactory *factory)
{
	HklGeometry *self = hkl_geometry_new(factory, &hkl_geometry_operations_defaults);
	HklHolder *h;

	h = hkl_geometry_add_holder(self);
	hkl_holder_add_rotation(h, MU, 0, 0, 1, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, OMEGA, 0, -1, 0, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, CHI, 1, 0, 0, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, PHI, 0, -1, 0, &hkl_unit_angle_deg);

	h = hkl_geometry_add_holder(self);
	hkl_holder_add_rotation(h, GAMMA, 0, 0, 1, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, DELTA, 0, -1, 0, &hkl_unit_angle_deg);

	return self;
}

static HklEngineList *hkl_engine_list_new_eulerian6C(const HklFactory *factory)
{
	HklEngineList *self = hkl_engine_list_new();

	hkl_engine_e6c_hkl_new(self);
	hkl_engine_e6c_psi_new(self);
	hkl_engine_q2_new(self);
	hkl_engine_qper_qpar_new(self);
	hkl_engine_tth2_new(self);
	hkl_engine_e6c_incidence_new(self);
	hkl_engine_e6c_emergence_new(self);

	return self;
}

REGISTER_DIFFRACTOMETER(eulerian6C, "E6C", HKL_GEOMETRY_EULERIAN6C_DESCRIPTION);
