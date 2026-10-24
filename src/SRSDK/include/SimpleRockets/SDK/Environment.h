#pragma once

#include "SimpleRockets/SDK/Log/ILogger.h"

namespace SimpleRockets::SDK
{
    /// <summary>宿主注入的运行期环境, MOD 通过静态方法取用</summary>
    class Environment final
    {
    public:
        /// <summary>由宿主构造并持有, MOD 不自行创建</summary>
        /// <param name="logger">日志输出</param>
        /// <param name="gameModuleBase">游戏模块基址</param>
        Environment(Log::ILogger& logger, void* gameModuleBase) noexcept
            : logger(&logger), gameModuleBase(gameModuleBase)
        {
        }

        Environment(const Environment&) = delete;
        Environment& operator=(const Environment&) = delete;

        /// <summary>
        /// 登记当前环境
        ///
        /// 宿主与每个 MOD 各调一次, 写入的指针只在各自的模块内可见
        /// </summary>
        /// <param name="environment">宿主创建的实例</param>
        static void Attach(Environment* environment) noexcept
        {
            current = environment;
        }

        /// <summary>已登记的环境, 未登记时为空</summary>
        [[nodiscard]] static Environment* GetCurrent() noexcept
        {
            return current;
        }

        /// <summary>日志输出</summary>
        /// <remarks>未调用 Attach 时解引用空指针</remarks>
        [[nodiscard]] static Log::ILogger& GetLogger() noexcept
        {
            return *current->logger;
        }

        /// <summary>游戏模块基址, 自行调用游戏函数时作为 RVA 的加数</summary>
        /// <remarks>未调用 Attach 时解引用空指针</remarks>
        [[nodiscard]] static void* GetModuleBase() noexcept
        {
            return current->gameModuleBase;
        }

        /// <summary>游戏地址, 基址加 RVA</summary>
        /// <param name="rva">相对游戏模块基址的偏移, 取 Game::Layout 里的常量</param>
        template <typename TPointer>
        [[nodiscard]] static TPointer GetAddress(Rva rva) noexcept
        {
            return reinterpret_cast<TPointer>(static_cast<Byte*>(current->gameModuleBase) + rva);
        }

    private:
        Log::ILogger* logger;
        void* gameModuleBase;

        /// <summary>当前环境, 每个模块各存一份, 均指向宿主实例</summary>
        static inline Environment* current = nullptr;
    };
}
