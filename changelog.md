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
- string-separated parameter parser (`paramparse`, `paramparse_n`)
- from now on renamed all string functions from `tou_str#` to `tou_s#`, except functions which correspond to standard library ones
- tou.h version string (`TOU_GET_VERSION`)
- more helper macros (`SILENCE`, `ARR_SIZE`)

- renamed `find_string_start`, `find_string_start_n` => `sfind`, `sfind_n`
- fixed `sfind[_n]` range bug
- searching for more than one string at once (`sfind_multiple`, `sfind_multiple_n`)
- helper iterate function for multiple search (`sfind_iter_multiple`, `sfind_iter_multiple_n`)

v1.5 (20240819)
===
- `llist_append` bugs fixed & simplified
- prepend functions added (`llist_prepend`, `llist_prependone`)
- [!] Simplified the llist api to only take ** where really needed (append, prepend)
  - also simplified the .INI api since it depends on llist
- additional llist helpers (`get_newest`, `get_oldest`, `get_newer`, `get_older`, `next_newer`, `next_older`)

- improved `llist_remove`
- improved llist handling of `LLIST_SINGLE_ELEM`
- llist printing for simple cases (`tou_llist_print`, `tou_llist_print_tail`)
- `tou_llist_gather` convenience alias
- `tou_llist_elem` convenience alias
- now detecting `LLIST_SINGLE_ELEM` and disabling parts that depend on it and fixed functions that unnecessarily depended on both elems

- nullptr convenience define
- `trim_back` now also returning the new '\0' position
- `read_file` now opening files as binary
- `read_fp` added

- each key and value in paramparse is now allocated separately
- added simple params printing (`tou_paramprint`)

- INI files can now be loaded from memory buffer using `ini_parse_buffer` (supports \\n and \\r\\n)
- added tou_queue and tou_stack structures

- updated and improved example file

v1.5.1 (20240820)
===
- surrounded lib with `extern "C"`
- `ini_set` can now be also be used to create empty sections
  - now returns a pointer to the property (key & value) object (or the section object) instead of just value which was just stored
