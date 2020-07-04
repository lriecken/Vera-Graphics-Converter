#!/bin/bash

./mac_build.sh
rm -rf deploy_mac
mkdir deploy_mac
cd deploy_mac
cp -r ../build/VeraGraphicsConverter.app ./
macdeployqt VeraGraphicsConverter.app -dmg
cd ..
