#pragma once

#include "SimpleRockets/SDK/Game/LayerRef.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>
    /// 运行时 HUD 的根层
    ///
    /// 由游戏在场景加载时构造, 内含相机、油门与分级控件、航向圈与六个状态按钮
    /// </summary>
    struct RuntimeLayerRef : LayerRef
    {
        explicit RuntimeLayerRef(void* pointer) noexcept : LayerRef{pointer} { }

        /// <summary>
        /// 游戏模块基址, 由对象头部的虚表指针反解
        /// </summary>
        /// <remarks>调用前 pointer 必须非空, 且对象确为 RuntimeLayer</remarks>
        [[nodiscard]] void* GetModuleBase() const noexcept
        {
            return reinterpret_cast<void*>(Read<UInt32>(pointer, 0) - Layout::RvaVtableRuntimeLayer);
        }
    };
}
