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
#include "hkl-pseudoaxis-common-eulerians-private.h"
#include "hkl-pseudoaxis-common-hkl-private.h"  // for RUBh_minus_Q, etc
#include "hkl-pseudoaxis-common-psi-private.h"  // for hkl_engine_psi_new, etc
#include "hkl-pseudoaxis-common-q-private.h"  // for hkl_engine_q2_new, etc
#include "hkl-pseudoaxis-common-readonly-private.h"
#include "hkl-pseudoaxis-common-tth-private.h"  // for hkl_engine_tth2_new, etc

#define MU "mu"
#define KOMEGA "komega"
#define KAPPA "kappa"
#define KPHI "kphi"
#define GAMMA "gamma"
#define DELTA "delta"

static void hkl_geometry_list_multiply_k6c_real(HklGeometryList *self,
						HklGeometryListItem *item)
{
	HklGeometry *geometry;
	HklGeometry *copy;
	double komega, komegap;
	double kappa, kappap;
	double kphi, kphip;

	geometry = item->geometry;
	komega = hkl_parameter_value_get(darray_item(geometry->axes, 1), HKL_UNIT_DEFAULT);
	kappa = hkl_parameter_value_get(darray_item(geometry->axes, 2), HKL_UNIT_DEFAULT);
	kphi = hkl_parameter_value_get(darray_item(geometry->axes, 3), HKL_UNIT_DEFAULT);

	kappa_2_kappap(komega, kappa, kphi, 50 * HKL_DEGTORAD, &komegap, &kappap, &kphip);

	copy = hkl_geometry_new_copy(geometry);
	/* TODO parameter list for the geometry */
	hkl_parameter_value_set(darray_item(copy->axes, 1), komegap, HKL_UNIT_DEFAULT, NULL);
	hkl_parameter_value_set(darray_item(copy->axes, 2), kappap, HKL_UNIT_DEFAULT, NULL);
	hkl_parameter_value_set(darray_item(copy->axes, 3), kphip, HKL_UNIT_DEFAULT, NULL);

	hkl_geometry_update(copy);
	hkl_geometry_list_add(self, copy);
	hkl_geometry_free(copy);
}


/***********************/
/* numerical functions */
/***********************/

static int _bissector_h_f1(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double mu = x->data[0];
	const double komega = x->data[1];
	const double kappa = x->data[2];
	const double gamma = x->data[4];
	double omega;

	CHECK_NAN(x->data, x->size);

	omega = komega + atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD)) - M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = fmod(omega, M_PI);
	f->data[4] = fmod(gamma - 2 * fmod(mu, M_PI), 2*M_PI);

	return  GSL_SUCCESS;
}

static const HklFunction bissector_h_f1 = {
	.function = _bissector_h_f1,
	.size = 5,
};

static int _bissector_h_f2(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double mu = x->data[0];
	const double komega = x->data[1];
	const double kappa = x->data[2];
	const double gamma = x->data[4];
	double omega;

	CHECK_NAN(x->data, x->size);

	omega = komega + atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD)) + M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = fmod(omega, M_PI);
	f->data[4] = fmod(gamma - 2 * fmod(mu, M_PI), 2*M_PI);


	return  GSL_SUCCESS;
}

static const HklFunction bissector_h_f2 = {
	.function = _bissector_h_f2,
	.size = 5,
};

static int _constant_kphi_h_f1(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double komega = x->data[1];
	const double kappa = x->data[2];
	double omega;

	CHECK_NAN(x->data, x->size);

	omega = komega + atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD)) - M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = fmod(omega, M_PI);

	return  GSL_SUCCESS;
}

static const HklFunction constant_kphi_h_f1 = {
	.function = _constant_kphi_h_f1,
	.size = 4,
};

static int _constant_kphi_h_f2(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double komega = x->data[1];
	const double kappa = x->data[2];
	double omega;

	CHECK_NAN(x->data, x->size);

	omega = komega + atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD)) + M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = fmod(omega, M_PI);

	return  GSL_SUCCESS;
}

