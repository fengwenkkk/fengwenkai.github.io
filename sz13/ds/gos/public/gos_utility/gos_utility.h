/**
 * @file          gos_utility.h
 * @brief         本文件用来存放李建聪写的通用函数
 * @author        lijiancong(lijiancong@gbcom.com.cn)
 * @version       0.1
 * @date          2021-04-23 17:07:31
 *
 * @copyright     Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_H__
#define GOS_PUBLIC_GOS_UTILITY_H__

#include "gos_utility/detail/macro_switch.hpp"                      ///< 宏定义开关
#include "gos_utility/detail/smart_ptr/shared_ptr.hpp"              ///< C++11 shared_ptr 的实现
#include "gos_utility/detail/smart_ptr/unique_ptr.hpp"              ///< C++11 unique_ptr 的实现
#include "gos_utility/detail/smart_ptr/scoped_ptr.hpp"              ///< scope 智能指针的实现
#include "gos_utility/detail/thread/lock_guard.hpp"                 ///< gos::lock_guard 的实现
#include "gos_utility/detail/thread/mutex_utility.hpp"              ///< gos::mutex 的实现
#include "gos_utility/detail/thread/threadsafe_queue.hpp"           ///< 线程安全队列的实现
#include "gos_utility/detail/thread/threadsafe_stack.hpp"           ///< 线程安全栈的实现
#include "gos_utility/detail/gos_log/log_utility.hpp"               ///< 日志相关工具函数
#include "gos_utility/detail/gos_log/log_stream.hpp"                ///< 日志流工具
#include "gos_utility/detail/optional_utility.hpp"                  ///< gos::optional 的实现
#include "gos_utility/detail/time_utility.hpp"                      ///< 时间的相关函数
#include "gos_utility/detail/ignore_unused.hpp"                     ///< 忽略没有使用的变量的工具函数
#include "gos_utility/detail/parser_interface.hpp"                  ///< 解析特殊字符串的接口
#include "gos_utility/detail/data_structure/array.hpp"              ///< gos::array 的实现
#include "gos_utility/detail/print_version.hpp"                     ///< 打印版本号的函数实现
#include "gos_utility/detail/color_print.hpp"                       ///< 颜色命令行输出的实现
#include "gos_utility/detail/string_utility.hpp"                    ///< 字符串相关工具
#include "gos_utility/detail/input_check_utility.hpp"               ///< 输入字符串检查工具类
#include "gos_utility/detail/erase_if.hpp"                          ///< erase_if 相关工具
#include "gos_utility/detail/empty_utility.hpp"                     ///< empty 相关工具
#include "gos_utility/detail/string_stream.hpp"                     ///< 字符串流工具
#include "gos_utility/detail/data_structure/bimap.hpp"              ///< 双键 map 的实现
#include "gos_utility/detail/base64_utility.hpp"                    ///< base64 编解码的实现
#include "gos_utility/detail/system_utility.hpp"                    ///< 系统相关的函数实现
#include "gos_utility/detail/index_utility.hpp"                     ///< index 分配对象实现
//#include "gos_utility/detail/uuid_utility.hpp"                      ///< uuid 的实现
#include "gos_utility/detail/escape_char_utility.hpp"               ///< 转义字符相关工具的实现
#include "gos_utility/detail/singleton.hpp"                         ///< 单例基类的实现
#include "gos_utility/detail/remove_file_utility.hpp"               ///< 删除文件工具的实现
#include "gos_utility/detail/json_parser_interface.hpp"             ///< Json 解析工具类实现
#include "gos_utility/detail/random_utility.hpp"                    ///< 随机相关函数的实现
#include "gos_utility/detail/crc16_utility.hpp"                     ///< CRC16 的实现
#include "gos_utility/detail/network/net_utility.hpp"               ///< 网络工具函数的实现
//#include "gos_utility/detail/binary_file/binary_file.hpp"           ///< 读写二进制文件工具
#include "gos_utility/detail/noncopyable.hpp"                       ///< 不可拷贝基类对象
#include "gos_utility/detail/memory_check.hpp"                      ///< 内存检查工具
#include "gos_utility/detail/final_action.hpp"                      ///< final_action 的实现
#include "gos_utility/detail/data_structure/circular_queue.hpp"     ///< 循环队列实现
#include "gos_utility/detail/thread/threadsafe_circular_queue.hpp"  ///< 线程安全循环队列的实现
#include "gos_utility/detail/check_delete.hpp"                      ///< 检查并 delete 完整类型的实现
#include "gos_utility/detail/thread/thread_utility.hpp"             ///< 线程相关的工具
#include "gos_utility/detail/debug_utility/debug_utility.hpp"       ///< debug 相关的工具
#include "gos_utility/detail/debug_utility/interval_time.hpp"       ///< 间隔时间记录类
#include "gos_utility/detail/debug_utility/object_live_time.hpp"    ///< 对象生存时间对象
#include "gos_utility/detail/debug_utility/profiler_utility.hpp"    ///< 用于实现profiler的测量工具
#include "gos_utility/detail/debug_utility/stop_watch.hpp"          ///< 用于记录多次时间间隔
#include "gos_utility/detail/debug_utility/tick_count.hpp"          ///< 记录时间的对象
#include "gos_utility/detail/debug_utility/interval_time_singleton_factory.hpp"  ///< 记录时间间隔的工厂

#include "gos_utility/thirdparty/INIReader.h"   ///< 第三方 ini 模块
#include "gos_utility/thirdparty/md5.h"         ///< 第三方 MD5
#include "gos_utility/thirdparty/kmp.h"         ///< 第三方 kmp 算法实现
#include "gos_utility/thirdparty/catch.hpp"     ///< 第三方 单元测试模块
#include "gos_utility/thirdparty/MemoryPool.h"  ///< 第三方 内存池模块

#endif  ///< GOS_PUBLIC_GOS_UTILITY_H__
