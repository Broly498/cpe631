#!/bin/sh

git submodule update --init
git submodule foreach "git checkout legacy"
