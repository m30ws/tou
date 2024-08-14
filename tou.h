/** 
	@file tou.h
	@brief Tou header library
	@author m30ws
	@version 1.4 (20240814)

	Git repo: https://github.com/m30ws/tou/

	Details
	-------

	Required always but only once! :
	- \#define TOU_IMPLEMENTATION
	
	Other various defines:
	- \#define TOU_LLIST_SINGLE_ELEM (deprecated)
	
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
#define TOU_GET_VERSION() (20240807)

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
#define TOU_DEBUG(x) if (TOU_DBG) { x; } else (void)0
#endif

/**
	@brief Debug prefix before messages used in PRINTD()
*/
#ifndef TOU_DEBUG_PREFIX
#define TOU_DEBUG_PREFIX "[:] "
#endif

/**
	@brief Debug print, used like printf()
*/
#ifndef TOU_PRINTD
#define TOU_PRINTD(format, ...) if (TOU_DBG) { fprintf(stdout, TOU_DEBUG_PREFIX format, ##__VA_ARGS__); } else (void)0
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


/* == Linked list == */
/**
	@addtogroup grp_llist Linked list
	@{
*/

/**
	@struct tou_llist_t
	@brief Linked list struct definition, typedef'd
*/
#ifndef TOU_LLIST_SINGLE_ELEM
	typedef struct tou_llist {
		struct tou_llist* prev; /**< previous element                     */
		struct tou_llist* next; /**< next element                         */
		void* dat1;             /**< useful data 1                        */
		void* dat2;             /**< useful data 2                        */
		char destroy_dat1 : 1;  /**< automatically deallocate this data ? */
		char destroy_dat2 : 1;  /**< automatically deallocate this data ? */
	} tou_llist_t;
#else
	typedef struct tou_llist {
		struct tou_llist* prev; /**< previous element                     */
		struct tou_llist* next; /**< next element                         */
		void* dat1;             /**< useful data                          */
		char destroy_dat1 : 1;  /**< automatically deallocate this data ? */
	} tou_llist_t;
#endif


/** @} */


// ==============================================================
// ||                        FUNC DECLS                        ||
// ==============================================================

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
*/
void tou_trim_back(char** str);

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
#define TOU_SILENCE( ... ) \
	_tou_g_saved_stdout = tou_disable_stdout(); \
	__VA_ARGS__ ; \
	tou_enable_stdout(_tou_g_saved_stdout);

/**
	Filedescriptor handle local to this compilation unit used with ::TOU_SILENCE.
	Since it isn't surrounded by `#ifdef TOU_IMPLEMENTATION` it'll be declared each time you \#include the header.
*/
int _tou_g_saved_stdout;


/** @} */


/** @} */


/* == Linked list == */
/**
	@addtogroup grp_llist Linked list
	@{
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
	@brief Creates/initializes new llist.
	
	Define creation of a new llist through \#define so even though it
	doesn't require anything more than =NULL currently it would be
	easier to extend in the future if a need for such initializer arises.
*/
#define tou_llist_new(...) ((TOU_DBG ? fprintf(stdout,TOU_DEBUG_PREFIX"Creating new llist :: NULL\n") : (void)0) , NULL)

/* */
// typedef tou_llist_t* tou_llist_elem;
/* */
// #define TOU_LLIST_UNPACK(elem) (*(elem))->dat1,(*(elem))->dat2

#ifndef TOU_LLIST_SINGLE_ELEM
	/**
		@brief Appends a brand new element to the llist and updates the pointer in place.
		
		Also returns pointer to the newly created element. (Newer elements are found
		using .next)

		@param[in] list Address of first element (tou_llist_t**)
		@param[in] dat1 Data 1
		@param[in] dat2 Data 2
		@param[in] dat1_is_dynalloc Should data1 be free()'d when destroying list?
		@param[in] dat2_is_dynalloc Should data2 be free()'d when destroying list?
		@return Pointer to the newly created element
	*/

	tou_llist_t** tou_llist_append(tou_llist_t** list, void* dat1, void* dat2, char dat1_is_dynalloc, char dat2_is_dynalloc);
	/**
		@brief Appends a brand new element to the llist containing only `dat1`
		and updates the pointer in place.
		
		`dat2` is set to a default of NULL and 0. Also returns pointer to the newly created element. (Newer elements are found
		using .next)

		@param[in] list Address of first element (tou_llist_t**)
		@param[in] dat1 Data 1
		@param[in] dat1_is_dynalloc Should data1 be free()'d when destroying list?
		@return Pointer to the newly created element
	*/
	tou_llist_t** tou_llist_appendone(tou_llist_t** list, void* dat1, char dat1_is_dynalloc);