static const HklFunction constant_kphi_h_f2 = {
	.function = _constant_kphi_h_f2,
	.size = 4,
};

static int _constant_phi_h_f1(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double komega = x->data[1];
	const double kappa = x->data[2];
	const double kphi = x->data[3];
	double omega, phi, p;

	CHECK_NAN(x->data, x->size);

	p = atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD));
	omega = komega + p - M_PI_2;
	phi = kphi + p + M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = fmod(omega, M_PI);
	f->data[4] = phi;

	return  GSL_SUCCESS;
}

static const HklFunction constant_phi_h_f1 = {
	.function = _constant_phi_h_f1,
	.size = 5,
};

static int _constant_phi_h_f2(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double komega = x->data[1];
	const double kappa = x->data[2];
	const double kphi = x->data[3];
	double omega, phi, p;

	CHECK_NAN(x->data, x->size);

	p = atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD));
	omega = komega + p + M_PI_2;
	phi = kphi + p - M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = fmod(omega, M_PI);
	f->data[4] = phi;

	return  GSL_SUCCESS;
}

static const HklFunction constant_phi_h_f2 = {
	.function = _constant_phi_h_f2,
	.size = 5,
};

static int _bissector_v(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double komega = x->data[0];
	const double kappa = x->data[1];
	const double delta = x->data[3];
	double omega;

	CHECK_NAN(x->data, x->size);

	omega = komega + atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD)) - M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = fmod(delta - 2 * fmod(omega, M_PI), 2*M_PI);

	return  GSL_SUCCESS;
}

static const HklFunction bissector_v = {
	.function = _bissector_v,
	.size = 4,
};

static int _constant_omega_v(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double komega = x->data[0];
	const double kappa = x->data[1];
	double omega;
	HklEngine *engine = params;
	double omega0 = darray_item(engine->mode->parameters, 0)->_value;

	CHECK_NAN(x->data, x->size);

	omega = komega + atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD)) - M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = omega0 - omega;

	return  GSL_SUCCESS;
}

static const HklFunction constant_omega_v = {
	.function = _constant_omega_v,
	.size = 4,
};

static int _constant_chi_v(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double kappa = x->data[1];
	double chi;
	HklEngine *engine = params;
	double chi0 = darray_item(engine->mode->parameters, 0)->_value;

	CHECK_NAN(x->data, x->size);

	chi = 2 * asin(sin(kappa/2.) * sin(50 * HKL_DEGTORAD));

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = chi0 - chi;

	return  GSL_SUCCESS;
}

static const HklFunction constant_chi_v = {
	.function = _constant_chi_v,
	.size = 4,
};

static int _constant_phi_v(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double kappa = x->data[1];
	const double kphi = x->data[2];
	double phi;
	HklEngine *engine = params;
	double phi0 = darray_item(engine->mode->parameters, 0)->_value;

	CHECK_NAN(x->data, x->size);

	phi = kphi + atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD)) + M_PI_2;

	RUBh_minus_Q(x->data, params, f->data);
	f->data[3] = phi0 - phi;

	return  GSL_SUCCESS;
}

static const HklFunction constant_phi_v = {
	.function = _constant_phi_v,
	.size = 4,
};

static int _double_diffraction_h(const gsl_vector *x, void *params, gsl_vector *f)
{
	const double komega = x->data[1];
	const double kappa = x->data[2];
	double omega;

	CHECK_NAN(x->data, x->size);

	omega = komega + atan(tan(kappa/2.)*cos(50 * HKL_DEGTORAD)) - M_PI_2;

	_double_diffraction(x->data, params, f->data);
	f->data[4] = fmod(omega, M_PI);

	return  GSL_SUCCESS;
}

static const HklFunction double_diffraction_h = {
	.function = _double_diffraction_h,
	.size = 5,
};

