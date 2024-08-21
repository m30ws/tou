/** 
	@file tou.h
	@brief Tou header library
	@author m30ws
	@version 1.5.1 (20240820)

	Git repo: https://github.com/m30ws/tou/

	Details
	-------

	Required always but only once! :
	- \#define TOU_IMPLEMENTATION
	
	Other various defines:
	- \#define TOU_LLIST_SINGLE_ELEM
	
	Things:
	- full linked list impl (todo: improve/cleanup error checking)
	- reading file in blocks (filename or FILE*)
	- string splitter, trimmer etc. different operations
	- .INI file parser / exporter(+JSON,XML)
	- safer string functions
	- disabling And restoring stdout
	- xml parser (todo)
	- statically allocated linked list (todo)
	- asset embedding (todo)
	
	- A ton of helper definitions
*/

#ifndef __TOU_H_
#define __TOU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* == Debug options and helpers == */
/**
	@addtogroup grp_debug Debug options
	@{
*/

/** A unique tou.h version identifier */
#define TOU_GET_VERSION() (20240820L)

/**
	@brief Control library debug output & debug code blocks
*/
#ifndef TOU_DBG
#define TOU_DBG 0
#endif

/**
	@brief Place a block of code inside that you wish to enable if debugging
 
	It's not a \#define but should get optimized
*/
#ifndef TOU_DEBUG
#define TOU_DEBUG(...) if (TOU_DBG) { __VA_ARGS__; } else (void)0
#endif

/**
	@brief Debug prefix before messages used in PRINTD()
*/
#ifndef TOU_DEBUG_PREFIX
#define TOU_DEBUG_PREFIX "[:] "
#endif

/**
	@brief Debug print, printf()-like but used for logging internals
*/
#ifndef TOU_PRINTD
#define TOU_PRINTD(format, ...) if (TOU_DBG) { fprintf(stdout, TOU_DEBUG_PREFIX format, ##__VA_ARGS__); } else (void)0
#endif

/**
	@brief User logging macro, similar to ESP_LOGI on ESP32 or dlog_print on Tizen
*/
#ifndef TOU_LOG
#define TOU_LOG(TAG, format, ...) fprintf(stdout, "[%s] " format "\n", TAG, ## __VA_ARGS__);
#endif

/** @} */

/** @cond */
#ifdef _WIN32
#include <io.h>
#define _TOU_DEVNULL_FILE "NUL:"
#else
#include <unistd.h>
#include <fcntl.h> // O_WRONLY
#define _TOU_DEVNULL_FILE "/dev/null"
#endif
/** @endcond */


/* == Various definitions == */
/**
	@addtogroup grp_various Various definitions
	@{
*/

/** @brief `nullptr` def in case i felt like using it */
#ifndef nullptr
#define nullptr ((void*)0)
#endif

/** @brief How many bytes to read when reading a file if unspecified */
#ifndef TOU_DEFAULT_BLOCKSIZE
#define TOU_DEFAULT_BLOCKSIZE 4096
#endif

/** @brief Data format version when exporting INI to JSON */
#define TOU_JSON_DATA_VER "1.0"

/** @brief Data format version when exporting INI to JSON */
#define TOU_XML_DATA_VER "1.0"

/**
	@brief Helps convert 's' into a literal
*/
#ifndef TOU_STR
#define TOU_STR(s) #s
#endif
/**
	@brief Helps convert 's' into C literal

	Ex. MSTR(0xFFBA) -> "0xFFBA"
	    MSTR(abcdef) -> "abcdef"
*/
#ifndef TOU_MSTR
#define TOU_MSTR(s) TOU_STR(s)
#endif

/**
	@brief Is character a whitespace char?
*/
#ifndef TOU_IS_BLANK
#define TOU_IS_BLANK(c) ((c)==' ' || (c)=='\n' || (c)=='\r' || (c)=='\t')
#endif

/**
	@brief Size of array, should also work with const char* []
*/
#ifndef TOU_ARRSIZE
#define TOU_ARRSIZE(x) (sizeof(x) / sizeof(x[0])) // /sizeof(__typeof__(x[0])))
#endif

/**
	@brief Quick and simple random integer

	Outputs values in range from `mn` (inclusive) to `mx` (not inclusive).

	@param[in] mx Upper bound (not included)
	@param[in] mn Lower bound (included)
*/
#ifndef TOU_RANDINT
#define TOU_RANDINT(mx, mn) ((int)(((float)rand()/RAND_MAX) * ((mx)-(mn)) + (mn)))
#endif

/**
	@brief Can be used while iterating to indicate user wishing to abort or continue as normal.
*/
enum tou_iter_action {
	TOU_BREAK    = 0,
	TOU_CONTINUE = 1,
};


/* Func ptrs */

/**
	@brief Arbitrary function pointer with 1 parameter
*/
typedef void* (*tou_func)(void*);

/**
	@brief Arbitrary function pointer with 2 parameters
*/
typedef void* (*tou_func2)(void*, void*);

/**
	@brief Arbitrary function pointer with 3 parameters
*/
typedef void* (*tou_func3)(void*, void*, void*);


/** @} */


/* == Linked list definitions == */
/**
	@ingroup grp_llist
	@{
*/

/**
	@brief Linked list struct definition, typedef'd
*/
typedef struct tou_llist
{
	struct tou_llist* prev;     /**< previous element                     */
	struct tou_llist* next;     /**< next element                         */
	void* dat1;                 /**< useful data                          */
#ifndef TOU_LLIST_SINGLE_ELEM
	void* dat2;                 /**< useful data                          */
#endif
	char destroy_dat1/* : 1*/;  /**< automatically deallocate this data ? */
#ifndef TOU_LLIST_SINGLE_ELEM
	char destroy_dat2/* : 1*/;  /**< automatically deallocate this data ? */
#endif
} tou_llist_t;

/** Make an element alias for convenience */
typedef struct tou_llist tou_llist_elem;


/** @} */


// ==============================================================
// ||                        FUNC DECLS                        ||
// ==============================================================

