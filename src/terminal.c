#include "terminal.h"

#include <sys/stat.h>

#include "config.h"

struct _MinitermTerminal {
	VteTerminal parent;
};

typedef struct _MinitermTerminalPrivate MinitermTerminalPrivate;

struct _MinitermTerminalPrivate {
	/* Title passed from command line. The value NULL indicates no title. */
	char *cmd_title;
	int default_font_size;

	/* Window for connecting signals. */
	GtkWindow *window;
	/*
	 * Box to add self to. This is so that we can dynamically add and
	 * remove the scrollbar. Should always be directly inside window.
	 */
	GtkWidget *box;
	/* Scrolled window, NULL indicates currently not in use. */
	GtkWidget *scrolled_window;

	/* Signal handlers. 0 indicates no signal connected. */
	unsigned long bell_handler;
	unsigned long focus_in_handler;
	unsigned long focus_out_handler;
	unsigned long window_title_changed_handler;
};

G_DEFINE_TYPE_WITH_PRIVATE(
	MinitermTerminal, miniterm_terminal, VTE_TYPE_TERMINAL)

static void miniterm_terminal_finalize(GObject *terminal);
static void miniterm_terminal_dispose(GObject *terminal);

/*
 * Sets the terminal's settings from the given settings. Ensures that all
 * widgets are correctly added to each other.
 */
static void update_from_settings(
	MinitermTerminal *terminal, MinitermSettings *settings);
/*
 * Adds terminal to its own window with a scrollbar. Assumes that terminal's
 * scrolled window is currently NULL and that its box contains no elements.
 */
static void add_with_scrollbar(MinitermTerminal *terminal);
/*
 * Adds terminal to its own window without a scrollbar. Assumes the terminal's
 * box contains no elements.
 */
static void add_without_scrollbar(MinitermTerminal *terminal);

/* Returns a GtkScrolledWindow containing widget. */
static GtkWidget *make_scrolled_window(GtkScrollable *widget);
/* Callback to set window urgency hint on beep events. */
static void window_urgency_hint_cb(
	MinitermTerminal *terminal, gpointer user_data);
/* Callback to unset window urgency hint on focus. */
static gboolean window_focus_cb(GtkWindow *window);
/* Callback to dynamically change window title. */
static void window_title_cb(MinitermTerminal *terminal);
/* Callback to react to key press events. */
static gboolean key_press_cb(MinitermTerminal *terminal, GdkEventKey *event);
static void exit_cb(
	MinitermTerminal *terminal, gint status, gpointer user_data);

/* Increases the font size of the terminal. */
static void increase_font_size(MinitermTerminal *terminal);
/* Decreases the font size of the terminal. */
static void decrease_font_size(MinitermTerminal *terminal);
/* Resets the font size of the terminal. */
static void reset_font_size(MinitermTerminal *terminal);

/* Clears all signal handlers if they exist. */
static void clear_signal_handlers(MinitermTerminal *terminal);

static void
miniterm_terminal_init(MinitermTerminal *terminal)
{
	MinitermTerminalPrivate *priv =
		miniterm_terminal_get_instance_private(terminal);
	priv->cmd_title = NULL;
	priv->default_font_size = 0;
	priv->bell_handler = 0;
	priv->focus_in_handler = 0;
	priv->focus_out_handler = 0;
	priv->window_title_changed_handler = 0;

	vte_terminal_set_cursor_shape(VTE_TERMINAL(terminal), CURSOR_SHAPE);
	vte_terminal_set_cursor_blink_mode(
		VTE_TERMINAL(terminal), CURSOR_BLINK);
	vte_terminal_set_word_char_exceptions(
		VTE_TERMINAL(terminal), WORD_CHARS);
	g_signal_connect(
		terminal, "key-press-event", G_CALLBACK(key_press_cb), NULL);

	priv->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	g_object_ref(priv->box);
	priv->scrolled_window = NULL;
}

static void
miniterm_terminal_class_init(MinitermTerminalClass *kclass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(kclass);
	object_class->dispose = miniterm_terminal_dispose;
	object_class->finalize = miniterm_terminal_finalize;
}

static void
miniterm_terminal_dispose(GObject *terminal)
{
	MinitermTerminalPrivate *priv = miniterm_terminal_get_instance_private(
		MINITERM_TERMINAL(terminal));
	g_clear_object(&priv->window);
	g_clear_object(&priv->box);
	g_clear_object(&priv->scrolled_window);
	G_OBJECT_CLASS(miniterm_terminal_parent_class)->dispose(terminal);
}

static void
miniterm_terminal_finalize(GObject *terminal)
{
	MinitermTerminalPrivate *priv = miniterm_terminal_get_instance_private(
		MINITERM_TERMINAL(terminal));
	g_free(priv->cmd_title);
	G_OBJECT_CLASS(miniterm_terminal_parent_class)->finalize(terminal);
}

