#!/bin/bash

virtualenv --python 3 --system-site-packages .venv
source .venv/bin/activate

cd esp-idf
./install.sh
source export.sh
cd ..
