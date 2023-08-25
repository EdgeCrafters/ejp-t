#!/usr/bin/env bash

rm -rf build && \
mkdir -p .ejs/cache && \
touch .ejs/cache/home.txt &&\
touch .ejs/cache/repo.txt &&\
touch .ejs/cache/problem.txt &&\
touch .ejs/cache/location.txt &&\
touch .ejs/cache/cookie.txt &&\
mkdir -p .ejs/repos &&\
mkdir -p .ejs/archives

export PATH=$PATH:/workspace/build/src