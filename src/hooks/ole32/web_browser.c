#include "web_browser.h"
#include "../../config.h"
#include "../../patch.h"

static HMODULE hOle32Module = NULL;
static HMODULE hOleAut32Module = NULL;
static PFNCOGETCLASSOBJECTPROC pCoGetClassObject = NULL;
static PFNCOCREATEINSTANCEPROC pCoCreateInstance = NULL;
static PFNINVOKEPROC pInvoke = NULL;
static PFNNAVIGATEPROC pNavigate = NULL;
static PFNSYSALLOCSTRINGLENPROC pSysAllocStringLen = NULL;
static PFNSYSFREESTRINGPROC pSysFreeString = NULL;

// {00000112-0000-0000-C000-000000000046}
const IID kIID_IObject = {0x112, 0, 0, {0xC0, 0, 0, 0, 0, 0, 0, 0x46}};
// {00020400-0000-0000-C000-000000000046}
const IID kIID_IDispatch = {0x20400, 0, 0, {0xC0, 0, 0, 0, 0, 0, 0, 0x46}};
// {00000001-0000-0000-C000-000000000046}
const IID kIID_IClassFactory = {0x1, 0, 0, {0xC0, 0, 0, 0, 0, 0, 0, 0x46}};
// {8856F961-340A-11D0-A96B-00C04FD705A2}
const IID kIID_MicrosoftWebBrowser = {
    0x8856F961, 0x340A, 0x11D0, {0xA9, 0x6B, 0, 0xC0, 0x4F, 0xD7, 0x05, 0xA2}};
// {D30C1661-CDAF-11d0-8A3E-00C04FC9E26E}
const IID kIID_IWebBrowser2 = {
    0xD30C1661, 0xCDAF, 0x11D0, {0x8A, 0x3E, 0, 0xC0, 0x4F, 0xC9, 0xE2, 0x6E}};

static BSTR RewriteURLW(BSTR urlw) {

    int len_urlw = lstrlenW(urlw);
    int len = WideCharToMultiByte(CP_ACP, 0, urlw, len_urlw, NULL, 0, NULL, NULL);

    PCHAR url = (PCHAR)AllocMem(len + 1);

    WideCharToMultiByte(CP_ACP, 0, urlw, len_urlw, url, len, NULL, NULL);

    url[len] = '\0';

    LPCSTR newURLA = RewriteURL(url);

    FreeMem(url);

    if (newURLA != NULL) {

        len = lstrlenA(newURLA);

        len_urlw = MultiByteToWideChar(CP_ACP, 0, newURLA, len, NULL, 0);

        BSTR newURLBSTR = pSysAllocStringLen(NULL, len_urlw);

        MultiByteToWideChar(CP_ACP, 0, newURLA, len, newURLBSTR, len_urlw);

        FreeMem((HLOCAL)newURLA);

        return newURLBSTR;
    }

    return NULL;
}

static HRESULT InvokeHook(IDispatch *This, DISPID dispIdMember, const IID *riid, LCID lcid,
                          WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult,
                          EXCEPINFO *pExcepInfo, UINT *puArgErr) {

    BSTR newURL = NULL;

    for (unsigned int i = 0u; i < pDispParams->cArgs; i++) {

        if (pDispParams->rgvarg[i].vt == VT_BSTR) {

            newURL = RewriteURLW(pDispParams->rgvarg[i].bstrVal);
            if (newURL != NULL) {
                Log("IDispatch->Invoke(%S -> %S)\r\n", pDispParams->rgvarg[i].bstrVal, newURL);
                pSysFreeString(pDispParams->rgvarg[i].bstrVal);
                pDispParams->rgvarg[i].bstrVal = newURL;
            } else {
                Log("IDispatch->Invoke(%S) // (no rewrite rules matched)\r\n",
                    pDispParams->rgvarg[i].bstrVal);
            }
        }
    }

    return pInvoke(This, dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo,
                   puArgErr);
}