#else
	tou_llist_t** tou_llist_append(tou_llist_t** list, void* dat1, char dat1_is_dynalloc);
	
	// Auto-convert SINGLE_ELEM style in case it was used
	#define tou_llist_appendone tou_llist_append
#endif

/**
	@brief Traverses elements using .next or .prev and frees each one
	including copied category string

	@param[in] list Either head or tail element
*/
void tou_llist_destroy(tou_llist_t* list);

/**
	@brief Removes an element from llist and deallocates it

	@param[in] elem Pointer to the element to be removed
*/
void tou_llist_remove(tou_llist_t* elem);

/**
	@brief Removes an element from llist but does not deallocate it
	
	@param[in] elem Pointer to the element to be removed
	@return tou_llist_t* Pointer to the element popped from llist
*/
tou_llist_t* tou_llist_pop(tou_llist_t* elem);

/**
	@brief Frees a standalone "element". Do NOT use on an element
	still associated with list; only on a pop()'d elem for example

	@param[in] elem Pointer to the element to be freed
*/
void tou_llist_free_element(tou_llist_t* elem);

/**
	@brief Traverses the whole list through ->next and returns the last element
	that was added into list (where ->next is NULL)

	@param[in] list Element of type tou_llist_t*
	@return tou_llist_t* Pointer to the head element
*/
tou_llist_t* tou_llist_get_head(tou_llist_t* list);

/**
	@brief Traverses the whole list through ->prev and returns the first element
	that was added to list (where ->prev is NULL)

	@param[in] list Element of type tou_llist_t*
	@return tou_llist_t* Pointer to the tail element
*/
tou_llist_t* tou_llist_get_tail(tou_llist_t* list);

/**
	@brief Checks if element is head of the list
	(->next should be NULL)

	@param[in] elem Element to check
	@return 1 or 0 depending on result
*/
char tou_llist_is_head(tou_llist_t* elem);

/**
	@brief Checks if element is tail of the list
	(->prev should be NULL)

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

#ifndef TOU_LLIST_SINGLE_ELEM
/**
	@brief Traverses the list starting from the passed element and tries
	to find the first matching element by simple equals-comparison
	using both parameters at the same time.

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@param[in] dat2 Comparison value (if enabled)
	@return Found element or NULL
*/
	tou_llist_t** tou_llist_find_exact(tou_llist_t** list, void* dat1, void* dat2);

/**
	@brief Traverses the list starting from the passed element and tries
	to find the first matching element by simple equals-comparison of param.

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@return Found element or NULL
*/
	tou_llist_t** tou_llist_find_exactone(tou_llist_t** list, void* dat1);

#else
	tou_llist_t** tou_llist_find_exact(tou_llist_t** list, void* dat1);

	// Auto-convert SINGLE_ELEM style in case it was used
	#define tou_llist_find_exactone tou_llist_find_exact
#endif

/**
	@brief Traverses the list starting from the passed element and tries
	to find the first matching element on the basis: if dat1 is not null,
	tries to match it as a nul-terminated string

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@return Found element or NULL
*/
tou_llist_t** tou_llist_find_key(tou_llist_t** list, void* dat1);

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
tou_llist_t** tou_llist_find_func(tou_llist_t** list, tou_func2 cb, void* userdata);

/**
	@brief Goes through the list and returns amount of elements.

	@param[in] list List to count elements
	@return Number of elements
*/
size_t tou_llist_len(tou_llist_t* list);

