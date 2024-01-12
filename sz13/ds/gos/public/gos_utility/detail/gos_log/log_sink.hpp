/**
 * @file            log_sink.hpp
 * @brief           日志的基础实现类
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2020-07-18 11:39:27
 *
 * @copyright       Copyright (c) 2021 GBCOM
 *
 */

#ifndef DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOG_SINK_HPP__
#define DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOG_SINK_HPP__

#include <string>

#include "gos_utility/detail/gos_log/file_helper.hpp"
#include "gos_utility/detail/thread/lock_guard_utility.hpp"
#include "gos_utility/detail/color_print.hpp"

namespace gos
{
class level_enum
{
public:
    enum
    {
        trace = 0,
        debug = 1,
        info = 2,
        warn = 3,
        error = 4,
        critical = 5,
        off = 6,
        n_levels
    };
};

inline std::string convert_level_enum_to_string(const int &level)
{
    if (level == level_enum::trace)
    {
        return "trace";
    }
    else if (level == level_enum::debug)
    {
        return "debug";
    }
    else if (level == level_enum::info)
    {
        return "info ";
    }
    else if (level == level_enum::warn)
    {
        return "warn ";
    }
    else if (level == level_enum::error)
    {
        return "error";
    }
    else if (level == level_enum::off)
    {
        return "off  ";
    }
    else
    {
        return "n_levels";
    }
}

inline int convert_string_to_level_enum(const std::string &level)
{
    if (level == "trace")
    {
        return level_enum::trace;
    }
    else if (level == "debug")
    {
        return level_enum::debug;
    }
    else if (level == "info")
    {
        return level_enum::info;
    }
    else if (level == "warn")
    {
        return level_enum::warn;
    }
    else if (level == "error")
    {
        return level_enum::error;
    }
    else if (level == "off")
    {
        return level_enum::off;
    }
    else
    {
        return level_enum::n_levels;
    }
}

class empty_mutex
{
public:
    void lock() {}
    void unlock() {}
};

class sink
{
public:
    sink() : log_level_(level_enum::trace), flush_level_(level_enum::trace) {}
    sink(int log_level, int flush_level) : log_level_(log_level), flush_level_(flush_level) {}
    virtual ~sink() {}
    virtual void log(const int level, const std::string &msg) = 0;
    virtual void flush() = 0;

    inline bool should_log(int msg_level) const
    {
        return msg_level >= log_level_;
    }

    inline void set_level(int log_level)
    {
        log_level_ = log_level;
    }

    inline int level() const
    {
        return log_level_;
    }

    inline bool should_flush(int msg_level) const
    {
        return msg_level >= flush_level_;
    }

    inline void set_flush_level(int flush_level)
    {
        flush_level_ = flush_level;
    }

protected:
    // sink log level - default is all
    int log_level_;
    int flush_level_;
};

template <typename Mutex>
class base_sink : public sink
{
public:
    base_sink() {}
    base_sink(int log_level, int flush_level) : sink(log_level, flush_level) {}
    virtual ~base_sink()
    {
        flush();
    }

    void log(const int level, const std::string &msg)
    {
        gos::lock_guard<Mutex> lock(mutex_);
        sink_it_(level, msg);
    }

    void flush()
    {
        gos::lock_guard<Mutex> lock(mutex_);
        flush_();
    }

protected:
    Mutex mutex_;

