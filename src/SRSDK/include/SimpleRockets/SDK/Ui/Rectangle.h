#pragma once

#include "SimpleRockets/SDK/Environment.h"
#include "SimpleRockets/SDK/Game/Layout/Offsets.h"
#include "SimpleRockets/SDK/Game/Layout/Rvas.h"
#include "SimpleRockets/SDK/Game/Layout/Sizes.h"
#include "SimpleRockets/SDK/Game/Memory.h"
#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Ui
{
    /// <summary>
    /// 纯色矩形, 对应游戏的 cfw::Rectangle
    ///
    /// 由 mod 创建并独占, 游戏不参与其生命周期;
    /// 没有贴图时只按填充色画一块矩形
    /// </summary>
    class Rectangle final
    {
    public:
        Rectangle() noexcept
        {
            using AllocateFunction = void*(__cdecl*)(Int32 size);
            const AllocateFunction allocate =
                Environment::GetAddress<AllocateFunction>(Game::Layout::RvaOperatorNew);
            void* storage = allocate(Game::Layout::SizeofRectangle);
            if (storage == nullptr)
            {
                return;
            }

            using CtorFunction = void*(__fastcall*)(void* self, void* edx, void* frame);
            Environment::GetAddress<CtorFunction>(Game::Layout::RvaRectangleCtor)(storage, nullptr, nullptr);
            pointer = storage;

            SetColor(static_cast<Byte>((DefaultInnerColor >> 0) & 0xFF),
                static_cast<Byte>((DefaultInnerColor >> 8) & 0xFF),
                static_cast<Byte>((DefaultInnerColor >> 16) & 0xFF),
                static_cast<Byte>((DefaultInnerColor >> 24) & 0xFF));
        }

        ~Rectangle() noexcept
        {
            if (pointer == nullptr)
            {
                return;
            }

            using ReleaseFunction = void(__cdecl*)(void* pointer, Int32 size);
            const ReleaseFunction release =
                Environment::GetAddress<ReleaseFunction>(Game::Layout::RvaOperatorDelete);

            void* frame = Game::Read<void*>(pointer, Game::Layout::OffsetRectangleFrame);
            if (frame != nullptr)
            {
                release(frame, Game::Layout::SizeofFrame);
            }

            void* animation = Game::Read<void*>(pointer, Game::Layout::OffsetRectangleAnimation);
            if (animation != nullptr)
            {
                const UInt32 vtable = Game::Read<UInt32>(animation, 0);
                using DestroyFunction = void(__fastcall*)(void* self, void* edx, Int32 deleteFlag);
                reinterpret_cast<DestroyFunction>(vtable)(animation, nullptr, 1);
            }

            release(pointer, Game::Layout::SizeofRectangle);
        }

        Rectangle(const Rectangle&) = delete;
        Rectangle& operator=(const Rectangle&) = delete;

        /// <summary>构造是否成功, 失败时各方法无动作</summary>
        [[nodiscard]] Boolean IsValid() const noexcept
        {
            return pointer != nullptr;
        }

        /// <summary>相对所属层左下角的位置与尺寸, 单位与层几何相同</summary>
        void SetBounds(Int32 x, Int32 y, Int32 width, Int32 height) noexcept
        {
            if (pointer == nullptr)
            {
                return;
            }

            const float halfWidth = static_cast<float>(width) * 0.5f;
            const float halfHeight = static_cast<float>(height) * 0.5f;
            Game::Write<float>(pointer, Game::Layout::OffsetRectangleCenterX,
                static_cast<float>(x) + halfWidth);
            Game::Write<float>(pointer, Game::Layout::OffsetRectangleCenterY,
                static_cast<float>(y) + halfHeight);
            Game::Write<float>(pointer, Game::Layout::OffsetRectangleWidth, static_cast<float>(width));
            Game::Write<float>(pointer, Game::Layout::OffsetRectangleHeight, static_cast<float>(height));
        }

        /// <summary>内层填充色; 外层保持构造时的不透明黑, 内缩量为 0 时被内层完全覆盖</summary>
        void SetColor(Byte red, Byte green, Byte blue, Byte alpha) noexcept
        {
            if (pointer == nullptr)
            {
                return;
            }

            Game::Write<Byte>(pointer, Game::Layout::OffsetRectangleInnerColorRed, red);
            Game::Write<Byte>(pointer, Game::Layout::OffsetRectangleInnerColorGreen, green);
            Game::Write<Byte>(pointer, Game::Layout::OffsetRectangleInnerColorBlue, blue);
            Game::Write<Byte>(pointer, Game::Layout::OffsetRectangleInnerColorAlpha, alpha);
        }

        /// <summary>填充, 坐标系已由所属层的绘制流程定位到本层左下角</summary>
        void Draw() noexcept
        {
            if (pointer == nullptr)
            {
                return;
            }

            using DrawFunction = void(__fastcall*)(void* self, void* edx);
            Environment::GetAddress<DrawFunction>(Game::Layout::RvaRectangleDraw)(pointer, nullptr);
        }

    private:
        /// <summary>内层填充的默认颜色, 取自原版燃料面板, 即 63% 黑</summary>
        static constexpr UInt32 DefaultInnerColor = 0xA0000000;

        void* pointer = nullptr;
    };
}
