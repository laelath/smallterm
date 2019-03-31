/*
 * MIT/X Consortium License
 *
 * © 2017 Justin Frank
 * © 2013 Jakub Klinkovský
 * © 2009 Sebastian Linke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <stdlib.h>
#include <vte/vte.h>

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif

#include "config.h"
#include "terminal.h"

static gboolean vte_spawn(VteTerminal *vte,
	GApplicationCommandLine *command_line, char *working_directory,
	char *command, char **environment);
/* Callback to exit miniterm with exit status of child process. */
static void window_close(GtkWindow *window, gint status, gpointer user_data);
static gboolean parse_arguments(GApplicationCommandLine *command_line, int argc,
	char *argv[], char **command, char **directory, gboolean *keep,
	char **title);
static void signal_handler(int signal);
static void new_window(GtkApplication *app,
	GApplicationCommandLine *command_line, gchar **argv, gint argc);
static void command_line(GApplication *app,
	GApplicationCommandLine *command_line, gpointer user_data);
static void set_geometry_hints(VteTerminal *vte, GdkGeometry *hints);

/* The application is global for use with signal handlers. */
static GApplication *_application = NULL;

static gboolean
vte_spawn(VteTerminal *vte, GApplicationCommandLine *command_line,
	char *working_directory, char *command, char **environment)
{
	GError *error = NULL;
	char **command_argv = NULL;
	/* Parse command into array */
	if (!command)
		command = vte_get_user_shell();
	g_shell_parse_argv(command, NULL, &command_argv, &error);
	if (error != NULL) {
		g_application_command_line_printerr(command_line,
			"Failed to parse command: %s\n", error->message);
		g_error_free(error);
		g_application_command_line_set_exit_status(
			command_line, EXIT_FAILURE);
		return FALSE;
	}
	/* Create pty object */
	VtePty *pty =
		vte_terminal_pty_new_sync(vte, VTE_PTY_NO_HELPER, NULL, &error);
	if (error) {
		g_application_command_line_printerr(command_line,
			"Failed to create pty: %s\n", error->message);
		g_error_free(error);
		g_application_command_line_set_exit_status(
			command_line, EXIT_FAILURE);
		return FALSE;
	}
	vte_terminal_set_pty(vte, pty);
	int child_pid;
	/* Spawn default shell (or specified command). */
	g_spawn_async(working_directory, command_argv, environment,
		(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH
			| G_SPAWN_LEAVE_DESCRIPTORS_OPEN), // flags from
							   // GSpawnFlags
		(GSpawnChildSetupFunc)
			vte_pty_child_setup, // an extra child setup function to
					     // run in the child just before
					     // exec()
		pty,	// user data for child_setup
		&child_pid, // a location to store the child PID
		&error);    // return location for a GError
	if (error) {
		g_application_command_line_printerr(
			command_line, "%s\n", error->message);
		g_error_free(error);
		g_application_command_line_set_exit_status(
			command_line, EXIT_FAILURE);
		return FALSE;
	}
	vte_terminal_watch_child(vte, child_pid);
	g_strfreev(command_argv);
	return TRUE;
}

static void
window_close(GtkWindow *window, gint status, gpointer user_data)
{
	(void)window;
	(void)status;
	GtkApplication *app = (GtkApplication *)user_data;
	int count = 0;
	GList *windows = gtk_application_get_windows(app);
	while (windows != NULL) {
		windows = windows->next;
		++count;
	}
	if (count == 1)
		g_application_quit(G_APPLICATION(app));
}

static gboolean
parse_arguments(GApplicationCommandLine *command_line, int argc, char *argv[],
	char **command, char **directory, gboolean *keep, char **title)
{
	gboolean version = FALSE; /* Show version? */
	gboolean help = FALSE;
	const GOptionEntry entries[] = {
		{"version", 'v', 0, G_OPTION_ARG_NONE, &version,
			"Display program version and exit.", 0},
		{"execute", 'e', 0, G_OPTION_ARG_STRING, command,
			"Execute command instead of default shell.", "COMMAND"},
		{"directory", 'd', 0, G_OPTION_ARG_STRING, directory,
			"Sets the working directory for the shell (or the command specified via -e).",
			"PATH"},
		{"keep", 'k', 0, G_OPTION_ARG_NONE, keep,
			"Don't exit the terminal after child process exits.",
			0},
		{"title", 't', 0, G_OPTION_ARG_STRING, title,
			"Set value of WM_NAME property; disables window_title_cb (default: 'MiniTerm')",
			"TITLE"},
		{"help", 'h', 0, G_OPTION_ARG_NONE, &help,
			"Display this message", 0},
		{NULL}};
	GError *error = NULL;
	GOptionContext *context = g_option_context_new(NULL);
	g_option_context_set_help_enabled(context, FALSE);
	g_option_context_add_main_entries(context, entries, NULL);
	g_option_context_parse(context, &argc, &argv, &error);
	if (help) {
		char *help_text =
			g_option_context_get_help(context, TRUE, NULL);
		g_application_command_line_print(command_line, "%s", help_text);
		g_free(help_text);
		g_option_context_free(context);
		return FALSE;
	}
	g_option_context_free(context);
	if (error) {
		g_application_command_line_printerr(command_line,
			"option parsing failed: %s\n", error->message);
		g_error_free(error);
		g_application_command_line_set_exit_status(
			command_line, EXIT_FAILURE);
		return FALSE;
	}
	if (version) {
		g_application_command_line_print(
			command_line, "miniterm " MINITERM_VERSION "\n");
		return FALSE;
	}
	return TRUE;
}

