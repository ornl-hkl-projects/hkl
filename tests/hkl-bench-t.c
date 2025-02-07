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
 * Copyright (C) 2003-2019, 2021, 2023 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 */
#include <stdio.h>
#include <sys/time.h>
#include <tap/basic.h>
#include "hkl.h"

static void hkl_test_bench_run_real(HklEngine *engine, HklGeometry *geometry,
				    size_t n_values, double values[n_values],
                                    size_t n)
{
	size_t i;
	const darray_string *modes = hkl_engine_modes_names_get(engine);
	const char **mode;

	/* pseudo -> geometry */
	darray_foreach(mode, *modes){
		double min, max, mean;
		const darray_string *parameters;
		size_t n_params;

		if (FALSE == hkl_engine_current_mode_set(engine, *mode, NULL))
			continue;

		parameters = hkl_engine_parameters_names_get(engine);
		n_params = darray_size(*parameters);
		if (n_params){
			double params[n_params];

			hkl_engine_parameters_values_get(engine,
							 params, n_params,
							 HKL_UNIT_DEFAULT);
			values[0] = 1;
			if(FALSE == hkl_engine_parameters_values_set(engine,
								     params, n_params,
								     HKL_UNIT_DEFAULT, NULL))
				continue;
		}

		mean = max = 0;
		min = 1000; /* arbitrary value always greater than the real min */
		for(i=0; i<n; ++i){
			struct timeval debut, fin, dt;
			double t;
			HklGeometryList *solutions;

			if(FALSE == hkl_geometry_set_values_v(geometry, HKL_UNIT_USER, NULL, 0., 0., 0., 0., 10., 10.))
				break;
			gettimeofday(&debut, NULL);
			solutions = hkl_engine_pseudo_axis_values_set(engine, values, n_values, HKL_UNIT_DEFAULT, NULL);
			if (NULL != solutions)
				hkl_geometry_list_free(solutions);
			gettimeofday(&fin, NULL);
			timersub(&fin, &debut, &dt);
			t = dt.tv_sec * 1000. + dt.tv_usec / 1000.;
			min = t < min ? t : min;
			max = t > max ? t : max;
			mean += t;
		}
		fprintf(stdout, "\"%s\" \"%s\" \"%s\" (%zd/%zd) iterations %f / %f / %f [min/mean/max] ms each\n",
			hkl_geometry_name_get(geometry),
			hkl_engine_name_get(engine),
			*mode, n, i, min, mean/n, max);
	}
}

static void hkl_test_bench_run_v(HklEngineList *engines, HklGeometry *geometry,
				 char const *name, int n, size_t n_values, ...)
{
	va_list ap;
	size_t i;
	HklEngine *engine = hkl_engine_list_engine_get_by_name(engines, name, NULL);
	double values[n_values];

	va_start(ap, n_values);
	for(i=0; i<n_values; ++i)
		values[i] = va_arg(ap, double);
	va_end(ap);

	hkl_test_bench_run_real(engine, geometry, n_values, values, n);
}

static void hkl_test_bench_k6c(int n)
{
	const HklFactory *factory;
	HklEngineList *engines;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;

	factory = hkl_factory_get_by_name("K6C", NULL);

	geom = hkl_factory_create_new_geometry(factory);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);

	sample = hkl_sample_new("test");

	engines = hkl_factory_create_new_engine_list(factory);
	hkl_engine_list_init(engines, geom, detector, sample);

	hkl_test_bench_run_v(engines, geom, "hkl", n, 3, 1., 0., 0.);
	hkl_test_bench_run_v(engines, geom, "eulerians", n, 3, 0., 90*HKL_DEGTORAD, 0.);
	hkl_test_bench_run_v(engines, geom, "psi", n, 1, 10.*HKL_DEGTORAD);
	hkl_test_bench_run_v(engines, geom, "q2", n, 2, 1., 10.*HKL_DEGTORAD);
	hkl_test_bench_run_v(engines, geom, "qper_qpar", n, 2, 1., 1.);

	hkl_engine_list_free(engines);
	hkl_sample_free(sample);
	hkl_detector_free(detector);
	hkl_geometry_free(geom);
}

static void hkl_test_bench_eulerians(void)
{
	HklEngineList *engines;
	HklEngine *engine;
	const char **mode;
	const darray_string *modes;
	const HklFactory *factory;
	HklGeometry *geometry;
	HklDetector *detector;
	HklSample *sample;

	factory = hkl_factory_get_by_name("K6C", NULL);

	geometry = hkl_factory_create_new_geometry(factory);
	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);

	sample = hkl_sample_new("test");
	engines = hkl_factory_create_new_engine_list(factory);
	hkl_engine_list_init(engines, geometry, detector, sample);

	engine = hkl_engine_list_engine_get_by_name(engines, "eulerians", NULL);
	modes = hkl_engine_modes_names_get(engine);

	darray_foreach(mode, *modes){
		static double values[3] = {0, 90 * HKL_DEGTORAD, 0};
		HklGeometryList *solutions;

		if(FALSE == hkl_engine_current_mode_set(engine, *mode, NULL))
			continue;

		/* studdy this degenerated case */
		solutions = hkl_engine_pseudo_axis_values_set(engine,
							      values, ARRAY_SIZE(values),
							      HKL_UNIT_DEFAULT, NULL);
		if (solutions) {
			const HklGeometryListItem *item;

			HKL_GEOMETRY_LIST_FOREACH(item, solutions){
				hkl_geometry_set(geometry,
						 hkl_geometry_list_item_geometry_get(item));
				if(FALSE == hkl_engine_pseudo_axis_values_get(engine, values, ARRAY_SIZE(values),
									      HKL_UNIT_DEFAULT, NULL))
					break;
			}
			hkl_geometry_list_free(solutions);
		}
	}

	hkl_engine_list_free(engines);
	hkl_sample_free(sample);
	hkl_detector_free(detector);
	hkl_geometry_free(geometry);
}

int main(int argc, char **argv)
{
	int n;

	plan(1);

	if (argc > 1)
		n = atoi(argv[1]);
	else
		n = 10;

	hkl_test_bench_k6c(n);
	hkl_test_bench_eulerians();

	ok(TRUE == TRUE, __func__);

	return 0;
}
