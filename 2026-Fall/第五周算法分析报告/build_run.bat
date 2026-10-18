@echo off
chcp 65001 >nul
rem ============================================================================
rem  思考题 6-2「d 叉堆」一键编译 + 运行
rem  需要 MinGW-w64（g++）。若 g++ 不在 PATH，请把下面的 GXX 改成绝对路径。
rem ============================================================================
setlocal
set GXX=g++
if not "%~1"=="" set GXX=%~1

echo [1/5] 编译 test_6_2.exe ...
%GXX% -O2 -std=c++17 -static -o test_6_2.exe test_6_2.cpp || goto :err

echo [2/5] 编译 bench_time.exe ...
%GXX% -O2 -std=c++17 -static -o bench_time.exe bench_6_2.cpp || goto :err

echo [3/5] 编译 bench_count.exe ...
%GXX% -O2 -std=c++17 -static -DDARY_HEAP_COUNT -o bench_count.exe bench_6_2.cpp || goto :err

echo [4/5] 运行正确性验证 ...
test_6_2.exe

echo [5/5] 运行复杂度实测（时间 + 比较次数）...
bench_time.exe
bench_count.exe

echo.
echo 完成：数据在 data\ ，图片请运行  python plot_bench.py
goto :eof

:err
echo 编译失败，请检查 g++ 是否可用。
exit /b 1