static int _constant_incidence_func(const gsl_vector *x, void *params, gsl_vector *f)
{
	static const HklVector Y = {
		.data = {0, 1, 0},
	};
	double incidence;
	double azimuth;
	HklEngine *engine = params;
	HklModeAutoWithInit *mode = container_of(engine->mode, HklModeAutoWithInit, mode);
	HklVector n;
	double incidence0;
	double azimuth0;
	HklVector ki;

	CHECK_NAN(x->data, x->size);

	RUBh_minus_Q(x->data, params, f->data);

	/* get the mode parameters */
	n.data[0] = darray_item(engine->mode->parameters, 0)->_value;
	n.data[1] = darray_item(engine->mode->parameters, 1)->_value;
	n.data[2] = darray_item(engine->mode->parameters, 2)->_value;
	incidence0 = darray_item(engine->mode->parameters, 3)->_value;
	azimuth0 = darray_item(engine->mode->parameters, 4)->_value;

	/* compute the two angles */


	/* first check that the mode was already initialized if not
	 * the surface is oriented along the nx, ny, nz axis for all
	 * diffractometer angles equal to zero */
	if(mode->geometry){
		HklQuaternion q0 = hkl_geometry_sample_holder_get(mode->geometry, mode->sample)->q;

		hkl_quaternion_conjugate(&q0);
		hkl_vector_rotated_quaternion(&n, &q0);
	}

	hkl_vector_rotated_quaternion(&n, &hkl_geometry_sample_holder_get(engine->geometry,
									  mode->sample)->q);

	ki = hkl_geometry_ki_get(engine->geometry);
	incidence = _incidence(&n, &ki);

	hkl_vector_project_on_plan(&n, &ki);
	azimuth = hkl_vector_angle(&n, &Y);

	f->data[3] = incidence0 - incidence;
	f->data[4] = azimuth0 - azimuth;

	return  GSL_SUCCESS;
}

static const HklFunction constant_incidence_func = {
	.function = _constant_incidence_func,
	.size = 5,
};

/*******/
/* K6C */
/*******/

/************/
/* hkl mode */
/************/

static HklMode *bissector_vertical(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&bissector_v};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_omega_vertical(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&constant_omega_v};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, constant_omega_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_chi_vertical(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&constant_chi_v};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, constant_chi_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_phi_vertical(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&constant_phi_v};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, constant_phi_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *lifting_detector_kphi(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KPHI, GAMMA, DELTA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *lifting_detector_komega(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KOMEGA, GAMMA, DELTA};
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
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
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
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, DELTA};
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
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&bissector_h_f1, &bissector_h_f2};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_phi_horizontal(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&constant_phi_h_f1, &constant_phi_h_f2};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, constant_phi_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_kphi_horizontal(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {MU, KOMEGA, KAPPA, GAMMA};
	static const HklFunction *functions[] = {&constant_kphi_h_f1, &constant_kphi_h_f2};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *double_diffraction_horizontal(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&double_diffraction_h};
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
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&psi_constant_vertical_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, psi_constant_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &psi_constant_vertical_mode_operations,
				 TRUE);
}

static HklMode *constant_incidence(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const HklFunction *functions[] = {&constant_incidence_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, constant_incidence_parameters),
	};

	return hkl_mode_auto_with_init_new(&info,
					   &constant_incidence_mode_operations,
					   TRUE);
}

static HklEngine *hkl_engine_k6c_hkl_new(HklEngineList *engines)
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
	hkl_engine_add_mode(self, lifting_detector_kphi());
	hkl_engine_add_mode(self, lifting_detector_komega());
	hkl_engine_add_mode(self, lifting_detector_mu());
	hkl_engine_add_mode(self, double_diffraction_vertical());
	hkl_engine_add_mode(self, bissector_horizontal());
	hkl_engine_add_mode(self, constant_phi_horizontal());
	hkl_engine_add_mode(self, constant_kphi_horizontal());
	hkl_engine_add_mode(self, double_diffraction_horizontal());
	hkl_engine_add_mode(self, psi_constant_vertical());
	hkl_engine_add_mode(self, constant_incidence());

	return self;
}

/************/
/* psi mode */
/************/