MinitermTerminal *
miniterm_terminal_new(bool keep, const char *title, GtkWindow *window)
{
	MinitermTerminal *terminal = g_object_new(MINITERM_TYPE_TERMINAL, NULL);
	MinitermTerminalPrivate *priv =
		miniterm_terminal_get_instance_private(terminal);
	priv->cmd_title = g_strdup(title);
	priv->window = window;
	g_object_ref(window);
	gtk_container_add(GTK_CONTAINER(priv->window), priv->box);
	if (!keep)
		g_signal_connect(
			terminal, "child-exited", G_CALLBACK(exit_cb), window);
	return terminal;
}

static void
update_from_settings(MinitermTerminal *terminal, MinitermSettings *settings)
{
	MinitermTerminalPrivate *priv =
		miniterm_terminal_get_instance_private(terminal);
	vte_terminal_set_audible_bell(
		VTE_TERMINAL(terminal), settings->audible_bell);
	vte_terminal_set_scrollback_lines(
		VTE_TERMINAL(terminal), settings->scrollback_lines);
	clear_signal_handlers(terminal);
	if (settings->urgent_on_bell) {
		priv->bell_handler = g_signal_connect(terminal, "bell",
			G_CALLBACK(window_urgency_hint_cb), NULL);
		priv->focus_in_handler = g_signal_connect(priv->window,
			"focus-in-event", G_CALLBACK(window_focus_cb), NULL);
		priv->focus_out_handler = g_signal_connect(priv->window,
			"focus-out-event", G_CALLBACK(window_focus_cb), NULL);
	}
	if (settings->dynamic_window_title && !priv->cmd_title)
		priv->window_title_changed_handler =
			g_signal_connect(terminal, "window-title-changed",
				G_CALLBACK(window_title_cb), NULL);
	if (settings->font_name != NULL) {
		PangoFontDescription *font =
			pango_font_description_from_string(settings->font_name);
		vte_terminal_set_font(VTE_TERMINAL(terminal), font);
		priv->default_font_size = pango_font_description_get_size(font);
		if (priv->default_font_size == 0)
			priv->default_font_size = 12 * PANGO_SCALE;
		pango_font_description_free(font);
	}
	if (settings->has_colors)
		vte_terminal_set_colors(VTE_TERMINAL(terminal),
			&settings->fg_color, &settings->bg_color,
			settings->color_palette, MINITERM_COLOR_COUNT);
	/*
	 * Increment the reference count so that removing it from the window
	 * doesn't cause it to get destroyed.
	 */
	g_object_ref(terminal);
	if (priv->scrolled_window != NULL) {
		gtk_container_remove(GTK_CONTAINER(priv->scrolled_window),
			GTK_WIDGET(terminal));
		gtk_container_remove(
			GTK_CONTAINER(priv->box), priv->scrolled_window);
		g_clear_object(&priv->scrolled_window);
	} else
		gtk_container_remove(
			GTK_CONTAINER(priv->box), GTK_WIDGET(terminal));
	if (settings->use_scrollbar)
		add_with_scrollbar(terminal);
	else
		add_without_scrollbar(terminal);
	g_object_unref(terminal);
}

bool
miniterm_terminal_load_settings(MinitermTerminal *terminal)
{
	char *config_dir =
		g_strconcat(g_get_user_config_dir(), "/miniterm", NULL);
	char *config_path = g_strconcat(config_dir, "/miniterm.conf", NULL);
	GKeyFile *config_file = g_key_file_new();
	MinitermSettings settings;
	miniterm_settings_init(&settings);
	if (g_key_file_load_from_file(config_file, config_path, 0, NULL)) {
		miniterm_settings_set_from_key_file(&settings, config_file);
	} else {
		mkdir(config_dir, 0777);
		miniterm_write_default_settings(config_path);
	}
	update_from_settings(terminal, &settings);
	miniterm_settings_set_from_key_file(&settings, config_file);
	g_free(config_dir);
	g_free(config_path);
	g_key_file_free(config_file);
	return true;
}

static void
window_urgency_hint_cb(MinitermTerminal *terminal, gpointer user_data)
{
	(void)user_data;
	gtk_window_set_urgency_hint(
		GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(terminal))),
		TRUE);
}

static gboolean
window_focus_cb(GtkWindow *window)
{
	gtk_window_set_urgency_hint(window, FALSE);
	return FALSE;
}

static void
window_title_cb(MinitermTerminal *terminal)
{
	gtk_window_set_title(
		GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(terminal))),
		vte_terminal_get_window_title(VTE_TERMINAL(terminal)));
}