    virtual void sink_it_(const int level, const std::string &msg) = 0;
    virtual void flush_() = 0;

private:
    base_sink(const base_sink &);
};

template <typename Mutex>
class stdout_sink : public base_sink<Mutex>
{
public:
    virtual void sink_it_(const int level, const std::string &msg)
    {
        std::cout << get_log_time();
        std::cout << convert_level_enum_to_string(level) << ": ";
        size_t begin_pos = msg.find_first_of("[", msg.find_first_of("[") + 1);
        size_t end_pos = msg.find_first_of("]", msg.find_first_of("]") + 1);
        size_t level_str = msg.substr(begin_pos + 1, end_pos - begin_pos - 1);
        std::cout << msg.substr(0, begin_pos + 1);
        gos::printf_color(get_cout_color(level_str), level_str.c_str());
        std::cout << msg.substr(end_pos) << std::endl;
    }
    virtual void flush_() {}

private:
    gos::color get_cout_color(const std::string &color_str)
    {
        if (color_str == "trace")
        {
            return color::ORIGIN;
        }
        else if (color_str == "debug")
        {
            return color::BLUE;
        }
        else if (color_str == "info")
        {
            return color::GREEN;
        }
        else if (color_str == "warn")
        {
            return color::YELLOW;
        }
        else if (color_str == "error")
        {
            return color::RED;
        }
        else if (color_str == "critical")
        {
            return color::RED;
        }
        else
        {
            return color::ORIGIN;
        }
    }
};

#ifdef WIN32
static const char s_acLinkBreak[] = "\r\n";
#else
static const char s_acLinkBreak[] = "\n";
#endif

template <typename Mutex>
class rotating_file_sink : public base_sink<Mutex>
{
public:
    rotating_file_sink(int log_level = level_enum::trace, int flush_level = level_enum::trace,
                       std::string base_filename = std::string("log/detail/detail_log.log"),
                       std::size_t max_size = 1048576 * 50, std::size_t max_files = 10, bool rotate_on_open = false)
        : base_filename_(base_filename),
          max_size_(max_size),
          max_files_(max_files),
          base_sink<Mutex>(log_level, flush_level)
    {
        file_helper_.open(calc_filename(base_filename_, 0));
        current_size_ = file_helper_.size();  // expensive. called only once
        if (rotate_on_open && current_size_ > 0)
        {
            rotate_();
        }
    }

    static inline std::string calc_filename(const std::string &filename, std::size_t index)
    {
        if (index == 0)
        {
            return filename;
        }

        std::string basename, ext;
        file_helper::split_by_extension(filename, basename, ext);
        return basename + gos::to_string(index) + ext;
    }

    inline std::string filename()
    {
        gos::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        return file_helper_.filename();
    }

    virtual void sink_it_(const int level, const std::string &input_msg)
    {
        std::string msg(get_log_time() + convert_level_enum_to_string(level) + ": " + input_msg + s_acLinkBreak);
        /// std::string formatted;
        /// base_sink<Mutex>::formatter_->format(msg, formatted);
        current_size_ += msg.size();
        if (current_size_ > max_size_)
        {
            rotate_();
            current_size_ = msg.size();
        }
        file_helper_.write(msg);
    }

    virtual void flush_()
    {
        file_helper_.flush();
    }

private:
    // Rotate files:
    // log.txt -> log.1.txt
    // log.1.txt -> log.2.txt
    // log.2.txt -> log.3.txt
    // log.3.txt -> delete
    inline void rotate_()
    {
        /// using std::stringo_str;
        /// using path_exists;
        file_helper_.close();
        for (size_t i = max_files_; i > 0; --i)
        {
            std::string src = calc_filename(base_filename_, i - 1);
            if (!path_exists(src))
            {
                continue;
            }
            std::string target = calc_filename(base_filename_, i);

            if (!rename_file_(src, target))
            {
                // if failed try again after a small delay.
                // this is a workaround to a windows issue, where very high rotation
                // rates can cause the rename to fail with permission denied (because of
                // antivirus?).
                sleep_for_millis(100);
                if (!rename_file_(src, target))
                {
                    file_helper_.reopen(true);  // truncate the log file anyway to prevent
                                                // it to grow beyond its limit!
                    current_size_ = 0;
                    throw("rotating_file_sink: failed renaming " + (src) + " to " + (target));
                }
            }
        }
        file_helper_.reopen(true);
    }

    // delete the target if exists, and rename the src file  to target
    // return true on success, false otherwise.
    inline bool rename_file_(const std::string &src_filename, const std::string &target_filename)
    {
        // try to delete the target file in case it already exists.
        (void)gos::remove(target_filename);
        return gos::rename(src_filename, target_filename) == 0;
    }

    std::string base_filename_;
    std::size_t max_size_;
    std::size_t max_files_;
    std::size_t current_size_;
    file_helper file_helper_;  /// 用于打开、写文件的对象
};

}  // namespace gos

#endif  // end of DS_GOS_PUBLIC_GOS_UTILITY_DETAIL_GOS_LOG_LOG_SINK_HPP__
