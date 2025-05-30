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

/*
 * This file contains many function pointer types and structures for Windows
 * APIs. Many of these structures are not actually needed for this patcher,
 * but were used in the process of developing it. These types make it easy to
 * hook various APIs to manipulate or inspect behavior.
 */

#pragma once

#include <windows.h>
#include <ws2tcpip.h>

// IWebBrowser 1 and 2
#include "exdispport.h"

#ifdef __MINGW32__

typedef struct _DNS_QUERY_REQUEST {
    ULONG Version;
    PCWSTR QueryName;
    WORD QueryType;
    ULONG64 QueryOptions;
    PVOID pDnsServerList;
    ULONG InterfaceIndex;
    PVOID pQueryCompletionCallback;
    PVOID pQueryContext;
} DNS_QUERY_REQUEST, *PDNS_QUERY_REQUEST;

typedef struct _DNS_QUERY_CANCEL {
    CHAR Reserved[32];
} DNS_QUERY_CANCEL, *PDNS_QUERY_CANCEL;

#endif

typedef struct tagINITCOMMONCONTROLSEX {
    DWORD dwSize; // size of this structure
    DWORD dwICC;  // flags indicating which classes to be initialized
} INITCOMMONCONTROLSEX, *LPINITCOMMONCONTROLSEX;

// comctl32.dll
typedef BOOL(STDCALL *PFNINITCOMMONCONTROLSEXPROC)(const INITCOMMONCONTROLSEX *picce);

// ole32.dll
typedef HRESULT(STDCALL *PFNCOGETCLASSOBJECTPROC)(const IID *rclsid, DWORD dwClsContext,
                                                  LPVOID pvReserved, const IID *riid, LPVOID *ppv);
typedef HRESULT(STDCALL *PFNCOCREATEINSTANCEPROC)(const IID *rclsid, LPUNKNOWN pUnkOuter,
                                                  DWORD dwClsContext, const IID *riid, LPVOID *ppv);

// oleaut32.dll
typedef BSTR(STDCALL *PFNSYSALLOCSTRINGLENPROC)(const OLECHAR *strIn, UINT ui);
typedef void(STDCALL *PFNSYSFREESTRINGPROC)(BSTR bstrString);

// ieframe.dll
typedef HRESULT(STDCALL *PFNINVOKEPROC)(IDispatch *This, DISPID dispIdMember, const IID *riid,
                                        LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
                                        VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
typedef HRESULT(STDCALL *PFNNAVIGATEPROC)(IWebBrowser2 *This, BSTR URL, VARIANT *Flags,
                                          VARIANT *TargetFrameName, VARIANT *PostData,
                                          VARIANT *Headers);

// general
typedef BOOL(WINAPI *PFNDLLMAINPROC)(HANDLE, DWORD, LPVOID);

// ws2_32.dll
typedef int(STDCALL *PFNCONNECTPROC)(SOCKET, const struct sockaddr *, int);
typedef BOOL(STDCALL *PFNCONNECTEXPROC)(SOCKET, const struct sockaddr *, int, PVOID, DWORD, LPDWORD,
                                        LPOVERLAPPED);
typedef int(STDCALL *PFNWSACONNECTPROC)(SOCKET, const struct sockaddr *, int, LPWSABUF, LPWSABUF,
                                        LPQOS, LPQOS);
typedef WORD(WSAAPI *PFNHTONSPROC)(WORD);
typedef INT(WSAAPI *PFNGETADDRINFO)(PCSTR, PCSTR, const PADDRINFOA, PADDRINFOA *);
typedef VOID(WSAAPI *PFNFREEADDRINFO)(PADDRINFOA);

// dnsapi.dll
typedef DNS_STATUS(STDCALL *PFNDNSQUERYEXPROC)(PDNS_QUERY_REQUEST pQueryRequest,
                                               PDNS_QUERY_RESULT pQueryResults,
                                               PDNS_QUERY_CANCEL pCancelHandle);

// advapi32.dll
typedef BOOL(STDCALL *PFNCRYPTDECRYPTPROC)(HCRYPTKEY, HCRYPTHASH, BOOL, DWORD, BYTE *, DWORD *);
typedef BOOL(STDCALL *PFNCRYPTGETHASHPARAMPROC)(HCRYPTHASH, DWORD, BYTE *, DWORD *, DWORD);
typedef BOOL(STDCALL *PFNCRYPTVERIFYSIGNATUREAPROC)(HCRYPTHASH, CONST BYTE *, DWORD, HCRYPTKEY,
                                                    LPCSTR, DWORD);
typedef BOOL(STDCALL *PFNCRYPTIMPORTKEYPROC)(HCRYPTPROV, CONST BYTE *, DWORD, HCRYPTKEY, DWORD,
                                             HCRYPTKEY *);

// user32.dll
typedef int(STDCALL *PFNMESSAGEBOXAPROC)(HWND, LPCSTR, LPCSTR, UINT);
typedef HWND(STDCALL *PFNCREATEWINDOWEXAPROC)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int,
                                              HWND, HMENU, HINSTANCE, LPVOID);
