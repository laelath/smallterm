/*
 * Copyright (c) 2018 Jason Waataja
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef MINITERM_SETTINGS_H
#define MINITERM_SETTINGS_H

#include <gdk/gdk.h>
#include <glib.h>
#include <stdbool.h>

#define MINITERM_COLOR_COUNT 16

typedef struct _MinitermSettings MinitermSettings;

struct _MinitermSettings {
	bool dynamic_window_title;
	bool urgent_on_bell;
	bool use_scrollbar;
	/* NULL indicates no user defined font. */
	char *font_name;

	/* Whether or not colors are valid. */
	bool has_colors;
	GdkRGBA fg_color;
	GdkRGBA bg_color;
	GdkRGBA color_palette[MINITERM_COLOR_COUNT];
};

bool miniterm_settings_init(MinitermSettings *settings, GKeyFile *config_file);
void miniterm_settings_destroy(MinitermSettings *settings);

#endif /* MINITERM_SETTINGS_H */
