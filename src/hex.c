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

#include "hex.h"
#include "config.h"

static BOOL ParseHex(CHAR ch, LPDWORD pOutValue) {
    if (ch >= '0' && ch <= '9') {
        *pOutValue = ch - '0';
    } else if (ch >= 'a' && ch <= 'f') {
        *pOutValue = ch - 'a' + 10;
    } else if (ch >= 'A' && ch <= 'F') {
        *pOutValue = ch - 'A' + 10;
    } else {
        return FALSE;
    }
    return TRUE;
}

static CHAR ToHexChar(BYTE bValue) {
    if (bValue <= 9) {
        return bValue + '0';
    } else if (bValue >= 0xa && bValue <= 0xf) {
        return bValue + 'a' - 10;
    }
    return 0;
}

static DWORD findNextReference(LPCSTR lpszText, LPVOID *pOutRef) {
    LPCSTR inPos;
    DWORD dif;
    *pOutRef = NULL;

    inPos = lpszText;
    while (*inPos) {
        if (*inPos++ == '\\' && *inPos != '\0' && *inPos++ == 'r') {
            dif = inPos - lpszText - 2;
            if ((*pOutRef = findCustomMemoryByReference(lpszText, dif)) == NULL)
                break;

            return dif + 2;
        }
    }
    return 0;
}

LPSTR ToHex(LPCVOID pData, DWORD dwLen) {
    LPSTR pHexData, pHexPtr;
    LPCSTR pcData = (LPCSTR)pData;
    pHexData = pHexPtr = (LPSTR)AllocMem(dwLen * 2 + 1);
    while (dwLen--) {
        *pHexPtr++ = ToHexChar((*pcData & 0xF0) >> 4);
        *pHexPtr++ = ToHexChar((*pcData & 0x0F) >> 0);
        pcData++;
    }
    *pHexPtr++ = 0;
    return pHexData;
}

DWORD FromHex(LPCSTR pHex, PVOID pData) {
    DWORD dwLen = 0;
    DWORD dwHexDigitVal, dwHexVal;
    PCHAR pcData = (PCHAR)pData;

    while (*pHex) {
        if (ParseHex(*pHex++, &dwHexDigitVal) == FALSE) {
            break;
        }
        dwHexVal = dwHexDigitVal << 4;
        if (ParseHex(*pHex++, &dwHexDigitVal) == FALSE) {
            break;
        }
        dwHexVal |= dwHexDigitVal;
        *pcData++ = (BYTE)dwHexVal;
        dwLen++;
    }

    return dwLen;
}

DWORD ParseAddress(LPCSTR lpszText) {
    LPCSTR pos = lpszText;
    DWORD hexDigitVal;
    DWORD result = 0;

    if (lpszText == NULL) {
        FatalError("ParseAddress: Received unexpected NULL string");
    }

    if (lpszText[0] == '\0') {
        return 0;
    }

    if (pos[0] != '0' || pos[1] != 'x' || pos[2] == 0) {
        FatalError("ParseAddress: Invalid input, expected 0x-prefixed hex number");
    }

    pos += 2;

    while (ParseHex(*pos, &hexDigitVal) == TRUE) {
        result <<= 4;
        result |= hexDigitVal;
        pos++;
    }

    if (*pos) {
        FatalError("ParseAddress: Unexpected text remaining after address: %s", pos);
    }

    return result;
}

// Parse Patch With Custom Memory Reference
void ParsePatch(LPCSTR lpszText, LPSTR *pDataOut, DWORD *pSizeOut) {
    DWORD hexDigitVal;
    DWORD hexOctetVal;
    LPCSTR inPos;
    LPSTR outPos;
    LPVOID refVal;
    DWORD refSize;
    *pSizeOut = 0;

    // Calculate length
    inPos = lpszText;
    while (*inPos) {
        switch (*inPos++) {
        case '\\':
            switch (*inPos++) {
            case 'x':
                if (ParseHex(*inPos++, &hexDigitVal) == FALSE) {
                    FatalError("ParsePatch: Bad hex escape near %s", inPos);
                }
                if (ParseHex(*inPos++, &hexDigitVal) == FALSE) {
                    FatalError("ParsePatch: Bad hex escape near %s", inPos);
                }
                (*pSizeOut)++;
                break;
            case '\\':
                (*pSizeOut)++;
                break;
            case 'r':
                refSize = findNextReference(inPos, &refVal);
                if (refSize != 0 && refVal != NULL) {
                    *pSizeOut += 4;
                    inPos += refSize;
                } else
                    *pSizeOut += 2;
                break;
            case '\0':
                FatalError("ParsePatch: Unexpected end of string in escape code");
                break;
            default:
                FatalError("ParsePatch: Unknown escape code: %s", inPos);
            }
            break;
        default:
            (*pSizeOut)++;
        }
    }

    // Allocate memory
    *pDataOut = (LPSTR)AllocMem(*pSizeOut);

    // Parse
    inPos = lpszText;
    outPos = *pDataOut;
    while (*inPos) {
        switch (*inPos) {
        case '\\':
            inPos++;
            switch (*inPos++) {
            case 'x':
                if (!ParseHex(*inPos++, &hexDigitVal)) {
                    FatalError("ParsePatch: Bad hex escape near %s", inPos);
                }
                hexOctetVal = hexDigitVal << 4;
                if (!ParseHex(*inPos++, &hexDigitVal)) {
                    FatalError("ParsePatch: Bad hex escape near %s", inPos);
                }
                hexOctetVal |= hexDigitVal;
                *outPos++ = (CHAR)(BYTE)hexOctetVal;
                break;
            case '\\':
                *outPos++ = '\\';
                break;
            case 'r':
                refSize = findNextReference(inPos, &refVal);
                if (refSize != 0 && refVal != NULL) {
                    memcpy(outPos, &refVal, 4);
                    outPos += 4;
                    inPos += refSize;
                } else {
                    *outPos++ = '\\';
                    *outPos++ = 'r';
				}
                break;
            case '\0':
                FatalError("ParsePatch: Unexpected end of string in escape code");
                break;
            default:
                FatalError("ParsePatch: Unknown escape code: %s", inPos);
            }
            break;
        default:
            *(outPos++) = *(inPos++);
        }
    }
}
