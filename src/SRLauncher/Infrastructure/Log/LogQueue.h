#pragma once

#include <memory>
#include <mutex>

#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Infrastructure::Log
{
    /// <summary>
    /// 日志记录的字节环形队列
    ///
    /// 记录头部 4 字节给出整条长度, 队列只按长度搬运, 不解析内容
    /// 尾部放不下一条记录时折返到缓冲区起点, 并在折返点写入 0 长度标记
    /// 容量耗尽时整条丢弃最旧记录, 丢弃量与字节数在下一次写入时合并播报
    /// </summary>
    class LogQueue final
    {
    public:
        /// <summary>capacityBytes 为环形缓冲的字节数</summary>
        explicit LogQueue(UInt32 capacityBytes);

        LogQueue(const LogQueue&) = delete;
        LogQueue& operator=(const LogQueue&) = delete;

        /// <summary>写入一条完整记录, 记录非法或空间不足时返回 0</summary>
        [[nodiscard]] Boolean Push(const Byte* record);

        /// <summary>拷贝出最旧的一条记录, 队列为空或 target 装不下时返回 0</summary>
        [[nodiscard]] Boolean Pop(Byte* target, UInt32 targetCapacity, UInt32& length);

    private:
        /// <summary>折返并丢弃最旧记录直到能容纳 recordLength</summary>
        void EnsureRoom(UInt32 recordLength);

        /// <summary>不覆盖未读数据时能否追加 recordLength 字节</summary>
        [[nodiscard]] Boolean CanAppend(UInt32 recordLength) const;

        /// <summary>丢弃最旧一条记录并累计丢弃量</summary>
        void DropOldest();

        /// <summary>累计丢弃量非零且空间允许时写入播报记录</summary>
        void WriteNotice();

        std::unique_ptr<Byte[]> buffer;
        std::mutex lock;
        UInt32 capacity;
        UInt32 readOffset;
        UInt32 writeOffset;
        UInt32 usedBytes;
        UInt32 droppedRecords;
        UInt32 droppedBytes;
    };
}
