#!/usr/bin/env bash

cmake -B build -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cd build
ctest --output-on-failure
lcov --directory . --capture --output-file coverage.info --rc geninfo_auto_base=1 --ignore-errors mismatch,mismatch
lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory cov-report

# Suppress the Gtk message about not loading modules "atk-bridge" and "canberra-gtk-module".
firefox -new-tab "file:///home/aldo/git/ip-lookup/build/cov-report/index.html" 2>/dev/null
