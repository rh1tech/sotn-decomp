// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef DISC_PROVIDER_H
#define DISC_PROVIDER_H

#include <stdbool.h>
#include "cdimage.h"

extern const cd_backend_t disc_stdio_backend;

// Open a CUE/BIN image and route every "disks/..." file read through it.
bool DiscProvider_Mount(const cd_backend_t* backend, const char* image_path);

#endif