/**
	@brief Constructs a dynamically allocated array containing only
	.dat1 properties (stored as void*).

	@param[in] list Linked list to construct array from
	@param[out] len Optional length of the array/llist
	@return Void* array of .dat1's
*/
void** tou_llist_gather_dat1(tou_llist_t* list, size_t* len);

/**
	@brief Constructs a dynamically allocated array containing only
	.dat2 properties (stored as void*).

	@param[in] list Linked list to construct array from
	@param[out] len Optional length of the array/llist
	@return Void* array of .dat2's
*/
void** tou_llist_gather_dat2(tou_llist_t* list, size_t* len);


/** @} */


/* == Strings higher level == */
/**
	@addtogroup grp_string 
	@{
*/

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
	@brief (Re)allocates enough memory for src and appends it to dst

	Does not know the size of the original destination and will not
	consider it!

	@param[in] dst String which will be expanded and copied into
	@param[in] src String to copy
	@return Realloc'd pointer with combined strings
*/
char* tou_sappend(char* dst, char* src);

/**
	@brief (Re)allocates enough memory for src and prepends it to dst

	Does not know the size of the original destination and will not
	consider it!

	@param[in] dst String which will be expanded and copied into
	@param[in] src String to copy
	@return Realloc'd pointer with combined strings
*/
char* tou_sprepend(char* dst, char* src);

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
#define tou_sisempty(elem, dat) \
	((elem) == NULL /* wanted to do without this one but i can already see segfaults happening without it */ || \
	 (elem)->dat == NULL || \
	 tou_strlen(tou_trim_front_pure((elem)->dat)) == 0)


/** @} */


/* == INI parser == */
/**
	@addtogroup grp_ini INI operations
	@{
*/

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
	@return Pointer to the allocated structured data
*/
tou_llist_t* tou_ini_parse_fp(FILE* fp);

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
	@return Pointer to the section (tou_llist_t**), or NULL
*/
tou_llist_t** tou_ini_get_section(tou_llist_t** inicontents, const char* section_name);

/**
	@brief Returns pointer to the property element matching given 'section_name' and 'key'.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@return Pointer to the property (tou_llist_t**), or NULL
*/
tou_llist_t** tou_ini_get_property(tou_llist_t** inicontents, const char* section_name, const char* key);

/** @} */

/**
	@brief Returns the contents of 'key' under the given 'section_name'.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@return Pointer to the contents, or NULL
*/
void* tou_ini_get(tou_llist_t** inicontents, const char* section_name, const char* key);

/**
	@brief Sets the contents of 'key' under the given 'section_name'
	to the specified value, reallocating memory if necessary.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@param[in] new_value 
	@return Pointer to the new value that was stored in struct, or NULL
*/
void* tou_ini_set(tou_llist_t** inicontents, const char* section_name, const char* key, char* new_value);

/**
	@brief Return a pointer to the llist entry of the matching section, or NULL.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@return Pointer to the section
*/
tou_llist_t* tou_ini_section(tou_llist_t** inicontents, const char* section_name);

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


/** @} */


/* == Parsers == */
/**
	@addtogroup grp_parsers_converters Parsers & Converters
	@{
*/

/**
	@brief Parses "parameter string" and returns a llist of key-value pairs,
		looking at the whole string (strlen will be used)

	Extra whitespaces are ignored and pairs may be key-only (values may be empty);
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

	Extra whitespaces are ignored and pairs may be key-only (values may be empty);
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


/** @} */


#endif // __TOU_H_


// ==============================================================
// ||                       IMPLEMENTATION                     ||
// ==============================================================


#ifdef TOU_IMPLEMENTATION
#if defined(TOU_IMPLEMENTATION_DONE)
#pragma error "TOU_IMPLEMENTATION already defined somewhere!"
#else

#define TOU_IMPLEMENTATION_DONE


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
	while (TOU_IS_BLANK(*ptr))
		ptr++;
	*str = ptr;
	return ptr;
}


