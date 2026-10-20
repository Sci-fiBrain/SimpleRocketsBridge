#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game
{
    /// <summary>Runtime::~Runtime, 相对游戏模块基址</summary>
    constexpr Rva RvaHookRuntimeDestructor = 0x3F090;

    /// <summary>Runtime::Initialize, 相对游戏模块基址</summary>
    constexpr Rva RvaHookInitialize = 0x3F230;

    /// <summary>Runtime::HandleUpdate, 相对游戏模块基址</summary>
    constexpr Rva RvaHookHandleUpdate = 0x3F7C0;

    /// <summary>Runtime::SaveXml, 相对游戏模块基址</summary>
    constexpr Rva RvaHookSaveXml = 0x3DF10;

    /// <summary>分级成功路径, 相对游戏模块基址</summary>
    constexpr Rva RvaHookActivateStage = 0x5A870;
}
