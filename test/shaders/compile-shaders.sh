#!/bin/zsh

SHADERC=../../thirdparty/bgfx/.build/osx-x64/bin/shadercDebug
VS_IN=vs_test.sc
FS_IN=fs_test.sc

VS_OUT=vs_test.bin
FS_OUT=fs_test.bin

$SHADERC -f $VS_IN -o $VS_OUT --type v --platform osx --profile metal
$SHADERC -f $FS_IN -o $FS_OUT --type f --platform osx --profile metal