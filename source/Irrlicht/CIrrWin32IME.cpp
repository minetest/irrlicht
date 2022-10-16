#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_

#include <math.h>
#include <msctf.h>
#include <malloc.h>
#include "os.h"
#include "IEventReceiver.h"
#include "CIrrWin32IME.h"

namespace irr
{
    static HWND                     gDefaultImeWnd = nullptr;
    static HIMC                     gDefaultIMC = nullptr;
    // static HIMC                     gPrevIMC = nullptr;
    static HWND                     gCurrentWnd;
    static bool                     gImeEnabled = false;

    void initIme(HWND value) {
        gCurrentWnd = value;
        gDefaultImeWnd = ImmGetDefaultIMEWnd(nullptr);
        gDefaultIMC = ImmGetContext(value);
        ImmReleaseContext(value, gDefaultIMC);
    }

    bool isImeEnabled() {
        return gImeEnabled;
    }

    void cancelImeEdit() {
        if (!gImeEnabled) return;
        HIMC vIMC = ImmGetContext(gCurrentWnd);
        if (vIMC) {
            ImmNotifyIME(vIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
            ImmReleaseContext(gCurrentWnd, vIMC);
        }
    }

    void clearIMEContent() {
        if (gCurrentWnd) {
            HIMC vIMC = ImmGetContext(gCurrentWnd);
            if (vIMC) {
                ImmSetOpenStatus( vIMC, false );
                ImmReleaseContext(gCurrentWnd, vIMC);
            }
            ImmAssociateContext(gCurrentWnd, nullptr);
        }
    }

    void createIMEContent(HWND hWnd) {
        if (hWnd == nullptr) {
            hWnd = gCurrentWnd;
        } else if (hWnd != gCurrentWnd) {
            gCurrentWnd = hWnd;
        }
        if (hWnd ==nullptr) return;
        if (gDefaultIMC) ImmAssociateContext(gCurrentWnd, gDefaultIMC);

        HIMC vIMC = ImmGetContext(hWnd);
        if (vIMC) {
            ImmSetOpenStatus(vIMC, true);
            ImmReleaseContext(hWnd, vIMC);
        }
    }

    void enableIME(bool enabled, HWND hWnd) {
        if (!enabled) {
            clearIMEContent();
        } else {
            clearIMEContent();
            createIMEContent(hWnd);
        }
        gImeEnabled = enabled;
        return;
    }

    LONG getStringFromIme(HIMC imm_context, WPARAM lparam,
                int type, const wchar_t* result, LONG string_size) {
        if (string_size <= 0)
            return ::ImmGetCompositionStringW(imm_context, type, NULL, 0);

        return ::ImmGetCompositionStringW(imm_context, type, const_cast<wchar_t*>(result), string_size);
    }

    LONG getResultFromIme(HIMC imm_context, WPARAM lparam, const wchar_t* result, LONG string_size) {
        if (!(lparam & GCS_RESULTSTR)) return IMM_ERROR_NODATA;

        LONG ret = getStringFromIme(imm_context, lparam, GCS_RESULTSTR, result, string_size);
        return ret;
    }

    void updateCompositionWindow(void* hWnd, s32 x, s32 y, s32 height)
    {
        if (gCurrentWnd != hWnd) {
            enableIME(gImeEnabled, (HWND)hWnd);
        }
        if (!gImeEnabled) return;

        COMPOSITIONFORM cf;
        static bool bNoReentrance = false;
        if (bNoReentrance) return;
        bNoReentrance = true;
        HIMC hIMC = ImmGetContext(gCurrentWnd);
        if(hIMC)
        {
            cf.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
            cf.ptCurrentPos.x = x;
            cf.ptCurrentPos.y = y;// - height;

            auto lRet = ImmSetCompositionWindow(hIMC, &cf);
            if (!lRet) {
                char s[32];
                snprintf(s, sizeof(s), "ImmSetCompositionWindow failed\n");
                irr::os::Printer::log((char*)&s, irr::ELL_ERROR);
            }
            ImmReleaseContext(gCurrentWnd, hIMC);
        }
        bNoReentrance = false;
    }
}
#endif
