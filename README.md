# Miniterm
Fork of https://github.com/nathan-hoad/tinyterm that adds functionality to act
as a replacement for rxvt-unicode.

## Synopsis
Miniterm is a terminal emulator that uses vte as a backend to provide a
minimalist interface. It features:

- Shortcuts for increasing/decreasing font size and copying/pasting
- Runs against vte3 for TrueColor support
- Runs a single instance to save memory, like urxvtd/urxvtc
- Parses a config file at XDG\_CONFIG\_HOME/miniterm/miniterm.conf

## Installation
### Dependencies
Miniterm requires the following dependencies:

- glib2
- gtk3
- vte3 (2.91+)

### Compilation
To install, simply run `make` and `make install` which may require root
permission.

## Usage
You can run Miniterm with the `miniterm` command.

## Configuration
### Colors and Font
Miniterm is configure with an ini-like file located in
XDG\_CONFIG\_HOME/miniterm/miniterm.conf. Currently, there are two sections.

The font can be configured in the "Font" section with the "font" option. Set it
to your favorite monospaced font: `font=Source Code Pro 11`

Colors are configured in the "Colors" section. There are the "foreground" and
"background" options as well as the options "color00" through "color0f". Set
these to the hexadecimal colors for your colorscheme. For solarized, use the
following options:

	[Colors]
	foreground=#657B83
	background=#002B36
	color00=#073642
	color01=#DC322F
	color02=#859900
	color03=#b58900
	color04=#268bd2
	color05=#d33682
	color06=#2aa198
	color07=#eee8d5
	color08=#002b36
	color09=#cb4b16
	color0a=#586e75
	color0b=#657b83
	color0c=#839496
	color0d=#6c71c4
	color0e=#93a1a1
	color0f=#3d36e3

### Scrollbar
For a scrollbar, set the `use-scrollbar` setting in the `[Misc]` section to `true`.

### Vim
For unknown reasons, vim does not color every line correctly. To fix this, add
`set t_ut=` in your .vimrc. In addition to that, add `set termguicolors` to your
.vimrc to add TrueColor support.
