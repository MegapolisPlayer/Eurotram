# Eurotram
repository

# Build instructions
This project has a CMake file - use it!

In order to run the program all of the models must be in the same directory, except for:
- shaders in the `shader/` directory
- announcements in the `cz/` directory, according to the announcement file (.ETANC)
- vehicle sounds in the `t3rp/` directory, according to the config file (t3rp.cfg)
- texture parcels textures in the `texparcel/` directory, files must have `tp-` prefix
- wall parcel textures in the `wall/` directory, files must have `wall-` prefix

All of these conditions should be automatically met if you copy the assets from their source directories.

Project code should work both on Linux and Windows.
