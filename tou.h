/** 
	@file tou.h
	@brief Tou header library
	=========================
	
	Required always but only once! :
	- #define TOU_IMPLEMENTATION

	Other various defines:
	- #define TOU_LLIST_SINGLE_ELEM

	Things:
	- full linked list impl
	- reading file in "blocks" (improve this)
	- string splitter, trimmer etc. different operations
	- .INI file parser / exporter(+JSON)
	- xml (todo)
	- safer string functions
	- disabling And restoring stdout
	- statically allocated linked list (todo)

	- A ton of helper definitions
*/


#ifndef __TOU_H_
#define __TOU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
	@brief Control library debug output & debug code blocks
*/
#ifndef TOU_DBG
#define TOU_DBG 0 
#endif

/**
	@brief Place a block of code inside that you wish to enable if debugging
 
	It's not a #define but should get optimized
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

/** @brief How many bytes to read when reading a file if unspecified */
#ifndef TOU_DEFAULT_BLOCKSIZE
#define TOU_DEFAULT_BLOCKSIZE 4096
#endif

/** @brief Data format version when exporting INI to JSON */
#define TOU_JSON_DATA_VER "1.0"

/**
	@brief Helps convert 's' into a literal
*/
#ifndef TOU_STR
#define TOU_STR(s) #s
#endif
/**
	@brief Helps convert 's' into C literal

	Ex. MSTR(0xFFBA) -> "0xFFBA"
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
enum {
	TOU_BREAK    = 0,
	TOU_CONTINUE = 1,
};


/* Func ptrs */

/**
	@brief Arbitrary function pointer with 1 parameter

	@param void* Parameter
	@return void* Return value
*/
typedef void* (*tou_func)(void*);
/**
	@brief Arbitrary function pointer with 2 parameters

	@param void* First parameter
	@param void* Second parameter
	@return void* Return value
*/
typedef void* (*tou_func2)(void*, void*);
/**
	@brief Arbitrary function pointer with 3 parameters

	@param void* First parameter
	@param void* Second parameter
	@param void* Third parameter
	@return void* Return value
*/
typedef void* (*tou_func3)(void*, void*, void*);


/* Llist defs */

/**
	@struct tou_llist
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
	} tou_llist;
#else
	typedef struct tou_llist {
		struct tou_llist* prev; /**< previous element                     */
		struct tou_llist* next; /**< next element                         */
		void* dat1;             /**< useful data                          */
		char destroy_dat1 : 1;  /**< automatically deallocate this data ? */
	} tou_llist;
#endif


// ==============================================================
// ||                        FUNC DECLS                        ||
// ==============================================================


/* == String functions == */

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
	http://www.courtesan.com/todd/papers/strlcpy.html for
	information on strlcpy().
	
	@param[out] dst Destination buffer
	@param[in] src Source string
	@param[in] size Max size after concatenation *including* null terminator
	@return Length of the string after concatenation
*/
size_t tou_strlcat(char* dst, const char* src, size_t size);

/**
	Strdup implementation to not depend on compilers.
	
	@param[in] src String to be duplicated
	@return Pointer to the newly allocated copy of the string
*/
char* tou_strdup(const char* src);

/**
	Returns a newly allocated copy of a string, capped at 'size' length,
	or NULL if unable to allocate. Appends NUL at the end.
	
	@param[in] src String to copy from
	@param[in] size Max copied bytes from string
	@return Pointer to a newly allocated string
*/
char* tou_strndup(const char* src, size_t size);

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


/* == File operations == */

/**
	@brief Reads file in blocks, calling user-provided function for each block.

	Automatically allocates memory and optionally returns amount read (may be set to null).
	Function receives 3 parameters:
		- data [in] Pointer to the beginning of new data
		- len [in] Amount of bytes actually read
		- userdata [in] User data provided
	Data pointer is actually offset in the total buffer where whole file is stored.

	@param[in] fp FILE* from which data is to be read
	@param[out] read_len Optional pointer to where to store file size
	@param[in] blocksize Size in bytes; set to 0 to use default (TOU_DEFAULT_BLOCKSIZE)
	@param[in] cb Function to call for each block
	@param[in] userdata Custom data to be passed to function
	@return Pointer to loaded file or NULL if error
*/
char* tou_read_fp_in_blocks(FILE* fp, size_t* read_len, size_t blocksize, tou_func3 cb, void* userdata);

