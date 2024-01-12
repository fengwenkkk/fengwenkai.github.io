/**
 * @file            profiler_utility.hpp
 * @brief           ����ʵ��profiler�Ĳ�������
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @version         0.1
 * @date            2021-04-13 10:38:22
 * @copyright       Copyright (c) 2021 GBCOM
 */

#ifndef GOS_UTILITY_PROFILER_UTILITY_HPP__
#define GOS_UTILITY_PROFILER_UTILITY_HPP__

#include "gos_utility/detail/macro_switch.hpp"
#include "gos_utility/detail/string_utility.hpp"
#include "gos_utility/detail/gos_log/log_stream.hpp"
#include "gos_utility/detail/debug_utility/tick_count.hpp"

namespace gos
{
/**
 * @brief           �������ܹ��ߣ����ڼ�¼ǰ��ʱ����ڴ�Ķ���
 * @author          lijiancong(lijiancong@gbcom.com.cn)
 * @date            2022-02-10 09:35:52
 * @note
 */
class ProfilerImpl
{
public:
    ProfilerImpl() : m_i64Tid(0), m_iLine(0), m_bHaveLineInfo(false)
    {
        clear();
    }

    /// ��ղ�������
    void clear()
    {
        m_tick_count.clear();
        m_ulStartMemUsage = 0;
        m_ulEndMemUsage = 0;
    }

    /// ������Ϣ
    void SetLineInfo(const std::string &strFuncName, const std::string &strFileName, INT32 iLine, const INT64 &tid,
                     const std::string &strHintInfo = "")
    {
        clear();
        m_strFunctionName = strFuncName;
        m_strFileName = gos_get_simple_file_name(strFileName.c_str());
        m_iLine = iLine;
        m_strHintInfo = strHintInfo;
        m_i64Tid = tid;
        m_bHaveLineInfo = true;
    }

    /// ��ʼ��ʱ
    void Start()
    {
        if (!gos_get_proc_mem_usage(&m_ulStartMemUsage))
        {
            m_ulStartMemUsage = 0;
        }
        m_tick_count.start();
    }

    /// ֹͣ��ʱ
    void Finish()
    {
        m_tick_count.finish();
        if (!gos_get_proc_mem_usage(&m_ulEndMemUsage))
        {
            m_ulEndMemUsage = 0;
        }
    }

    /// ��ȡ��־�ı�
    std::string GetLogString() const
    {
        std::string strLog = GetTimeString() + " " + GetMemoryString();

        if (!m_strHintInfo.empty())
        {
            strLog += " Info: " + m_strHintInfo;
        }

        strLog += GetLineInfoString();

        return strLog;
    }

    /// ��ӡ��־
    void Report(const std::string &strLog)
    {
        LOGD << GetLogString();
    }

    /// ��ȡʱ���(��λ: ����)
    uint64_t GetTimeDiffMillisecond() const
    {
        return m_tick_count.get_ms();
    }

    /// ��ȡʱ���(��λ: ΢��)
    uint64_t GetTimeDiffMicrosecond() const
    {
        return m_tick_count.get_us();
    }

    /// ��ȡ�ڴ��ֵ(��λ: KB)
    int32_t GetMemoryDiffKb() const
    {
        if (m_ulStartMemUsage < m_ulEndMemUsage)
        {
            return m_ulEndMemUsage - m_ulStartMemUsage;
        }
        else
        {
            return -1 * (m_ulStartMemUsage - m_ulEndMemUsage);
        }
    }

private:
    tick_count m_tick_count;        ///< ���ʱ���¼����
    UINT32 m_ulStartMemUsage;       ///< ��ʼ�ڴ�ʹ����
    UINT32 m_ulEndMemUsage;         ///< �����ڴ�ʹ����
    INT64 m_i64Tid;                 ///< �����������ĸ� thread ��
    std::string m_strFunctionName;  ///< ����������
    std::string m_strFileName;      ///< �ļ�������
    INT32 m_iLine;                  ///< �ļ�������
    bool m_bHaveLineInfo;           ///< �Ƿ��Ѿ�����������Ϣ
    std::string m_strHintInfo;      ///< ��ʾ��Ϣ

    /// ��ȡʱ�����ı�
    std::string GetTimeString() const
    {
        std::string strTime = m_tick_count.get_time_string();
        /// ����
        while (strTime.size() < 8)
        {
            strTime = " " + strTime;
        }
        std::string strRet("Spend Time: ");
        strRet += strTime;

        return strRet;
    }

    /// ��ȡ�ڴ�仯�ı�
    std::string GetMemoryString() const
    {
        std::string strMemoryString;
        strMemoryString += " Memory Change: " + gos::to_string(m_ulStartMemUsage / 1024) + " ~ " +
                           gos::to_string(m_ulEndMemUsage / 1024) + " KB";
        return strMemoryString;
    }

    /// ��ȡ����Ϣ�ı�
    std::string GetLineInfoString() const
    {
        /// ������Ϣʱ�������Ϣ
        if (m_bHaveLineInfo)
        {
            std::string strLine;
            strLine += gos::get_log_line_info(m_strFunctionName, m_strFileName, m_iLine, m_i64Tid);
            return strLine;
        }
        else
        {
            return "";
        }
    }
};

class ProfilerObj
{
public:
    ProfilerObj(const std::string &strFuncName, const std::string &strFileName, INT32 iLine, const INT64 &tid,
                const std::string &strHintInfo = "")
    {
        Impl.SetLineInfo(strFuncName, strFileName, iLine, tid, strHintInfo);
        /// ���캯�����һ�л�ȡ��ǰʱ��
        Impl.Start();
    }

    ~ProfilerObj()
    {
        Impl.Finish();
        Impl.Report(Impl.GetLogString());
    }

private:
    ProfilerImpl Impl;
};

/**
 * @brief ��ȡĳ��������ִ��ʱ��
 *
 * @pre ʾ��
 * @code
 * int f() {
 *  PROFILER(LOG);
 *  /// do something
 * }
 * @endcode
 * ����f��ִ��ʱ���ͨ��LOG��־����ָ���ӡ����
 *
 */
#ifdef USE_GOS_PROFILER
#define PROFILER() ::gos::ProfilerObj gos__profiler__impl__(__FUNCTION__, __FILE__, __LINE__, ::gos::get_tid())

#define PROFILER_INFO(hint_info) \
    ::gos::ProfilerObj gos__profiler__impl__(__FUNCTION__, __FILE__, __LINE__, ::gos::get_tid(), hint_info)
#else
#define PROFILER()
#define PROFILER_INFO(hint_info)
#endif
}  // namespace gos

#endif  ///< GOS_UTILITY_PROFILER_UTILITY_HPP__
