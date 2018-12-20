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

#ifndef MINITERM_TERMINAL_H
#define MINITERM_TERMINAL_H

#include <gtk/gtk.h>
#include <stdbool.h>
#include <vte/vte.h>

#include "settings.h"

#define MINITERM_TYPE_TERMINAL (miniterm_terminal_get_type())
G_DECLARE_FINAL_TYPE(
	MinitermTerminal, miniterm_terminal, MINITERM, TERMINAL, VteTerminal);

/* The title may be NULL. */
MinitermTerminal *miniterm_terminal_new(
	bool keep, const char *title, GtkWindow *window);
/* Loads from default path. Returns whether it succeeded. */
bool miniterm_terminal_load_settings(MinitermTerminal *terminal);
/*
 * Assumes terminal's window contains no widgets. Adds terminal to its own
 * window with a scrollbar.
 */
void miniterm_terminal_add_with_scrollbar(MinitermTerminal *terminal);
/*
 * Assumes terminal's window contains no widgets. Adds terminal to its own
 * window without a scrollbar.
 */
void miniterm_terminal_add_without_scrollbar(MinitermTerminal *terminal);

#endif /* MINITERM_TERMINAL_H */