#ifdef __cplusplus
extern "C" {
#endif

/* == String functions == */
/**
	@addtogroup grp_string String operations
	@{
*/

/*
 * strlcpy() and strcat() functions were acquired from
 * Todd Miller under the following license:
 *
 * Copyright (c) 1998, 2015 Todd C. Miller <millert@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Also see: https://www.millert.dev/papers/strlcpy
 */

/**
	@brief Copies string SRC to DST, always places terminator.
	
	If SRC is longer than SIZE - 1
	characters, only SIZE - 1 characters are copied.  A null
	terminator is always written to DST, unless SIZE is 0.
	Returns the length of SRC, not including the null terminator.
	
	strlcpy() is not in the standard C library, but it is an
	increasingly popular extension.  See
	~~http://www.courtesan.com/todd/papers/strlcpy.html~~ https://www.millert.dev/papers/strlcpy for
	information on strlcpy().
	
	@param[out] dst Destination buffer
	@param[in] src Source string
	@param[in] size Max size to be copied *including* null terminator
	@return Length of the copied string
*/
size_t tou_strlcpy(char* dst, const char* src, size_t size);

/**
	@brief Concatenates string SRC to DST, always places terminator.
	
	The concatenated string is
	limited to SIZE - 1 characters.  A null terminator is always
	written to DST, unless SIZE is 0.  Returns the length that the
	concatenated string would have assuming that there was
	sufficient space, not including a null terminator.
	
	strlcat() is not in the standard C library, but it is an
	increasingly popular extension.  See
	~~http://www.courtesan.com/todd/papers/strlcpy.html~~ https://www.millert.dev/papers/strlcpy for
	information on strlcpy().
	
	@param[out] dst Destination buffer
	@param[in] src Source string
	@param[in] size Max size after concatenation *including* null terminator
	@return Length of the string after concatenation
*/
size_t tou_strlcat(char* dst, const char* src, size_t size);

/**
	@brief Same as strlen() but checks for null pointer.

	@param[in] str String, may be NULL
	@return Length of the string
*/
size_t tou_strlen(const char* str);

/**
	@brief Strdup implementation; returns a malloc'd copy of the string.
	
	@param[in] src String to be duplicated
	@return Pointer to the newly allocated copy of the string
*/
char* tou_strdup(const char* src);

/**
	@brief Returns a newly allocated copy of a string, capped at 'size' length
	(not including '\0'), or NULL if unable to allocate. Appends NUL at the end.
	
	@param[in] src String to copy from
	@param[in] size Max copied bytes from string
	@return Pointer to a newly allocated string
*/
char* tou_strndup(const char* src, size_t size);

/**
	@brief Finds the first occurence of `ch` in the string and returns pointer
	to it, or NULL if not found.

	Character `ch` is converted and compared as `char`.

	@param[in] src String to look in
	@param[in] ch Character to search for
	@return Pointer to the character found, or NULL
*/
char* tou_strchr(const char* src, int ch);

/**
	@brief Finds the last occurence of `ch` in the string and returns pointer
	to it, or NULL if not found.

	Character `ch` is converted and compared as `char`.

	@param[in] src String to look in
	@param[in] ch Character to search for
	@return Pointer to the character found, or NULL
*/
char* tou_strrchr(const char* src, int ch);

/**
	@brief Trims whitespaces from front and back of the string.
	       !! This is a destructive operation since trim_back is destructive. !!
	
	@param[in] str Pointer to the string to be trimmed
	@return Pointer to the new start in string
*/
char* tou_trim_string(char** str);

/**
	@brief Trims whitespaces only from the front of the string.
	
	@param[in] str Pointer to the string to be trimmed
	@return Pointer to the new start in string
*/
char* tou_trim_front(char** str);

/**
	@brief Trims whitespaces only from the back of the string.
	!! This is a destructive operation !!
	
	@param[in] str Pointer to the string to be trimmed
	@return Pointer to the character-after-last ('\0')
*/
char* tou_trim_back(char** str);

/**
	@brief Returns pointers to the first and last
	character that are not whitespaces.
	
	@param[in] str Pointer to the string to be trimmed
	@param[out] start Pointer to the start of contents
	@param[out] end Pointer to the byte after the contents (or NUL)
*/
void tou_trim_string_pure(char* str, char** start, char** end);

/**
	@brief Returns pointer to the first character from
	the front that isn't a whitespace.
	
	@param[in] str Pointer to the string to be trimmed
	@return Pointer to first non-whitespace char
*/
char* tou_trim_front_pure(char* str);

/**
	@brief Returns pointer to the first byte AFTER the
	contents of the string, or the NUL byte.
	
	@param[in] str Pointer to the string to be trimmed
	@return Pointer to the first trimmed byte (or NUL)
*/
char* tou_trim_back_pure(char* str);

/**
	@brief Converts all characters in `str` to lowercase in-place.

	@param[in,out] str String to convert
	@return Pointer to the same string
*/
char* tou_slower(char* str);

/**
	@brief Converts all characters in `str` to uppercase in-place.

	@param[in,out] str String to convert
	@return Pointer to the same string
*/
char* tou_supper(char* str);

/**
	@brief Replaces all occurences of `oldch` with `newch`

	Be careful not to pass pointer like this:
	```c
	char* ss = "aeiouoiea/1/2/3";
	```
	as it will segfault (it is read-only). Instead pass string which looks like this:
	```c
	char ss[] = "aeiouoiea/1/2/3";
	```

	@param[in] ss String in which to replace occurences
	@param[in] oldch Character to replace
	@param[in] newch Character with which to replace `oldch`
	@return Original `ss` pointer
*/
char* tou_replace_ch(char* ss, char oldch, char newch);

/**
	@brief Finds start of substring(keyword) in the given char*.
	
	@param[in] str Source string
	@param[in] kwd Which keyword to search for
	@return Pointer to the beginning of the keyword in the text or NULL
*/
char* tou_sfind(const char* str, const char* kwd);

/**
	@brief Finds start of substring(keyword) in the given char*, looking
	at a max of `maxlen` characters.
	
	@param[in] str Source string
	@param[in] kwd Which keyword to search for
	@param[in] maxlen Looks only at the first `maxlen` characters
	@return Pointer to the beginning of the keyword in the text or NULL
*/
char* tou_sfind_n(const char* str, const char* kwd, size_t maxlen);

/**
	@brief Like ::tou_sfind but searches for more than one string at a time.

	`found_idx` may be NULL if you don't need to know which keyword was found.

	@param[in] str String to search in
	@param[in] kwds Array of char* keywords to search for
	@param[in] n_kwds Count of keywords to search for in `kwds`
	@param[out] found_idx Which of the strings in `kwds` was found
	@return Pointer to the first found string
*/
char* tou_sfind_multiple(const char* str, const char** kwds, int n_kwds, int* found_idx);

/**
	@brief Like ::tou_sfind_n but searches for more than one string at a time.

	`found_idx` may be NULL if you don't need to know which keyword was found.

	@param[in] str String to search in
	@param[in] kwds Array of char* keywords to search for
	@param[in] n_kwds Count of keywords to search for in `kwds`
	@param[out] found_idx Which of the strings in `kwds` was found
	@param[in] maxlen Max characters from `str` to consider
	@return Pointer to the first found string
*/
char* tou_sfind_multiple_n(const char* str, const char** kwds, int n_kwds, int* found_idx, size_t maxlen);

/**
	@brief Iteratively finds occurences of strings in `kwds`
			and calls user function for each one.

	The callback receives the following args:
	- `found` [in] Pointer to the keyword found in `str`
	- `kwd` [in] Pointer to that keyword in `kwds`
	- `userdata` [in,out] User data passed at the beginning

	@param[in] src String to search in
	@param[in] kwds Array of char* keywords to search for
	@param[in] n_kwds Count of keywords to search for in `kwds`
	@param[in] cb User callback called for each found token
	@param[in] userdata User data passed to callback
*/
void tou_sfind_iter_multiple(const char* src, const char* kwds[], int n_kwds, tou_func3 cb, void* userdata);

/**
	@brief Iteratively finds occurences of strings in `kwds`
			and calls user function for each one, up to `maxlen`
			characters in `src`.

	The callback receives the following args:
	- `found` [in] Pointer to the keyword found in `str`
	- `kwd` [in] Pointer to that keyword in `kwds`
	- `userdata` [in,out] User data passed at the beginning

	@param[in] src String to search in
	@param[in] kwds Array of char* keywords to search for
	@param[in] n_kwds Count of keywords to search for in `kwds`
	@param[in] cb User callback called for each found token
	@param[in] userdata User data passed to callback
	@param[in] maxlen Max characters from `str` to consider
*/
void tou_sfind_iter_multiple_n(const char* src, const char* kwds[], int n_kwds, tou_func3 cb, void* userdata, size_t maxlen);

/**
	@brief Splits string using a delimiter that may be longer than one character.

	@param[in] str String to be split
	@param[in] delim Delimiter string
	@return Linked list containing tokens
*/
tou_llist_t* tou_split(char* str, const char* delim);

/**
	@brief (Re)allocates enough memory for src and appends it to dst.

	Does not know the size of the original destination and will not
	consider it!

	@param[in] dst String which will be expanded and copied into
	@param[in] src String to copy
	@return Realloc'd pointer with combined strings
*/
char* tou_sappend(char* dst, char* src);

/**
	@brief (Re)allocates enough memory for additional character
	src and appends it to dst.

	Does not know the size of the original destination and will not
	consider it!

	@param[in] dst String which will be expanded and copied into
	@param[in] src Character to copy
	@return Realloc'd pointer with combined strings
*/
char* tou_sappendch(char* dst, char src);

/**
	@brief (Re)allocates enough memory for src and prepends it to dst.

	Does not know the size of the original destination and will not
	consider it!

	@param[in] dst String which will be expanded and copied into
	@param[in] src String to copy
	@return Realloc'd pointer with combined strings
*/
char* tou_sprepend(char* dst, char* src);

/**
	@brief (Re)allocates enough memory for additional character
	src and prepends it to dst.

	Does not know the size of the original destination and will not
	consider it!

	@param[in] dst String which will be expanded and copied into
	@param[in] src String to copy
	@return Realloc'd pointer with combined strings
*/
char* tou_sprependch(char* dst, char src);

/**
 * @brief Helper for `tou_sreplace` that takes the whole string into consideration.
 * 
 * @param[in,out] str String to be searched for tokens
 * @param[in] repl_str String to replace
 * @param[in] with_str String to replace with
 * @return Pointer to the newly allocated replaced string
 *
 */
char* tou_sreplace(char* str, char* repl_str, char* with_str);

/**
 * @brief Replaces string `repl_str` with `with_str` in the `str`,
 *        testing `str` up to the character `*len_ptr`.
 * 
 * When all tokens that can be found are replaces, the remainder of the `str`
 * (until '\0' is found) is copied to the end of the buffer.
 * If parameter `len_ptr` is passed as NULL or set to 0, the whole string will be taken into
 * consideration using strlen. If given, it will also be set to new length after replacing.
 * Cutting off at length is performed by temporarily modifying original string and placing
 * '\0' at that position, which is removed right after no more tokens to be replaced can be found.
 * 
 * [!] If you use this function in a loop and keep reassigning new pointers to it
 *     don't forget the original pointer will be invalid at the end! (either 
 *     reassign the new pointer to it or don't use it after anymore.)
 * 
 * @param[in,out] str String to be searched for tokens
 * @param[in] repl_str String to replace
 * @param[in] with_str String to replace with
 * @param[in,out] len_ptr Pointer to the length variable
 * @return Pointer to the newly allocated replaced string
 * 
 */
char* tou_sreplace_n(char* str, char* repl_str, char* with_str, size_t* len_ptr);

/**
	@brief Checks if length of given string element (.dat1/2)
	       is zero after trimming it from the start.
	
	@param[in] elem llist element to check
	@param[in] dat One of: `dat1`, `dat2`
	@return Positive if dat1/2 is empty, 0 otherwise
*/
#ifndef tou_sisempty
#define tou_sisempty(elem, dat) \
	((elem) == NULL /* wanted to do without this one but i can already see segfaults happening without it */ || \
	 (elem)->dat == NULL || \
	 tou_strlen(tou_trim_front_pure((elem)->dat)) == 0)
#endif


/** @} */


/* == File operations == */
/**
	@addtogroup grp_file File operations
	@{
*/

/**
	@brief Reads file in blocks, calling user-provided function for each block.

	Automatically allocates memory and returns amount read.
	Function receives 3 parameters:
	- `data` [in] Pointer to new data
	- `len` [in] Amount of bytes actually read
	- `userdata` [in,out] User data provided

	Look into this function's source and/or ::tou_block_store_cb and
	::tou_block_store_struct for more info about using it.

	@param[in] fp FILE* from which data is to be read
	@param[in] blocksize Size in bytes; set to 0 to use default (::TOU_DEFAULT_BLOCKSIZE)
	@param[in] cb Function to call for each block
	@param[in] userdata Custom data to be passed to function
	@return Total bytes read
*/
size_t tou_read_fp_in_blocks(FILE* fp, size_t blocksize, tou_func3 cb, void* userdata);

/**
	@addtogroup grp_file_lower Lower file operations
	@{
*/

/**
	@brief Struct containing buffer and count to be used in block-reading

	This struct is automatically instantiated by ::tou_read_file and passed to
	::tou_read_fp_in_blocks, but may also be used manually for any purpose.
*/
typedef struct {
	char* buffer; /**< Buffer to append data to */
	size_t size; /**< Length of the buffer contents */
} tou_block_store_struct;

/**
	@brief Appends the given block to the buffer in
	`tou_block_store_struct` through realloc().

	Memory for '\0' at the end will be allocated, but will not be
	indicated in `.size` parameter. This function is automatically
	passed to ::tou_read_fp_in_blocks by ::tou_read_file, but may
	also be used manually for any purpose.

	@param[in] blockdata Pointer to the beginning of new data
	@param[in] len Amount of bytes actually read
	@param[in] userdata User data provided
	@return Whether to continue with read iterations (::tou_iter_action)
*/
void* tou_block_store_cb(void* blockdata, void* len, void* userdata);

/** @} */

/**
	@brief Reads file from `filename`.

	Automatically allocates memory and optionally
	returns amount read (may be set to null).

	@param[in] filename Either file name or ""/"stdin" to read from stdin
	@param[out] read_len Optional pointer to where to store file size
	@return Pointer to loaded file or NULL if error
*/
char* tou_read_file(const char* filename, size_t* read_len);

/**
	@brief Reads from `fp`.

	Automatically allocates memory and optionally
	returns amount read (may be set to null).

	@param[in] fp FILE* to read from
	@param[out] read_len Optional pointer to where to store file size
	@return Pointer to loaded data or NULL on error
*/
char* tou_read_fp(FILE* fp, size_t* read_len);


/* == System/IO control == */
/**
	@addtogroup grp_file_sysio System/IO control
	@{
*/

/**
	@brief Redirects STDOUT to `/dev/null` (or `NUL:` on Windows)

	Return value should be saved if you wish to
	restore STDOUT function.

	@return New file descriptor for the saved STDOUT
*/
int tou_disable_stdout();

/**
	@brief Re-enables STDOUT that had been disabled using
	the saved file descriptor from previous function.

	@param[in] saved_fd Saved STDOUT fd
*/
void tou_enable_stdout(int saved_fd);

/**
	@brief Shorthand for surrounding a piece of code with ::tou_disable_stdout and ::tou_enable_stdout
*/
#ifndef TOU_SILENCE
#define TOU_SILENCE( ... ) \
	_tou_g_saved_stdout = tou_disable_stdout(); \
	__VA_ARGS__ ; \
	tou_enable_stdout(_tou_g_saved_stdout);
#endif

/**
	Filedescriptor handle local to this compilation unit used with ::TOU_SILENCE.
	Since it isn't surrounded by `#ifdef TOU_IMPLEMENTATION` it'll be declared each time you \#include the header.
*/
static int _tou_g_saved_stdout;


/** @} */


/** @} */


/* == Linked list == */
/**
	@addtogroup grp_llist Linked list
	@{
*/

/**
	@cond
*/
/** 
	@brief Address of `dat1` or `dat2` parameter of object.
	
	Used when not a simple address of dat1/dat2 where you have tou_llist_t* is required, but rather when
	you get list as double ptr and you want changes you make to the inner pointer reflect in
	the original place instead of just locally. This construct is used multiple times when dealing
	with .INI parser and I got tired of typing it and decided to extract it here.
	
	Example: when `section` is a simple pointer you would use:
	
		tou_llist_t* list = &(section->dat2); // parentheses for clarity
	
	however, if you wanted to reflect changes in the `section` ptr you would have to have `section` as a pointer-to-pointer and:
	
		tou_llist_t** list = &((*section)->dat2);
	
	which is exactly what this macro does, just professionally cluttered.
*/
#ifndef TOU_LLIST_DAT_ADDR
#define TOU_LLIST_DAT_ADDR(list, param) ((tou_llist_t**)(&((*(list))->param)))
#endif
/**
	@endcond
*/

/** 
	@brief Creates/initializes new llist.
	
	Define creation of a new llist through \#define so even though it
	currently doesn't require anything more than =NULL it will be
	easier to extend in the future if a need for such initializer arises.
*/
#ifndef tou_llist_new
#define tou_llist_new(...) ((TOU_DBG ? (fprintf(stdout,TOU_DEBUG_PREFIX"Creating new llist :: NULL\n"), (void)0) : (void)0) , NULL)
#endif

/* */
// typedef tou_llist_t* tou_llist_elem;
/* */
// #define TOU_LLIST_UNPACK(elem) (*(elem))->dat1,(*(elem))->dat2

/**
	@brief Appends a brand new element to the list/given element and updates it in place.
	
	Additionally, returns a pointer to the newly created element.

	@param[in,out] elem Address of the element (tou_llist_t**)
	@param[in] dat1 Data 1
	@param[in] dat2 Data 2
	@param[in] dat1_is_dynalloc Should dat1 be free()'d when destroying list?
	@param[in] dat2_is_dynalloc Should dat2 be free()'d when destroying list?
	@return Pointer to the newly created element
*/

tou_llist_t* tou_llist_append
(
	tou_llist_t** elem,
	void* dat1,
#ifndef TOU_LLIST_SINGLE_ELEM
	void* dat2,
#endif
	char dat1_is_dynalloc
#ifndef TOU_LLIST_SINGLE_ELEM
	, char dat2_is_dynalloc
#endif
);

/**
	@brief Appends a brand new element to the list/given element which
	contains only `dat1` and if the pointer was the head, updates it in place.
	
	`dat2` is set to a default of NULL and 0.
	Additionally, returns a pointer to the newly created element.

	@param[in,out] elem Address of the element (tou_llist_t**)
	@param[in] dat1 Data 1
	@param[in] dat1_is_dynalloc Should dat1 be free()'d when destroying list?
	@return Pointer to the newly created element
*/
tou_llist_t* tou_llist_appendone(tou_llist_t** elem, void* dat1, char dat1_is_dynalloc);

/**
	@brief Prepends a brand new element to the list/given element.
	
	Additionally, returns a pointer to the newly created element.

	@param[in,out] elem Address of the element (tou_llist_t**)
	@param[in] dat1 Data 1
	@param[in] dat2 Data 2
	@param[in] dat1_is_dynalloc Should dat1 be free()'d when destroying list?
	@param[in] dat2_is_dynalloc Should dat2 be free()'d when destroying list?
	@return Pointer to the newly created element
*/

tou_llist_t* tou_llist_prepend
(
	tou_llist_t** elem,
	void* dat1,
#ifndef TOU_LLIST_SINGLE_ELEM
	void* dat2,
#endif
	char dat1_is_dynalloc
#ifndef TOU_LLIST_SINGLE_ELEM
	, char dat2_is_dynalloc
#endif
);

/**
	@brief Prepends a brand new element to the list/given
	element which only contains `dat1`.
	
	`dat2` is set to a default of NULL and 0.
	Additionally, returns a pointer to the newly created element.

	@param[in,out] elem Address of the element (tou_llist_t**)
	@param[in] dat1 Data 1
	@param[in] dat1_is_dynalloc Should dat1 be free()'d when destroying list?
	@return Pointer to the newly created element
*/
tou_llist_t* tou_llist_prependone(tou_llist_t** elem, void* dat1, char dat1_is_dynalloc);

/**
	@brief Traverses elements using .next or .prev and frees each one
	including copied category string

	@param[in] list Either head or tail element
*/
void tou_llist_destroy(tou_llist_t* list);

/**
	@brief Removes an element from llist and deallocates it

	If you were to for example remove the head you should assign
	the return value to something or else you will be left with
	an invalid pointer and a segfault because this function simply
	reconnects .prev and .next if they exist, and frees the element.
	
	If `elem` was the head, function will return .prev to be used as
	the new head. Otherwise, it will return .next which may be used
	or ignored.

	@param[in] elem Pointer to the element to be removed
	@return Element that should optionally take `elem`'s place
*/
tou_llist_t* tou_llist_remove(tou_llist_t* elem);

/**
	@brief Disconnects an element from llist but does not deallocate it.
	You should destroy it by calling ::tou_llist_free_element or manually.
	
	@param[in] elem Pointer to the element to be removed
	@return Pointer to the element popped from llist
*/
tou_llist_t* tou_llist_pop(tou_llist_t* elem);

/**
	@brief Frees an element and its contents.

	Do NOT use on an element still associated with list;
	use only on a ::tou_llist_pop'd elem for example.

	@param[in] elem Pointer to the element to be freed
*/
void tou_llist_free_element(tou_llist_t* elem);

/**
	@brief Traverses the whole list using `.next` and returns the last element
	that was added into list (where `.next` is NULL)

	@param[in] list Element of type tou_llist_t*
	@return Pointer to the head element
*/
tou_llist_t* tou_llist_get_head(tou_llist_t* list);

/**
	@brief Traverses the whole list using `.prev` and returns the first element
	that was added to list (where `.prev` is NULL)

	@param[in] list Element of type ::tou_llist_t*
	@return Pointer to the tail element
*/
tou_llist_t* tou_llist_get_tail(tou_llist_t* list);

/**
	@brief Returns the element that was added to the list last.
	
	Alias for ::tou_llist_get_head.

	@param[in] list Element of type ::tou_llist_t*
	@return Pointer to the newest element
*/
tou_llist_t* tou_llist_get_newest(tou_llist_t* list);

/**
	@brief Returns the element that was added to the list first.
	
	Alias for ::tou_llist_get_tail.

	@param[in] list Element of type ::tou_llist_t*
	@return Pointer to the oldest element
*/
tou_llist_t* tou_llist_get_oldest(tou_llist_t* list);

/**
	@brief Returns the element added after `elem`.

	Returns `.next` of the `elem`.

	@param[in] elem Element of type ::tou_llist_t*
	@return Pointer to the requested element
*/
tou_llist_t* tou_llist_get_newer(tou_llist_t* elem);

/**
	@brief Returns the element added before `elem`.

	Returns `.prev` of the `elem`.

	@param[in] elem Element of type ::tou_llist_t*
	@return Pointer to the requested element
*/
tou_llist_t* tou_llist_get_older(tou_llist_t* elem);

/**
	@brief Updates `elem` to point to the element added after itself.

	`elem` is updated to `.next` of the `elem`.
	That same `.next` is additionally returned from function.

	@param[in,out] elem Address of the element
	@return Pointer to the element
*/
tou_llist_t* tou_llist_next_newer(tou_llist_t** elem);

/**
	@brief Updates `elem` to point to the element added before itself.

	`elem` is updated to `.prev` of the `elem`.
	That same `.next` is additionally returned from function.

	@param[in,out] elem Address of the element
	@return Pointer to the element
*/
tou_llist_t* tou_llist_next_older(tou_llist_t** elem);

/**
	@brief Checks if element is head of the list
	(`.next` should be NULL)

	@param[in] elem Element to check
	@return 1 or 0 depending on result
*/
char tou_llist_is_head(tou_llist_t* elem);

/**
	@brief Checks if element is tail of the list
	(`.prev` should be NULL)

	@param[in] elem Element to check
	@return 1 or 0 depending on result
*/
char tou_llist_is_tail(tou_llist_t* elem);

/**
	@brief Iterate through and call the specified function for each element
	Automatically checks whether given element is head or tail and 
	iterates accordingly.
	If given function returns a 0 the iteration terminates early.

	@param[in] list Head or tail of the list
	@param[in] cb Function to be called for each element
*/
void tou_llist_iter(tou_llist_t* list, tou_func cb);

/**
	@brief Traverses the list starting from the passed element and tries
	to find the first matching element by simple equals-comparison
	using both parameters at the same time.

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@param[in] dat2 Comparison value (if enabled)
	@return Found element or NULL
*/
tou_llist_t* tou_llist_find_exact
(
	tou_llist_t* list,
	void* dat1
#ifndef TOU_LLIST_SINGLE_ELEM
	, void* dat2
#endif
);

/**
	@brief Traverses the list starting from the passed element and tries
	to find the first matching element by simple equals-comparison of param.

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@return Found element or NULL
*/
tou_llist_t* tou_llist_find_exactone(tou_llist_t* list, void* dat1);

/**
	@brief Traverses the list starting from the passed element and tries
	to find the first matching element on the basis: if dat1 is not null,
	tries to match it as a nul-terminated string

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@return Found element or NULL
*/
tou_llist_t* tou_llist_find_key(tou_llist_t* list, void* dat1);

/**
	@brief Traverses the list starting from the passed element and tries
	to find the first matching element by checking the return value
	of the given callback comparison function.

	If function's return value equals 0 the element is considered matched
	and the search terminates, otherwise search continues.

	@param[in] list Element to start the search from
	@param[in] cb Custom comparison function
	@param[in] userdata Custom data to be given to the cb() function
	@return Found element or NULL
*/
tou_llist_t* tou_llist_find_func(tou_llist_t* list, tou_func2 cb, void* userdata);

/**
	@brief Goes through the list and returns amount of elements.

	@param[in] list List to count elements
	@return Number of elements
*/
size_t tou_llist_len(tou_llist_t* list);

/**
	@brief Constructs a dynamically allocated ARRAY containing
	only .dat1 properties (stored as void*).

	@param[in] list Linked list to construct array from
	@param[out] len Optional length of the array/llist
	@return Void* array of .dat1's
*/
void** tou_llist_gather_dat1(tou_llist_t* list, size_t* len);

/**
	@brief Constructs a dynamically allocated ARRAY containing
	only .dat2 properties (stored as void*).

	@param[in] list Linked list to construct array from
	@param[out] len Optional length of the array/llist
	@return Void* array of .dat2's
*/
#ifndef TOU_LLIST_SINGLE_ELEM
void** tou_llist_gather_dat2(tou_llist_t* list, size_t* len);
#endif

/** @brief Convenience alias for gathering first element */
#define tou_llist_gather tou_llist_gather_dat1

/**
	@brief Macro that goes through the elements from head to tail and prints
	their `.dat1` and `.dat2` fields using given format specifiers.

	If TOU_LLIST_SINGLE_ELEM is defined it will accept only `dat1_spec`
	and print only `.dat1` (`dat2_spec` will be ignored if specified).

	It is not enough if for example your wish is to print the value
	of `int*`, but if you just want that pointer, string or if you
	stored value itself into the field it will come in handy.

	@param[in] llist List to print
	@param[in] dat1_spec Format specifier for `.dat1`
	@param[in] dat2_spec Format specifier for `.dat2`
*/
#ifndef tou_llist_print
#ifndef TOU_LLIST_SINGLE_ELEM

	#define tou_llist_print(llist, dat1_spec, dat2_spec, ...)                          \
	{                                                                                  \
		tou_llist_t* copy = tou_llist_get_newest(llist);                               \
		printf("List contents: (%d)\n", tou_llist_len(copy));                          \
		while (copy) {                                                                 \
			printf("  |\n  " dat1_spec " :: " dat2_spec "\n", copy->dat1, copy->dat2); \
			copy = tou_llist_get_older(copy);                                          \
		}                                                                              \
	} (void)0

#else

	#define tou_llist_print(llist, dat1_spec, ...)            \
	{                                                         \
		tou_llist_t* copy = tou_llist_get_newest(llist);      \
		printf("List contents: (%d)\n", tou_llist_len(copy)); \
		while (copy) {                                        \
			printf("  |\n  " dat1_spec "\n", copy->dat1);     \
			copy = tou_llist_get_older(copy);                 \
		}                                                     \
	} (void)0

#endif
#endif

/**
	@brief Macro that goes through the elements from tail to head and prints
	their `.dat1` and `.dat2` fields using given format specifiers.

	If TOU_LLIST_SINGLE_ELEM is defined it will accept only `dat1_spec`
	and print only `.dat1` (`dat2_spec` will be ignored if specified).

	It is not enough if for example your wish is to print the value
	of `int*`, but if you just want that pointer, string or if you
	stored value itself into the field it will come in handy.

	@param[in] llist List to print
	@param[in] dat1_spec Format specifier for `.dat1`
	@param[in] dat2_spec Format specifier for `.dat2`
*/
#ifndef tou_llist_print_tail
#ifndef TOU_LLIST_SINGLE_ELEM

	#define tou_llist_print_tail(llist, dat1_spec, dat2_spec, ...)                 \
	{                                                                              \
		tou_llist_t* copy = tou_llist_get_oldest(llist);                           \
		printf("List contents: (%d)\n", tou_llist_len(copy));                      \
		while (copy) {                                                             \
			printf("  |\n  "dat1_spec" :: "dat2_spec"\n", copy->dat1, copy->dat2); \
			copy = tou_llist_get_newer(copy);                                      \
		}                                                                          \
	} (void)0

#else

	#define tou_llist_print_tail(llist, dat1_spec, ...)       \
	{                                                         \
		tou_llist_t* copy = tou_llist_get_oldest(llist);      \
		printf("List contents: (%d)\n", tou_llist_len(copy)); \
		while (copy) {                                        \
			printf("  |\n  "dat1_spec"\n", copy->dat1);       \
			copy = tou_llist_get_newer(copy);                 \
		}                                                     \
	} (void)0

#endif
#endif


/** @} */


/* == Stack == */
/**
	@addtogroup grp_stack Stack
	@{
*/

/** @brief Make a stack alias for convenience */
typedef tou_llist_t tou_stack;

/**
	@brief Creates a new stack object

	@return Stack object
*/
tou_llist_t* tou_stack_new();

/**
	@brief Destroys existing stack object

	@param[in] stack Object to destroy
*/
void tou_stack_destroy(tou_llist_t* stack);

/**
	@brief Returns the length of the stack

	@param[in] stack Stack object
	@return Stack length
*/
size_t tou_stack_len(tou_llist_t* stack);

/**
	@brief Pushes a new element to the top of the stack

	@param[in] stack Stack object
	@param[in] elem Element to push 
*/
void tou_stack_push(tou_llist_t** stack, void* elem);

/**
	@brief Pops an element from the top of the stack

	@param[in] stack Pointer to the stack object
	@return Element
*/
void* tou_stack_pop(tou_llist_t** stack);


/** @} */


/* == Queue == */
/**
	@addtogroup grp_queue Queue
	@{
*/

/** @brief Make a queue alias for convenience */
typedef tou_llist_t tou_queue;

/**
	@brief Creates a new queue object

	@return Queue object
*/
tou_llist_t* tou_queue_new();

/**
	@brief Destroys existing queue object

	@param[in] queue Object to destroy
*/
void tou_queue_destroy(tou_llist_t* queue);

/**
	@brief Returns the length of the queue

	@param[in] queue Queue object
	@return Queue length
*/
size_t tou_queue_len(tou_llist_t* queue);

/**
	@brief Pushes a new element to the top of the Queue

	@param[in] queue Queue object
	@param[in] elem Element to push 
*/
void tou_queue_push(tou_llist_t** queue, void* elem);

/**
	@brief Pops an element from the top of the Queue

	@param[in] queue Pointer to the queue object
	@return Element
*/
void* tou_queue_pop(tou_llist_t** queue);


/** @} */


/* == INI parser == */
/**
	@addtogroup grp_ini INI operations
	Loading, saving and working with .ini files.

	Loaded data is internally structured as a linked list (::tou_llist_t)
	of INI sections with first parameter being the section name and second
	being a sub-linked-list containing its properties where first and
	second property represent the property's key and value respectively.
	You may get a relatively similar visual by calling ::tou_ini_print.
	```
	<SECTION> -(first)-> "section 1"
	 :     |
	 :  (second)
	 :     |
	 :     +-> <PROPERTY> -(first)-> "persons name"
	 :     :        |
	 :     :     (second)
  	 :     :        |
	 :     :        +-> "John"
	 :     :
	 :     +-> <PROPERTY> -> "persons age"
	 :             |     
     :             +-> "654"
	 :
	<SECTION> -> "section 2"
	 :     |
	 :     +-> <PROPERTY> -> "Text contents"
	 :     :        |
	 :     .        +-> "Lorem ipsum dolor sit amet,    and  even  more  text ...     "
     .       
	```

	@{
*/

#ifdef TOU_LLIST_SINGLE_ELEM
#warning TOU_LLIST_SINGLE_ELEM DEFINED, TOU_INI DISABLED.
#else


/**
	@brief Parses given FILE* as .INI file and constructs structured data.

	Parser supports using semicolon (;) and hashtag (#) for comments,
	however they must be on a separate line by themselves.
	Specifying the same section name more than once is not supported (that
	may change in the future where the definitions would merge and "flatten"
	into one combined section).
	
	Whitespaces may be added between sections, properties and section name
	brackets for better readability. When specifying properties both keys
	and values may have *some* whitespaces in them.
	However, all whitespaces at the beginning and the end of the *key* will be
	removed, as well as all whitespaces at the end of the *value*. If the value
	starts with a whitespace, only that single whitespace will be removed.
	Keep in mind that keys having whitespaces inside of them will be sensitive
	to them when reading the data.

	Structured data is constructed as two layers of llists where the primary list
	that gets returned from function represents sections in .INI file where the
	section names and properties are stored in .dat1 and .dat2 fields respectively.
	Properties in .dat2 are stored as sub-llist where keys and values are allocated
	and are stored as strings in .dat1 and .dat2 fields respectively.

	@param[in] fp File pointer to where to read the .INI data from
	@return Newly allocated structured data
*/
tou_llist_t* tou_ini_parse_fp(FILE* fp);

/**
	@brief Parses given buffer as .INI file and constructs structured data.

	[!] Modifies buffer.
	Refer to ::tou_ini_parse_fp docs for more info about how data is parsed.

	@param[in] buf Buffer to read from
	@return Newly allocated structured data
*/
tou_llist_t* tou_ini_parse_buffer(char* buf);

/**
	Parses a single line as .INI section/property and adds it to structure.

	[!] Modifies data.
	Refer to ::tou_ini_parse_fp docs for more info about how data is parsed.

	@param[in,out] inicontents Structure to which append
	@param[in] line Buffer containing line to be parsed
	@return Continue parsing (::TOU_CONTINUE) or parsing error encountered (::TOU_BREAK)
*/
int tou_ini_parse_line(tou_llist_t** inicontents, char* line);

/**
	@brief Destroys contents of .INI structure including deallocation
	of inner structures; assumes head was passed.

	@param[in] inicontents Parsed INI structure
*/
void tou_ini_destroy(tou_llist_t* inicontents);

/**
	@brief Prints the contents of the parsed .INI structure
	to stdout in a structured graphical format.

	@param[in] inicontents Parsed INI structure
*/
void tou_ini_print(tou_llist_t* inicontents);

/**
	@addtogroup grp_ini_lower Lower level functions
	@{
*/

/**
	@brief Returns pointer to the section element matching given 'section_name'.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@return Pointer to the section or NULL
*/
tou_llist_t* tou_ini_get_section(tou_llist_t* inicontents, const char* section_name);

/**
	@brief Returns pointer to the property element matching given 'section_name' and 'key'.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@return Pointer to the property or NULL
*/
tou_llist_t* tou_ini_get_property(tou_llist_t* inicontents, const char* section_name, const char* key);

/** @} */

/**
	@brief Returns the contents of 'key' under the given 'section_name'.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@return Pointer to the contents, or NULL
*/
void* tou_ini_get(tou_llist_t* inicontents, const char* section_name, const char* key);

/**
	@brief Sets the contents of `key` under the given `section_name`
	to the specified value, reallocating memory if necessary.

	Returns either pointer to the property (key & value) object that was just stored,
	the section name if just a new section was stored, or NULL on error;

	Section name must always be present, unlike key and value parameters which may be NULL.
	If key is NULL or empty(""), only the new section will be created. If the section already
	exists this is a no-op.
	If both section name and key are present, value may also be "" or NULL
	which will both result in being stored as an empty value ("") in the structure.

	@param[in,out] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@param[in] new_value 
	@return Pointer to the property object, section object, or NULL
*/
tou_llist_t* tou_ini_set(tou_llist_t** inicontents, const char* section_name, const char* key, char* new_value);

/**
	@brief Return a pointer to the llist entry of the matching section, or NULL.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@return Pointer to the section
*/
tou_llist_t* tou_ini_section(tou_llist_t* inicontents, const char* section_name);

/**
	@brief Return the value of the matching property in the given section, or NULL.

	@param[in] section Pointer to the parsed .INI structure
	@param[in] property_name 
	@return Value of the property
*/
void* tou_ini_property(tou_llist_t* section, const char* property_name);

/**
	@brief Writes .INI structure data to file stream.

	Return error codes:
	 0 = OK, no error,
	-1 = NULL received in params,
	-2 = Cannot write to stream,
	-3 = Invalid section name encountered in struct,
	-4 = Invalid property data encountered in struct

	@param[in] inicontents Pointer to the starting element
	@param[in] fp File* stream to write to
	@return Zero if successful, error code if failed
*/
int tou_ini_save_fp(tou_llist_t* inicontents, FILE* fp);

/**
	@brief Exports .INI structure data as JSON to the file stream.

	Return error codes:
	-  0 = OK, no error,
	- -1 = NULL received in params,
	- -2 = Cannot write to stream,
	- -3 = Invalid section name encountered in struct,
	- -4 = Invalid property data encountered in struct

	@param[in] inicontents Pointer to the starting element
	@param[in] fp File* stream to write to
	@return Zero if successful, error code if failed
*/
int tou_ini_save_fp_json(tou_llist_t* inicontents, FILE* fp);

/**
	@brief Exports .INI structure data as XML to the file stream.

	Return error codes:
	-  0 = OK, no error,
	- -1 = NULL received in params,
	- -2 = Cannot write to stream,
	- -3 = Invalid section name encountered in struct,
	- -4 = Invalid property data encountered in struct

	@param[in] inicontents Pointer to the starting element
	@param[in] fp File* stream to write to
	@return Zero if successful, error code if failed
*/
int tou_ini_save_fp_xml(tou_llist_t* inicontents, FILE* fp);

#endif


/** @} */


/* == Parsers == */
/**
	@addtogroup grp_parsers_converters Parsers & Converters
	@{
*/

#ifdef TOU_LLIST_SINGLE_ELEM
#warning TOU_LLIST_SINGLE_ELEM DEFINED, PARSERS & CONVERTERS DISABLED.
#else

/**
	@brief Parses "parameter string" and returns a llist of key-value pairs,
		looking at the whole string (strlen will be used)

	Extra whitespaces are ignored and pairs may be key-only (values may be empty).

	Example:
		` 5 adjust abc=   def     123=456 	`
	Is returned as llist where elements are:
		`{{"5", NULL}, {"adjust", NULL}, {"abc", "def"}, {"123", "456"}}`

	@param[in] str Pointer to parse
	@param[in] param_sep String used to split pairs
	@param[in] keyval_sep String used to split key-value pairs
	@return Pairs in llist
*/
tou_llist_t* tou_paramparse(char* str, const char* param_sep, const char* keyval_sep);

/**
	@brief Parses "parameter string" and returns a llist of key-value pairs

	Extra whitespaces are ignored and pairs may be key-only (values may be empty).

	Example:
		` 5 adjust abc=   def     123=456 	`
	Is returned as llist where elements are:
		`{{"5", NULL}, {"adjust", NULL}, {"abc", "def"}, {"123", "456"}}`

	@param[in] str Pointer to parse
	@param[in] param_sep String used to split pairs
	@param[in] keyval_sep String used to split key-value pairs
	@param[in] maxlen Looks only at the first `maxlen` characters in str
	@return Pairs in llist
*/
tou_llist_t* tou_paramparse_n(char* str, const char* param_sep, const char* keyval_sep, size_t maxlen);

/**
	@brief Attempts to print "params" parsed by ::tou_paramparse or ::tou_paramparse_n

	@param[in] params List containing params
*/
void tou_paramprint(tou_llist_t* params);

#endif


/** @} */


#ifdef __cplusplus
}
#endif