/**
	@brief Reads file in blocks.

	Automatically allocates memory and optionally
	returns amount read (may be set to null).

	@param[in] filename Either file name or ""/"stdin" to read from stdin
	@param[out] read_len Optional pointer to where to store file size
	@return Pointer to loaded file or NULL if error
*/
char* tou_read_file(const char* filename, size_t* read_len);


/* == System/IO control == */

/**
	@brief Redirects STDOUT to /dev/null or NUL: files.

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


/* == Linked list == */

/** 
	@brief Address of `dat1` or `dat2` parameter of object.
	
	Used when not a simple address of dat1/dat2 where you have tou_llist* is required, but rather when
	you get list as double ptr and you want changes you make to the inner pointer reflect in
	the original place instead of just locally. This construct is used multiple times when dealing
	with .INI parser and I got tired of typing it and decided to extract it here.
	
	Example: when `section` is a simple pointer you would use:
	
		tou_llist* list = &(section->dat2); // parentheses for clarity
	
	however, if you wanted to reflect changes in the `section` ptr you would have to have `section` as a pointer-to-pointer and:
	
		tou_llist** list = &((*section)->dat2);
	
	which is exactly what this macro does, just professionally cluttered.
*/
#ifndef TOU_LLIST_DAT_ADDR
#define TOU_LLIST_DAT_ADDR(list, param) ((tou_llist**)(&((*(list))->param)))
#endif

/** 
	@brief Creates/initializes new llist.
	
	Define creation of a new llist through #define so even though it
	doesn't require anything more than =NULL currently it would be
	easier to extend in the future if a need for such initializer arises.
*/
#define tou_llist_new(...) ((TOU_DBG ? fprintf(stdout,TOU_DEBUG_PREFIX"Creating new llist :: NULL\n") : (void)0) , NULL)

/* */
// typedef tou_llist* tou_llist_elem;
/* */
// #define TOU_LLIST_UNPACK(elem) (*(elem))->dat1,(*(elem))->dat2

#ifndef TOU_LLIST_SINGLE_ELEM
	/**
		@brief Appends a brand new element to the llist and updates the pointer in place.
		
		Also returns pointer to the newly created element. (Newer elements are found
		using .next)

		@param[in] list Address of first element (tou_llist**)
		@param[in] dat1 Data 1
		@param[in] dat2 Data 2
		@param[in] dat1_is_dynalloc Should data1 be free()'d when destroying list?
		@param[in] dat2_is_dynalloc Should data2 be free()'d when destroying list?
		@return Pointer to the newly created element
	*/

	tou_llist** tou_llist_append(tou_llist** list, void* dat1, void* dat2, char dat1_is_dynalloc, char dat2_is_dynalloc);
	/**
		@brief Appends a brand new element to the llist containing only `dat1`
		and updates the pointer in place.
		
		`dat2` is set to a default of NULL and 0. Also returns pointer to the newly created element. (Newer elements are found
		using .next)

		@param[in] list Address of first element (tou_llist**)
		@param[in] dat1 Data 1
		@param[in] dat1_is_dynalloc Should data1 be free()'d when destroying list?
		@return Pointer to the newly created element
	*/
	tou_llist** tou_llist_appendone(tou_llist** list, void* dat1, char dat1_is_dynalloc);

#else
	tou_llist** tou_llist_append(tou_llist** list, void* dat1, char dat1_is_dynalloc);
	
	// Auto-convert SINGLE_ELEM style in case it was used
	#define tou_llist_appendone tou_llist_append
#endif

/**
	@brief Traverses elements using .next or .prev and frees each one
	including copied category string

	@param[in] list Either head or tail element
*/
void tou_llist_destroy(tou_llist* list);

/**
	@brief Removes an element from llist and deallocates it

	@param[in] elem Pointer to the element to be removed
*/
void tou_llist_remove(tou_llist* elem);

/**
	@brief Removes an element from llist but does not deallocate it
	
	@param[in] elem Pointer to the element to be removed
	@return tou_llist* Pointer to the element popped from llist
*/
tou_llist* tou_llist_pop(tou_llist* elem);

/**
	@brief Frees a standalone "element". Do NOT use on an element
	still associated with list; only on a pop()'d elem for example

	@param[in] elem Pointer to the element to be freed
*/
void tou_llist_free_element(tou_llist* elem);

