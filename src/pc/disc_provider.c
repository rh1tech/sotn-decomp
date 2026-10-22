// SPDX-License-Identifier: AGPL-3.0-or-later
// Serves "disks/<version>/..." game files straight out of a CUE/BIN image
// (see cdimage.c), so no extracted copy of the disc is needed.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"
#include "disc_provider.h"

static const char* StripDiskPrefix(const char* path) {
    // "disks/us/BIN/F_GAME.BIN" -> "BIN/F_GAME.BIN"
    const char* p = strchr(path, '/');
    if (!p) {
        return path;
    }
    p = strchr(p + 1, '/');
    return p ? p + 1 : path;
}

static void* DiscOpen(const char* path, size_t* length) {
    cd_file_t* f = malloc(sizeof(cd_file_t));
    if (!f) {
        return NULL;
    }
    if (!cd_fopen(f, StripDiskPrefix(path))) {
        free(f);
        return NULL;
    }
    *length = f->entry->size;
    return f;
}

static size_t DiscRead(void* handle, void* dst, size_t offset, size_t len) {
    cd_file_t* f = handle;
    cd_fseek(f, (uint32_t)offset);
    return cd_fread(f, dst, (uint32_t)len);
}

static void DiscClose(void* handle) { free(handle); }

static const FileProvider disc_provider = {
    .open = DiscOpen,
    .read = DiscRead,
    .close = DiscClose,
};

bool DiscProvider_Mount(const cd_backend_t* backend, const char* image_path) {
    cd_result_t r = cd_open(backend, image_path);
    if (r != CD_OK) {
        ERRORF("unable to open disc image '%s' (%d)", image_path, r);
        return false;
    }
    INFOF("disc image '%s': volume %s, %u files", image_path, cd_volume_id(),
          cd_entry_count());
    FileSetProvider(&disc_provider);
    return true;
}

// Host (stdio) storage backend
static void* StdioOpen(const char* path) { return fopen(path, "rb"); }
static uint32_t StdioRead(void* h, uint64_t off, void* buf, uint32_t len) {
    if (fseeko((FILE*)h, (off_t)off, SEEK_SET) != 0) {
        return 0;
    }
    return (uint32_t)fread(buf, 1, len, (FILE*)h);
}
static uint64_t StdioSize(void* h) {
    fseeko((FILE*)h, 0, SEEK_END);
    return (uint64_t)ftello((FILE*)h);
}
static void StdioClose(void* h) { fclose((FILE*)h); }

const cd_backend_t disc_stdio_backend = {
    .open = StdioOpen,
    .read = StdioRead,
    .size = StdioSize,
    .close = StdioClose,
    .alloc = malloc,
};