#endif // __TOU_H_


// ==============================================================
// ||                       IMPLEMENTATION                     ||
// ==============================================================


#ifdef TOU_IMPLEMENTATION
#if defined(TOU_IMPLEMENTATION_DONE)
#pragma error "TOU_IMPLEMENTATION already defined somewhere!"
#else

#define TOU_IMPLEMENTATION_DONE


////////////////////////////////////////
///             Strings              ///
////////////////////////////////////////


/*  */
size_t tou_strlcpy(char* dst, const char* src, size_t size)
{
	size_t src_len;

	src_len = strlen (src);
	if (size > 0)
	{
		size_t dst_len = size - 1;
		if (src_len < dst_len)
			dst_len = src_len;
		memcpy (dst, src, dst_len);
		dst[dst_len] = '\0';
	}
	return src_len;
}


/*  */
size_t tou_strlcat(char* dst, const char* src, size_t size)
{
	size_t src_len, dst_len;

	src_len = strlen (src);
	dst_len = strlen (dst);
	if (size > 0 && dst_len < size) 
	{
		size_t copy_cnt = size - dst_len - 1;
		if (src_len < copy_cnt)
			copy_cnt = src_len;
		memcpy (dst + dst_len, src, copy_cnt);
		dst[dst_len + copy_cnt] = '\0';
	}
	return src_len + dst_len;
}


