
# Credit to https://patrickhenson.com/2018/06/07/uncrustify-configuration.html for recursive uncrustify command below that I altered
find . \( -name "*.cpp" -o -name "*.c" -o -name "*.h" \) -exec uncrustify -c ./.uncrustify.cfg --no-backup {} +
find . -name '*.uncrustify' -type f | while read NAME ; do mv "${NAME}" "${NAME%.uncrustify}" ; done