/**
	@brief Traverses the whole list through ->next and returns the last element
	that was added into list (where ->next is NULL)

	@param[in] list Element of type tou_llist*
	@return tou_llist* Pointer to the head element
*/
tou_llist* tou_llist_get_head(tou_llist* list);

/**
	@brief Traverses the whole list through ->prev and returns the first element
	that was added to list (where ->prev is NULL)

	@param[in] list Element of type tou_llist*
	@return tou_llist* Pointer to the tail element
*/
tou_llist* tou_llist_get_tail(tou_llist* list);

/**
	@brief Checks if element is head of the list
	(->next should be NULL)

	@param[in] elem Element to check
	@return 1 or 0 depending on result
*/
char tou_llist_is_head(tou_llist* elem);

/**
	@brief Checks if element is tail of the list
	(->prev should be NULL)

	@param[in] elem Element to check
	@return 1 or 0 depending on result
*/
char tou_llist_is_tail(tou_llist* elem);

/**
	@brief Iterate through and call the specified function for each element
	Automatically checks whether given element is head or tail and 
	iterates accordingly.
	If given function returns a 0 the iteration terminates early.

	@param[in] list Head or tail of the list
	@param[in] cb Function to be called for each element
*/
void tou_llist_iter(tou_llist* list, tou_func cb);

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
	tou_llist** tou_llist_find_exact(tou_llist** list, void* dat1, void* dat2);

/**
	@brief Traverses the list starting from the passed element and tries
	to find the first matching element by simple equals-comparison of param.

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@return Found element or NULL
*/
	tou_llist** tou_llist_find_exactone(tou_llist** list, void* dat1);

#else
	tou_llist** tou_llist_find_exact(tou_llist** list, void* dat1);

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
tou_llist** tou_llist_find_key(tou_llist** list, void* dat1);

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
tou_llist** tou_llist_find_func(tou_llist** list, tou_func2 cb, void* userdata);

/**
	@brief Goes through the list and returns amount of elements.

	@param[in] list List to count elements
	@return Number of elements
*/
size_t tou_llist_len(tou_llist* list);

/**
	@brief Constructs a dynamically allocated array containing only
	.dat1 properties (stored as void*).

	@param[in] list Linked list to construct array from
	@param[out] len Optional length of the array/llist
	@return Void* array of .dat1's
*/
void** tou_llist_gather_dat1(tou_llist* list, size_t* len);

/**
	@brief Constructs a dynamically allocated array containing only
	.dat2 properties (stored as void*).

	@param[in] list Linked list to construct array from
	@param[out] len Optional length of the array/llist
	@return Void* array of .dat2's
*/
void** tou_llist_gather_dat2(tou_llist* list, size_t* len);

/* == Static linked list == */
/*typedef struct tou_sll {
	struct tou_sll* prev;
	struct tou_sll* next;
	//void* data;
} tou_sll;
 
tou_sll** tou_sll_append(tou_sll** list, void* dat1, char dat1_is_dynalloc)
{
	tou_sll** curr_head = (tou_sll**) list;
	// tou_sll* new_head = malloc(sizeof(*new_head));
	tou_sll* new_head = _next_free_elem();

	new_head->prev = *curr_head;
	new_head->next = NULL;
	if (*curr_head != NULL)
		(*curr_head)->next = new_head;

	new_head->dat1 = dat1;
	new_head->destroy_dat1 = dat1_is_dynalloc;

	*curr_head = new_head;
	return curr_head;
}*/
// #define tou_sll_new(varname, n, sizeofone) tou_sll varname[(n)*((sizeofone)+sizeof(tou_sll))] = {0};


/* == Strings higher level == */

/**
	@brief Finds start of substring(keyword) in the given char*.
	
	@param[in] ptr Source string
	@param[in] kwd Which keyword to search for
	@return Pointer to the beginning of the keyword in the text or NULL
*/
char* tou_find_string_start(char* ptr, char* kwd);

/**
	@brief Splits string using a delimiter that may be longer than one character.

	@param[in] str String to be split
	@param[in] delim Delimiter string
	@return Linked list containing tokens
*/
tou_llist* tou_split(char* str, char* delim);


/* == INI parser == */