/*  */
size_t tou_strlen(const char* str)
{
	if (!str) return 0;
	return strlen(str);
}


/*  */
char* tou_strdup(const char* src)
{
	size_t src_len = strlen(src);
	char* copy;
	if ((copy = malloc(src_len + 1)) == NULL)
		return NULL;
	// tou_strlcpy(copy, src, src_len + 1);
	memcpy(copy, src, src_len + 1);
	return copy;
}


/*  */
char* tou_strndup(const char* src, size_t size)
{
	size_t src_len = strlen(src);
	char* copy = NULL;

	if (size > 0) {
		size_t copy_len = size;
		if (src_len < copy_len)
			copy_len = src_len;
		if ((copy = malloc(copy_len + 1)) == NULL)
			return NULL;
		// tou_strlcpy(copy, src, copy_len + 1);
		memcpy(copy, src, copy_len + 1);
	}
	return copy;
}


/*  */
char* tou_strchr(const char* src, int ch)
{
	while (*src) {
		if (*src == (char)ch)
			return (char*)src;
		src++;
	}

	if (ch == '\0') // '\0' can also be searched for
		return (char*)src;
	else
		return NULL;
}


/*  */
char* tou_strrchr(const char* src, int ch)
{
	char* pos = NULL;

	while (*src) {
		if (*src == (char)ch)
			pos = (char*)src;
		src++;
	}

	if (ch == '\0') // '\0' can also be searched for
		return (char*)src;
	else
		return pos;
}


/*  */
char* tou_trim_string(char** str)
{
	tou_trim_back(str);
	return tou_trim_front(str);
}


