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

#ifndef __HKL_GUI_MACROS_H__
#define __HKL_GUI_MACROS_H__

#include <gtk/gtk.h>

#include "hkl.h"
#include "hkl-gui-pseudoaxes.h"


/* TODO put in a private common header */
#define get_object(builder, type, priv, name) do{			\
		priv->name = type(gtk_builder_get_object(builder, #name)); \
		if(priv->name == NULL)					\
			fprintf(stdout, "%s is NULL ???", #name);	\
	}while(0);

#define get_model(filename)						\
	(0 == access(filename, R_OK)) ? filename :			\
	((0 == access("../data/" filename, R_OK)) ? "../data/" filename : \
	 ((0 == access(PKGDATA "/hkl3d/" filename, R_OK)) ? PKGDATA "/hkl3d/" filename : NULL))

static void get_ui(GtkBuilder *builder, const char *filename)
{
	if (0 == access(filename, R_OK))
		gtk_builder_add_from_file (builder, filename, NULL);
	else{
		char *ui = g_build_filename(PKGDATA, filename, NULL);
		gtk_builder_add_from_file (builder, ui, NULL);
		g_free(ui);
	}
}

#endif /* __HKL_GUI_MACROS_H__ */
