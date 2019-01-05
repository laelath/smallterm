/*
 * MIT/X Consortium License
 *
 * © 2017 Justin Frank
 * © 2015 Nathan Hoad
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

#define MINITERM_VERSION "1.4.2"

/* Terminal emulation (value of $TERM) (default: xterm) */
#define TERMINFO "xterm-256color"

/* One of VTE_CURSOR_SHAPE_BLOCK, VTE_CURSOR_SHAPE_IBEAM,
 * VTE_CURSOR_SHAPE_UNDERLINE */
#define CURSOR_SHAPE VTE_CURSOR_SHAPE_BLOCK

/* One of VTE_CURSOR_BLINK_SYSTEM, VTE_CURSOR_BLINK_ON, VTE_CURSOR_BLINK_OFF */
#define CURSOR_BLINK VTE_CURSOR_BLINK_OFF

/* Selection behavior for double-clicks */
#define WORD_CHARS "-A-Za-z0-9:./?%&#_=+@~"
