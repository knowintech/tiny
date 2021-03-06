/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2017-8-5
*
* @file   WdcInfo.h
*
* @remark
*
*/

#ifndef __WDC_INFO_H___
#define __WDC_INFO_H___

#include <tiny_base.h>
#include <tiny_typedef.h>

TINY_BEGIN_DECLS


typedef struct _WdcInfo
{
    char        ssid[32];
    char        password[32];
    uint32_t    checksum;
} WdcInfo;

bool WdcInfo_Read(WdcInfo *thiz);

bool WdcInfo_Write(WdcInfo *thiz);


TINY_END_DECLS

#endif /* __WDC_INFO_H___ */