/*  */
char* tou_trim_front(char** str)
{
	if (str == NULL || *str == NULL)
		return NULL;
	
	char* ptr = *str;
	while (*ptr != '\0' && TOU_IS_BLANK(*ptr))
		ptr++;
	*str = ptr;
	return ptr;
}


/*  */
char* tou_trim_back(char** str)
{
	if (str == NULL || *str == NULL)
		return NULL;

	char* ptr = (*str + strlen(*str) - 1);
	while (ptr >= *str && TOU_IS_BLANK(*ptr)) {
		*ptr = '\0';
		ptr--;
	}
	ptr++;
	return ptr;
}


/*  */
void tou_trim_string_pure(char* str, char** start, char** end)
{
	char* back = tou_trim_back_pure(str);
	char* front = tou_trim_front_pure(str);

	*end = back;
	*start = front;
}


/*  */
char* tou_trim_front_pure(char* str)
{
	if (str == NULL)
		return NULL;

	while (*str != '\0' && TOU_IS_BLANK(*str))
		str++;
	return str;
}


/*  */
char* tou_trim_back_pure(char* str)
{
	if (str == NULL)
		return NULL;

	char* ptr = (str + strlen(str) - 1);
	while (ptr >= str && TOU_IS_BLANK(*ptr)) {
		ptr--;
	}
	ptr++; // Inc to point at the first trimmed/NUL byte
	return ptr;
}


/*  */
char* tou_slower(char* str)
{
	if (!str)
		return NULL;

	char* ptr = str;
	while (*ptr++ = tolower(*ptr)) ;
	return str;
}

/*  */
char* tou_supper(char* str)
{
	if (!str)
		return NULL;

	char* ptr = str;
	while (*ptr++ = toupper(*ptr)) ;
	return str;
}


/*  */
char* tou_replace_ch(char* ss, char oldch, char newch)
{
	TOU_PRINTD("[replace_ch] replacing ('%c'->'%c') \"%s\" => ", oldch, newch, ss);
	char* ptr = tou_strchr(ss, oldch);
	while (ptr) {
		*ptr = newch;
		ptr = tou_strchr(ptr + 1, oldch);
	}
	TOU_PRINTD("\"%s\"\n", ss);
	return ss;
}


/*  */
char* tou_sfind(const char* src, const char* kwd)
{
	return tou_sfind_n(src, kwd, strlen(src));
}


/*  */
char* tou_sfind_n(const char* src, const char* kwd, size_t maxlen)
{
	if (src == NULL)
		return NULL;

	const char* ret_ptr = NULL;
	const int kwd_len = strlen(kwd);
	const char* kwd_ptr = kwd;

	if (kwd_len > maxlen)
		return NULL;
	
	while (maxlen + 1 > 0 ){//&& *src) {
		if (*kwd_ptr == '\0') {
			// only time it should be is when kwd is found
			ret_ptr = src - kwd_len;
			break;
		}

		if (*src == '\0')
			break;

		if (*kwd_ptr == *src) {
			kwd_ptr++;
		} else {
			kwd_ptr = kwd;
		}

		src++;
		maxlen--;
	}

	return (char*)ret_ptr;
}


/*  */
char* tou_sfind_multiple(const char* str, const char** kwds, int n_kwds, int* found_idx)
{
	return tou_sfind_multiple_n(str, kwds, n_kwds, found_idx, ((size_t)-1) - 1);
}


/*  */
char* tou_sfind_multiple_n(const char* str, const char** kwds, int n_kwds, int* found_idx, size_t maxlen)
{
	if (str == NULL || kwds == NULL || n_kwds < 1)
		return NULL;

	if (maxlen < 1) {
		if (found_idx) *found_idx = -1;
		return NULL;
	}

	int kwd_len[n_kwds];
	const char* kwd_ptr[n_kwds];
	const char* ret_ptr = NULL;

	// Kwds which are NULL will not stop function but will be
	// ignored later in search loop and their kwd_len will be set to 0
	for (int i = 0; i < n_kwds; i++) {
		kwd_ptr[i] = kwds[i];
		kwd_len[i] = tou_strlen(kwds[i]);
	}

	while (maxlen + 1 > 0 && *str) {
		for (int i = 0; i < n_kwds; i++) {

			if (kwd_ptr[i] == NULL) // Skip NULL keywords
				continue;
			if (*(kwd_ptr[i]) == '\0') {
				// only time it should be is when that kwd is found
				ret_ptr = str - kwd_len[i];
				if (found_idx)
					*found_idx = i;
				goto jmp_sfind_multiple_n_break; // break;
			}

			if (*(kwd_ptr[i]) == *str) {
				kwd_ptr[i]++; // this character matched next char in this kwd
			} else {
				kwd_ptr[i] = kwds[i]; // reset this kwd tracker
			}

		}

		str++;
		maxlen--;
	}

jmp_sfind_multiple_n_break:
	// Do the check one more time in case matching string is found
	// at the end of `str` (*str will break before the check has a chance)
	for (int i = 0; i < n_kwds; i++) {
		if (*(kwd_ptr[i]) == '\0') {
			ret_ptr = str - kwd_len[i];
			if (found_idx)
				*found_idx = i;
			break;
		}
	}
	return (char*)ret_ptr;
}


/*  */
void tou_sfind_iter_multiple(const char* src, const char* kwds[], int n_kwds, tou_func3 cb, void* userdata)
{
	tou_sfind_iter_multiple_n(src, kwds, n_kwds, cb, userdata, ((size_t)-1) - 1);
}


/*  */
void tou_sfind_iter_multiple_n(const char* src, const char* kwds[], int n_kwds, tou_func3 cb, void* userdata, size_t maxlen)
{
	char* found = NULL;
	int found_idx = -1;
	const char* text_ptr = src;
	size_t remaining = maxlen - 0;

	while ((found = tou_sfind_multiple_n(text_ptr, kwds, n_kwds, &found_idx, remaining)) != NULL) {
		// TOU_PRINTD("[iter_find_multiple_n] calling user func with (\"%s\", \"%s\", %p)\n", text_ptr, kwds[found_idx], userdata);

		// Call user func
		if ((ssize_t)cb((void*)found, (void*)kwds[found_idx], userdata) == (ssize_t)TOU_BREAK) {
			TOU_PRINTD("[iter_find_multiple_n] breaking early\n");
			return;
		}
		// Iter continue
		text_ptr = found + tou_strlen(kwds[found_idx]);
		remaining -= (text_ptr - src);
	}
}


/*  */
tou_llist_t* tou_split(char* str, const char* delim)
{
	if (!str || !delim)
		return NULL;

	TOU_PRINTD("[tou_split] STR_LEN :: %zu\n", tou_strlen(str));

	size_t delim_len = tou_strlen(delim);
	tou_llist_t* list = NULL;
	char* pos_start = str;
	char* pos_delim = tou_sfind(str, delim);
	
	while (pos_delim) {
		// TODO: swap with tou_str[n]dup() ?
		char* buf = malloc(pos_delim-pos_start + 1);
		tou_strlcpy(buf, pos_start, pos_delim-pos_start + 1);
		TOU_PRINTD("[tou_split] BUF: %s\n", buf);
		tou_llist_appendone(&list, buf, 1);

		// Find next occurence
		pos_start = pos_delim + delim_len;
		pos_delim = tou_sfind(pos_start, delim);
	}

	size_t len = tou_strlen(pos_start);
	TOU_PRINTD("[tou_split] FINAL LEN :: %zu\n", len);
	if (len > 0) {
		// Append last part till the end
		// TODO: swap with tou_strndup() ?
		char* buf = malloc(len + 1);
		tou_strlcpy(buf, pos_start, len + 1);
		TOU_PRINTD("[tou_split] BUF: %s\n", buf);
		tou_llist_appendone(&list, buf, 1);
	}

	return list;//tou_llist_get_tail(list);
}


/*  */
char* tou_sappend(char* dst, char* src)
{
	size_t dst_len = tou_strlen(dst);
	size_t src_len = tou_strlen(src);

	size_t new_size = dst_len + src_len + 1;
	dst = realloc(dst, new_size);
	tou_strlcpy(dst + dst_len, src, new_size);

	return dst;
}


/*  */
char* tou_sappendch(char* dst, char src)
{
	size_t dst_len = tou_strlen(dst);

	size_t new_size = dst_len + 1 + 1;
	dst = realloc(dst, new_size);

	*(dst + dst_len) = src;
	*(dst + dst_len + 1) = '\0';

	return dst;
}


/*  */
char* tou_sprepend(char* dst, char* src)
{
	size_t dst_len = tou_strlen(dst);
	size_t src_len = tou_strlen(src);

	char* new_mem = malloc(dst_len + src_len + 1);

	memcpy(new_mem,           src, src_len);
	memcpy(new_mem + src_len, dst, dst_len + 1); // copy \0 too

	return new_mem;
}


/*  */
char* tou_sprependch(char* dst, char src)
{
	size_t dst_len = tou_strlen(dst);

	char* new_mem = malloc(dst_len + 1 + 1);

	*new_mem = src;
	memcpy(new_mem + 1, dst, dst_len + 1); // don't forget to copy \0 too

	return new_mem;
}


/*  */
char* tou_sreplace(char* str, char* repl_str, char* with_str)
{
	return tou_sreplace_n(str, repl_str, with_str, NULL);
}


/*  */
char* tou_sreplace_n(char* str, char* repl_str, char* with_str, size_t* len_ptr)
{
	if (!str || !repl_str) {
		TOU_PRINTD("[tou_sreplace_n] string or replace string NULL\n");
		return NULL;
	}

	size_t true_len = tou_strlen(str);
	size_t repl_len = tou_strlen(repl_str);
	size_t with_len = tou_strlen(with_str);
	
	size_t len = 0;
	if (len_ptr != NULL) {
		len = *len_ptr;
	}
	if (len == 0 || len > true_len)
		len = true_len;

	char tmp_replaced_ch = '\0';
	if (len < true_len) {
		tmp_replaced_ch = str[len];
		str[len] = '\0';
	}

	char* search_ptr = str;
	char* dst = NULL;
	char* tmp_dst; // used when realloc'ing dst & when iterating last part
	size_t current_size = 0;
	const int with_alloc_mult = 4; // prealloc for more than one and shrink in the end?
	
	size_t copydiff = 0;
	char* next_with = NULL;

	while ((next_with = tou_sfind(search_ptr, repl_str)) != NULL)
	{
		copydiff = next_with - search_ptr;

		if ((tmp_dst = realloc(dst, current_size + copydiff + with_len*with_alloc_mult + 1)) == NULL) {
			TOU_PRINTD("[tou_sreplace_n] realloc failed (%zu bytes)\n", current_size + copydiff + with_len*with_alloc_mult + 1);
			if (dst != NULL) // Would be NULL if this was the first replace
				*(dst + current_size) = '\0'; // needed?
			break;
		}
		dst = tmp_dst;

		memcpy(dst + current_size,            search_ptr, copydiff);
		memcpy(dst + current_size + copydiff, with_str,   with_len);
		// *(dst + current_size + copydiff + with_len) = '\0';
		
		current_size += copydiff + with_len;
		search_ptr   += copydiff + repl_len;
	}

	// Calc size of last one + remainder of source
	size_t rest = tou_strlen(search_ptr); // last part + non-searched remainder since replaced char was returned to place
	if ((tmp_dst = realloc(dst, current_size + rest + 1)) == NULL) {
		TOU_PRINTD("[tou_sreplace_n] last part realloc failed (%zu)\n", current_size + rest + 1);
		return dst;
	}
	dst = tmp_dst;

	// Copy the remaining unmodified part from search_ptr to (tmp_)dst
	tmp_dst = dst + current_size;
	while (*search_ptr)
		*(tmp_dst++) = *(search_ptr++);
	*tmp_dst = '\0';
	current_size += rest;

	// Restore replaced char if it was needed
	if (tmp_replaced_ch != '\0')
		str[len] = tmp_replaced_ch;

	// Resize to fit if larger (?)
	tmp_dst = realloc(dst, current_size + 1);
	if (tmp_dst) {
		// realloc succeded
		if (len_ptr) {
			*len_ptr = current_size;
		}
		return tmp_dst;
	}

	// realloc failed
	if (len_ptr) {
		*len_ptr = tou_strlen(dst);
	}
	return dst;
}


////////////////////////////////////////
///              Files               ///
////////////////////////////////////////


