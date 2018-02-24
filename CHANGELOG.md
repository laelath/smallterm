# Change Log
This project adheres to Semantic Versioning

## [1.1.3]
### Fixed
- No longer builds in debug mode by default.

### Changed
- Internal changes.

## [1.1.2]
### Fixed
- Fixed errors parsing command line arguments crashing all terminals
- Fixed opening help and version closing all terminals
- Fixed version and help not using the right output

## [1.1.1] - 2017-01-25
### Fixed
- Fixe error in README.

### Added
- Add vim section to README
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
