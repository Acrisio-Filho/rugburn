/**
 * Copyright 2018-2024 John Chadwick <john@jchw.io>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include <windows.h>

#include "../third_party/ijl/ijl.h"

#ifdef __cplusplus
extern "C" {
#endif

const IJLibVersion *__stdcall ijlGetLibVersionWrapper() { return ijlGetLibVersion(); }

IJLERR __stdcall ijlInitWrapper(JPEG_CORE_PROPERTIES *jcprops) { return ijlInit(jcprops); }

IJLERR __stdcall ijlFreeWrapper(JPEG_CORE_PROPERTIES *jcprops) { return ijlFree(jcprops); }

IJLERR __stdcall ijlReadWrapper(JPEG_CORE_PROPERTIES *jcprops, IJLIOTYPE iotype) {
    return ijlRead(jcprops, iotype);
}

IJLERR __stdcall ijlWriteWrapper(JPEG_CORE_PROPERTIES *jcprops, IJLIOTYPE iotype) {
    return ijlWrite(jcprops, iotype);
}

const char *__stdcall ijlErrorStrWrapper(IJLERR code) { return ijlErrorStr(code); }

#ifdef __cplusplus
}
#endif
