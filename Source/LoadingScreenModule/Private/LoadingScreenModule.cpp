#include "LoadingScreenModule.h"
#include "SLoadingScreen.h"
#include "MoviePlayer.h"
#include "LevelLoadingSettings.h"

void FLoadingScreenModule::StartupModule()
{
    // 모듈 초기화 시 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("로딩 스크린 모듈::StartupModule"));

    // 로딩 화면 설정 가져오기 시도
    if (ULevelLoadingSettings* Settings = GetMutableDefault<ULevelLoadingSettings>())
    {
        // 설정에서 배경 이미지 경로 가져오기
        const FSoftObjectPath& BGPath = Settings->BackgroundImage;

        // 배경 이미지가 지정되어 있는지 확인
        if (!BGPath.IsNull())
        {
            // 텍스처를 동기적으로 로드하여 저장
            // StaticLoadObject 매개변수:
            // 1. 로드할 클래스 (UTexture2D)
            // 2. 외부 객체 (nullptr = 기본값)
            // 3. 에셋 경로
            BackgroundTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *BGPath.ToString()));
        }
    }
}

bool FLoadingScreenModule::IsGameModule() const
{
    // 런타임 모듈임을 식별, 에디터 전용이 아님
    return true;
}

void FLoadingScreenModule::StartLoadingScreen(const FString& MapName)
{
    // 로딩 화면이 요청될 때 로그 출력
    UE_LOG(LogTemp, Warning, TEXT("로딩 스크린 모듈::StartLoadingScreen"));

    // 로딩 화면 설정 가져오기
    ULevelLoadingSettings* Settings = GetMutableDefault<ULevelLoadingSettings>();

    // 이 맵이 로딩 화면을 표시해야 하는지 확인
    bool bShouldShowLoadingScreen = false;

    // 로딩 화면을 표시해야 하는 모든 맵을 순회
    for (const FSoftObjectPath& MapPath : Settings->MapsWithLoadingScreens)
    {
        // 현재 맵 이름이 우리 목록의 어떤 것과 일치하는지 확인
        // 경로 형식에 더 유연하도록 Contains() 사용 - 문자열 매칭
        if (MapPath.GetAssetPathString().Contains(MapName))
        {
            bShouldShowLoadingScreen = true;
            break;
        }
    }

    // 이 맵이 로딩 화면을 표시하지 않아야 하면 조기 종료
    if (!bShouldShowLoadingScreen)
    {
        return;
    }

    // 로딩 화면 설정을 모두 담을 구조체 생성
    FLoadingScreenAttributes LoadingScreen;
    
    // 로딩이 완료되었을 때 자동으로 로딩 화면을 제거하지 않음
    LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
    
    // 로딩 화면이 표시될 최소 시간 설정 (초 단위)
    // 빠른 로딩 시 "깜빡이는" 로딩 화면을 방지함
    LoadingScreen.MinimumLoadingScreenDisplayTime = Settings->MinimumLoadingScreenDisplayTime;
    
    // 로딩 화면에 사용할 위젯 설정
    LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreen).BackgroundTexture(BackgroundTexture);
    
    // 이러한 설정으로 로딩 화면을 표시하도록 MoviePlayer에 지시
    GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);

}

// 언리얼 엔진의 모듈 시스템에 이 모듈을 등록
IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreenModule);