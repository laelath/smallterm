# Miniterm
Fork of https://github.com/nathan-hoad/tinyterm that adds functionality to act
as a replacement for rxvt-unicode. Focus on simplicity while maintaining ease of
use.

## Synopsis
Miniterm is a terminal emulator that uses vte as a backend to provide a
minimalist interface. It features:

- Shortcuts for increasing/decreasing font size and copying/pasting
- Runs against vte3 for TrueColor support
- Runs a single instance to save memory, like urxvtd/urxvtc
- Parses a config file at `$XDG\_CONFIG\_HOME/miniterm/miniterm.conf`

## Installation
### Dependencies
Miniterm requires the following dependencies:

- glib2
- gtk3
- vte3 (2.91+)

### Building
Building Miniterm requires CMake and a Make program such as GNU Make. Start by
cloning the repository, then creating a build directory:
```bash
git clone https://github.com/laelath/miniterm.git
cd miniterm
mkdir build
cd build
```

Run CMake with the desired options. For a release build run:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Finally, build and install the program. This may require root permissions.
```bash
make
sudo make install
```

## Usage
You can run Miniterm with the `miniterm` command.

## Configuration
### Colors and Font
Miniterm is configure with an ini-like file located in
`$XDG\_CONFIG\_HOME/miniterm/miniterm.conf`.

The font can be configured in the `Font` section with the `font` option. Set it
to your favorite monospaced font: `font=Source Code Pro 11`

Colors are configured in the `Colors` section. There are the `foreground` and
`background` options as well as the options `color00` through `color0f`. Set
these to the hexadecimal colors for your color scheme. For solarized, use the
following options:

	[Colors]
	foreground=#839496
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

You can enable a transparent terminal with the `transparency` option. This
setting should be a number between 0 and 1. A value of 0 indicates complete
transparency and 1 indicates no transparency. The default is no transparency.

### Misc
There are several options in the `Misc` section of the configuration file.
#### Scrollbar
For a scrollbar, set the `scrollbar-type` setting to either `automatic` or
`always` in the `Misc` section. To disable the scrollbar set it to `never`.

#### Size
The default size can be set with the `columns` and `rows` options.

### Other
If the configuration file doesn't exist, Miniterm will create one automatically.
See the generated `$XDG\_CONFIG\_HOME/miniterm/miniterm.conf` for all available
options.

## Miscellaneous
### Weird Colors in Vim
Sometimes Vim doesn't color every line correctly. To fix this, add `set t_ut=`
in your `.vimrc`. In addition to that, add `set termguicolors` to your `.vimrc`
to add TrueColor support.

### Contributing
## Formatting
This project is formatted using
[clang-format](https://clang.llvm.org/docs/ClangFormat.html). Please run this
command on your code before committing.
