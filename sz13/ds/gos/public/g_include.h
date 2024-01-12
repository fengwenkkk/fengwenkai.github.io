/******************************************************************************
模块名      : public
文件名      : g_include.h
相关文件    :
实现功能    : 平台公共文件
修改记录    :
------------------------------------------------------------------------------
日期        修改人      修改内容
2010/05/08  雷杰        创建
******************************************************************************/
#ifndef _G_INCLUDE_H_
#define _G_INCLUDE_H_

#ifdef __BORLANDC__
#ifdef __cplusplus
#include <vcl.h>
#endif
#endif

#include "gos_typedef.h"
#include "gos_socket.h"
#include "gos_api.h"
#include "gos_mem.h"
#include "gos_func.h"
#include "gos_hash.h"
#include "gos_hashlist.h"
#include "gos_list.h"
#include "gos_time.h"
#include "gos_json.h"
#include "gos_xml.h"
#include "gos_conf_file.h"
#include "gos_file.h"
#include "gos_filedb.h"
#include "gos_ftp.h"

#ifdef __cplusplus
#include <vector>

#include "g_xml.h"
#include "g_json.h"
#include "g_string.h"
#include "g_conf_file.h"
#include "g_file.h"
#include "g_filedb.h"
#include "g_lock.h"
#include "g_thread.h"
#include "g_hash.h"
#include "g_hashlist.h"
#include "g_map.h"
#include "g_list.h"
#include "g_tuple.h"
#include "g_vector.h"
#include "g_mem.h"
#include "g_logger.h"
#include "g_proc.h"
#include "g_array_util.h"

#include "g_serial_port.h"
#include "g_sms.h"
#include "g_mail.h"
#include "g_telnet.h"
#include "g_log.h"

#include "../util/g_string_util.h"

#ifdef USE_GOS_UTILITY
#include "gos_utility/gos_utility.h"
#endif

#endif

#endif /* _G_PUBLIC_H_ */
/******************************* 头文件结束 **********************************/