/*  */
void tou_trim_back(char** str)
{
	if (str == NULL || *str == NULL)
		return;

	char* ptr = (*str + strlen(*str) - 1);
	while (TOU_IS_BLANK(*ptr)) {
		*ptr = '\0';
		ptr--;
	}
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
				goto sfindmultiplen_break; // break;
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
sfindmultiplen_break:
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
		if (cb((void*)found, (void*)kwds[found_idx], userdata) == TOU_BREAK) {
			TOU_PRINTD("[iter_find_multiple_n] breaking early\n");
			return;
		}
		// Iter continue
		text_ptr = found + tou_strlen(kwds[found_idx]);
		remaining -= (text_ptr - src);
	}
	// // Calls user func for the remaining part which
	// // doesn't have kwd in it but with idx will be set to -1
	// if (text_ptr && *text_ptr) {
	// 	// TOU_PRINTD("[iter_find_multiple_n] calling user func with (\"%s\", NULL, %p)\n", text_ptr, userdata);
	// 	cb((void*)text_ptr, NULL, userdata);
	// }
}


/*  */
tou_llist_t* tou_split(char* str, const char* delim)
{
	if (!str || !delim)
		return NULL;

	TOU_PRINTD("[tou_split] STR_LEN :: %d\n", strlen(str));

	size_t delim_len = strlen(delim);
	tou_llist_t* list = NULL;
	char* pos_start = str;
	char* pos_delim = tou_sfind(str, delim);
	
	while (pos_delim) {

		// TODO: swap with tou_str[n]dup() ?
		char* buf = malloc(pos_delim-pos_start + 1);
		// strncpy(buf, pos_start, pos_delim-pos_start);
		tou_strlcpy(buf, pos_start, pos_delim-pos_start + 1);
		TOU_PRINTD("[tou_split] BUF: %s\n", buf);
		tou_llist_append(&list, buf, NULL, 1, 0);

		// Find next occurence
		pos_start = pos_delim + delim_len;
		pos_delim = tou_sfind(pos_start/*pos_delim + delim_len*/, delim);
	}

	size_t len = strlen(pos_start);
	TOU_PRINTD("[tou_split] FINAL LEN :: %d\n", len);
	if (len > 0) {
		// Append last part till the end
		// TODO: swap with tou_strndup() ?
		char* buf = malloc(len + 1);
		tou_strlcpy(buf, pos_start, len + 1);
		TOU_PRINTD("[tou_split] BUF: %s\n", buf);
		tou_llist_append(&list, buf, NULL, 1, 0);
	}

	return list;//tou_llist_get_tail(list);
}


/*  */
char* tou_sappend(char* dst, char* src)
{
	size_t dst_len = strlen(dst);
	size_t src_len = strlen(src);

	size_t new_size = dst_len + src_len + 1;
	dst = realloc(dst, new_size);
	tou_strlcpy(dst + dst_len, src, new_size);

	return dst;
}


/*  */
char* tou_sprepend(char* dst, char* src)
{
	size_t dst_len = strlen(dst);
	size_t src_len = strlen(src);

	char* new_mem = malloc(dst_len + src_len + 1);

	memcpy(new_mem,           src, src_len);
	memcpy(new_mem + src_len, dst, dst_len + 1); // copy \0 too

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

	size_t true_len = strlen(str);
	size_t repl_len = strlen(repl_str);
	size_t with_len = strlen(with_str);
	
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
	size_t rest = strlen(search_ptr); // last part + non-searched remainder since replaced char was returned to place
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
		*len_ptr = strlen(dst);
	}
	return dst;
}