/*  */
size_t tou_read_fp_in_blocks(FILE* fp, size_t blocksize, tou_func3 cb, void* userdata)
{
	// Clamp blocksize to TOU_DEFAULT_BLOCKSIZE if not in following range:
	if (blocksize < 1 || blocksize > 0xFFFFFF) {
		blocksize = TOU_DEFAULT_BLOCKSIZE;
		TOU_PRINTD("[tou_read_fp_in_blocks] clamping blocksize to " TOU_MSTR(TOU_DEFAULT_BLOCKSIZE) "\n");
	}

	size_t curr_bufsize = 0;
	char blockbuf[blocksize];
	size_t bytes_read = 0;

	while (1) {
		// Clear buf
		memset(blockbuf, 0, blocksize);

		// Load data into buf
		size_t cnt = fread(blockbuf, 1, blocksize, fp);
		if (cnt <= 0)
			break;

		// Store real read length
		bytes_read += cnt;

		// Do the useful things
		if (cb) {
			if ((ssize_t)cb(blockbuf, (void*) cnt, userdata) == (ssize_t)TOU_BREAK) {
				// User stopped iteration
				TOU_PRINTD("[tou_read_fp_in_blocks] iteration aborted.\n");
				break;
			}
		}
	}

	return bytes_read;
}


/*  */
void* tou_block_store_cb(void* blockdata, void* len, void* userdata)
{
	char* block = (char*) blockdata;
	size_t size = (size_t) len;
	tou_block_store_struct* data = (tou_block_store_struct*) userdata;

	TOU_PRINTD("\n[tou_block_store_cb] Block\n=====\n%.*s (...first %d bytes)\n===== (%zu)\n", (size>64)?64:size, block, (size>64)?64:size, size);

	if (size > 0) {
		char* new_buffer = realloc(data->buffer, data->size + size + 1); // accomodate \0
		if (!new_buffer){ 
			TOU_PRINTD("[tou_block_store_cb] cannot realloc memory\n");
			// return (void*) TOU_BREAK;
		} else {
			memcpy(new_buffer + data->size, block, size);
			*(new_buffer + data->size + size) = '\0';
			data->buffer = new_buffer;
			data->size += size;
			TOU_PRINTD("[tou_block_store_cb] appended block {size=%zu}\n", data->size);
		}
	}
	
	return (void*)TOU_CONTINUE;
}

/*  */
char* tou_read_file(const char* filename, size_t* read_len)
{
	FILE* fp;
	if (!filename || strlen(filename) == 0 || strcmp("stdin", filename) == 0) { // stdin
		fp = stdin;
	} else if ((fp = fopen(filename, "rb")) == NULL) { // file
		TOU_PRINTD("[read_file] cannot open '%s' (as \"rb\")\n", filename);
		return NULL;
	}

	TOU_PRINTD("[read_file] reading from: %s\n", (fp==stdin) ? "STDIN" : filename);
	char* read = tou_read_fp(fp, read_len);

	if (fp != stdin) {
		fclose(fp);
		TOU_PRINTD("[read_file] closed.\n");
	}

	return read;
}


/*  */
char* tou_read_fp(FILE* fp, size_t* read_len)
{
	if (fp == NULL)
		return NULL;

	TOU_PRINTD("[read_fp] reading from: FILE* %p\n", fp);

	// char* read = tou_read_fp_in_blocks(fp, read_len, -1, NULL, NULL); // default block size, without function
	// tou_read_fp_in_blocks -> (FILE* fp, size_t* read_len, size_t blocksize, tou_func3 cb, void* userdata)
	tou_block_store_struct tmp = {NULL, 0};
	tou_read_fp_in_blocks(fp, TOU_DEFAULT_BLOCKSIZE, tou_block_store_cb, &tmp);
	
	TOU_PRINTD("[read_fp] finished reading.\n");

	if (read_len)
		*read_len = tmp.size;

	return tmp.buffer;
}


/*  */
int tou_disable_stdout()
{
	TOU_PRINTD("[Disabling stdout.]\n");
	
	fflush(stdout);
	int stdout_fd;

#ifdef _WIN32
	// #pragma message "bindobs"
	stdout_fd = _dup(1);
	FILE* fp_nul = fopen(_TOU_DEVNULL_FILE, "w");
	_dup2(_fileno(fp_nul), 1);
	fclose(fp_nul);

#elif __linux__
	// #pragma message "bibux"
	stdout_fd = dup(1);
	int new = open(_TOU_DEVNULL_FILE, O_WRONLY);
	dup2(new, 1);
	close(new);
#endif
	
	return stdout_fd;
}


/*  */
void tou_enable_stdout(int saved_fd)
{
	fflush(stdout);

#ifdef _WIN32
	// #pragma message "bindobs"
	_dup2(saved_fd, 1);
	_flushall();

#elif __linux__
	// #pragma message "bibux"
	dup2(saved_fd, 1);
	close(saved_fd);
#endif

	TOU_PRINTD("[Enabled stdout.]\n");
}


////////////////////////////////////////
///           Linked list            ///
////////////////////////////////////////


/*  */
tou_llist_t* tou_llist_append
(
	tou_llist_t** node_ref,
	void* dat1,
#ifndef TOU_LLIST_SINGLE_ELEM
	void* dat2,
#endif
	char dat1_is_dynalloc
#ifndef TOU_LLIST_SINGLE_ELEM
	, char dat2_is_dynalloc
#endif
) {
	if (node_ref == NULL)
		return NULL;

	// Spawn new node
	tou_llist_t* new_node = malloc(sizeof(*new_node));

	new_node->prev = NULL;
	new_node->next = NULL;
	
	new_node->dat1 = dat1;
	new_node->destroy_dat1 = dat1_is_dynalloc;
#ifndef TOU_LLIST_SINGLE_ELEM
	new_node->dat2 = dat2;
	new_node->destroy_dat2 = dat2_is_dynalloc;
#endif

	// Given node is empty list
	if (*node_ref == NULL) {
		*node_ref = new_node; // Update passed reference to point to the new node
		// printf("[::] LIST IS EMPTY, RETURNING NEW_NODE\n");
		return new_node;
	}
	// Given node/list already has something and this node is head
	tou_llist_t* prev_node = (*node_ref);
	if (prev_node->next == NULL) {
		prev_node->next = new_node; // Update passed node's .next
		new_node->prev = prev_node; // Update new node's .prev
		*node_ref = new_node; // Update passed reference to point to the new node
		// printf("[::] LIST GIVEN IS HEAD, RETURNING NEW_NODE\n");
		return new_node;
	}

	// The given node is now somewhere inbetween (after head)

	// Setup new node links
	tou_llist_t* previous_next = prev_node->next; // Save previously-next node
	new_node->prev = prev_node; // Update new node's .prev to point to the passed node
	new_node->next = previous_next; // Update new node's .next to
									// point to the previously-next node

	// Setup previous (current) node links
	prev_node->next = new_node; // Update previously-next node to point to newly created node
	if (previous_next != NULL)
		previous_next->prev = new_node; // If previously-next actually exists,
										// make its .prev point to newly created node

	// printf("[::] LIST IS IN THE MIDDLE, RETURNING ;  %s\n", (*node_ref)->dat1);
	return new_node;
}


/*  */
tou_llist_t* tou_llist_appendone(tou_llist_t** elem, void* dat1, char dat1_is_dynalloc)
{
#ifndef TOU_LLIST_SINGLE_ELEM
	return tou_llist_append(elem, dat1, NULL, dat1_is_dynalloc, 0);
#else
	return tou_llist_append(elem, dat1, dat1_is_dynalloc);
#endif
}


/*  */
tou_llist_t* tou_llist_prepend
(
	tou_llist_t** node_ref,
	void* dat1,
#ifndef TOU_LLIST_SINGLE_ELEM
	void* dat2,
#endif
	char dat1_is_dynalloc,
#ifndef TOU_LLIST_SINGLE_ELEM
	char dat2_is_dynalloc
#endif
) {
	if (node_ref == NULL)
		return NULL;

	// Spawn new node
	tou_llist_t* new_node = malloc(sizeof(*new_node));

	new_node->prev = NULL;
	new_node->next = NULL;

	new_node->dat1 = dat1;
	new_node->destroy_dat1 = dat1_is_dynalloc;
#ifndef TOU_LLIST_SINGLE_ELEM
	new_node->dat2 = dat2;
	new_node->destroy_dat2 = dat2_is_dynalloc;
#endif

	// Given node is empty list
	// handle empty list same as append (head <=> tail)
	if (*node_ref == NULL) {
		*node_ref = new_node; // Update passed reference to point to the new node
		return new_node;
	}

	// Given node/list already has something and this node is tail
	tou_llist_t* prev_node = (*node_ref);
	if (prev_node->prev == NULL) {
		prev_node->prev = new_node; // Update passed node's .prev
		new_node->next = prev_node; // Update new node's .next
		return new_node;
	}

	// The given node is now somewhere inbetween (after tail)

	// Setup new node links
	tou_llist_t* previous_prev = prev_node->prev; // Save previously-prev node
	new_node->next = prev_node; // Update new node's .next to point to the passed node
	new_node->prev = previous_prev; // Update new node's .prev to
									// point to the previously-prev node

	// Setup previous (current) node links
	prev_node->prev = new_node; // Update previously-prev node to point to newly created node
	if (previous_prev != NULL)
		previous_prev->next = new_node; // If previously-prev actually exists,
										// make its .next point to newly created node
	return new_node;
}


/*  */
tou_llist_t* tou_llist_prependone(tou_llist_t** elem, void* dat1, char dat1_is_dynalloc)
{
#ifndef TOU_LLIST_SINGLE_ELEM
	return tou_llist_prepend(elem, dat1, NULL, dat1_is_dynalloc, 0);
#else
	return tou_llist_prepend(elem, dat1, dat1_is_dynalloc);
#endif
}


