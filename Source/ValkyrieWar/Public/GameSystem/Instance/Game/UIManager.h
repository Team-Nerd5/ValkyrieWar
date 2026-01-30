// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/GameInstance.h"

#include "GameSystem/Base/BaseWidget.h"
#include "GameSystem/Instance/Game/GameManager.h"

#include "Data/Enums.h"
#include "UIManager.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
private:
    /** UI 레이어에 따라 적절한 ZOrder 계산 (Persistent: 낮음, Popup: 높음) */
    int32 CalculateZOrder(UBaseWidget* Widget) const;

    /** 현재 Popup 스택 상태에 따라 입력 모드 변경 (UIOnly ↔ GameOnly) */
    void NotifyInputModeChange();

    /**
     * 위젯을 캐시에서 가져오거나 없으면 새로 생성
     * - 성능 최적화: 자주 열리는 UI의 생성 비용 절감
     * - 위젯 상태 유지: 닫았다 다시 열어도 이전 상태 보존
     *
     * @param WidgetClass 가져올/생성할 위젯 클래스
     * @return 캐싱된 또는 새로 생성된 위젯 (실패 시 nullptr)
     */
    template<typename T>
    T* GetOrCreateWidgetInternal(TSubclassOf<T> WidgetClassFactory);

    /**
     * 특정 타입의 UI를 닫고 뷰포트에서 제거 (타입 기반)
     * - Persistent: 맵에서 제거
     * - Popup: 스택에서 제거하고 새로운 Top에 FocusGained 알림
     * - 주의: 위젯은 캐시에 유지되므로 완전히 파괴되지 않음
     */
    template<typename T>
    void CloseUIInternal(TSubclassOf<T> targetClassFactory);

    /**
     * UI를 열고 뷰포트에 추가
     * - 이미 열려있으면 기존 인스턴스 반환
     * - Persistent UI는 맵에 저장
     * - Popup UI는 스택에 추가하고 이전 Top에 FocusLost 알림
     *
     * @param TargetClass 열고자 하는 UI 위젯 클래스
     * @return 열린 UI 위젯 인스턴스 (실패 시 nullptr)
     */
    template<typename T>
    T* OpenUIInternal(TSubclassOf<T> TargetClassFactory);

    /**
     * 실제 UI 닫기 작업을 수행하는 내부 헬퍼 함수
     * - 템플릿 CloseUI와 오버로드 CloseUI가 모두 사용
     * - 중복 로직 방지 및 유지보수성 향상
     */
    void CloseUIInternal(UBaseWidget* Widget);

    template<typename T>
    TSubclassOf<T> GetUIClassInternal(E_UITYPE InUIType);

public:
    template<typename T>
    T* OpenUI(E_UITYPE InUIType);

    template<typename T>
    T* GetOrCreateWidget(E_UITYPE InUIType);

    template<typename T>
    void CloseUI(E_UITYPE InUIType);

    /**
     * 위젯 인스턴스를 직접 받아서 닫기 (인스턴스 기반)
     * - 주로 위젯의 닫기 요청 델리게이트 핸들러에서 사용
     * @param Widget 닫고자 하는 위젯 인스턴스
     */
    void CloseUI(UBaseWidget* Widget);

    /**
     * 스택 최상위 Popup만 닫기
     * - ESC 키 바인딩 등에 사용
     * - 스택에서 Pop하고 그 아래 팝업에 포커스 전달
     */
    void CloseTopPopupUI();

    /**
     * 열려있는 모든 Popup UI 닫기
     * - 게임 오버, 씬 전환 등에 사용
     */
    void CloseAllPopupUI();

    /**
     * Popup 스택의 새로운 Top에 포커스 전달
     * - 중간 팝업이 닫혔을 때 호출됨
     */
    void RefreshTopPopupUI();

    /**
    * 레벨 전환시 모든 UI상태 리셋 함수(Controller)
    * - 캐싱된 UI들을 모두 초기화하여 레벨 전환시 재생성 보장.
    */
    void ResetAllUIStates();

    /** Popup UI가 하나라도 열려있는지 확인 */
    FORCEINLINE bool HasOpenPopupUI() const { return PopupUIStack.Num() > 0; }
    FORCEINLINE int GetPopupStackSize() const { return PopupUIStack.Num(); }

