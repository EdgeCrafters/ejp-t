#!/usr/bin/env bash

rm -rf build && \
mkdir -p .ejp/cache && \
touch .ejp/cache/home.txt &&\
touch .ejp/cache/repo.txt &&\
touch .ejp/cache/problem.txt &&\
touch .ejp/cache/location.txt &&\
touch .ejp/cache/cookie.txt &&\
mkdir -p .ejp/repos &&\
mkdir -p .ejp/archives

export PATH=$PATH:/workspace/build/src