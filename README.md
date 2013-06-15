lua_sample2
===========

# DESCRIPTION

Luaにより与えられる関数をC言語で積分するサンプル(pthreadによる並列化)

# Luaのインストール

# LuaのC言語組み込み用ライブラリの取得

http://luabinaries.sourceforge.net/download.html
* 今回lua-5.2.1_MacOS107_lib.tar.gz をダウンロードして解凍

# ファイル構成

    ├── Makefile
    ├── README.md
    ├── include
    │   ├── lauxlib.h
    │   ├── lua.h
    │   ├── lua.hpp
    │   ├── luaconf.h
    │   └── lualib.h
    ├── integrate.c
    └── pi.lua
