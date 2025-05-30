@echo off
REM Change to script directory
cd /d "%~dp0"

REM Set MSYS2 compiler path
set PATH=%PATH%;C:\msys64\ucrt64\bin

REM Force delete existing executables
taskkill /F /IM manual_test.exe >nul 2>&1
del /F /Q manual_test.exe >nul 2>&1
taskkill /F /IM minimal_test.exe >nul 2>&1
del /F /Q minimal_test.exe >nul 2>&1
taskkill /F /IM queue_demo.exe >nul 2>&1
del /F /Q queue_demo.exe >nul 2>&1
taskkill /F /IM basic_tests.exe >nul 2>&1
del /F /Q basic_tests.exe >nul 2>&1

REM Build and run manual test
echo Building manual_test...
g++ -I implementation/include -I implementation/src manual_test.cpp -o manual_test.exe
if %errorlevel% equ 0 (
    echo Running manual_test...
    manual_test.exe
) else (
    echo Manual test compilation failed
)

REM Build and run minimal test
echo Building minimal_test...
g++ minimal_test.cpp -o minimal_test.exe
if %errorlevel% equ 0 (
    echo Running minimal_test...
    minimal_test.exe
) else (
    echo Minimal test compilation failed
)

REM Build and run queue demo
echo Building queue_demo...
g++ -I implementation/include -I implementation/src queue_demo.cpp -o queue_demo.exe
if %errorlevel% equ 0 (
    echo Running queue_demo...
    queue_demo.exe
) else (
    echo Queue demo compilation failed
)

REM Build and run basic tests
echo Building basic_tests...
g++ -I implementation/include -I implementation/src implementation/tests/basic_tests.cpp -o basic_tests.exe -L C:\msys64\ucrt64\lib -lgtest -lgtest_main -lpthread
if %errorlevel% equ 0 (
    echo Running basic_tests...
    basic_tests.exe
) else (
    echo Basic tests compilation failed
)

REM Pause to keep window open
pause