static HRESULT STDCALL NavigateHook(IWebBrowser2 *This, BSTR URL, VARIANT *Flags,
                                    VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers) {

    BSTR newURL = NULL;

    newURL = RewriteURLW(URL);
    if (newURL != NULL) {
        Log("IWebBrowser2->Navigate(%S -> %S)\r\n", URL, newURL);
        URL = newURL;
    } else {
        Log("IWebBrowser2->Navigate(%S) // (no rewrite rules matched)\r\n", URL);
    }

    HRESULT hResult = pNavigate(This, URL, Flags, TargetFrameName, PostData, Headers);

    if (newURL != NULL)
        pSysFreeString(newURL);

    return hResult;
}

static HRESULT STDCALL CoGetClassObjectHook(const IID *rclsid, DWORD dwClsContext,
                                            LPVOID pvReserved, const IID *riid, LPVOID *ppv) {

    static void *g_pInvoke = NULL;

    HRESULT ret = pCoGetClassObject(rclsid, dwClsContext, pvReserved, riid, ppv);

    if (ret >= 0) {

        if (memcmp((LPCVOID)rclsid, &kIID_MicrosoftWebBrowser, sizeof(IID)) == 0) {

            IClassFactory *obj = (IClassFactory *)*ppv;

            IDispatch *iDispatch1 = NULL;
            IDispatch *iDispatch2 = NULL;

            HRESULT ret2 =
                obj->lpVtbl->CreateInstance(obj, NULL, &kIID_IObject, (LPVOID *)&iDispatch1);

            if (ret2 < 0) {
                Log("CreateInstance failed. %d\r\n", ret2);
                return ret;
            }

            ret2 = iDispatch1->lpVtbl->QueryInterface(iDispatch1, &kIID_IDispatch,
                                                      (LPVOID *)&iDispatch2);

            if (ret2 < 0) {
                Log("QueryInterface failed. %d\r\n", ret2);
                return ret;
            }

            if (g_pInvoke != iDispatch2->lpVtbl->Invoke) {
                g_pInvoke = (PVOID)iDispatch2->lpVtbl->Invoke;
                pInvoke = (PFNINVOKEPROC)HookFunc(g_pInvoke, (PVOID)InvokeHook);
                Log("Install InvokeHook(0x%08p): 0x%08p -> 0x%08p\r\n", g_pInvoke, InvokeHook,
                    pInvoke);
            }
        }
    }

    return ret;
}

static HRESULT STDCALL CoCreateInstanceHook(const IID *rclsid, LPUNKNOWN pUnkOuter,
                                            DWORD dwClsContext, const IID *riid, LPVOID *ppv) {

    static void *g_pNavigate = NULL;

    HRESULT ret = pCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);

    if (ret >= 0) {

        if (memcmp(rclsid, &kIID_MicrosoftWebBrowser, sizeof(IID)) == 0) {

            IUnknown *obj = (IUnknown *)*ppv;
            IWebBrowser2 *webbrowser2 = NULL;

            HRESULT ret2 =
                obj->lpVtbl->QueryInterface(obj, &kIID_IWebBrowser2, (LPVOID *)&webbrowser2);
            if (ret2 < 0) {
                Log("QueryInterface failed. %d\r\n", ret2);
                return ret;
            }

            if (g_pNavigate != webbrowser2->lpVtbl->Navigate) {
                g_pNavigate = (PVOID)webbrowser2->lpVtbl->Navigate;
                pNavigate = (PFNNAVIGATEPROC)HookFunc(g_pNavigate, (PVOID)NavigateHook);
                Log("Install NavigateHook(0x%08p): 0x%08p -> 0x%08p\r\n", g_pNavigate, NavigateHook,
                    pNavigate);
            }
        }
    }

    return ret;
}

void InitWebBrowserHook() {
    hOle32Module = LoadLib("ole32");
    hOleAut32Module = LoadLib("oleaut32");
    pSysAllocStringLen = (PFNSYSALLOCSTRINGLENPROC)GetProc(hOleAut32Module, "SysAllocStringLen");
    pSysFreeString = (PFNSYSFREESTRINGPROC)GetProc(hOleAut32Module, "SysFreeString");
    pCoGetClassObject = (PFNCOGETCLASSOBJECTPROC)HookProc(hOle32Module, "CoGetClassObject",
                                                          (PVOID)CoGetClassObjectHook);
    pCoCreateInstance = (PFNCOCREATEINSTANCEPROC)HookProc(hOle32Module, "CoCreateInstance",
                                                          (PVOID)CoCreateInstanceHook);
}
