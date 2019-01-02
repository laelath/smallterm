# Change Log
This project adheres to Semantic Versioning

## Unreleased
### Fixed
- Fix documentation.
- Fix `scrollback-lines` setting.

## [1.4.0] - 2019-01-01
### Added
- Add options to config file previously available as compile options.
- Add ability to reload config file on the fly. Press CTRL+Shift+R.
- Add `scrollbar-type` option that allows always, never, or automatic
  scrollbars.

### Changed
- Move project to CMake.
- Make icon show a terminal rather than generic picture.

### Fixed
- Fix licenses.
- Fix default window sizing when using a scrollbar.

### Removed
- Remove `use-scrollbar` option. Use `scrollbar-type` instead.

## [1.2.0] 2018-12-16
# Added
- Add a scroll bar.

### Fixed
- Fix README

## [1.1.3] - 2018-02-24
### Fixed
- No longer builds in debug mode by default.

### Changed
- Internal changes.

## [1.1.2] - 2018-01-29
### Fixed
- Fixed errors parsing command line arguments crashing all terminals
- Fixed opening help and version closing all terminals
- Fixed version and help not using the right output

## [1.1.1] - 2017-01-25
### Fixed
- Fix error in README.

### Added
- Add Vim section to README.
- Add change log.

## [1.1.0] - 2017-01-25
### Added
- Add desktop file to launch in a desktop environment

### Fixed
- Fix memory leak when parsing config file.
- Fix issues with pkg-config in Makefile.

### Changed
- Only loads colors if all colors were parsed successfully (this was true
  before, but it was because the function that is called fails if not all colors
are valid).
- Refactor code for formatting.
- There are now function prototypes at the beginning of the file.
- Various minor changes to the code.
