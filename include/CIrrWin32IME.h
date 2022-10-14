#ifndef __C_IRR_WIN32_IME_H_INCLUDED__
#define __C_IRR_WIN32_IME_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_

#include <windows.h>

namespace irr
{
  extern "C" IRRLICHT_API void initIme(HWND value);
  extern "C" IRRLICHT_API bool isImeEnabled();
  extern "C" IRRLICHT_API void updateCompositionWindow(void* hWnd, s32 x, s32 y, s32 height);
  extern "C" IRRLICHT_API void cancelImeEdit();
  extern "C" IRRLICHT_API void enableIME(bool enabled, HWND hWnd);
  extern "C" IRRLICHT_API void createIMEContent(HWND hWnd);
  extern "C" IRRLICHT_API void clearIMEContent();
  extern "C" IRRLICHT_API LONG getStringFromIme(HIMC imm_context, WPARAM lparam,
                        int type, const wchar_t* result, LONG string_size);
  extern "C" IRRLICHT_API LONG getResultFromIme(HIMC imm_context, WPARAM lparam, const wchar_t* result, LONG string_size);
}

#endif // _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#endif // __C_IRR_WIN32_IME_H_INCLUDED__
