#pragma once

#ifndef VRAM_CFG_H
#define VRAM_CFG_H

#include "g_include.h"

extern "C"
{
int vram_set(char *szCfgFile, char *szKey, char *szValue);
int vram_get(char *szCfgFile, CHAR *szKey, CHAR *szValue, UINT32 ulMaxLen);
int vram_quick_save(int fd, int iPos, char *szValue, int iValueLen);
int vram_del(char *szCfgFile, const char *szKey);
int vram_commit();
int vram_strlen(char *s);
}

#endif