static HklMode *psi_vertical(void)
{
	static const char *axes_r[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};
	static const char *axes_w[] = {KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&psi_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, psi_parameters),
	};

	return hkl_mode_psi_new(&info);
}

static HklEngine *hkl_engine_k6c_psi_new(HklEngineList *engines)
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

REGISTER_READONLY_INCIDENCE(hkl_engine_k6c_incidence_new,
			    P99_PROTECT({MU, KOMEGA, KAPPA, KPHI}),
			    surface_parameters_y);

REGISTER_READONLY_EMERGENCE(hkl_engine_k6c_emergence_new,
			    P99_PROTECT({MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA}),
			    surface_parameters_y);

#define HKL_GEOMETRY_KAPPA6C_DESCRIPTION				\
	"For this geometry there is a special parameters called :math:`\\alpha` which is the\n" \
	"angle between the kappa rotation axis and the  :math:`\\vec{y}` direction.\n" \
	"\n"								\
	"+ xrays source fix allong the :math:`\\vec{x}` direction (1, 0, 0)\n" \
	"+ 4 axes for the sample\n"					\
	"\n"								\
	"  + **mu** : rotating around the :math:`\\vec{z}` direction (0, 0, 1)\n" \
	"  + **komega** : rotating around the :math:`-\\vec{y}` direction (0, -1, 0)\n" \
	"  + **kappa** : rotating around the :math:`\\vec{x}` direction (0, :math:`-\\cos\\alpha`, :math:`-\\sin\\alpha`)\n" \
	"  + **kphi** : rotating around the :math:`-\\vec{y}` direction (0, -1, 0)\n" \
	"\n"								\
	"+ 2 axes for the detector\n"					\
	"\n"								\
	"  + **gamma** : rotation around the :math:`\\vec{z}` direction (0, 0, 1)\n" \
	"  + **delta** : rotation around the :math:`-\\vec{y}` direction (0, -1, 0)\n"

static const char* hkl_geometry_kappa6C_axes[] = {MU, KOMEGA, KAPPA, KPHI, GAMMA, DELTA};

static HklGeometry *hkl_geometry_new_kappa6C(const HklFactory *factory)
{
	HklGeometry *self = hkl_geometry_new(factory, &hkl_geometry_operations_defaults);
	double alpha = 50 * HKL_DEGTORAD;
	HklHolder *h;

	h = hkl_geometry_add_holder(self);
	hkl_holder_add_rotation(h, MU, 0, 0, 1, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, KOMEGA, 0, -1, 0, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, KAPPA, 0, -cos(alpha), -sin(alpha), &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, KPHI, 0, -1, 0, &hkl_unit_angle_deg);

	h = hkl_geometry_add_holder(self);
	hkl_holder_add_rotation(h, GAMMA, 0, 0, 1, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, DELTA, 0, -1, 0, &hkl_unit_angle_deg);

	return self;
}

static HklEngineList *hkl_engine_list_new_kappa6C(const HklFactory *factory)
{
	HklEngineList *self = hkl_engine_list_new();

	self->geometries->multiply = hkl_geometry_list_multiply_k6c_real;
	hkl_engine_k6c_hkl_new(self);
	hkl_engine_eulerians_new(self);
	hkl_engine_k6c_psi_new(self);
	hkl_engine_q2_new(self);
	hkl_engine_qper_qpar_new(self);
	hkl_engine_k6c_incidence_new(self);
	hkl_engine_tth2_new(self);
	hkl_engine_k6c_emergence_new(self);

	return self;
}

REGISTER_DIFFRACTOMETER(kappa6C, "K6C", HKL_GEOMETRY_KAPPA6C_DESCRIPTION);

/***********************/
/* SOLEIL sirius kappa */
/***********************/

/* hkl mode */