static gboolean
key_press_cb(MinitermTerminal *terminal, GdkEventKey *event)
{
	VteTerminal *vte = VTE_TERMINAL(terminal);
	const guint key = gdk_keyval_to_lower(event->keyval);
	const guint modifiers =
		event->state & gtk_accelerator_get_default_mod_mask();
	if ((modifiers == (GDK_CONTROL_MASK | GDK_SHIFT_MASK))) {
		switch (key) {
		case GDK_KEY_c:
#if VTE_CHECK_VERSION(0, 50, 0)
			vte_terminal_copy_clipboard_format(
				VTE_TERMINAL(terminal), VTE_FORMAT_TEXT);
#else
			vte_terminal_copy_clipboard(vte);
#endif
			return TRUE;
		case GDK_KEY_v:
			vte_terminal_paste_clipboard(vte);
			return TRUE;
		case GDK_KEY_plus:
			increase_font_size(terminal);
			return TRUE;
		}
	} else if (modifiers == GDK_CONTROL_MASK) {
		switch (key) {
		case GDK_KEY_minus:
			decrease_font_size(terminal);
			return TRUE;
		case GDK_KEY_equal:
			reset_font_size(terminal);
			return TRUE;
		}
	}
	return FALSE;
}

static void
exit_cb(MinitermTerminal *terminal, gint status, gpointer user_data)
{
	(void)status;
	gtk_window_close(GTK_WINDOW(user_data));
}

static void
increase_font_size(MinitermTerminal *terminal)
{
	PangoFontDescription *font = pango_font_description_copy_static(
		vte_terminal_get_font(VTE_TERMINAL(terminal)));
	pango_font_description_set_size(
		font, (pango_font_description_get_size(font) / PANGO_SCALE + 1)
			      * PANGO_SCALE);
	vte_terminal_set_font(VTE_TERMINAL(terminal), font);
	pango_font_description_free(font);
}

static void
decrease_font_size(MinitermTerminal *terminal)
{
	PangoFontDescription *font = pango_font_description_copy_static(
		vte_terminal_get_font(VTE_TERMINAL(terminal)));
	const gint size =
		pango_font_description_get_size(font) / PANGO_SCALE - 1;
	if (size > 0) {
		pango_font_description_set_size(font, size * PANGO_SCALE);
		vte_terminal_set_font(VTE_TERMINAL(terminal), font);
	}
	pango_font_description_free(font);
}

static void
reset_font_size(MinitermTerminal *terminal)
{
	MinitermTerminalPrivate *priv =
		miniterm_terminal_get_instance_private(terminal);
	PangoFontDescription *font = pango_font_description_copy_static(
		vte_terminal_get_font(VTE_TERMINAL(terminal)));
	pango_font_description_set_size(font, priv->default_font_size);
	vte_terminal_set_font(VTE_TERMINAL(terminal), font);
	pango_font_description_free(font);
}

static void
clear_signal_handlers(MinitermTerminal *terminal)
{
	MinitermTerminalPrivate *priv =
		miniterm_terminal_get_instance_private(terminal);
	if (priv->bell_handler != 0) {
		g_signal_handler_disconnect(terminal, priv->bell_handler);
		priv->bell_handler = 0;
	}
	if (priv->focus_in_handler != 0) {
		g_signal_handler_disconnect(
			priv->window, priv->focus_in_handler);
		priv->focus_in_handler = 0;
	}
	if (priv->focus_out_handler != 0) {
		g_signal_handler_disconnect(
			priv->window, priv->focus_out_handler);
		priv->focus_out_handler = 0;
	}
	if (priv->window_title_changed_handler != 0) {
		g_signal_handler_disconnect(
			terminal, priv->window_title_changed_handler);
		priv->window_title_changed_handler = 0;
	}
}

void
add_with_scrollbar(MinitermTerminal *terminal)
{
	MinitermTerminalPrivate *priv =
		miniterm_terminal_get_instance_private(terminal);
	priv->scrolled_window = make_scrolled_window(GTK_SCROLLABLE(terminal));
	g_object_ref(priv->scrolled_window);
	gtk_box_pack_start(
		GTK_BOX(priv->box), priv->scrolled_window, TRUE, TRUE, 0);
}

void
add_without_scrollbar(MinitermTerminal *terminal)
{
	MinitermTerminalPrivate *priv =
		miniterm_terminal_get_instance_private(terminal);
	gtk_box_pack_start(
		GTK_BOX(priv->box), GTK_WIDGET(terminal), TRUE, TRUE, 0);
}

static GtkWidget *
make_scrolled_window(GtkScrollable *widget)
{
	GtkAdjustment *hadjustment =
		gtk_scrollable_get_hadjustment(GTK_SCROLLABLE(widget));
	GtkAdjustment *vadjustment =
		gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(widget));
	GtkWidget *scrolled_window =
		gtk_scrolled_window_new(hadjustment, vadjustment);
	gtk_scrolled_window_set_overlay_scrolling(
		GTK_SCROLLED_WINDOW(scrolled_window), FALSE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(widget));
	return scrolled_window;
}
