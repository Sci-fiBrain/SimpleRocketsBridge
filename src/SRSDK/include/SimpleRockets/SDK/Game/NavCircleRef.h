#pragma once

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/SDK/Game/Ref.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 航向锁定控制器, 取自 RuntimeLayerRef::GetNavCircle
    ///
    /// 锁定期间由游戏驱动船体转向锁定朝向, mod 只负责改目标
    /// </summary>
    struct NavCircleRef : Ref
    {
        explicit NavCircleRef(void* pointer) noexcept : Ref{pointer} { }

        /// <summary>当前航向, 弧度; 相对世界坐标轴, 0 为 +x 方向, π/2 为 +y 方向</summary>
        [[nodiscard]] Double GetHeading() const noexcept
        {
            return Read<Double>(pointer, Layout::OffsetNavCircleHeading);
        }

        /// <summary>锁定朝向, 弧度</summary>
        [[nodiscard]] Double GetLockedHeading() const noexcept
        {
            return Read<Double>(pointer, Layout::OffsetNavCircleLockedHeading);
        }

        /// <summary>设置锁定朝向, 弧度; 锁定中时下一帧生效</summary>
        void SetLockedHeading(Double heading) const noexcept
        {
            Write<Double>(pointer, Layout::OffsetNavCircleLockedHeading, heading);
        }

        /// <summary>是否处于锁定状态</summary>
        [[nodiscard]] Boolean GetLocked() const noexcept
        {
            return Read<Boolean>(pointer, Layout::OffsetNavCircleLocked);
        }

        /// <summary>以当前航向作为锁定朝向并进入锁定</summary>
        void LockHeading() const noexcept
        {
            using LockHeadingFunction = void(__fastcall*)(void* self, void* edx);
            Environment::GetAddress<LockHeadingFunction>(Layout::RvaNavCircleLockHeading)(pointer, nullptr);
        }

        /// <summary>退出锁定, 恢复手动转向</summary>
        void UnlockHeading() const noexcept
        {
            using UnlockHeadingFunction = void(__fastcall*)(void* self, void* edx);
            Environment::GetAddress<UnlockHeadingFunction>(Layout::RvaNavCircleUnlockHeading)(pointer, nullptr);
        }
    };
}
