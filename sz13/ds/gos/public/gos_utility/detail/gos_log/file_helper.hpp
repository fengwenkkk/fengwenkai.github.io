/**
 * @file            file_helper.hpp
 * @brief           文件操作相关的函数
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2020-07-18 15:35:21
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_FILE_HELPER_H_
#define GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_FILE_HELPER_H_

#include <string>
#include "gos_utility/detail/system_utility.hpp"
#include "gos_utility/detail/string_utility.hpp"

namespace gos
{
/// @name     file_helper
/// @brief    用于实现循环写文件的操作
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:37:55
/// @warning  线程不安全
class file_helper
{
public:
    file_helper() : open_tries_(5), open_interval_(10), fd_(NULL) {}
    ~file_helper()
    {
        close();
    }

    /// @name     open
    /// @brief    打开一个文件
    ///
    /// @param    fname     [in]  文件名称
    /// @param    truncate  [in]  文件是否覆写
    ///
    /// @return   NONE
    ///
    /// @author   Lijiancong, pipinstall@163.com
    /// @date     2020-07-18 16:01:09
    /// @warning  线程不安全
    inline void open(const std::string &fname, bool truncate = false)
    {
        close();
        filename_ = fname;
        const char *mode = truncate ? ("wb") : ("ab");

        for (int tries = 0; tries < open_tries_; ++tries)
        {
            // create containing folder if not exists already.
            gos::create_dir(dir_name(fname));
            if (!gos::fopen_s(&fd_, fname.c_str(), mode))
            {
                return;
            }

            gos::sleep_for_millis(open_interval_);
        }

        throw("Failed opening file " + filename_ + " for writing");
    }

    /// @name     reopen
    /// @brief    重新打开一个文件
    ///
    /// @param    truncate  [in]  是否覆写
    ///
    /// @return   NONE
    ///
    /// @author   Lijiancong, pipinstall@163.com
    /// @date     2020-07-18 16:02:45
    /// @warning  线程不安全
    inline void reopen(bool truncate)
    {
        if (filename_.empty())
        {
            throw("Failed re opening file - was not opened before");
        }
        this->open(filename_, truncate);
    }

    /// @name     flush
    /// @brief    刷新文件流
    ///
    /// @param    NONE
    ///
    /// @return   NONE
    ///
    /// @author   Lijiancong, pipinstall@163.com
    /// @date     2020-07-18 16:04:12
    /// @warning  线程不安全
    inline void flush()
    {
        std::fflush(fd_);
    }

    /// @name     close
    /// @brief    关闭一个文件
    ///
    /// @param    NONE
    ///
    /// @return   NONE
    ///
    /// @author   Lijiancong, pipinstall@163.com
    /// @date     2020-07-18 16:04:49
    /// @warning  线程不安全
    inline void close()
    {
        if (fd_ != NULL)
        {
            std::fclose(fd_);
            fd_ = NULL;
        }
    }

    /// @name     write
    /// @brief    向文件中写入
    ///
    /// @param    buf [in]  要写入的string
    ///
    /// @return   NONE
    ///
    /// @author   Lijiancong, pipinstall@163.com
    /// @date     2020-07-18 16:05:29
    /// @warning  线程不安全
    inline void write(const std::string &buf)
    {
        size_t msg_size = buf.size();
        const char *data = buf.data();
        if (std::fwrite(data, 1, msg_size, fd_) != msg_size)
        {
            throw("Failed writing to file " + (filename_));
        }
    }

    /// @name     size
    /// @brief    获取文件大小
    ///
    /// @param    NONE
    ///
    /// @return   NONE
    ///
    /// @author   Lijiancong, pipinstall@163.com
    /// @date     2020-07-18 16:06:30
    /// @warning  线程不安全
    inline size_t size() const
    {
        if (fd_ == NULL)
        {
            return 0;
            //throw("Cannot use size() on closed file " + (filename_));
        }
        return filesize(fd_);
    }

    const std::string &filename() const
    {
        return filename_;
    }

    //
    // return file path and its extension:
    //
    /// @name     split_by_extension
    /// @brief    分割文件名称成文件路径加名字与其扩展名
    /// @details
    ///          "mylog.txt" => ("mylog", ".txt")
    ///          "mylog" => ("mylog", "")
    ///          "mylog." => ("mylog.", "")
    ///          "/dir1/dir2/mylog.txt" => ("/dir1/dir2/mylog", ".txt")
    //
    ///          the starting dot in filenames is ignored (hidden files):
    //
    ///          ".mylog" => (".mylog". "")
    ///          "my_folder/.mylog" => ("my_folder/.mylog", "")
    ///          "my_folder/.mylog.txt" => ("my_folder/.mylog", ".txt")
    ///
    /// @param    fname [in]  文件名称
    ///
    /// @return   分割后的文件名和扩展名
    ///
    /// @author   Lijiancong, pipinstall@163.com
    /// @date     2020-07-18 16:09:39
    /// @warning  线程不安全
    static void split_by_extension(const std::string &fname, std::string &str1, std::string &str2)
    {
        /// size_t ext_index = fname.rfind('.');
        size_t ext_index = gos::string_rfind(fname, ".");

        // no valid extension found - return whole path and empty string as
        // extension
        if (ext_index == std::string::npos || ext_index == 0 || ext_index == fname.size() - 1)
        {
            str1 = fname;
            str2.clear();
            return;
        }

        // treat cases like "/etc/rc.d/somelogfile or "/abc/.hiddenfile"
        /// size_t folder_index = fname.rfind();
        size_t folder_index = gos::string_rfind(fname, gos::to_string(gos::folder_sep));
        if (folder_index != std::string::npos && folder_index >= ext_index - 1)
        {
            str1 = fname;
            str2.clear();
            return;
        }

        // finally - return a valid base and extension tuple
        str1 = fname.substr(0, ext_index);
        str2 = fname.substr(ext_index);
        return;
    }

private:
    const int open_tries_;
    const int open_interval_;
    std::FILE *fd_;
    std::string filename_;

    file_helper(const file_helper &);
    file_helper &operator=(const file_helper &);
};
}  // namespace gos

#endif  // end of  GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_FILE_HELPER_H_
