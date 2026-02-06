using UnrealBuildTool;

// 중요: .uproject의 기본 로딩 타이밍보다 먼저 모듈이 로드되어야 함
// 그렇지 않으면 LoadingScreen 초기화가 ShowModule 등을 위해 충분히 일찍 이루어지지 않음
public class LoadingScreenModule : ModuleRules
{
    public LoadingScreenModule(ReadOnlyTargetRules Target) : base(Target)
    {
        // 미리 컴파일된 헤더의 기본 헤더 파일 지정
        PrivatePCHHeaderFile = "Public/LoadingScreen.h";

        // 더 나은 컴파일 시간을 위해 공유 미리 컴파일된 헤더 사용
        PCHUsage = PCHUsageMode.UseSharedPCHs;

        // 중요: UnrealEngine이 로드하는 기본 모듈을 추가하여 private 종속성 초기화
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",         // Core UE 기능
                "CoreUObject",  // 기본 UObject 시스템
                "MoviePlayer",  // !!동영상 재생을 위한 모듈이지만 로딩화면 설정을 담을 구조체가 여기 있고, Texture2D도 지원함!!
                "Engine",       // UTexture2D를 위한 메인 UE 엔진 모듈
                "Slate",        // UI 프레임워크
                "SlateCore"     // Core Slate 기능
            }
        );
    }
}