typedef LRESULT(STDCALL *PFNSENDMESSAGEPROC)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
typedef BOOL(STDCALL *PFNPEEKMESSAGEAPROC)(LPMSG, HWND, UINT, UINT, UINT);
typedef BOOL(STDCALL *PFNGETMESSAGEAPROC)(LPMSG, HWND, UINT, UINT);
typedef UINT_PTR(STDCALL *PFNSETTIMERPROC)(HWND, UINT_PTR, UINT, TIMERPROC);
typedef int(STDCALL *PFNWVSPRINTFAPROC)(LPSTR, LPCSTR, void *);

// kernel32.dll
typedef HMODULE(STDCALL *PFNLOADLIBRARYAPROC)(LPCSTR);
typedef VOID(STDCALL *PFNFREELIBRARYPROC)(HMODULE);
typedef FARPROC(STDCALL *PFNGETPROCADDRESSPROC)(HMODULE, LPCSTR);
typedef DWORD(STDCALL *PFNGETENVIRONMENTVARIABLEAPROC)(LPCSTR, LPSTR, DWORD);
typedef BOOL(STDCALL *PFNCREATEPROCESSAPROC)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES,
                                             LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR,
                                             LPSTARTUPINFOA, LPPROCESS_INFORMATION);
typedef BOOL(STDCALL *PFNISWOW64PROCESSPROC)(HANDLE, PBOOL);
typedef HANDLE(STDCALL *PFNCREATEFILEAPROC)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD,
                                            DWORD, HANDLE);
typedef HANDLE(STDCALL *PFNCREATEFILEMAPPINGAPROC)(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD,
                                                   DWORD, LPCSTR);
typedef HANDLE(STDCALL *PFNOPENMUTEXAPROC)(DWORD, BOOL, LPCSTR);
typedef HANDLE(STDCALL *PFNCREATEMUTEXAPROC)(LPSECURITY_ATTRIBUTES, BOOL, LPCSTR);
typedef DWORD(STDCALL *PFNGETCURRENTTHREADIDPROC)(VOID);
typedef DWORD(STDCALL *PFNGETCURRENTPROCESSIDPROC)(VOID);
typedef VOID(STDCALL *PFNSLEEPPROC)(DWORD);
typedef DWORD(STDCALL *PFNSLEEPEXPROC)(DWORD, BOOL);
typedef DWORD(STDCALL *PFNWAITFORSINGLEOBJECTPROC)(HANDLE, DWORD);
typedef HANDLE(STDCALL *PFNOPENEVENTAPROC)(DWORD, BOOL, LPCSTR);
typedef HANDLE(STDCALL *PFNCREATEEVENTAPROC)(LPSECURITY_ATTRIBUTES lpEventAttributes,
                                             BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);
typedef BOOL(STDCALL *PFNGETEXITCODEPROCESSPROC)(HANDLE hProcess, LPDWORD lpExitCode);
typedef HLOCAL(STDCALL *PFNLOCALALLOCPROC)(UINT, UINT);
typedef HLOCAL(STDCALL *PFNLOCALFREEPROC)(HLOCAL);
typedef HMODULE(STDCALL *PFNGETMODULEHANDLEAPROC)(LPCSTR);
typedef DWORD(STDCALL *PFNGETMODULEFILENAMEAPROC)(HMODULE, LPCH, DWORD);
typedef DWORD(STDCALL *PFNGETFILEATTRIBUTESAPROC)(LPCSTR);
typedef VOID(STDCALL *PFNEXITPROCESSPROC)(UINT);
typedef DWORD(STDCALL *PFNGETFILESIZEPROC)(HANDLE, LPDWORD);
typedef BOOL(STDCALL *PFNREADFILEPROC)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL(STDCALL *PFNWRITEFILEPROC)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL(STDCALL *PFNCLOSEHANDLEPROC)(HANDLE);
typedef LPSTR(STDCALL *PFNLSTRCPYAPROC)(LPSTR, LPCSTR);
typedef DWORD(STDCALL *PFNGETLASTERRORPROC)(VOID);
typedef BOOL(STDCALL *PFNSETENVIRONMENTVARIABLEAPROC)(LPCSTR, LPCSTR);
typedef BOOL(STDCALL *PFNVIRTUALPROTECTEXPROC)(HANDLE, LPVOID, SIZE_T, DWORD, PDWORD);
typedef BOOL(STDCALL *PFNREADPROCESSMEMORYPROC)(HANDLE, LPCVOID, LPVOID, SIZE_T, SIZE_T *);
typedef BOOL(STDCALL *PFNWRITEPROCESSMEMORYPROC)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T *);
typedef BOOL(STDCALL *PFNFLUSHINSTRUCTIONCACHEPROC)(HANDLE, LPCVOID, SIZE_T);
typedef BOOL(STDCALL *PFNVIRTUALPROTECTPROC)(LPVOID, SIZE_T, DWORD, PDWORD);
typedef HANDLE(STDCALL *PFNGETCURRENTPROCESSPROC)(VOID);
typedef HANDLE(STDCALL *PFNCREATETHREADPROC)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE,
                                             LPVOID, DWORD, LPDWORD);
typedef HANDLE(STDCALL *PFNGETSTDHANDLEPROC)(DWORD);
typedef SIZE_T(STDCALL *PFNVIRTUALQUERYPROC)(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer,
                                             SIZE_T dwLength);
typedef VOID(STDCALL *PFNGETSTARTUPINFOAPROC)(LPSTARTUPINFOA lpStartupInfo);
typedef VOID(STDCALL *PFNGETSTARTUPINFOWPROC)(LPSTARTUPINFOW lpStartupInfo);

// wininet.dll
typedef HINTERNET(STDCALL *PFNINTERNETOPENURLAPROC)(HINTERNET, LPCSTR, LPCSTR, DWORD, DWORD,
                                                    DWORD_PTR);
typedef HINTERNET(STDCALL *PFNINTERNETCONNECTAPROC)(HINTERNET hInternet, LPCSTR lpszServerName,
                                                    INTERNET_PORT nServerPort, LPCSTR lpszUserName,
                                                    LPCSTR lpszPassword, DWORD dwService,
                                                    DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET(STDCALL *PFNHTTPOPENREQUESTAPROC)(HINTERNET hConnect, LPCSTR lpszVerb,
                                                    LPCSTR lpszObjectName, LPCSTR lpszVersion,
                                                    LPCSTR lpszReferrer, LPCSTR *lplpszAcceptTypes,
                                                    DWORD dwFlags, DWORD_PTR dwContext);
typedef BOOL(STDCALL *PFNHTTPSENDREQUESTAPROC)(HINTERNET hRequest, LPCSTR lpszHeaders,
                                               DWORD dwHeadersLength, LPVOID lpOptional,
                                               DWORD dwOptionalLength);
typedef BOOL(STDCALL *PFNHTTPSENDREQUESTEXAPROC)(HINTERNET hRequest,
                                                 LPINTERNET_BUFFERSA lpBuffersIn,
                                                 LPINTERNET_BUFFERSA lpBuffersOut, DWORD dwFlags,
                                                 DWORD_PTR dwContext);
typedef BOOL(STDCALL *PFNHTTPENDREQUESTAPROC)(HINTERNET hRequest, LPINTERNET_BUFFERSA lpBuffersOut,
                                              DWORD dwFlags, DWORD_PTR dwContext);
typedef BOOL(STDCALL *PFNHTTPADDREQUESTHEADERSAPROC)(HINTERNET hRequest, LPCSTR lpszHeaders,
                                                     DWORD dwHeadersLength, DWORD dwModifiers);
typedef BOOL(STDCALL *PFNHTTPQUERYINFOAPROC)(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer,
                                             LPDWORD lpdwBufferLength, LPDWORD lpdwIndex);
typedef BOOL(STDCALL *PFNINTERNETCLOSEHANDLEPROC)(HINTERNET hInternet);
typedef BOOL(STDCALL *PFNINTERNETQUERYDATAAVAILABLEPROC)(HINTERNET hFile,
                                                         LPDWORD lpdwNumberOfBytesAvailable,
                                                         DWORD dwFlags, DWORD_PTR dwContext);
typedef BOOL(STDCALL *PFNINTERNETWRITEFILEPROC)(HINTERNET hFile, LPCVOID lpBuffer,
                                                DWORD dwNumberOfBytesToWrite,
                                                LPDWORD lpdwNumberOfBytesWritten);
typedef BOOL(STDCALL *PFNINTERNETREADFILEPROC)(HINTERNET hFile, LPVOID lpBuffer,
                                               DWORD dwNumberOfBytesToRead,
                                               LPDWORD lpdwNumberOfBytesRead);
typedef BOOL(STDCALL *PFNINTERNETQUERYOPTIONAPROC)(HINTERNET hInternet, DWORD dwOption,
                                                   LPVOID lpBuffer, LPDWORD lpdwBufferLength);
typedef BOOL(STDCALL *PFNINTERNETSETOPTIONAPROC)(HINTERNET hInternet, DWORD dwOption,
                                                 LPVOID lpBuffer, DWORD dwBufferLength);
typedef BOOL(STDCALL *PFNINTERNETCRACKURLAPROC)(LPCSTR lpszUrl, DWORD dwUrlLength, DWORD dwFlags,
                                                LPURL_COMPONENTSA lpUrlComponents);

// winmm.dll
typedef DWORD(STDCALL *PFNTIMEGETTIMEPROC)();

// ijl15.dll
typedef int(STDCALL *PFNIJLGETLIBVERSIONPROC)();
typedef int(STDCALL *PFNIJLINITPROC)(int);
typedef int(STDCALL *PFNIJLFREEPROC)(int);
typedef int(STDCALL *PFNIJLREADPROC)(int, int);
typedef int(STDCALL *PFNIJLWRITEPROC)(int, int);
typedef int(STDCALL *PFNIJLERRORSTRPROC)(int);
