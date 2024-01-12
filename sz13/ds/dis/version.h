/**
 * @file            version.h
 * @brief           用来存放版本信息的文件
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-07-12 15:09:23
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef DS_DIS_VERSION_H__
#define DS_DIS_VERSION_H__

#ifndef PRODUCT_VERSION
#ifndef DIS_VERSION
extern char *g_szVersion;
#define DIS_VERSION g_szVersion
#endif
#else
#define DIS_VERSION PRODUCT_VERSION
#endif

#endif  ///< DS_DIS_VERSION_H__
