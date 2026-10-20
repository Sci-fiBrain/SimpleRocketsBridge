#pragma once

#include <Windows.h>

#include "SimpleRockets/Types.h"

namespace SimpleRockets::Launcher::Infrastructure
{
    /// <summary>
    /// 一对匿名管道句柄
    ///
    /// 写端可被子进程继承, 读端仅在当前进程使用
    /// </summary>
    class PipePair final
    {
    public:
        PipePair();
        ~PipePair();

        PipePair(const PipePair&) = delete;
        PipePair& operator=(const PipePair&) = delete;

        [[nodiscard]] Boolean IsValid() const;
        [[nodiscard]] HANDLE Read() const;
        [[nodiscard]] HANDLE Write() const;

        void Close();

    private:
        HANDLE readEnd;
        HANDLE writeEnd;
    };
}
