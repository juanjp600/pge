# PGE - pulsegun engine

little more than a graphics api wrapper

# [Documentation](https://juanjp600.github.io/pge/)

# A note on linking

At this time PGE is only to be linked statically, as it transfers ownership of raw pointers, which could result in the [cross-DLL problem](https://stackoverflow.com/questions/443147/c-mix-new-delete-between-libs), see [#32](/../../issues/32).

# [Guidelines](GUIDELINES.md)

# License (Zlib)

This project is licensed under the zlib license. Please refer to the [LICENSE.md](LICENSE.md) file for more details.

Any other asset that does not fall under this license will be explicitly marked as such inside the file.