/**
	@brief Parses given FILE* as .INI file and constructs structured data.

	Parser supports using semicolon (;), hashtag (#) and percent (%) for
	comments, however they must be on a separate line by themselves.
	Specifying the same section name more than once is not supported (that
	may change in the future where the definitions would merge and "flatten"
	into one combined section).
	When specifying properties both keys and values may have whitespaces in
	them but not at the start and at the end (of course that also means that
	such keys will be sensitive to those whitespaces when using the data).
	Whitespaces may be added between sections, properties and section name
	brackets for better readability.

	Structured data is constructed as two layers of llists where the primary list
	that gets returned from function represents sections in .INI file where the
	section names and properties are stored in .dat1 and .dat2 fields respectively.
	Properties in .dat2 are stored as sub-llist where keys and values are allocated
	and are stored as strings in .dat1 and .dat2 fields respectively.

	@param[in] fp File pointer to where to read the .INI data from
	@return Pointer to the allocated structured data
*/
tou_llist* tou_ini_parse_fp(FILE* fp);

/**
	@brief Destroys contents of .INI structure including deallocation
	of inner structures; assumes head was passed.

	@param[in] inicontents Parsed INI structure
*/
void tou_ini_destroy(tou_llist* inicontents);

/**
	@brief Prints the contents of the parsed .INI structure
	to stdout in a structured graphical format.

	@param[in] inicontents Parsed INI structure
*/
void tou_ini_print(tou_llist* inicontents);

/**
	@brief Returns pointer to the section element matching given 'section_name'.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@return Pointer to the section (tou_llist**), or NULL
*/
tou_llist** tou_ini_get_section(tou_llist** inicontents, const char* section_name);

/**
	@brief Returns pointer to the property element matching given 'section_name' and 'key'.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@return Pointer to the property (tou_llist**), or NULL
*/
tou_llist** tou_ini_get_property(tou_llist** inicontents, const char* section_name, const char* key);

/**
	@brief Returns the contents of 'key' under the given 'section_name'.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@return Pointer to the contents, or NULL
*/
char* tou_ini_get(tou_llist** inicontents, const char* section_name, const char* key);

/**
	@brief Sets the contents of 'key' under the given 'section_name'
	to the specified value, reallocating memory if necessary.

	@param[in] inicontents Pointer to the parsed .INI structure
	@param[in] section_name 
	@param[in] key 
	@param[in] new_value 
	@return Pointer to the new value that was stored in struct, or NULL
*/
char* tou_ini_set(tou_llist** inicontents, const char* section_name, const char* key, char* new_value);

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
int tou_ini_save_fp(tou_llist* inicontents, FILE* fp);

