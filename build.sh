#! /usr/bin/env bash

emcc main.c -o build/index.html -O3 --shell-file html_templates/shell_minimal.html -s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'HEAPU8']" -s EXPORTED_FUNCTIONS='["_malloc", "_main"]' -s USE_SDL=2 -sASSERTIONS --js-library=library.js
