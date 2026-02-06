#pragma once
#include "Modules/ModuleInterface.h"

// 로딩 스크린 모듈 구현
// 게임플레이 중 로딩 화면의 초기화 및 표시를 처리
class FLoadingScreenModule : public IModuleInterface
{
public:
    // 모듈이 처음 로드될 때 호출됨
    virtual void StartupModule() override;

    // 게임 모듈로 식별, 에디터 모듈이 아님
    virtual bool IsGameModule() const override;

    // 레벨 전환 중에 로딩 화면 표시
    virtual void StartLoadingScreen(const FString& MapName);

private:
    // 배경 텍스처를 저장하여 가비지 컬렉션되는 것을 방지
    UTexture2D* BackgroundTexture;

};