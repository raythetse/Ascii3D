#pragma once

// The version number, in one place. Ascii3D.rc puts it in the exe's file properties, and the
// release job in .github/workflows/ci.yml checks that a pushed vX.Y.Z tag matches it.
// These are #defines, not constexpr, because the resource compiler only runs the C preprocessor.
#define ASCII3D_VERSION_MAJOR 1
#define ASCII3D_VERSION_MINOR 0
#define ASCII3D_VERSION_PATCH 0

// "1.0.0": the extra macro makes the arguments expand to numbers before # turns them into a string
#define ASCII3D_STRINGIZE_(x) #x
#define ASCII3D_STRINGIZE(x) ASCII3D_STRINGIZE_(x)
#define ASCII3D_VERSION_STRING ASCII3D_STRINGIZE(ASCII3D_VERSION_MAJOR.ASCII3D_VERSION_MINOR.ASCII3D_VERSION_PATCH)
