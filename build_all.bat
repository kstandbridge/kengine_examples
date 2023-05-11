@echo off

pushd kengine_tests
call build.bat
popd
pushd kengine_preprocessor
call build.bat
popd

pushd console_hello
call build.bat
popd
pushd headless_hello
call build.bat
popd
pushd preprocessor_hello
call build.bat
popd
pushd test_hello
call build.bat
popd
pushd window_hello
call build.bat
popd
REM pushd directx_hello
REM call build.bat
REM popd

pushd minesweeper
call build.bat
popd
pushd sim8086
call build.bat
popd

popd