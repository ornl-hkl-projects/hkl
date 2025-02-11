/* This file is part of the hkl3d library.
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
 * Copyright (C) 2010-2019 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 *          Oussama Sboui <sboui@synchrotron-soleil.fr>
 */
#include <string.h>

#include "hkl3d.h"
#include "tap/basic.h"
#include "tap/hkl-tap.h"

#define MODEL_FILENAME "data/diffabs.yaml"

static void check_model_validity(Hkl3D *hkl3d)
{
	uint i, j;
	uint len;
	int res;
	Hkl3DObject *obji;
	Hkl3DObject *objj;

	res = TRUE;

	/* imported 1 config files with 7 Hkl3DObjects */
	res &= DIAG(hkl3d->config->len == 1);
	res &= DIAG(hkl3d->config->models[0]->len == 7);

	/* all Hkl3DObjects must have a different axis_name */
	len = hkl3d->config->models[0]->len;
	for(i=0;i<len; ++i){
		obji = hkl3d->config->models[0]->objects[i];
		for (j=1; j<len-i; ++j){
			objj = hkl3d->config->models[0]->objects[i+j];
			if(!(strcmp(obji->axis_name, objj->axis_name))){
				res &= DIAG(FALSE);
				break;
			}
		}
		obji++;
	}

	/* check the _movingObjects validity, all Hkl3DAxis must have a size of 1 */
	for(i=0; i<6; ++i)
		res &= DIAG(hkl3d->geometry->axes[i]->len == 1);

	ok(res == TRUE, "no identical objects");
}

/* check the collision and that the right axes are colliding */
static void check_collision(Hkl3D *hkl3d)
{
	char buffer[1000];
	int res = TRUE;
	double values[] = {23, 0., 0., 0., 0., 0.};

	/* check the collision and that the right axes are colliding */
	res &= DIAG(hkl_geometry_axis_values_set(hkl3d->geometry->geometry,
						 values, ARRAY_SIZE(values),
						 HKL_UNIT_USER, NULL));

	res &= DIAG(hkl3d_is_colliding(hkl3d) == TRUE);
	strcpy(buffer, "");

	/* now check that only delta and mu are colliding */
	for(size_t i=0; i<hkl3d->config->models[0]->len; ++i){
		const char *name;
		int tmp;

		name = hkl3d->config->models[0]->objects[i]->axis_name;
		tmp = hkl3d->config->models[0]->objects[i]->is_colliding == TRUE;
		/* add the colliding axes to the buffer */
		if(tmp){
			strcat(buffer, " ");
			strcat(buffer, name);
		}

		if(!strcmp(name, "mu") || !strcmp(name, "delta"))
			res &= DIAG(tmp == TRUE);
		else
			res &= DIAG(tmp == FALSE);
	}
	ok(res == TRUE,  "collision [%s]", buffer);
}

static void check_no_collision(Hkl3D *hkl3d)
{
	int res = TRUE;
	double i;

	/* check that rotating around komega/kappa/kphi do not create collisison */
	res &= DIAG(hkl_geometry_set_values_v(hkl3d->geometry->geometry,
					      HKL_UNIT_USER, NULL,
					      0., 0., 0., 0., 0., 0.));
	/* komega */
	for(i=0; i<=360; i=i+10){
		double values[] = {0., i, 0., 0., 0., 0.};

		res &= DIAG(hkl_geometry_axis_values_set(hkl3d->geometry->geometry,
							 values, ARRAY_SIZE(values),
							 HKL_UNIT_USER, NULL));
		res &= DIAG(hkl3d_is_colliding(hkl3d) == FALSE);
	}

	/* kappa */
	for(i=0; i<=360; i=i+10){
		double values[] = {0., 0., i, 0., 0., 0.};

		res &= DIAG(hkl_geometry_axis_values_set(hkl3d->geometry->geometry,
							 values, ARRAY_SIZE(values),
							 HKL_UNIT_USER, NULL));
		res &= DIAG(hkl3d_is_colliding(hkl3d) == FALSE);
	}

	/* kphi */
	for(i=0; i<=360; i=i+10){
		double values[] = {0., 0., 0., i, 0., 0.};

		res &= DIAG(hkl_geometry_axis_values_set(hkl3d->geometry->geometry,
							 values, ARRAY_SIZE(values),
							 HKL_UNIT_USER, NULL));
		res &= DIAG(hkl3d_is_colliding(hkl3d) == FALSE);
	}
	ok(res == TRUE, "no-collision");
}

int main(void)
{
	char* filename;
	const HklFactory *factory;
	HklGeometry *geometry;
	Hkl3D *hkl3d;

	factory = hkl_factory_get_by_name("K6C", NULL);
	geometry = hkl_factory_create_new_geometry(factory);

	/* compute the filename of the diffractometer config file */
	filename  = test_file_path(MODEL_FILENAME);
	hkl3d = hkl3d_new(filename, geometry);

	plan(3);
	check_model_validity(hkl3d);
	check_collision(hkl3d);
	check_no_collision(hkl3d);
	/* TODO add/remove object*/

	hkl3d_free(hkl3d);
	test_file_path_free(filename);
	hkl_geometry_free(geometry);

	return 0;
}
