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

#pragma once

#include "common.h"

VOID Patch(LPVOID dst, LPCVOID src, DWORD size);
VOID InstallHook(PVOID pfnProc, LPCVOID pfnTargetProc);
DWORD CountOpcodeBytes(LPCVOID fn, DWORD minBytes);
PBYTE BuildTrampoline(DWORD fn, DWORD prefixLen);
PVOID HookFunc(PVOID pfnProc, PVOID pvTargetProc);
PVOID HookProc(HMODULE hModule, LPCSTR szName, PVOID pfnTargetProc);
PVOID BuildThiscallToStdcallThunk(LPCVOID pfnProc);
PVOID BuildStdcallToThiscallThunk(LPCVOID pfnProc);
PVOID BuildStdcallToVirtualThiscallThunk(DWORD dwVtblOffset);