/*  */
void tou_llist_destroy(tou_llist_t* list)
{
	if (!list) return;

	if (list->next == NULL) { // this is head.
		tou_llist_t *prev, *curr = list;

		while (curr != NULL) {
			prev = curr->prev;
			if (curr->destroy_dat1) free(curr->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
			if (curr->destroy_dat2) free(curr->dat2);
#endif
			curr->prev = NULL;
			free(curr);
			curr = prev;
		}

	} else { // this is tail (or inbetween)
		tou_llist_t *next, *curr = list;
		
		while (curr != NULL) {
			next = curr->next;
			if (curr->destroy_dat1) free(curr->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
			if (curr->destroy_dat2) free(curr->dat2);
#endif
			curr->next = NULL;
			free(curr);
			curr = next;
		}
	}
}


/*  */
tou_llist_t* tou_llist_remove(tou_llist_t* elem)
{	
	tou_llist_t *next = elem->next, *prev = elem->prev;
	tou_llist_pop(elem);
	tou_llist_free_element(elem);

	if (next == NULL) // This element was head
		return prev; // prev will be the new head
	else
		return next; // if assigned, next will be the new head
}


/*  */
tou_llist_t* tou_llist_pop(tou_llist_t* elem)
{
	if (!elem) return NULL;

	if (elem->prev) {
		elem->prev->next = elem->next;
	}
	if (elem->next) {
		elem->next->prev = elem->prev;
	}

	return elem;
}


/*  */
void tou_llist_free_element(tou_llist_t* elem)
{
	if (!elem) return;

	if (elem->destroy_dat1) free(elem->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
	if (elem->destroy_dat2) free(elem->dat2);
#endif

	free(elem);
}


/*  */
tou_llist_t* tou_llist_get_head(tou_llist_t* list)
{
	if (!list) return NULL;

	while (list->next)
		list = list->next;
	
	return list;
}


/*  */
tou_llist_t* tou_llist_get_tail(tou_llist_t* list)
{
	if (!list) return NULL;

	while (list->prev)
		list = list->prev;

	return list;
}


/*  */
tou_llist_t* tou_llist_get_newest(tou_llist_t* list)
{
	return tou_llist_get_head(list);
}


/*  */
tou_llist_t* tou_llist_get_oldest(tou_llist_t* list)
{
	return tou_llist_get_tail(list);
}


/*  */
tou_llist_t* tou_llist_get_newer(tou_llist_t* elem)
{
	if (elem == NULL) return NULL;
	return elem->next;
}


/*  */
tou_llist_t* tou_llist_get_older(tou_llist_t* elem)
{
	if (elem == NULL) return NULL;
	return elem->prev;
}


/*  */
tou_llist_t* tou_llist_next_newer(tou_llist_t** elem)
{
	if (elem == NULL) return NULL;
	return *elem = tou_llist_get_newer(*elem);
}


/*  */
tou_llist_t* tou_llist_next_older(tou_llist_t** elem)
{
	if (elem == NULL) return NULL;
	return *elem = tou_llist_get_older(*elem);
}


/*  */
char tou_llist_is_head(tou_llist_t* elem)
{
	return elem == NULL || elem->next == NULL;
}


/*  */
char tou_llist_is_tail(tou_llist_t* elem)
{
	return elem == NULL || elem->prev == NULL;
}


/*  */
void tou_llist_iter(tou_llist_t* list, tou_func cb)
{
	if (list == NULL || cb == NULL)
		return;

	if (list->next == NULL) { // this is head.
		while (list) {
			if (cb(list) == 0)
				break;
			list = list->prev;
		}

	} else { // this is tail (or inbetween)
		while (list) {
			if (cb(list) == 0)
				break;
			list = list->next;
		}
	}
}


/*  */
tou_llist_t* tou_llist_find_exact
(
	tou_llist_t* list,
	void* dat1
#ifndef TOU_LLIST_SINGLE_ELEM
	, void* dat2
#endif
) {
	if (list == NULL)
		return NULL;
	/* TOU_PRINTD("[find_exact] %p, %p, %p\n", list, dat1, dat2); */

	while (list) {
		if (list->dat1 == dat1)
			return list;
#ifndef TOU_LLIST_SINGLE_ELEM
		if (list->dat2 == dat2)
			return list;
#endif
		list = list->prev;
	}

	return NULL;
}

/*  */
tou_llist_t* tou_llist_find_exactone(tou_llist_t* list, void* dat1)
{
#ifndef TOU_LLIST_SINGLE_ELEM
	if (list == NULL)
		return NULL;

	while (list) {
		if (list->dat1 == dat1)
			return list;
		list = list->prev;
	}

	return NULL;
#else

	return tou_llist_find_exact(list, dat1);

#endif
}


/*  */
tou_llist_t* tou_llist_find_key(tou_llist_t* list, void* dat1)
{
	if (list == NULL)
		return NULL;

	while (list) {
		if (strcmp(list->dat1, dat1) == 0) {
			return list;
		}
		list = list->prev;
	}

	TOU_PRINTD("[llist_find_key] returning NULL\n");
	return NULL;
}


/*  */
tou_llist_t* tou_llist_find_func(tou_llist_t* list, tou_func2 cb, void* userdata)
{
	if (list == NULL || cb == NULL)
		return NULL;

	while (list) {
		if ((size_t) cb(list, userdata) == TOU_BREAK)
			return list;
		list = list->prev;
	}

	return NULL;
}


/*  */
size_t tou_llist_len(tou_llist_t* list)
{
	if (list == NULL)
		return 0;

	size_t len = 0;

	if (list->next == NULL) { // this is head.
		while (list) {
			len++;
			list = list->prev;
		}

	} else { // this is tail (or inbetween)
		while (list) {
			len++;
			list = list->next;
		}
	}

	return len;
}


/*  */
void** tou_llist_gather_dat1(tou_llist_t* list, size_t* len)
{
	if (list == NULL) {
		TOU_PRINTD("[llist_gather_dat1] received null parameter\n");
		return NULL;
	}

	if (len != NULL)
		*len = 0;

	size_t _len = tou_llist_len(list);
	if (_len < 1) {
		TOU_PRINTD("[llist_gather_dat1] list has 0 length\n");
		return NULL;
	}

	void** gathered;
	if ((gathered = malloc(_len * sizeof(void*))) == NULL) {
		TOU_PRINTD("[llist_gather_dat1] dynamic allocation failed\n");
		return NULL;
	}

	for (size_t i = 0; i < _len; i++) {
		gathered[i] = list->dat1;
		list = list->prev;
	}

	if (len != NULL)
		*len = _len;
	return gathered;
}


/*  */
#ifndef TOU_LLIST_SINGLE_ELEM
void** tou_llist_gather_dat2(tou_llist_t* list, size_t* len)
{
	if (list == NULL) {
		TOU_PRINTD("[llist_gather_dat2] received null parameter\n");
		return NULL;
	}

	if (len != NULL)
		*len = 0;

	size_t _len = tou_llist_len(list);
	if (_len < 1) {
		TOU_PRINTD("[llist_gather_dat2] list has 0 length\n");
		return NULL;
	}

	void** gathered;
	if ((gathered = malloc(_len * sizeof(void*))) == NULL) {
		TOU_PRINTD("[llist_gather_dat2] dynamic allocation failed\n");
		return NULL;
	}

	for (size_t i = 0; i < _len; i++) {
		gathered[i] = list->dat2;
		list = list->prev;
	}

	if (len != NULL)
		*len = _len;
	return gathered;
}
#endif


////////////////////////////////////////
///               Stack              /// 
////////////////////////////////////////


/*  */
tou_llist_t* tou_stack_new()
{
	return tou_llist_new();
}


/*  */
void tou_stack_destroy(tou_llist_t* stack)
{
	return tou_llist_destroy(stack);
}


/*  */
size_t tou_stack_len(tou_llist_t* stack)
{
	return tou_llist_len(stack);
}


/*  */
void tou_stack_push(tou_llist_t** stack, void* elem)
{
	if (stack == NULL)
		return;
	tou_llist_t* top = tou_llist_get_newest(*stack);
	tou_llist_appendone(&top, (void*)elem, 0);
	*stack = top;
	return;
}


/*  */
void* tou_stack_pop(tou_llist_t** stack)
{
	if (stack == NULL || *stack == NULL)
		return NULL;

	tou_llist_t* top = tou_llist_get_newest(*stack);
	if (!top)
		return NULL;

	tou_llist_t* saved = tou_llist_get_older(top);
	void* val = tou_llist_pop(top);
	if (val) val = ((tou_llist_t*) val)->dat1;

	tou_llist_free_element(top);
	*stack = saved;

	return val;
}


////////////////////////////////////////
///               Queue              /// 
////////////////////////////////////////


/*  */
tou_llist_t* tou_queue_new()
{
	return tou_llist_new();
}

/*  */
void tou_queue_destroy(tou_llist_t* queue)
{
	return tou_llist_destroy(queue);
}

/*  */
size_t tou_queue_len(tou_llist_t* queue)
{
	return tou_llist_len(queue);
}

/*  */
void tou_queue_push(tou_llist_t** queue, void* elem)
{
	if (queue == NULL)
		return;
	tou_llist_t* bottom = tou_llist_get_oldest(*queue);
	tou_llist_t* new_elem = tou_llist_prependone(&bottom, (void*)elem, 0);
	if (*queue == NULL)
		*queue = new_elem;
	return;
}

/*  */
void* tou_queue_pop(tou_llist_t** queue)
{
	if (queue == NULL)
		return NULL;
	tou_llist_t* top = tou_llist_get_newest(*queue);
	if (top == NULL)
		return NULL;

	tou_llist_t* saved = tou_llist_get_older(top);
	void* val = tou_llist_pop(top);
	if (val) val = ((tou_llist_t*) val)->dat1;

	tou_llist_free_element(top);
	*queue = saved;
	return val;
}



////////////////////////////////////////
///            .INI files            ///
////////////////////////////////////////

#ifndef TOU_LLIST_SINGLE_ELEM

/*  */
tou_llist_t* tou_ini_parse_fp(FILE* fp)
{
	if (fp == NULL) {
		TOU_PRINTD("ini_parse_fp received empty fp\n");
		return NULL;
	}

	tou_llist_t* inicontents = tou_llist_new();
	char line_buf[256+1];
	char* line = line_buf;
	size_t line_no = 0;

	while (fgets(line, 256+1, fp)) {
		line_no++;

		int status = tou_ini_parse_line(&inicontents, line);
		if (status == TOU_BREAK) {
			TOU_PRINTD("Invalid line encountered while parsing (line %zu): %s\n", line_no, line);
			tou_ini_destroy(inicontents);
			return NULL;
		}
	}

	return inicontents;
}


/*  */
tou_llist_t* tou_ini_parse_buffer(char* buf)
{
	if (buf == NULL) {
		TOU_PRINTD("ini_parse_buf received empty buffer\n");
		return NULL;
	}

	tou_llist_t* inicontents = tou_llist_new();
	size_t line_no = 0;

	char* pos;
	const char* nls[] = {"\r\n", "\n"};
	int found_idx;

	while ((pos = tou_sfind_multiple(buf, nls, 2, &found_idx)) != NULL)
	{
		line_no++;
		*pos = '\0';

		int status = tou_ini_parse_line(&inicontents, buf);
		if (status == TOU_BREAK) {
			TOU_PRINTD("Invalid line encountered while parsing (line %zu): %s\n", line_no, buf);
			tou_ini_destroy(inicontents);
			return NULL;
		}

		buf = pos + strlen(nls[found_idx]);
	}

	// Last line
	if (tou_strlen(buf) > 0) {
		line_no++;
		if (tou_ini_parse_line(&inicontents, buf) == TOU_BREAK) {
			TOU_PRINTD("Invalid line encountered while parsing (line %zu): %s\n", line_no, buf);
			tou_ini_destroy(inicontents);
			return NULL;
		}
	}

	return inicontents;
}


/*  */
int tou_ini_parse_line(tou_llist_t** inicontents, char* line)
{
	// Ignore null lines but break if inicontents is null
	if (inicontents == NULL)
		return TOU_BREAK;
	if (line == NULL)
		return TOU_CONTINUE;

	// Cut off front whitespace
	tou_trim_front(&line);
	if (*line == '\0')
		return TOU_CONTINUE;

	// Check if comment line
	switch(*line) {
		case ';':
		case '#':
			return TOU_CONTINUE;
	}

	// Check if section declaration
	if (*line == '[') {

		tou_trim_back(&line);
		size_t line_len = strlen(line);
		
		// Cut off front bracket
		line++;
		line_len--;

		// If other bracket doesn't exist ignore it and still parse as valid syntax
		if (line[line_len - 1] == ']') {
			line[line_len - 1] = '\0';
			line_len--;
		}

		// Append section
		tou_ini_set(inicontents, line, NULL, NULL);
		// tou_llist_append(inicontents,
		// 	tou_strdup(line), NULL,
		// 	1, 0);

		TOU_PRINTD("[ini_parse_line] appended section: %s\n", (*inicontents)->dat1);
		return TOU_CONTINUE;
	}

	// Alright then, check if line matches a property line type; otherwise it's an invalid line
	
	// Test if either delimiter exists in line
	char* prop = tou_sfind(line, " = ");
	int prop_len = 3;

	if (prop == NULL) {
		prop = tou_sfind(line, "=");
		prop_len = 1;

		// Invalid line
		if (prop == NULL) {
			int len = strlen(line);
			if (line[len - 1] == '\n')
				line[len - 1] = '\0';
			return TOU_BREAK;
		}
	}

	// Property encountered before any section declaration
	if (*inicontents == NULL) {
		TOU_PRINTD("[ini_parse_line] property encountered before any section declaration\n");
		return TOU_BREAK;
	}

	// Duplicate key and value and append to list
	*prop = '\0';
	char* key = line;
	char* val = prop + prop_len;

	if (TOU_IS_BLANK(*val)) // Remove leading (only first one) whitespace
		val++;
	tou_trim_back(&key);
	tou_trim_back(&val);

	TOU_PRINTD("[ini_parse_line] setting property key=%s, val=%s\n", key, val);
	// tou_llist_append((tou_llist_t**)( &((*inicontents)->dat2) ),    // append to "current section" element
	// 	tou_strdup(key), tou_strdup(val),                           // copy key, copy val
	// 	1, 1);                                                      // auto dealloc both key&val
	tou_ini_set(inicontents, (*inicontents)->dat1, key, val);

	return TOU_CONTINUE;
}


/*  */
void tou_ini_destroy(tou_llist_t* inicontents)
{
	tou_llist_t* section = inicontents;
	while (section) {
		tou_llist_destroy(section->dat2); // Destroy each props sublist
		TOU_PRINTD("Destroying section: %s\n", section->dat1);
		section = section->prev;
	}
	tou_llist_destroy(inicontents); // Destroy whole structure
	TOU_PRINTD("Destroyed INI structure.\n");
}


/*  */
void tou_ini_print(tou_llist_t* inicontents)
{
	if (inicontents == NULL) {
		TOU_PRINTD("ini_print received empty struct\n");
		return;
	}

	tou_llist_t* section = inicontents;
	tou_llist_t* props;
	fprintf(stdout, "<.INI STRUCTURE>\n");
	fprintf(stdout, " |\n");

	while (section) {
		props = section->dat2;
		char next_section_ch = (section->prev == NULL) ? ' ' : '|';
		fprintf(stdout, " +-> [SECTION] \"%s\"\n", (char*)section->dat1);
		while (props) {
			fprintf(stdout, " %c     |\n", next_section_ch);
			fprintf(stdout, " %c     +-> [PROP] \"%s\": \"%s\"\n",
				next_section_ch, (char*)props->dat1, (char*)props->dat2);
			props = props->prev;
		}
		fprintf(stdout, " %c\n", next_section_ch);
		section = section->prev;
	}
}


/*  */
tou_llist_t* tou_ini_get_section(tou_llist_t* inicontents, const char* section_name)
{
	if (inicontents == NULL || section_name == NULL) {
		TOU_PRINTD("ini_get_section received empty params\n");
		return NULL;
	}

	tou_llist_t* sect = tou_llist_find_key(inicontents, (void*)section_name);
	if (sect == NULL) {
		TOU_PRINTD("ini_get_section unable to find given section\n");
		return NULL;
	}

	return sect;
}


/*  */
tou_llist_t* tou_ini_get_property(tou_llist_t* inicontents, const char* section_name, const char* key)
{
	if (inicontents == NULL || section_name == NULL || key == NULL) {
		TOU_PRINTD("ini_get_property received empty params\n");
		return NULL;
	}

	tou_llist_t* sect = tou_llist_find_key(inicontents, (void*) section_name);
	if (sect == NULL) {
		TOU_PRINTD("ini_get_property unable to find given section\n");
		return NULL;
	}
	// TOU_PRINTD("TOU_INI_GET_PROPERTY found SECTION %s\n", sect->dat1);

	tou_llist_t* prop = tou_llist_find_key(sect->dat2, (void*)key );
	if (prop == NULL) {
		TOU_PRINTD("ini_get_property unable to find given key\n");
	}

	return prop;
}


/*  */
tou_llist_t* tou_ini_section(tou_llist_t* inicontents, const char* section_name)
{
	tou_llist_t* sect = tou_ini_get_section(inicontents, (void*)section_name);
	if (sect == NULL)
		return NULL;
	return sect->dat2;
}


/*  */
void* tou_ini_property(tou_llist_t* section, const char* property_name)
{
	tou_llist_t* prop = tou_llist_find_key(section, (void*)property_name);
	if (prop == NULL)
		return NULL;
	return prop->dat2;
}


/*  */
void* tou_ini_get(tou_llist_t* inicontents, const char* section_name, const char* key)
{
	tou_llist_t* prop = tou_ini_get_property(inicontents, section_name, key);
	TOU_PRINTD("ini_get  prop: %p\n", prop);

	if (prop)
		return prop->dat2;
	return NULL;
}


/*  */
tou_llist_t* tou_ini_set(tou_llist_t** inicontents, const char* section_name, const char* key, char* new_value)
{
	if (inicontents == NULL /*|| *inicontents == NULL*/ || section_name == NULL /*|| key == NULL || new_value == NULL*/) {
		TOU_PRINTD("[ini_set] received empty params\n");
		return NULL;
	}

	// Retrieve or create a new section
	tou_llist_t* sect = tou_llist_find_key(*inicontents, (void*)section_name);
	if (sect == NULL) {
		TOU_PRINTD("[ini_set] section [%s] not found, allocating new...\n", section_name);
		sect = tou_llist_append(inicontents, tou_strdup(section_name), NULL, 1, 0);
	}

	// If key in unspecified ignore value and return a pointer to the new (or alredy existing) section
	if (key == NULL || tou_strlen(key) == 0) {
		TOU_PRINTD("[ini_set] key is empty, returning\n", section_name);
		return sect;
	}
	
	// Both section name and key are present, now we can (re)alloc space for the value
	tou_llist_t* prop = tou_llist_find_key(sect->dat2, (void*)key);

	// Fix up value if needed (NULL into empty "")
	if (new_value == NULL)
		new_value = ""; // will get dup()'d

	if (prop == NULL) {
		// Allocate new property...
		TOU_PRINTD("[ini_set] property not found, creating new...\n");

		prop = tou_llist_append(
			(tou_llist_t**)(&sect->dat2),
			tou_strdup(key), tou_strdup(new_value),
			1, 1);

		TOU_PRINTD("[ini_set] %s, %s\n", prop->dat1, prop->dat2);
		return prop;

	} else {
		// ...or set/realloc the existing one
		TOU_PRINTD("[ini_set] property found, setting...\n");

		char* old_value = prop->dat2;
		size_t old_len = strlen(old_value);
		size_t new_len = strlen(new_value);
		
		if (new_len > old_len)
			old_value = realloc(old_value, new_len + 1);

		tou_strlcpy(old_value, new_value, new_len + 1);
		prop->dat2 = old_value;

		return prop;
	}
}


/*  */
int tou_ini_save_fp(tou_llist_t* inicontents, FILE* fp)
{
	if (inicontents == NULL || fp == NULL) {
		TOU_PRINTD("[ini_save_fp] received empty params\n");
		return -1;
	}
	
	tou_llist_t* section = inicontents;
	while (section) {
		if (section->dat1 == NULL) {
			TOU_PRINTD("[ini_save_fp] Invalid section name encountered\n");
			return -3;
		}
		// Print section name
		fprintf(fp, "[%s]", (char*)section->dat1);
		if (ferror(fp)) {
			TOU_PRINTD("[ini_save_fp] Error writing to stream\n");
			return -2;
		}
		
		tou_llist_t* prop = section->dat2;
		while (prop) {
			if (prop->dat1 == NULL || prop->dat2 == NULL) {
				TOU_PRINTD("[ini_save_fp] Invalid property data encountered\n");
				return -4;
			}
			fprintf(fp, "\n%s = %s", (char*)prop->dat1, (char*)prop->dat2);
			prop = prop->prev;
		}

		fprintf(fp, "\n");
		section = section->prev;
	}

	return 0;
}


/*  */
int tou_ini_save_fp_json(tou_llist_t* inicontents, FILE* fp)
{
	if (inicontents == NULL || fp == NULL) {
		TOU_PRINTD("[ini_save_fp_json] received empty params\n");
		return -1;
	}
	
	// Print initial json struct and metadata
	fprintf(fp, "{\n\t\"ver\": " TOU_MSTR(TOU_JSON_DATA_VER) ",\n\t\"sections\": {");
	if (ferror(fp)) {
		TOU_PRINTD("[ini_save_fp_json] Error writing to stream\n");
		return -2;
	}

	tou_llist_t* section = inicontents;
	// size_t nsections = 0;
	while (section) {
		if (section->dat1 == NULL) {
			TOU_PRINTD("[ini_save_fp_json] Invalid section name encountered\n");
			return -3;
		}
		// Print section name
		fprintf(fp, "\n\t\t\"%s\": {", section->dat1);
		
		tou_llist_t* prop = section->dat2;
		// size_t nprops = 0;
		while (prop) {
			if (prop->dat1 == NULL || prop->dat2 == NULL) {
				TOU_PRINTD("[ini_save_fp_json] Invalid property data encountered\n");
				return -4;
			}

			// Print property
			fprintf(fp, "%s\n\t\t\t\"%s\": \"%s\"", (prop==section->dat2 ? "":","), prop->dat1, prop->dat2);
			// nprops++;
			prop = prop->prev;
		}

		fprintf(fp, "\n\t\t}%s", (section->prev==NULL ? "":","));
		// nsections++;
		section = section->prev;
	}

	// fprintf(fp, "\n\t},\n\t\"section_count\": %zu\n}\n", nsections);
	fprintf(fp, "\n\t}\n}");

	return 0;
}

/*  */
int tou_ini_save_fp_xml(tou_llist_t* inicontents, FILE* fp)
{
	if (inicontents == NULL || fp == NULL) {
		TOU_PRINTD("[ini_save_fp_xml] received empty params\n");
		return -1;
	}
	tou_llist_t* section = inicontents;
	
	// Print initial json struct and metadata
	const char* root_tag = "root";
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(fp, "<%s ver=" TOU_MSTR(TOU_XML_DATA_VER) " len=\"%zu\">", root_tag, tou_llist_len(inicontents));
	if (ferror(fp)) {
		TOU_PRINTD("[ini_save_fp_xml] Error writing to stream\n");
		return -2;
	}

	while (section) {
		if (section->dat1 == NULL) {
			TOU_PRINTD("[ini_save_fp_xml] Invalid section name encountered\n");
			return -3;
		}
		tou_llist_t* prop = section->dat2;

		// Print section name and attr(s)
		char* sect_name_tag = tou_replace_ch(tou_strdup(section->dat1), ' ', '-');
		fprintf(fp, "\n\t<%s len=\"%zu\">", sect_name_tag, tou_llist_len(prop));
		
		while (prop) {
			if (prop->dat1 == NULL || prop->dat2 == NULL) {
				TOU_PRINTD("[ini_save_fp_xml] Invalid property data encountered\n");
				return -4;
			}
			char* prop_name_tag = tou_replace_ch(tou_strdup(prop->dat1), ' ', '-');

			// Print property
			fprintf(fp, "\n\t\t<%s>%s</%s>", prop_name_tag, prop->dat2, prop_name_tag);
			prop = prop->prev;
		}

		fprintf(fp, "\n\t</%s>", sect_name_tag);
		section = section->prev;
	}

	fprintf(fp, "\n</%s>", root_tag);

	return 0;
}

#endif


////////////////////////////////////////
///             Parsers              ///
////////////////////////////////////////

#ifndef TOU_LLIST_SINGLE_ELEM

/*  */
tou_llist_t* tou_paramparse(char* str, const char* param_sep, const char* keyval_sep)
{
	return tou_paramparse_n(str, param_sep, keyval_sep, 0);
}


/*  */
tou_llist_t* tou_paramparse_n(char* str, const char* param_sep, const char* keyval_sep, size_t maxlen)
{
	size_t psep_len = strlen(param_sep);
	size_t kvsep_len = strlen(keyval_sep);

	if (maxlen == 0)
		maxlen = strlen(str);

	// save char to restore it later //
	char saved = str[maxlen];
	str[maxlen] = '\0';
	
	// parse //
	// Initially split by param separator and afterwards parse each param
	tou_llist_t* params = tou_split(str, param_sep);

	while (params) {
		if (!params->dat1)
			params = params->prev;

		bool dont = false;
		char* trimmed_both = params->dat1;
		tou_trim_string(&trimmed_both); // trim to beginning of the key at the front, and the back of the value
		size_t both_len = strlen(trimmed_both);

		if (strlen(trimmed_both) > 0) { // in case split() inserted empty "" element
			// params->dat1 dupe will be done after processing value
			
			// Find value offset from current key ptr
			char* sep = tou_sfind(trimmed_both, keyval_sep);
			if (sep) {
				*sep = '\0'; // cuts off key name in dat1 to be realloc'd later
				char* trimmed_val = sep + kvsep_len;
				tou_trim_front(&trimmed_val);

				params->dat2 = tou_strdup(trimmed_val);
				
				if (params->dat2 != NULL)
					params->destroy_dat2 = 1;
			} else {
				params->dat2 = NULL;
				params->destroy_dat2 = 0;
			}
			
			// There's a '\0' either at the separator or at the end
			// so this trims back of the key
			size_t trimmed_key_len = tou_trim_back(&trimmed_both) - trimmed_both;
			memmove(params->dat1, trimmed_both, trimmed_key_len + 1); // move trimmed key to front
			params->dat1 = realloc(params->dat1, trimmed_key_len + 1); // free(params->dat1)
			params->destroy_dat1 = 1;

		} else {
			// remove this empty element
			// TODO: does the updated llist_remove() perform this operation?
			tou_llist_pop(params);
			char prev_was_null = (params->prev == NULL);
			tou_llist_t* relink = (params->prev) ? params->prev : params->next;
			tou_llist_free_element(params);
			params = relink;

			if (prev_was_null)
				break;

			dont = true; // don't process params->prev as usual since we just adjusted list
		}
		if (!dont) params = params->prev;
	}

	// restore char
	str[maxlen] = saved;

	return tou_llist_get_head(params);
}


/*  */
void tou_paramprint(tou_llist_t* params)
{
	params = tou_llist_get_oldest(params);
	printf("Parameters (%d):\n", tou_llist_len(params));
	while (params) {
		printf("  \"%s\" : %p\n", params->dat1, params->dat2);
		tou_llist_next_newer(&params);
	}
	printf("\n");
}

#endif


/* Loading icon
printf("⣿ ⣾⣽⣻⢿⡿⣟⣯⣷\n");
printf("⠿ ⠾⠽⠻⠟⠯⠷\n"); */


#endif // TOU_IMPLEMENTATION_DONE
#endif // TOU_IMPLEMENTATION