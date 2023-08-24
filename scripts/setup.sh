#!/usr/bin/env bash

rm -rf build && \
mkdir -p .ejs/cache && \
touch .ejs/cache/home.txt &&\
touch .ejs/cache/problemLocation.txt &&\
touch .ejs/cache/wbLocation.txt &&\
touch .ejs/cache/cookie.txt &&\
mkdir -p .ejs/repos &&\
mkdir -p .ejs/archives

export PATH=$PATH:/workspace/build/src