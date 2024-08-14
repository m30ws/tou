# Changelog

v1.0
===
- File operations

v1.1
===
- String operations and helpers

v1.2
===
- Linked list & INI file operations

v1.3 (20240807)
===
- INI api improvements
- `strlower`, `strupper`, `s(tring)append`, `s(tring)prepend`

v1.4 (20240814)
===
- fixed strndup length bug
- forgot to trim_back key while parsing .ini
- string-separated parameter parser
- from now on renamed all string functions from `tou_str#` to `tou_s#`, except functions which correspond to standard library ones
- tou.h version string (`TOU_GET_VERSION`)
- more helper macros (`SILENCE`, `ARR_SIZE`)

- renamed `find_string_start`, `find_string_start_n` => `sfind`, `sfind_n`
- fixed `sfind[_n]` range bug
- searching for more than one string at once (`sfind_multiple`, `sfind_multiple_n`)
- helper iterate function for multiple search (`sfind_iter_multiple`, `sfind_iter_multiple_n`)