static HklMode *bissector_vertical_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&bissector_v};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO("bissector_vertical", axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_omega_vertical_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&constant_omega_v};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS("constant_omega_vertical", axes_r, axes_w,
					       functions, constant_omega_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_chi_vertical_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&constant_chi_v};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS("constant_chi_vertical", axes_r, axes_w,
					       functions, constant_chi_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_phi_vertical_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&constant_phi_v};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS("constant_phi_vertical", axes_r, axes_w,
					       functions, constant_phi_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *lifting_detector_kphi_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KPHI, DELTA, GAMMA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO("lifting_detector_kphi", axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *lifting_detector_komega_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KOMEGA, DELTA, GAMMA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO("lifting_detector_komega", axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *lifting_detector_mu_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {MU, DELTA, GAMMA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO("lifting_detector_mu", axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *double_diffraction_vertical_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&double_diffraction_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS("double_diffraction_vertical", axes_r, axes_w,
					       functions, double_diffraction_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *bissector_horizontal_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {MU, KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&bissector_h_f1, &bissector_h_f2};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO("bissector_horizontal", axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_phi_horizontal_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {MU, KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&constant_phi_h_f1, &constant_phi_h_f2};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS("constant_phi_horizontal", axes_r, axes_w,
					       functions, constant_phi_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *constant_kphi_horizontal_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {MU, KOMEGA, KAPPA, DELTA};
	static const HklFunction *functions[] = {&constant_kphi_h_f1, &constant_kphi_h_f2};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO("constant_kphi_horizontal", axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *double_diffraction_horizontal_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {MU, KOMEGA, KAPPA, KPHI, DELTA};
	static const HklFunction *functions[] = {&double_diffraction_h};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS("double_diffraction_horizontal", axes_r, axes_w,
					       functions, double_diffraction_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_mode_operations,
				 TRUE);
}

static HklMode *psi_constant_vertical_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&psi_constant_vertical_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS("psi_constant_vertical", axes_r, axes_w,
					       functions, psi_constant_parameters),
	};

	return hkl_mode_auto_new(&info,
				 &psi_constant_vertical_mode_operations,
				 TRUE);
}

static HklMode *constant_incidence_soleil_sirius_kappa(void)
{
	static const char* axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char* axes_w[] = {KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const HklFunction *functions[] = {&constant_incidence_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS("constant_incidence", axes_r, axes_w,
					       functions, constant_incidence_parameters),
	};

	return hkl_mode_auto_with_init_new(&info,
					   &constant_incidence_mode_operations,
					   TRUE);
}

static HklEngine *hkl_engine_soleil_sirius_kappa_hkl_new(HklEngineList *engines)
{
	HklEngine *self;
	HklMode *default_mode;

	self = hkl_engine_hkl_new(engines);

	default_mode = bissector_vertical_soleil_sirius_kappa();
	hkl_engine_add_mode(self, default_mode);
	hkl_engine_mode_set(self, default_mode);

	hkl_engine_add_mode(self, constant_omega_vertical_soleil_sirius_kappa());
	hkl_engine_add_mode(self, constant_chi_vertical_soleil_sirius_kappa());
	hkl_engine_add_mode(self, constant_phi_vertical_soleil_sirius_kappa());
	hkl_engine_add_mode(self, lifting_detector_kphi_soleil_sirius_kappa());
	hkl_engine_add_mode(self, lifting_detector_komega_soleil_sirius_kappa());
	hkl_engine_add_mode(self, lifting_detector_mu_soleil_sirius_kappa());
	hkl_engine_add_mode(self, double_diffraction_vertical_soleil_sirius_kappa());
	hkl_engine_add_mode(self, bissector_horizontal_soleil_sirius_kappa());
	hkl_engine_add_mode(self, constant_phi_horizontal_soleil_sirius_kappa());
	hkl_engine_add_mode(self, constant_kphi_horizontal_soleil_sirius_kappa());
	hkl_engine_add_mode(self, double_diffraction_horizontal_soleil_sirius_kappa());
	hkl_engine_add_mode(self, psi_constant_vertical_soleil_sirius_kappa());
	hkl_engine_add_mode(self, constant_incidence_soleil_sirius_kappa());

	return self;
}

/* psi mode */

static HklMode *psi_vertical_soleil_sirius_kappa()
{
	static const char *axes_r[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};
	static const char *axes_w[] = {KOMEGA, KAPPA, KPHI, GAMMA};
	static const HklFunction *functions[] = {&psi_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO_WITH_PARAMS(__func__, axes_r, axes_w,
					       functions, psi_parameters),
	};

	return hkl_mode_psi_new(&info);
}

static HklEngine *hkl_engine_soleil_sirius_kappa_psi_new(HklEngineList *engines)
{
	HklEngine *self;
	HklMode *default_mode;

	self = hkl_engine_psi_new(engines);

	default_mode = psi_vertical_soleil_sirius_kappa();
	hkl_engine_add_mode(self, default_mode);
	hkl_engine_mode_set(self, default_mode);

	return self;
}

/* SOLEIL SIRIUS KAPPA */

#define HKL_GEOMETRY_TYPE_SOLEIL_SIRIUS_KAPPA_DESCRIPTION		\
	"+ xrays source fix along the :math:`\\vec{x}` direction (1, 0, 0)\n" \
	"+ 4 axes for the sample\n"					\
	"\n"								\
	"  + **" MU "** : rotating around the :math:`-\\vec{z}` direction (0, 0, -1)\n" \
	"  + **" KOMEGA "** : rotating around the :math:`-\\vec{y}` direction (0, -1, 0)\n" \
	"  + **" KAPPA "** : rotating around the :math:`\\vec{x}` direction (0, :math:`-\\cos\\alpha`, :math:`-\\sin\\alpha`)\n" \
	"  + **" KPHI "** : rotating around the :math:`-\\vec{y}` direction (0, -1, 0)\n" \
	"\n"								\
	"+ 2 axes for the detector\n"					\
	"\n"								\
	"  + **" DELTA "** : rotation around the :math:`-\\vec{z}` direction (0, 0, -1)\n" \
	"  + **" GAMMA "** : rotation around the :math:`-\\vec{y}` direction (0, -1, 0)\n"

static const char* hkl_geometry_soleil_sirius_kappa_axes[] = {MU, KOMEGA, KAPPA, KPHI, DELTA, GAMMA};

static HklGeometry *hkl_geometry_new_soleil_sirius_kappa(const HklFactory *factory)
{
	HklGeometry *self = hkl_geometry_new(factory, &hkl_geometry_operations_defaults);
	double alpha = 50 * HKL_DEGTORAD;
	HklHolder *h;

	h = hkl_geometry_add_holder(self);
	hkl_holder_add_rotation(h, MU, 0, 0, -1, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, KOMEGA, 0, -1, 0, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, KAPPA, 0, -cos(alpha), -sin(alpha), &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, KPHI, 0, -1, 0, &hkl_unit_angle_deg);

	h = hkl_geometry_add_holder(self);
	hkl_holder_add_rotation(h, DELTA, 0, 0, -1, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, GAMMA, 0, -1, 0, &hkl_unit_angle_deg);

	return self;
}

static HklEngineList *hkl_engine_list_new_soleil_sirius_kappa(const HklFactory *factory)
{
	HklEngineList *self = hkl_engine_list_new();

	self->geometries->multiply = hkl_geometry_list_multiply_k6c_real;
	hkl_engine_soleil_sirius_kappa_hkl_new(self);
	hkl_engine_eulerians_new(self);
	hkl_engine_soleil_sirius_kappa_psi_new(self);
	hkl_engine_q2_new(self);
	hkl_engine_qper_qpar_new(self);
	hkl_engine_tth2_new(self);
	hkl_engine_k6c_incidence_new(self);
	hkl_engine_k6c_emergence_new(self);

	return self;
}

REGISTER_DIFFRACTOMETER(soleil_sirius_kappa, "SOLEIL SIRIUS KAPPA", HKL_GEOMETRY_TYPE_SOLEIL_SIRIUS_KAPPA_DESCRIPTION);
