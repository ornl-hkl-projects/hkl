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
#include "hkl-factory-private.h"        // for autodata_factories_, etc
#include "hkl-pseudoaxis-common-hkl-private.h"  // for hkl_engine_hkl_new, etc
#include "hkl-pseudoaxis-common-q-private.h"  // for hkl_engine_q2_new, etc
#include "hkl-pseudoaxis-common-tth-private.h"  // for hkl_engine_tth2_new, etc
#include "hkl-pseudoaxis-common-readonly-private.h"

#define BASEPITCH "basepitch"
#define THETAH "thetah"
#define ALPHAY "alphay"
#define ALPHAX "alphax"
#define DELTA "delta"
#define GAMMA "gamma"

/************/
/* mode hkl */
/************/

static HklMode* lifting_detector_thetah()
{
	static const char *axes_r[] = {BASEPITCH, THETAH, ALPHAY, ALPHAX, DELTA, GAMMA};
	static const char* axes_w[] = {THETAH, DELTA, GAMMA};
	static const HklFunction *functions[] = {&RUBh_minus_Q_func};
	static const HklModeAutoInfo info = {
		HKL_MODE_AUTO_INFO(__func__, axes_r, axes_w, functions),
	};

	return hkl_mode_auto_new(&info,
				 &hkl_full_mode_operations,
				 TRUE);
}

static HklEngine *hkl_engine_soleil_sirius_turret_hkl_new(HklEngineList *engines)
{
	HklEngine *self;
	HklMode *default_mode;

	self = hkl_engine_hkl_new(engines);

	default_mode = lifting_detector_thetah();
	hkl_engine_add_mode(self, default_mode);
	hkl_engine_mode_set(self, default_mode);

	return self;
}

/*****************/
/* mode readonly */
/*****************/

REGISTER_READONLY_INCIDENCE(hkl_engine_soleil_sirius_turret_incidence_new,
			    P99_PROTECT({BASEPITCH, THETAH, ALPHAY, ALPHAX}),
			    surface_parameters_z);

REGISTER_READONLY_EMERGENCE(hkl_engine_soleil_sirius_turret_emergence_new,
			    P99_PROTECT({BASEPITCH, THETAH, ALPHAY, ALPHAX, DELTA, GAMMA}),
			    surface_parameters_z);

/************************/
/* SOLEIL SIRIUS TURRET */
/************************/

#define HKL_GEOMETRY_TYPE_SOLEIL_SIRIUS_TURRET_DESCRIPTION		\
	"+ xrays source fix allong the :math:`\\vec{x}` direction (1, 0, 0)\n" \
	"+ 4 axes for the sample\n"					\
	"\n"								\
	"  + **" BASEPITCH "** : rotation around the :math:`\\vec{y}` direction (0, 1, 0)\n" \
	"  + **" THETAH "** : rotation around the :math:`-\\vec{z}` direction (0, 0, -1)\n" \
	"  + **" ALPHAY "** : rotation around the :math:`\\vec{y}` direction (0, 1, 0)\n" \
	"  + **" ALPHAX "** : rotating around the :math:`\\vec{x}` direction (1, 0, 0)\n" \
	"\n"								\
	"+ 3 axis for the detector\n"					\
	"\n"								\
	"  + **" BASEPITCH "** : rotation around the :math:`\\vec{y}` direction (0, 1, 0)\n" \
	"  + **" DELTA "** : rotation around the :math:`-\\vec{y}` direction (0, 0, -1)\n" \
	"  + **" GAMMA "** : rotation around the :math:`\\vec{z}` direction (0, -1, 0)\n"

static const char* hkl_geometry_soleil_sirius_turret_axes[] = {BASEPITCH, THETAH, ALPHAY, ALPHAX, DELTA, GAMMA};

static HklGeometry *hkl_geometry_new_soleil_sirius_turret(const HklFactory *factory)
{
	HklGeometry *self = hkl_geometry_new(factory, &hkl_geometry_operations_defaults);
	HklHolder *h;

	h = hkl_geometry_add_holder(self);
	hkl_holder_add_rotation(h, BASEPITCH, 0, 1, 0, &hkl_unit_angle_mrad);
	hkl_holder_add_rotation(h, THETAH, 0, 0, -1, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, ALPHAY, 0, 1, 0, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, ALPHAX, 1, 0, 0, &hkl_unit_angle_deg);

	h = hkl_geometry_add_holder(self);
	hkl_holder_add_rotation(h, BASEPITCH, 0, 1, 0, &hkl_unit_angle_mrad);
	hkl_holder_add_rotation(h, DELTA, 0, 0, -1, &hkl_unit_angle_deg);
	hkl_holder_add_rotation(h, GAMMA, 0, -1, 0, &hkl_unit_angle_deg);

	return self;
}

static HklEngineList *hkl_engine_list_new_soleil_sirius_turret(const HklFactory *factory)
{
	HklEngineList *self = hkl_engine_list_new();

	hkl_engine_soleil_sirius_turret_hkl_new(self);
	hkl_engine_q2_new(self);
	hkl_engine_qper_qpar_new(self);
	hkl_engine_tth2_new(self);
	hkl_engine_soleil_sirius_turret_incidence_new(self);
	hkl_engine_soleil_sirius_turret_emergence_new(self);

	return self;
}

REGISTER_DIFFRACTOMETER(soleil_sirius_turret, "SOLEIL SIRIUS TURRET", HKL_GEOMETRY_TYPE_SOLEIL_SIRIUS_TURRET_DESCRIPTION);
