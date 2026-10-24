#pragma once

#include <type_traits>

#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 游戏对象视图的基类, 只持有一个原始指针
    ///
    /// 派生视图按固定偏移读写同一块内存, 对象生命周期由游戏负责
    /// </summary>
    struct Ref
    {
        /// <summary>原始游戏对象指针</summary>
        void* pointer;

        explicit Ref(void* pointer) noexcept : pointer{pointer} { }

        /// <summary>是否未指向任何游戏对象</summary>
        [[nodiscard]] Boolean IsNull() const noexcept
        {
            return pointer == nullptr;
        }

        /// <summary>
        /// 把同一个指针换成更窄的视图, 不校验实际类型
        /// </summary>
        /// <typeparam name="TRef">同一继承链上的 Ref 派生类型, 调用方需自行确认对象确实是该类型</typeparam>
        template <typename TRef> [[nodiscard]] TRef As() const noexcept
        {
            static_assert(std::is_base_of_v<Ref, TRef>, "As 只能在同一 Ref 继承链内转换");
            return TRef{pointer};
        }

    protected:
        /// <summary>取虚表指定槽位的函数地址</summary>
        [[nodiscard]] void* GetSlotFunction(VtableSlot slot) const noexcept
        {
            const UInt32 vtable = Read<UInt32>(pointer, 0);
            return reinterpret_cast<void*>(*reinterpret_cast<const UInt32*>(vtable + slot * sizeof(void*)));
        }
    };
}
