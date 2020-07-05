#!/bin/bash

./ubuntu_build.sh
rm -rf deploy_linux
mkdir deploy_linux
cp build/VeraGraphicsConverter deploy_linux/
cp instres/VeraGraphicsConverter.desktop deploy_linux/
cd deploy_linux

linuxdeployqt-6-x86_64.AppImage VeraGraphicsConverter.desktop -appimage -bundle-non-qt-libs
cd ..
