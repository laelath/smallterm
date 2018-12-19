#include "settings.h"

#include "config.h"

static void miniterm_settings_default(MinitermSettings *settings);
static void miniterm_settings_set_colors(
	MinitermSettings *settings, GKeyFile *config_file);
static void config_file_get_bool(
	bool *dest, GKeyFile *config_file, char *group_name, char *key);

bool
miniterm_settings_init(MinitermSettings *settings, GKeyFile *config_file)
{
	miniterm_settings_default(settings);
	config_file_get_bool(&settings->dynamic_window_title, config_file,
		"Misc", "urgent-on-bell");
	config_file_get_bool(&settings->urgent_on_bell, config_file, "Misc",
		"urgent-on-bell");
	config_file_get_bool(&settings->use_scrollbar, config_file, "Misc",
		"urgent-on-bell");
	settings->font_name =
		g_key_file_get_string(config_file, "Font", "font", NULL);
	miniterm_settings_set_colors(settings, config_file);
	return true;
}

static void
miniterm_settings_default(MinitermSettings *settings)
{
	settings->dynamic_window_title = true;
	settings->urgent_on_bell = true;
	settings->use_scrollbar = false;
	settings->font_name = NULL;
	settings->has_colors = false;
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
