// SPDX-License-Identifier: AGPL-3.0-or-later
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"

// A game file is either a host FILE* or a handle from the FileProvider.
typedef struct {
    FILE* fp;
    void* handle;
    size_t pos;
} GameFile;

static const FileProvider* g_FileProvider;

void FileSetProvider(const FileProvider* provider) {
    g_FileProvider = provider;
}

static bool UseProvider(const char* filename) {
    return g_FileProvider && strncmp(filename, "disks/", 6) == 0;
}

static bool GameFileOpen(GameFile* gf, const char* filename, size_t* length) {
    gf->pos = 0;
    gf->fp = NULL;
    gf->handle = NULL;
    if (UseProvider(filename)) {
        gf->handle = g_FileProvider->open(filename, length);
        return gf->handle != NULL;
    }
    gf->fp = fopen(filename, "rb");
    if (gf->fp == NULL) {
        return false;
    }
    fseek(gf->fp, 0, SEEK_END);
    *length = ftell(gf->fp);
    fseek(gf->fp, 0, SEEK_SET);
    return true;
}

static void GameFileSeek(GameFile* gf, size_t offset) {
    gf->pos = offset;
    if (gf->fp) {
        fseek(gf->fp, offset, SEEK_SET);
    }
}

static size_t GameFileRead(GameFile* gf, void* dst, size_t len) {
    size_t n;
    if (gf->fp) {
        n = fread(dst, 1, len, gf->fp);
    } else {
        n = g_FileProvider->read(gf->handle, dst, gf->pos, len);
    }
    gf->pos += n;
    return n;
}

static void GameFileClose(GameFile* gf) {
    if (gf->fp) {
        fclose(gf->fp);
    } else if (gf->handle) {
        g_FileProvider->close(gf->handle);
    }
    gf->fp = NULL;
    gf->handle = NULL;
}

size_t FileRead(const struct FileOpenRead* f, void* dst, size_t len) {
    return GameFileRead((GameFile*)f->file, dst, len);
}

struct FileAsStringCbParams {
    bool (*callback)(const struct FileAsString*);
    void* param;
};
static bool _FileAsStringCb(const struct FileOpenRead* f) {
    void* content = malloc(f->length + 1);
    if (!content) {
        ERRORF("unable to allocate %d bytes for '%s'", f->length, f->filename);
        return false;
    }

    size_t bytesread = FileRead(f, content, f->length);
    if (bytesread != f->length) {
        ERRORF("unable to read %d bytes for '%s'", f->length, f->filename);
        free(content);
        return false;
    }
    ((char*)content)[f->length] = '\0';

    struct FileAsStringCbParams* p = (struct FileAsStringCbParams*)f->param;
    struct FileAsString fc = {
        .filename = f->filename,
        .content = (const char*)content,
        .length = f->length + 1,
        .param = p->param,
    };
    bool r = p->callback(&fc);
    free(content);
    return true;
}

struct FileUseContentCbParams {
    bool (*callback)(const struct FileUseContent*);
    void* param;
};
static bool _FileUseContentCb(const struct FileOpenRead* f) {
    void* content = malloc(f->length);
    if (!content) {
        ERRORF("unable to allocate %d bytes for '%s'", f->length, f->filename);
        return false;
    }

    size_t bytesread = FileRead(f, content, f->length);
    if (bytesread != f->length) {
        ERRORF("unable to read %d bytes for '%s'", f->length, f->filename);
        free(content);
        return false;
    }

    struct FileAsStringCbParams* p = (struct FileAsStringCbParams*)f->param;
    struct FileUseContent fc = {
        .filename = f->filename,
        .content = content,
        .length = f->length,
        .param = p->param,
    };
    bool r = p->callback(&fc);
    free(content);
    return r;
};

// Simply open a binary file and pass the handler to a reader callback by
// performing error handling and ensuring the file always gets closed.
// The callback will receive file name and file size, it will need to return
// true if successful.
bool FileOpenRead(
    bool (*cb)(const struct FileOpenRead*), const char* filename, void* param) {
    INFOF("open '%s'", filename);
    GameFile gf;
    size_t len;
    if (!GameFileOpen(&gf, filename, &len)) {
        ERRORF("unable to open '%s'", filename);
        return false;
    }

    struct FileOpenRead s = {
        .filename = filename,
        .file = &gf,
        .length = len,
        .param = param,
    };
    bool r = cb(&s);
    GameFileClose(&gf);
    return r;
}

// Read the content of a binary file into the specified buffer by maxlen bytes.
// The number of read bytes is returned; a negative value represents a failure.
int FileReadToBuf(const char* filename, void* dst, int offset, size_t maxlen) {
    INFOF("open '%s'", filename);
    GameFile gf;
    size_t len;
    if (!GameFileOpen(&gf, filename, &len)) {
        ERRORF("unable to open '%s'", filename);
        return -1;
    }
    GameFileSeek(&gf, offset);
    size_t bytesRead = GameFileRead(&gf, dst, maxlen);
    GameFileClose(&gf);
    return (int)(bytesRead);
}

// Read the content of a file as a null-terminated string and pass it to
// the specified callback. The function will take care of both validation
// and allocation of the unmanaged resources.
// The callback needs to return true if the operation is successful.
bool FileAsString(bool (*cb)(const struct FileAsString* file),
                  const char* filename, void* param) {
    struct FileAsStringCbParams p = {cb, param};
    return FileOpenRead(_FileAsStringCb, filename, &p);
}

// Read the specified file into a buffer and pass it to the specified callback.
// The function will take care of both validation and deallocation.
// The callback needs to return true if the operation is successful.
bool FileUseContent(bool (*cb)(const struct FileUseContent* file, void* param),
                    const char* filename, void* param) {
    struct FileUseContentCbParams p = {cb, param};
    return FileOpenRead(_FileUseContentCb, filename, &p);
}
