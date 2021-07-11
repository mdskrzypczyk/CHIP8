#!/bin/bash

# Expand '**' recursively
shopt -s globstar

files_to_check=(
    "include/**/*.h"
    "include/**/*.hpp"
    "src/*.[ch]"
    "src/*.[ch]pp"
    "tests/*.[ch]pp"
)

trailing_whitespace_files=()
format_errors_files=()

# clear patch file
patch_file=check-format-$(date +%Y%m%d-%H%M).patch
echo "" > ${patch_file}

for f in ${files_to_check[@]}; do

    [ ! -f $f ] && continue

    # Search for files containing trailing whitespace
    grep '\s$' $f >/dev/null && trailing_whitespace_files+=($f)

    # Search for formatting errors using clang-format
    clang-format $f | diff -q $f - >/dev/null || format_errors_files+=($f)
done

if [ ${#trailing_whitespace_files[@]} -ne 0 ]; then
    printf 'There is some trailing whitespace in the following files:\n'
    printf '  %s\n' "${trailing_whitespace_files[@]}"
fi

if [ ${#format_errors_files[@]} -ne 0 ]; then
    printf 'There are some formatting errors in the following files:\n'
    printf '    %s\n' "${format_errors_files[@]}"
    printf '\n'

    for f in ${format_errors_files[@]}; do
        clang-format $f | diff -u --color $f -
        clang-format $f | diff -uN --color $f - | sed 's|+++ -|+++ '"$f"'|' | sed 's|--- |--- a/|' | sed 's|+++ |+++ b/|' >> ${patch_file}
    done
fi

# Exit with an error in case of whitespace or formatting errors
[ ${#trailing_whitespace_files[@]} -ne 0 ] && exit 1
[ ${#format_errors_files[@]} -ne 0 ] && exit 1
exit 0