/*  */
size_t tou_read_fp_in_blocks(FILE* fp, size_t blocksize, tou_func3 cb, void* userdata)
{
	// Clamp blocksize to TOU_DEFAULT_BLOCKSIZE if not in following range:
	if (blocksize < 1 || blocksize > 0xFFFFFF) {
		blocksize = TOU_DEFAULT_BLOCKSIZE;
		TOU_PRINTD("[tou_read_fp_in_blocks] clamping blocksize to "TOU_MSTR(TOU_DEFAULT_BLOCKSIZE)"\n");
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
			if (cb(blockbuf, (void*) cnt, userdata) == TOU_BREAK) {
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
	} else if ((fp = fopen(filename, "r")) == NULL) { // file
		TOU_PRINTD("[read_file] cannot open '%s'\n", filename);
		return NULL;
	}

	TOU_PRINTD("[read_file] reading from: %s\n", (fp==stdin) ? "STDIN" : filename);

	// char* read = tou_read_fp_in_blocks(fp, read_len, -1, NULL, NULL); // default block size, without function
	// tou_read_fp_in_blocks -> (FILE* fp, size_t* read_len, size_t blocksize, tou_func3 cb, void* userdata)
	tou_block_store_struct tmp = {NULL, 0};
	tou_read_fp_in_blocks(fp, TOU_DEFAULT_BLOCKSIZE, tou_block_store_cb, &tmp);
	
	TOU_PRINTD("[read_file] finished reading blocks.\n");

	if (fp != stdin) {
		fclose(fp);
		TOU_PRINTD("[read_file] closed.\n");
	}

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


#ifndef TOU_LLIST_SINGLE_ELEM
	/*  */
	tou_llist_t** tou_llist_append(tou_llist_t** list, void* dat1, void* dat2, char dat1_is_dynalloc, char dat2_is_dynalloc)
	{
		tou_llist_t** curr_head = (tou_llist_t**) list;
		tou_llist_t* new_head = malloc(sizeof(*new_head));

		new_head->prev = *curr_head;
		new_head->next = NULL;
		if (*curr_head != NULL)
			(*curr_head)->next = new_head;

		new_head->dat1 = dat1;
		new_head->dat2 = dat2;
		new_head->destroy_dat1 = dat1_is_dynalloc;
		new_head->destroy_dat2 = dat2_is_dynalloc;

		*curr_head = new_head;
		return curr_head;
	}

	/*  */
	tou_llist_t** tou_llist_appendone(tou_llist_t** list, void* dat1, char dat1_is_dynalloc)
	{
		return tou_llist_append(list, dat1, NULL, dat1_is_dynalloc, 0);
	}

#else
	/*  */
	tou_llist_t** tou_llist_append(tou_llist_t** list, void* dat1, char dat1_is_dynalloc)
	{
		tou_llist_t** curr_head = (tou_llist_t**) list;
		tou_llist_t* new_head = malloc(sizeof(*new_head));

		new_head->prev = *curr_head;
		new_head->next = NULL;
		if (*curr_head != NULL)
			(*curr_head)->next = new_head;

		new_head->dat1 = dat1;
		new_head->destroy_dat1 = dat1_is_dynalloc;

		*curr_head = new_head;
		return curr_head;
	}
#endif


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
void tou_llist_remove(tou_llist_t* elem)
{
	if (!elem) return;

	if (elem->prev) {
		elem->prev->next = elem->next;
	}
	if (elem->next) {
		elem->next->prev = elem->prev;
	}

	if (elem->destroy_dat1) free(elem->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
	if (elem->destroy_dat2) free(elem->dat2);
#endif
	free(elem);
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
char tou_llist_is_head(tou_llist_t* elem)
{
	return elem->next == NULL;
}


/*  */
char tou_llist_is_tail(tou_llist_t* elem)
{
	return elem->prev == NULL;
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


/*
	[[WIP]] Manipulate elements from function during iteration ?
	
	Iterate through and call the specified function for each element
	Automatically checks whether given element is head or tail and 
	iterates accordingly.
	If given function returns a value different from 0 the iteration
	terminates early.

	@param[in] list ADDRESS of the Head or tail of the list
	@param[in] cb Function to be called for each element
*/
/* void tou_llist_iterex(tou_llist_t** list, tou_func cb)
{
	if (!list || !cb)
		return;

	if (list->prev == NULL) { // iter-from-head
		while (list) {
			if (cb(list) != 0)
				break;
			list = list->next;
		}

	} else if (list->next == NULL) { // iter-from-tail
		while (list) {
			if (cb(list) != 0)
				break;
			list = list->prev;
		}
	}
} */


#ifndef TOU_LLIST_SINGLE_ELEM
	/*  */
	tou_llist_t** tou_llist_find_exact(tou_llist_t** list, void* dat1, void* dat2)
	{
		if (list == NULL || *list == NULL)
			return NULL;

		/* TOU_PRINTD("[find_exact] %p, %p, %p\n", list, dat1, dat2); */

		tou_llist_t** tracker = list;
		while (*tracker) {
			/* TOU_PRINTD("[find_exact] %s (%p), %s (%p)\n",
				(*tracker)->dat1, (*tracker)->dat1, (*tracker)->dat2, (*tracker)->dat2); */
			if ((*tracker)->dat1 == dat1)
				return tracker;
			if ((*tracker)->dat2 == dat2)
				return tracker;
			tracker = &((*tracker)->prev);
		}

		return NULL;
	}

	/*  */
	tou_llist_t** tou_llist_find_exactone(tou_llist_t** list, void* dat1)
	{
		if (list == NULL || *list == NULL)
			return NULL;

		tou_llist_t** tracker = list;
		while (*tracker) {
			if ((*tracker)->dat1 == dat1)
				return tracker;
			tracker = &((*tracker)->prev);
		}

		return NULL;
	}
#else
	/*  */
	tou_llist_t** tou_llist_find_exact(tou_llist_t** list, void* dat1)
	{
		if (list == NULL || *list == NULL)
			return NULL;

		/* TOU_PRINTD("[find_exact] %p, %p\n", list, dat1); */

		tou_llist_t** tracker = list;
		while (*tracker) {
			/* TOU_PRINTD("[find_exact] %s (%p)\n",
				(*tracker)->dat1, (*tracker)->dat1); */
			if ((*tracker)->dat1 == dat1)
				return tracker;
			tracker = &((*tracker)->prev);
		}

		return NULL;
	}
#endif


/*  */
tou_llist_t** tou_llist_find_key(tou_llist_t** list, void* dat1)
{
	if (list == NULL || *list == NULL)
		return NULL;

	tou_llist_t** tracker = list;
	while (*tracker) {
		if (strcmp((*tracker)->dat1, dat1) == 0) {
			TOU_PRINTD("[llist_find_key] FOUND %s (%p) %s\n", (*tracker)->dat1, (*tracker)->dat2, (*tracker)->dat2);
			return tracker;
		}
		tracker = &((*tracker)->prev);
	}

	TOU_PRINTD("[llist_find_key] returning NULL\n");
	return NULL;
}


/*  */
tou_llist_t** tou_llist_find_func(tou_llist_t** list, tou_func2 cb, void* userdata)
{
	if (list == NULL || *list == NULL || cb == NULL)
		return NULL;

	tou_llist_t** tracker = list;
	while (*tracker) {
		if ((size_t) cb(*tracker, userdata) == TOU_BREAK)
			return tracker;
		tracker = &((*tracker)->prev);
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


/*  */
tou_llist_t* tou_ini_parse_fp(FILE* fp)
{
	if (!fp) {
		TOU_PRINTD("ini_parse_fp received empty fp\n");
		return NULL;
	}

	tou_llist_t* inistruct = tou_llist_new(); //= NULL
	char line_buf[256+1];
	char* line = line_buf;
	int line_len;
	size_t line_no = 0;

	while (fgets(line, 256+1, fp)) {
		line_no++; // inc line tracker right away

		tou_trim_front(&line);
		if (*line == '\0')
			continue;

		// Comment line
		switch(*line) {
			case ';':
			case '#':
				continue;
		}

		// Section indication
		if (*line == '[') {

			tou_trim_back(&line);
			line_len = strlen(line);
			
			// Cut off front bracket
			line++;
			line_len--;

			// If other bracket doesn't exist ignore it and still parse as valid syntax
			if (line[line_len - 1] == ']') {
				line[line_len - 1] = '\0';
				line_len--;
			}

			// Append section
			tou_llist_append(&inistruct,
				tou_strdup(line), NULL,
				1, 0);

			TOU_PRINTD("Appended section: %s\n", inistruct->dat1);
			continue;
		}

		// Alright then, check if line matches a property line type; otherwise its an invalid line
		int prop_len;
		char* prop;
		
		// Test if either delimiter exists in line
		prop = tou_sfind(line, " = ");
		prop_len = 3;

		if (prop == NULL) {
			prop = tou_sfind(line, "=");
			prop_len = 1;

			// Invalid line
			if (prop == NULL) {
				int len = strlen(line);
				if (line[len - 1] == '\n')
					line[len - 1] = '\0';
				TOU_PRINTD("Invalid line encountered while parsing (line %zu): %s\n", line_no, line);
				tou_ini_destroy(inistruct);
				return NULL;
			}
		}

		// Property encountered before any section declaration
		if (inistruct == NULL) {
			TOU_PRINTD("Property specified before any section (line %zu): %s\n", line_no, line);
			tou_ini_destroy(inistruct);
			return NULL;
		}

		// Duplicate key and value and append to list
		*prop = '\0';
		char* key = line;
		char* val = prop + prop_len;

		if (TOU_IS_BLANK(*val)) // Remove leading (only 1) whitespace
			val++;
		tou_trim_back(&key);
		tou_trim_back(&val);

		TOU_PRINTD("  Key: %s, Val: %s\n", key, val);
		tou_llist_append((tou_llist_t**)(&inistruct->dat2),    // append to "current section" element
			tou_strdup(key), tou_strdup(val),                // copy key, copy val
			1, 1);                                           // auto dealloc both key&val
	}

	return inistruct;
}


/*
	Parses given buffer as .INI file and returns structured data.

	@param[in] contents Buffer containing raw .INI data
	@return Pointer to the allocated structured data
*/
// void* tou_ini_parse(char* contents)
// {
// 	// raise notimplemented :*
// 	return NULL;
// }


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
	fprintf(stdout, "<.INI structure>\n");
	fprintf(stdout, " |\n");

	while (section) {
		props = section->dat2;
		char next_section_ch = (section->prev == NULL) ? ' ' : '|';
		fprintf(stdout, " +-> [Section] \"%s\"\n", section->dat1);
		while (props) {
			fprintf(stdout, " %c     |\n", next_section_ch);
			fprintf(stdout, " %c     +-> [Prop] \"%s\": \"%s\"\n",
				next_section_ch, props->dat1, props->dat2);
			props = props->prev;
		}
		fprintf(stdout, " %c\n", next_section_ch);
		section = section->prev;
	}
}


/*  */
tou_llist_t** tou_ini_get_section(tou_llist_t** inicontents, const char* section_name)
{
	if (inicontents == NULL || *inicontents == NULL || section_name == NULL) {
		TOU_PRINTD("ini_get_section received empty params\n");
		return NULL;
	}

	tou_llist_t** sect = tou_llist_find_key(inicontents, (void*)section_name);
	if (sect == NULL) {
		TOU_PRINTD("ini_get_section unable to find given section\n");
		return NULL;
	}

	return sect;
}


/*  */
tou_llist_t** tou_ini_get_property(tou_llist_t** inicontents, const char* section_name, const char* key)
{
	if (inicontents == NULL || *inicontents == NULL || section_name == NULL || key == NULL) {
		TOU_PRINTD("ini_get_property received empty params\n");
		return NULL;
	}

	tou_llist_t** sect = tou_llist_find_key(inicontents, (void*) section_name);
	if (sect == NULL) {
		TOU_PRINTD("ini_get_property unable to find given section\n");
		return NULL;
	}
	// TOU_PRINTD("TOU_INI_GET_PROPERTY found SECTION %s\n", (*sect)->dat1);

	tou_llist_t** prop = tou_llist_find_key( (tou_llist_t**)(&((*sect)->dat2)), (void*)key );
	// tou_llist_t** prop = tou_llist_find_key(TOU_LLIST_DAT_ADDR(sect, dat2), (void*)key );
	if (prop == NULL) {
		TOU_PRINTD("ini_get_property unable to find given key\n");
	}

	return prop;
}


/*  */
tou_llist_t* tou_ini_section(tou_llist_t** inicontents, const char* section_name)
{
	tou_llist_t** sect = tou_ini_get_section(inicontents, (void*)section_name);
	if (sect == NULL)
		return NULL;
	return (*sect)->dat2;
}


/*  */
void* tou_ini_property(tou_llist_t* section, const char* property_name)
{
	tou_llist_t** prop = tou_llist_find_key(&section, (void*)property_name);
	if (prop == NULL)
		return NULL;
	return (*prop)->dat2;
}


/*  */
void* tou_ini_get(tou_llist_t** inicontents, const char* section_name, const char* key)
{
	tou_llist_t** prop = tou_ini_get_property(inicontents, section_name, key);
	TOU_PRINTD("ini_get  prop: %p\n", prop);

	if (prop)
		return (*prop)->dat2;
	return NULL;
}


/*  */
void* tou_ini_set(tou_llist_t** inicontents, const char* section_name, const char* key, char* new_value)
{
	if (inicontents == NULL || *inicontents == NULL || section_name == NULL || key == NULL || new_value == NULL) {
		TOU_PRINTD("[ini_set] received empty params\n");
		return NULL;
	}

	tou_llist_t** sect = tou_llist_find_key(inicontents, (void*) section_name);
	if (sect == NULL) {
		TOU_PRINTD("[ini_set] section not found, allocating new...\n");
		// TODO: tou_ini_new_section ?
		sect = tou_llist_append(inicontents, tou_strdup(section_name), NULL, 1, 0);
	}
	
	tou_llist_t** prop = tou_llist_find_key( (tou_llist_t**)(&((*sect)->dat2)), (void*) key );

	if (prop == NULL) {
		// Allocate new property...
		TOU_PRINTD("[ini_set] property not found\n");
		prop = tou_llist_append( (tou_llist_t**)(&((*sect)->dat2)),
			tou_strdup(key), tou_strdup(new_value),
			1, 1);
		TOU_PRINTD("[ini_set] %s, %s\n", (*prop)->dat1, (*prop)->dat2);
		return (*prop)->dat2;

	} else {
		// ...or set/realloc the existing one
		TOU_PRINTD("[ini_set] property found\n");
		char* old_value = (*prop)->dat2;
		size_t old_len = strlen(old_value);
		size_t new_len = strlen(new_value);
		
		if (new_len > old_len)
			old_value = realloc(old_value, new_len + 1);

		tou_strlcpy(old_value, new_value, new_len + 1);
		(*prop)->dat2 = old_value;

		return (*prop)->dat2;
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
		fprintf(fp, "[%s]", section->dat1);
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
			fprintf(fp, "\n%s = %s", prop->dat1, prop->dat2);
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
	fprintf(fp, "<%s ver="TOU_MSTR(TOU_XML_DATA_VER)" len=\"%zu\">", root_tag, tou_llist_len(inicontents));
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
		char* sect_name_tag = tou_replace_ch(strdup(section->dat1), ' ', '-');
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

	// save char to restore it later
	char saved = str[maxlen];
	str[maxlen] = '\0';
	
	// parse
	tou_llist_t* params = tou_split(str, param_sep);
	tou_llist_t* ret = params;

	while (params) {
		bool dont = false;
		char* trimmed_key = tou_trim_front_pure(params->dat1);

		if (!tou_sisempty(params, dat1)) { // in case split() inserted empty "" element
			if (trimmed_key != params->dat1) {
				memmove(params->dat1, trimmed_key, strlen(trimmed_key) + 1); // move trimmed key to front
			}
			
			char* sep = tou_sfind(params->dat1, keyval_sep);
			if (sep) {
				*sep = '\0'; // cuts off key name in dat1
				params->dat2 = sep + kvsep_len; // assign val to dat2

				char* trimmed_val = tou_trim_front_pure(params->dat2);
				if (trimmed_val != params->dat2) {
					memmove(params->dat2, trimmed_val, strlen(trimmed_val) + 1); // move trimmed val to front
				}

			} else {
				params->dat2 = NULL;
			}

		} else {
			// remove this empty element
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


/* Loading icon
printf("⣿ ⣾⣽⣻⢿⡿⣟⣯⣷\n");
printf("⠿ ⠾⠽⠻⠟⠯⠷\n"); */


#endif // TOU_IMPLEMENTATION_DONE
#endif // TOU_IMPLEMENTATION