private:
    /** 현재 열려있는 Persistent UI들의 맵 (클래스 -> 인스턴스) */
    UPROPERTY()
    TMap<TSubclassOf<UBaseWidget>, UBaseWidget*> PersistentUIMap;

    /**
     * 열려있는 Popup UI들의 스택 (Last = 최상위/포커스 중)
     * 나중에 추가된 것이 위에 표시되고 먼저 입력 받음
     */
    UPROPERTY()
    TArray<UBaseWidget*> PopupUIStack;

    /**
     * 생성된 모든 위젯의 캐시 (클래스 -> 인스턴스)
     * 성능: 매번 CreateWidget 호출 방지
     * 상태 유지: UI를 닫아도 데이터 보존
     */
    UPROPERTY()
    TMap<TSubclassOf<UBaseWidget>, UBaseWidget*> CachedWidgets;
};

// 템플릿 함수 구현

template<typename T>
inline T* UUIManager::GetOrCreateWidget(E_UITYPE InUIType)
{
    TSubclassOf<T> TargetClass = GetUIClass<T>(InUIType);
    if (TargetClass)
    {
        return GetOrCreateWidgetInternal(TargetClass);
    }

    return nullptr;
}

template<typename T>
T* UUIManager::GetOrCreateWidgetInternal(TSubclassOf<T> WidgetClassFactory)
{
    TSubclassOf<UBaseWidget> TargetClassFactory = WidgetClassFactory;

    // 캐싱된 UI가 있으면 반환(클래스 설계도로 실제 인스턴스 키 있는지 검사)
    if (CachedWidgets.Contains(TargetClassFactory))
    {
        return Cast<T>(CachedWidgets[TargetClassFactory]);
    }

    // 없으면 생성 후 캐시에 저장
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        return nullptr;
    }

    T* NewWidget = CreateWidget<T>(PC, WidgetClassFactory);
    if (NewWidget)
    {
        CachedWidgets.Add(TargetClassFactory, NewWidget);
    }
    return NewWidget;
}

template<typename T>
TSubclassOf<T> UUIManager::GetUIClassInternal(E_UITYPE InUIType)
{
    UGameManager* GameManager = Cast<UGameManager>(GetGameInstance());

    if (GameManager)
    {
        return GameManager->GetUIClass(InUIType);
    }

    return nullptr;
}

template<typename T>
T* UUIManager::OpenUI(E_UITYPE InUIType)
{
    TSubclassOf<T> TargetClass = GetUIClass<T>(InUIType);
    if (TargetClass)
    {
        return OpenUIInternal(TargetClass);
    }

    return nullptr;
}

template<typename T>
T* UUIManager::OpenUIInternal(TSubclassOf<T> TargetClassFactory)
{
    if (!TargetClassFactory)
    {
        return nullptr;
    }

    //캐싱된 인스턴스가 있다면 반환, 없으면 생성 후 반환
    T* Widget = GetOrCreateWidget<T>(TargetClassFactory);
    if (!Widget)
    {
        return nullptr;
    }

    // 이미 열려있으면 기존 인스턴스 사용
    if (Widget->IsOpen())
    {
        return Widget;
    }

    UBaseWidget* BaseWidget = Widget;

    // Persistent 타입 UI일 때
    if (BaseWidget->UILayer == EUILayer::PERSISTENT)
    {
        TSubclassOf<UBaseWidget> BaseClassFactory = TargetClassFactory;
        if (!PersistentUIMap.Contains(BaseClassFactory))
        {
            PersistentUIMap.Add(BaseClassFactory, BaseWidget);
        }

        BaseWidget->OpenUI();
        int32 ZOrder = CalculateZOrder(BaseWidget);
        BaseWidget->AddToViewport(ZOrder);
    }
    else // Popup 타입 UI일 때
    {
        // 현재의 Top에 FocusLost 알림
        if (PopupUIStack.Num() > 0)
        {
            PopupUIStack.Last()->OnFocusLost();
        }

        // 스택에 추가
        PopupUIStack.Add(BaseWidget);

        BaseWidget->OpenUI();
        int32 ZOrder = CalculateZOrder(BaseWidget);
        BaseWidget->AddToViewport(ZOrder);

        // 입력 모드 변경
        NotifyInputModeChange();
    }

    return Widget;
}

template<typename T>
inline void UUIManager::CloseUI(E_UITYPE InUIType)
{
    TSubclassOf<T> TargetClass = GetUIClass<T>(InUIType);
    if (TargetClass)
    {
        CloseUIInternal(TargetClass);
    }
}

template<typename T>
void UUIManager::CloseUIInternal(TSubclassOf<T> targetClassFactory)
{
    if (!targetClassFactory)
    {
        return;
    }

    TSubclassOf<UBaseWidget> baseClassFactory = targetClassFactory;

    // 캐싱중인 UI라면 리턴
    if (!CachedWidgets.Contains(baseClassFactory))
    {
        return;
    }

    UBaseWidget* widget = CachedWidgets[baseClassFactory];

    //실제 닫기 로직은 내부 헬퍼함수 호출
    CloseUIInternal(widget);
}
