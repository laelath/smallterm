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

#include "settings.h"

#include <stdio.h>

#include "config.h"

static void miniterm_settings_set_colors(
	MinitermSettings *settings, GKeyFile *config_file);
static void config_file_get_bool(
	bool *dest, GKeyFile *config_file, char *group_name, char *key);
static void config_file_get_int(
	int *dest, GKeyFile *config_file, char *group_name, char *key);

void
miniterm_settings_init(MinitermSettings *settings)
{
	settings->dynamic_window_title = true;
	settings->urgent_on_bell = true;
	settings->use_scrollbar = false;
	settings->audible_bell = false;
	settings->scrollback_lines = MINITERM_DEFAULT_SCROLLBACK_LINES;
	settings->font_name = NULL;
	settings->has_colors = false;
}

bool
miniterm_settings_set_from_key_file(
	MinitermSettings *settings, GKeyFile *config_file)
{
	config_file_get_bool(&settings->dynamic_window_title, config_file,
		"Misc", "dynamic-window-title");
	config_file_get_bool(&settings->urgent_on_bell, config_file, "Misc",
		"urgent-on-bell");
	config_file_get_bool(
		&settings->use_scrollbar, config_file, "Misc", "use-scrollbar");
	config_file_get_int(&settings->scrollback_lines, config_file, "Misc",
		"use-scrollbar");
	if (settings->scrollback_lines < 0) {
		fprintf(stderr, "Invalid scrollback lines: %i\n",
			settings->scrollback_lines);
		settings->scrollback_lines = 0;
	}
	settings->font_name =
		g_key_file_get_string(config_file, "Font", "font", NULL);
	miniterm_settings_set_colors(settings, config_file);
	return true;
}

void
miniterm_settings_destroy(MinitermSettings *settings)
{
	g_free(settings->font_name);
}

static void
miniterm_settings_set_colors(MinitermSettings *settings, GKeyFile *config_file)
{
	settings->has_colors = false;
	char *fg_string = g_key_file_get_string(
		config_file, "Colors", "foreground", NULL);
	if (!fg_string)
		return;
	if (!gdk_rgba_parse(&settings->fg_color, fg_string)) {
		g_free(fg_string);
		return;
	}
	char *bg_string = g_key_file_get_string(
		config_file, "Colors", "background", NULL);
	if (!bg_string) {
		g_free(fg_string);
		return;
	}
	if (!gdk_rgba_parse(&settings->bg_color, bg_string)) {
		g_free(fg_string);
		g_free(bg_string);
		return;
	}
	for (int i = 0; i < MINITERM_COLOR_COUNT; ++i) {
		char key[8];
		snprintf(key, sizeof(key), "color%02x", i);
		char *cl_string =
			g_key_file_get_string(config_file, "Colors", key, NULL);
		if (!cl_string) {
			g_free(fg_string);
			g_free(bg_string);
			return;
		}
		if (!gdk_rgba_parse(&settings->color_palette[i], cl_string)) {
			g_free(fg_string);
			g_free(bg_string);
			g_free(cl_string);
			return;
		}
		g_free(cl_string);
	}
	g_free(fg_string);
	g_free(bg_string);
	settings->has_colors = true;
}

static void
config_file_get_bool(
	bool *dest, GKeyFile *config_file, char *group_name, char *key)
{
	GError *err = NULL;
	bool value = g_key_file_get_boolean(config_file, group_name, key, &err);
	if (err == NULL)
		*dest = value;
	else
		g_error_free(err);
}

static void
config_file_get_int(
	int *dest, GKeyFile *config_file, char *group_name, char *key)
{
	GError *err = NULL;
	int value = g_key_file_get_integer(config_file, group_name, key, &err);
	if (err == NULL)
		*dest = value;
	else
		g_error_free(err);
}

void
miniterm_write_default_settings(const char *config_path)
{
	FILE *file = fopen(config_path, "w");
	if (file) {
		fprintf(file, "[Font]\n#font=\n\n"
			      "[Colors]\n#foreground=\n#background=\n"
			      "#color00=\n#color01=\n#color02=\n#color03=\n"
			      "#color04=\n#color05=\n#color06=\n#color07=\n"
			      "#color08=\n#color09=\n#color0a=\n#color0b=\n"
			      "#color0c=\n#color0d=\n#color0e=\n#color0f=\n\n"
			      "[Misc]\n"
			      "#dynamic-window-title=\n"
			      "#urgent-on-bell=\n"
			      "#audible-bell=\n"
			      "#scrollback-lines=\n"
			      "#use-scrollbar=\n");
		fclose(file);
	}
}