/**
	@brief Exports .INI structure data as JSON to the file stream.

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
int tou_ini_save_fp_json(tou_llist* inicontents, FILE* fp);


// ==============================================================
// ||                         FUNCTIONS                        ||
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
		memcpy(copy, src, src_len + 1);
	}
	return copy;
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
char* tou_find_string_start(char* ptr, char* kwd)
{
	int kwd_len = strlen(kwd);
	char* kwd_ptr = kwd;
	char* ret_ptr = NULL;
	
	while (*ptr) {
		if (*kwd_ptr == '\0') {
			// only time it should be is when kwd is found
			ret_ptr = ptr - kwd_len;
			break;
		}

		if (*kwd_ptr == *ptr) {
			kwd_ptr++;
		} else {
			kwd_ptr = kwd;
		}

		ptr++;
	}

	return ret_ptr;
}


/*  */
tou_llist* tou_split(char* str, char* delim)
{
	if (!str || !delim)
		return NULL;

	TOU_PRINTD("[tou_split] STR_LEN :: %d\n", strlen(str));

	size_t delim_len = strlen(delim);
	tou_llist* list = NULL;
	char* pos_start = str;
	char* pos_delim = tou_find_string_start(str, delim);
	
	while (pos_delim) {

		// TODO: swap with tou_str[n]dup() ?
		char* buf = malloc(pos_delim-pos_start + 1);
		// strncpy(buf, pos_start, pos_delim-pos_start);
		tou_strlcpy(buf, pos_start, pos_delim-pos_start + 1);
		TOU_PRINTD("[tou_split] BUF: %s\n", buf);
		tou_llist_append(&list, buf, NULL, 1, 0);

		// Find next occurence
		pos_start = pos_delim + delim_len;
		pos_delim = tou_find_string_start(pos_start/*pos_delim + delim_len*/, delim);
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

	return tou_llist_get_tail(list);
}


/*  */
char* tou_read_fp_in_blocks(FILE* fp, size_t* read_len, size_t blocksize, tou_func3 cb, void* userdata)
{
	// Clamp blocksize to TOU_DEFAULT_BLOCKSIZE if not in following range:
	if (blocksize < 1 || blocksize > 0xFFFFFF) {
		blocksize = TOU_DEFAULT_BLOCKSIZE;
		TOU_DEBUG( printf(TOU_DEBUG_PREFIX "clamping blocksize to " TOU_MSTR(TOU_DEFAULT_BLOCKSIZE)"\n") );
	}

	size_t curr_bufsize = blocksize;
	char blockbuf[blocksize];
	char* filebuf = NULL;
	if ((filebuf = malloc(curr_bufsize)) == NULL) {
		TOU_DEBUG( printf(TOU_DEBUG_PREFIX "error allocating initial %zu bytes\n", curr_bufsize) );
		return NULL;
	}

	filebuf[0] = '\0';

	size_t rlen = 0;
	while (1) {
		size_t cnt = fread(blockbuf, 1, blocksize, fp);
		if (cnt <= 0)
			break;

		rlen += cnt;

		curr_bufsize += blocksize;
		char* newbuf = realloc(filebuf, curr_bufsize);
		if (!newbuf) {
			TOU_DEBUG( printf(TOU_DEBUG_PREFIX "error allocating %d bytes\n", curr_bufsize) );
			// if (fp != stdin) fclose(fp);
			// if (read_len) *read_len = rlen-cnt;
			// return NULL;
			rlen -= cnt;
			break;
		}

		filebuf = newbuf;
		strcat(filebuf, blockbuf);

		if (cb) {
			if (cb(filebuf, (void*) cnt, userdata) == 0) {
				// User stopped iteration
				TOU_PRINTD("[tou_read_fp_in_blocks] iteration aborted.\n");
				break;
			}
		}
	}

	if (fp != stdin)
		fclose(fp);

	if (read_len)
		*read_len = rlen;
	return filebuf;
}


/*  */
char* tou_read_file(const char* filename, size_t* read_len)
{
	FILE* fp;
	if (!filename || strlen(filename) == 0 || strcmp("stdin", filename) == 0) { // stdin
		fp = stdin;
	} else if ((fp = fopen(filename, "r")) == NULL) { // file
		// TOU_DEBUG( printf(TOU_DEBUG_PREFIX "cannot open '%s'\n", filename) );
		TOU_PRINTD("[read_file] cannot open '%s'\n", filename);
		return NULL;
	}

	// TOU_DEBUG( printf(TOU_DEBUG_PREFIX "reading from: %s\n", (fp==stdin) ? "STDIN" : filename) );
	TOU_PRINTD("[read_file] reading from: %s\n", (fp==stdin) ? "STDIN" : filename);
	char* read = tou_read_fp_in_blocks(fp, read_len, -1, NULL, NULL); // default block size, without function

	if (fp != stdin)
		fclose(fp);
	return read;
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
}


/*  */
int tou_disable_stdout()
{
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


#ifndef TOU_LLIST_SINGLE_ELEM
	/*  */
	tou_llist** tou_llist_append(tou_llist** list, void* dat1, void* dat2, char dat1_is_dynalloc, char dat2_is_dynalloc)
	{
		tou_llist** curr_head = (tou_llist**) list;
		tou_llist* new_head = malloc(sizeof(*new_head));

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
	tou_llist** tou_llist_appendone(tou_llist** list, void* dat1, char dat1_is_dynalloc)
	{
		return tou_llist_append(list, dat1, NULL, dat1_is_dynalloc, 0);
	}

#else
	/*  */
	tou_llist** tou_llist_append(tou_llist** list, void* dat1, char dat1_is_dynalloc)
	{
		tou_llist** curr_head = (tou_llist**) list;
		tou_llist* new_head = malloc(sizeof(*new_head));

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
void tou_llist_destroy(tou_llist* list)
{
	if (!list) return;

	if (list->next == NULL) { // this is head.
		tou_llist *prev, *curr = list;

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
		tou_llist *next, *curr = list;
		
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
void tou_llist_remove(tou_llist* elem)
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
tou_llist* tou_llist_pop(tou_llist* elem)
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
void tou_llist_free_element(tou_llist* elem)
{
	if (!elem) return;

	if (elem->destroy_dat1) free(elem->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
	if (elem->destroy_dat2) free(elem->dat2);
#endif

	free(elem);
}


/*  */
tou_llist* tou_llist_get_head(tou_llist* list)
{
	if (!list) return NULL;

	while (list->next)
		list = list->next;
	
	return list;
}


/*  */
tou_llist* tou_llist_get_tail(tou_llist* list)
{
	if (!list) return NULL;

	while (list->prev)
		list = list->prev;

	return list;
}


/*  */
char tou_llist_is_head(tou_llist* elem)
{
	return elem->next == NULL;
}


/*  */
char tou_llist_is_tail(tou_llist* elem)
{
	return elem->prev == NULL;
}


/*  */
void tou_llist_iter(tou_llist* list, tou_func cb)
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
/* void tou_llist_iterex(tou_llist** list, tou_func cb)
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
	tou_llist** tou_llist_find_exact(tou_llist** list, void* dat1, void* dat2)
	{
		if (list == NULL || *list == NULL)
			return NULL;

		/* TOU_PRINTD("[find_exact] %p, %p, %p\n", list, dat1, dat2); */

		tou_llist** tracker = list;
		while (*tracker) {
			/* TOU_PRINTD("[find_exact] %s (%p), %s (%p)\n",
				(*tracker)->dat1, (*tracker)->dat1, (*tracker)->dat2, (*tracker)->dat2); */
			if ((*tracker)->dat1 == dat1) return tracker;
			if ((*tracker)->dat2 == dat2) return tracker;
			tracker = &((*tracker)->prev);
		}

		return NULL;
	}

	/*  */
	tou_llist** tou_llist_find_exactone(tou_llist** list, void* dat1)
	{
		if (list == NULL || *list == NULL)
			return NULL;

		tou_llist** tracker = list;
		while (*tracker) {
			if ((*tracker)->dat1 == dat1) return tracker;
			tracker = &((*tracker)->prev);
		}

		return NULL;
	}
#else
	/*  */
	tou_llist** tou_llist_find_exact(tou_llist** list, void* dat1)
	{
		if (list == NULL || *list == NULL)
			return NULL;

		/* TOU_PRINTD("[find_exact] %p, %p\n", list, dat1); */

		tou_llist** tracker = list;
		while (*tracker) {
			/* TOU_PRINTD("[find_exact] %s (%p)\n",
				(*tracker)->dat1, (*tracker)->dat1); */
			if ((*tracker)->dat1 == dat1) return tracker;
			tracker = &((*tracker)->prev);
		}

		return NULL;
	}
#endif


/*  */
tou_llist** tou_llist_find_key(tou_llist** list, void* dat1)
{
	if (list == NULL || *list == NULL)
		return NULL;

	tou_llist** tracker = list;
	while (*tracker) {
		if (strcmp((*tracker)->dat1, dat1) == 0) {
			//!!TOU_PRINTD("[llist_find_key] FOUND %s (%p)\n", (*tracker)->dat1, (*tracker)->dat2);
			return tracker;
		}
		tracker = &((*tracker)->prev);
	}

	TOU_PRINTD("[llist_find_key] returning NULL\n");
	return NULL;
}


/*  */
tou_llist** tou_llist_find_func(tou_llist** list, tou_func2 cb, void* userdata)
{
	if (list == NULL || *list == NULL || cb == NULL)
		return NULL;

	tou_llist** tracker = list;
	while (*tracker) {
		if ((size_t) cb(*tracker, userdata) == 0)
			return tracker;
		tracker = &((*tracker)->prev);
	}

	return NULL;
}


/*  */
size_t tou_llist_len(tou_llist* list)
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
void** tou_llist_gather_dat1(tou_llist* list, size_t* len)
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
void** tou_llist_gather_dat2(tou_llist* list, size_t* len)
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


// tou_sll


/*  */
tou_llist* tou_ini_parse_fp(FILE* fp)
{
	if (!fp) {
		TOU_PRINTD("ini_parse_fp received empty fp\n");
		return NULL;
	}

	tou_llist* inistruct = tou_llist_new(); //= NULL
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
		if (*line == ';' || *line == '#' || *line == '%')
			continue;

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
		prop = tou_find_string_start(line, " = ");
		prop_len = 3;

		if (prop == NULL) {
			prop = tou_find_string_start(line, "=");
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
		tou_trim_back(&val);

		TOU_PRINTD("  Key: %s, Val: %s\n", key, val);
		tou_llist_append((tou_llist**)(&inistruct->dat2),    // append to "current section" element
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
void tou_ini_destroy(tou_llist* inicontents)
{
	tou_llist* section = inicontents;
	while (section) {
		tou_llist_destroy(section->dat2); // Destroy each props sublist
		TOU_PRINTD("Destroying section: %s\n", section->dat1);
		section = section->prev;
	}
	tou_llist_destroy(inicontents); // Destroy whole structure
	TOU_PRINTD("Destroyed INI structure.\n");
}


/*  */
void tou_ini_print(tou_llist* inicontents)
{
	if (inicontents == NULL) {
		TOU_PRINTD("ini_print received empty struct\n");
		return;
	}

	tou_llist* section = inicontents;
	tou_llist* props;
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
		printf(" %c\n", next_section_ch);
		section = section->prev;
	}
}


/*  */
tou_llist** tou_ini_get_section(tou_llist** inicontents, const char* section_name)
{
	if (inicontents == NULL || *inicontents == NULL || section_name == NULL) {
		TOU_PRINTD("ini_get_section received empty params\n");
		return NULL;
	}

	tou_llist** sect = tou_llist_find_key(inicontents, (void*)section_name);
	if (sect == NULL) {
		TOU_PRINTD("ini_get_section unable to find given section\n");
		return NULL;
	}

	return sect;
}


/*  */
tou_llist** tou_ini_get_property(tou_llist** inicontents, const char* section_name, const char* key)
{
	if (inicontents == NULL || *inicontents == NULL || section_name == NULL || key == NULL) {
		TOU_PRINTD("ini_get_property received empty params\n");
		return NULL;
	}

	tou_llist** sect = tou_llist_find_key(inicontents, (void*) section_name);
	if (sect == NULL) {
		TOU_PRINTD("ini_get_property unable to find given section\n");
		return NULL;
	}
	// TOU_PRINTD("TOU_INI_GET_PROPERTY found SECTION %s\n", (*sect)->dat1);

	tou_llist** prop = tou_llist_find_key( (tou_llist**)(&((*sect)->dat2)), (void*)key );
	if (prop == NULL) {
		TOU_PRINTD("ini_get_property unable to find given key\n");
	}

	return prop;
}


/*  */
char* tou_ini_get(tou_llist** inicontents, const char* section_name, const char* key)
{
	tou_llist** prop = tou_ini_get_property(inicontents, section_name, key);
	TOU_PRINTD("ini_get  prop: %p\n", prop);

	if (prop)
		return (*prop)->dat2;
	return NULL;
}


/*  */
char* tou_ini_set(tou_llist** inicontents, const char* section_name, const char* key, char* new_value)
{
	if (inicontents == NULL || *inicontents == NULL || section_name == NULL || key == NULL || new_value == NULL) {
		TOU_PRINTD("[ini_set] received empty params\n");
		return NULL;
	}

	tou_llist** sect = tou_llist_find_key(inicontents, (void*) section_name);
	if (sect == NULL) {
		TOU_PRINTD("[ini_set] section not found, allocating new...\n");
		// TODO: tou_ini_new_section ?
		sect = tou_llist_append(inicontents, tou_strdup(section_name), NULL, 1, 0);
	}
	
	tou_llist** prop = tou_llist_find_key( (tou_llist**)(&((*sect)->dat2)), (void*) key );

	if (prop == NULL) {
		// Allocate new property...
		TOU_PRINTD("[ini_set] property not found\n");
		prop = tou_llist_append( (tou_llist**)(&((*sect)->dat2)),
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
int tou_ini_save_fp(tou_llist* inicontents, FILE* fp)
{
	if (inicontents == NULL || fp == NULL) {
		TOU_PRINTD("[ini_save_fp] received empty params\n");
		return -1;
	}
	
	tou_llist* section = inicontents;
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
		
		tou_llist* prop = section->dat2;
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
int tou_ini_save_fp_json(tou_llist* inicontents, FILE* fp)
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

	tou_llist* section = inicontents;
	// size_t nsections = 0;
	while (section) {
		if (section->dat1 == NULL) {
			TOU_PRINTD("[ini_save_fp_json] Invalid section name encountered\n");
			return -3;
		}
		// Print section name
		fprintf(fp, "\n\t\t\"%s\": {", section->dat1);
		
		tou_llist* prop = section->dat2;
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


/* Loading icon
printf("⣿ ⣾⣽⣻⢿⡿⣟⣯⣷\n");
printf("⠿ ⠾⠽⠻⠟⠯⠷\n"); */


#endif // TOU_IMPLEMENTATION_DONE
#endif // TOU_IMPLEMENTATION

#endif // __TOU_H_