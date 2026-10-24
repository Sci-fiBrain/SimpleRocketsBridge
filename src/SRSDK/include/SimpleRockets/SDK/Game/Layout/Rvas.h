#pragma once

#include "SimpleRockets/Types.h"

namespace SimpleRockets::SDK::Game::Layout
{
    /// <summary>Runtime::~Runtime, 相对游戏模块基址</summary>
    constexpr Rva RvaHookRuntimeDestructor = 0x3F090;

    /// <summary>Runtime::Initialize, 相对游戏模块基址</summary>
    constexpr Rva RvaHookInitialize = 0x3F230;

    /// <summary>Runtime::HandleUpdate, 相对游戏模块基址</summary>
    constexpr Rva RvaHookHandleUpdate = 0x3F7C0;

    /// <summary>Runtime::SaveXml, 相对游戏模块基址</summary>
    constexpr Rva RvaHookSaveXml = 0x3DF10;

    /// <summary>PodObject 推进分级的入口, 参数 (this)</summary>
    constexpr Rva RvaHookActivateNextStage = 0x5A870;

    /// <summary>RuntimeLayer 构造器, 建完层内全部 HUD 元素后返回 this</summary>
    constexpr Rva RvaHookLayerLoaded = 0x34DA0;

    /// <summary>NavCircle::LockHeading, 以当前航向为锁定朝向, 参数 (this)</summary>
    constexpr Rva RvaNavCircleLockHeading = 0x488D0;

    /// <summary>NavCircle::UnlockHeading, 清锁定标志并恢复手动转向, 参数 (this)</summary>
    constexpr Rva RvaNavCircleUnlockHeading = 0x48740;

    /// <summary>ShipView::ScreenToWorld, 参数 (this, Int32 x, Int32 y), 结果写入出参</summary>
    constexpr Rva RvaShipViewScreenToWorld = 0x4F510;

    /// <summary>PartTree::FindPartTreeAtPoint, 参数 (this, const Vector2D*, bool), 返回命中的零件树</summary>
    constexpr Rva RvaPartTreeFindAtPoint = 0x56520;

    /// <summary>StateButton 构造器, 参数 (this, const std::string& name), 返回 this</summary>
    constexpr Rva RvaStateButtonCtor = 0x4E2A0;

    /// <summary>cfw::Layer 的便捷方法, 参数 (this, child); 内部调槽 6 后自己再调一次布局</summary>
    constexpr Rva RvaLayerAddChildAndLayout = 0xC18E0;

    /// <summary>cfw::Layer 构造器, 参数 (this, Int32), 返回 this</summary>
    constexpr Rva RvaLayerCtor = 0x128B0;

    /// <summary>cfw::Layer 析构, 参数 (this); 删除析构在 0x12960, 它先调这里再 operator delete</summary>
    constexpr Rva RvaLayerDestructor = 0x12990;

    /// <summary>cfw::List&lt;cfw::Layer*&gt; 的虚表, 位于层的 +0x50, 用于反解模块基址</summary>
    constexpr Rva RvaVtableLayerChildren = 0x120F58;

    constexpr Rva RvaLayerTunnelEvent = 0x12AF0;
    constexpr Rva RvaLayerHandleEvent = 0x12B60;
    constexpr Rva RvaLayerBubbleEvent = 0x12B30;
    constexpr Rva RvaLayerAddedToParent = 0x99F0;
    constexpr Rva RvaLayerInsertChild = 0x12CC0;
    constexpr Rva RvaLayerAddChild = 0x12CF0;
    constexpr Rva RvaLayerRemoveChild = 0x12D80;
    constexpr Rva RvaLayerUpdateLayout = 0x12DA0;
    constexpr Rva RvaLayerSetVisible = 0x12E90;
    constexpr Rva RvaLayerSetPadding = 0x13060;
    constexpr Rva RvaLayerWidth = 0x13080;
    constexpr Rva RvaLayerHeight = 0x13090;
    constexpr Rva RvaLayerSetSize = 0x130A0;
    constexpr Rva RvaLayerX = 0x130C0;
    constexpr Rva RvaLayerY = 0x130D0;
    constexpr Rva RvaLayerSetPosition = 0x130E0;
    constexpr Rva RvaLayerCenterInLayer = 0x13100;
    constexpr Rva RvaLayerScreenX = 0x13160;
    constexpr Rva RvaLayerScreenY = 0x13180;
    constexpr Rva RvaLayerIsPointInLayer = 0x131A0;
    constexpr Rva RvaLayerDrawHierarchy = 0x12EB0;
    constexpr Rva RvaLayerDraw = 0x56A0;
    constexpr Rva RvaLayerPostDraw = 0x56A0;
    constexpr Rva RvaLayerHandleTouch = 0x13290;
    constexpr Rva RvaLayerHandleGesture = 0xB900;
    constexpr Rva RvaLayerHandleMultiTouch = 0xB900;
    constexpr Rva RvaLayerHandleUpdate = 0xB900;
    constexpr Rva RvaLayerHandleScroll = 0xB900;
    constexpr Rva RvaLayerHandleBackButton = 0xB910;
    constexpr Rva RvaLayerHandleKey = 0xB900;
    constexpr Rva RvaLayerCanTunnelEventToChildren = 0xA120;

    /// <summary>cfw::Button 的点击分发, 位于虚表槽 33; 也用于反解模块基址</summary>
    constexpr Rva RvaButtonDispatchClick = 0x1E690;

    constexpr Rva RvaVtableRuntime = 0x1248D8;
    constexpr Rva RvaVtableRuntimeLayer = 0x124198;
    constexpr Rva RvaVtableShipView = 0x125BF0;
    constexpr Rva RvaVtableLayer = 0x120ED4;
    constexpr Rva RvaVtableStateButton = 0x125B60;
    constexpr Rva RvaVtableCallback = 0x12421C;
    constexpr Rva RvaVtableNavCircle = 0x1256C4;

    constexpr Rva RvaVtableOrbitNode = 0x126AC4;
    constexpr Rva RvaVtableShipOrbitNode = 0x125AC4;
    constexpr Rva RvaVtablePlanet = 0x1268BC;
    constexpr Rva RvaVtableShip = 0x124670;

    constexpr Rva RvaVtablePartTree = 0x1260C0;
    constexpr Rva RvaVtablePartConnection = 0x126BA0;
    constexpr Rva RvaVtablePartObject = 0x125F10;
    constexpr Rva RvaVtablePodObject = 0x126390;
    constexpr Rva RvaVtableTankObject = 0x126820;
    constexpr Rva RvaVtableEngineObject = 0x12679C;
    constexpr Rva RvaVtableFixtureObject = 0x12BC8C;
    constexpr Rva RvaVtableRcsObject = 0x12BF9C;
    constexpr Rva RvaVtableSolarPanelObject = 0x12BE54;
    constexpr Rva RvaVtableParachuteObject = 0x12BF28;
    constexpr Rva RvaVtableWheelObject = 0x12BDC4;
    constexpr Rva RvaVtableDockPortObject = 0x12BD4C;
    constexpr Rva RvaVtableDockConnectorObject = 0x12BB30;
    constexpr Rva RvaVtableDetacherObject = 0x12BA88;
    constexpr Rva RvaVtableLanderObject = 0x12BC18;
}
