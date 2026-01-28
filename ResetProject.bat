@echo off
chcp 65001
cls
setlocal enabledelayedexpansion

:: ==============================================================================
:: [중요] 아까 복사한 엔진 경로를 아래에 붙여넣으세요! (따옴표 안에)
:: 예시: set "EngineRoot=C:\Program Files\Epic Games\UE_5.3"
:: ==============================================================================
set "EngineRoot=C:\Program Files\Epic Games\UE_5.5"


:: ==============================================================================
:: 1. 경로 검증 (제대로 입력했는지 확인)
:: ==============================================================================
if not exist "%EngineRoot%" (
    echo [오류] 엔진 경로를 찾을 수 없습니다!
    echo 입력하신 경로: "%EngineRoot%"
    echo.
    echo 배치 파일을 우클릭 - 편집 눌러서 'EngineRoot' 변수에 
    echo 올바른 언리얼 엔진 설치 경로를 넣어주세요.
    pause
    exit /b
)

:: 주요 도구 경로 설정
set "SelectorExe=C:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"
set "BuildBat=%EngineRoot%\Engine\Build\BatchFiles\Build.bat"

if not exist "%SelectorExe%" (
set "SelectorExe=C:\Program Files\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"
)
:: 도구 파일이 진짜 있는지 2차 확인
if not exist "%SelectorExe%" (
    echo [오류] UnrealVersionSelector.exe가 없습니다.
    echo 엔진 경로가 맞는지 다시 확인해주세요.
    pause
    exit /b
)
if not exist "%BuildBat%" (
    echo [오류] Build.bat 파일이 없습니다.
    pause
    exit /b
)

:: ==============================================================================
:: 2. 현재 프로젝트 경로 설정 및 청소
:: ==============================================================================
set "CurrentPath=%~dp0"
echo [엔진 경로] %EngineRoot%
echo [현재 경로] %CurrentPath%
echo.

echo [1단계: 폴더 청소]
set DELETE_LIST=Binaries Intermediate DerivedDataCache
for %%F in (%DELETE_LIST%) do (
    if exist "%CurrentPath%%%F" (
        echo  - 삭제 중: %%F
        rd /s /q "%CurrentPath%%%F"
    )
)

:: ==============================================================================
:: 3. .uproject 파일 찾기
:: ==============================================================================
set "UProjectFile="
set "ProjectName="
for %%A in ("%CurrentPath%*.uproject") do (
    set "UProjectFile=%%~fA"
    set "ProjectName=%%~nA"
    goto :FoundProject
)

:FoundProject
if not defined UProjectFile (
    echo [오류] .uproject 파일을 찾을 수 없습니다.
    pause
    exit /b
)
echo [타겟 프로젝트] !ProjectName!

:: ==============================================================================
:: 4. 프로젝트 파일 생성 (Generate Project Files)
:: ==============================================================================
echo.
echo [2단계: 솔루션 파일 생성]
"%SelectorExe%" /projectfiles "!UProjectFile!"

:: ==============================================================================
:: 5. 빌드 실행 (Build)
:: ==============================================================================
echo.
echo [3단계: 소스 코드 빌드 시작]
echo  - 타겟: !ProjectName!Editor (Win64 / Development)
echo  - 컴파일 중입니다... (창을 끄지 마세요)
echo.

call "%BuildBat%" !ProjectName!Editor Win64 Development "!UProjectFile!" -waitmutex

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [실패] 빌드 중 오류가 발생했습니다!
) else (
    echo.
    echo [성공] 모든 작업이 완료되었습니다!
)

echo.
pause