static void
signal_handler(int signal)
{
	(void)signal;
	g_application_quit(_application);
}

static void
set_geometry_hints(VteTerminal *vte, GdkGeometry *hints)
{
	hints->base_width = vte_terminal_get_char_width(vte);
	hints->base_height = vte_terminal_get_char_height(vte);
	hints->min_width = vte_terminal_get_char_width(vte);
	hints->min_height = vte_terminal_get_char_height(vte);
	hints->width_inc = vte_terminal_get_char_width(vte);
	hints->height_inc = vte_terminal_get_char_height(vte);
}

static void
new_window(GtkApplication *app, GApplicationCommandLine *command_line,
	gchar **argv, gint argc)
{
	/* Variables for parsed command-line arguments */
	char *command = NULL;
	char *directory = NULL;
	gboolean keep = FALSE;
	char *title = NULL;
	if (!parse_arguments(command_line, argc, argv, &command, &directory,
		    &keep, &title)) {
		return;
	}
	if (directory == NULL) {
		const char *cwd =
			g_application_command_line_get_cwd(command_line);
		if (cwd != NULL) {
			directory = malloc(strlen(cwd) + 1);
			strcpy(directory, cwd);
		}
	}
	/* Create window. */
	GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
	g_signal_connect(window, "delete-event", G_CALLBACK(window_close), app);
	gtk_window_set_title(GTK_WINDOW(window), title ? title : "miniterm");
	/* Set window icon supplied by an icon theme. */
	GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
	GdkPixbuf *icon =
		gtk_icon_theme_load_icon(icon_theme, "terminal", 48, 0, NULL);
	if (icon) {
		gtk_window_set_icon(GTK_WINDOW(window), icon);
		g_object_unref(icon);
	}

	/* Create terminal widget */
	MinitermTerminal *term =
		miniterm_terminal_new(keep, title, GTK_WINDOW(window));
	VteTerminal *vte = VTE_TERMINAL(term);
	GdkGeometry geo_hints;
	/* Apply geometry hints to handle terminal resizing */
	set_geometry_hints(vte, &geo_hints);
	gtk_window_set_geometry_hints(GTK_WINDOW(window), GTK_WIDGET(term),
		&geo_hints,
		GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);

	miniterm_terminal_load_settings(term);

	/* Show widgets and run main loop. */
	gtk_widget_show_all(window);

	/* Set the OS window id environment variable */
#ifdef GDK_WINDOWING_X11
	if (GDK_IS_X11_DISPLAY(gtk_widget_get_display(window))) {
		XID wid = GDK_WINDOW_XID(gtk_widget_get_window(window));
		char wid_str[64];
		snprintf(wid_str, 64, "%lu", wid);
		setenv("WINDOWID", wid_str, TRUE);
	}
#endif

	if (!vte_spawn(vte, command_line, directory, command, NULL)) {
		gtk_window_close(GTK_WINDOW(window));
		g_free(command);
		g_free(directory);
		g_free(title);
		return;
	}
	/* Cleanup. */
	g_free(command);
	g_free(directory);
	g_free(title);
}

static void
command_line(GApplication *app, GApplicationCommandLine *command_line,
	gpointer user_data)
{
	(void)user_data;
	int argv;
	char **argc =
		g_application_command_line_get_arguments(command_line, &argv);
	g_application_command_line_set_exit_status(command_line, EXIT_SUCCESS);
	new_window(GTK_APPLICATION(app), command_line, argc, argv);
}

/*
 * This program is a minimalist vte based terminal emulator that uses a basic
 * config file.
 */
int
main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	/* Register signal handler. */
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	GtkApplication *app = gtk_application_new(
		"us.laelath.miniterm", G_APPLICATION_HANDLES_COMMAND_LINE);
	g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
}
