/*

Required always but only once! :
	#define TOU_IMPLEMENTATION

Other defines:
	#define TOU_LLIST_SINGLE_ELEM

*/
#ifndef __TOU_H_
#define __TOU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOU_STR(s) #s
#define TOU_MSTR(s) TOU_STR(s)

#define TOU_IS_BLANK(c) ((c)==' ' || (c)=='\n' || (c)=='\r' || (c)=='\t')

#define TOU_DEFAULT_BLOCKSIZE 4096

#ifndef TOU_DBG
#define TOU_DBG 0 // set default value
#endif
#ifndef TOU_DEBUG
#define TOU_DEBUG(x) if (TOU_DBG) { x; } else (void)0
#endif
#ifndef _TOU_DEBUG_PREFIX
#define _TOU_DEBUG_PREFIX "[:] "
#endif
#ifndef TOU_PRINTD
#define TOU_PRINTD(format, ...) if (TOU_DBG) { printf(_TOU_DEBUG_PREFIX format, __VA_ARGS__); } else (void)0
#endif

/* Func ptrs */
typedef void* (*tou_funcptr)(void*);


// ==============================================================
// ||                        FUNC DECLS                        ||
// ==============================================================


/* Linked list */
#ifndef TOU_LLIST_SINGLE_ELEM
	typedef struct tou_llist {
		struct tou_llist* prev;
		struct tou_llist* next;
		void* dat1;
		void* dat2;
		char destroy_dat1 : 1;
		char destroy_dat2 : 1;
	} tou_llist;

	tou_llist* tou_llist_append(tou_llist** list, void* dat1, void* dat2, char dat1_is_dynalloc, char dat2_is_dynalloc);
	tou_llist* tou_llist_find_exact(tou_llist* list, void* dat1, void* dat2);
#else
	typedef struct tou_llist {
		struct tou_llist* prev;
		struct tou_llist* next;
		void* dat1;
		char destroy_dat1 : 1;
	} tou_llist;

	tou_llist* tou_llist_append(tou_llist** list, void* dat1, char dat1_is_dynalloc);
	tou_llist* tou_llist_find_exact(tou_llist* list, void* dat1);
#endif

void tou_llist_remove(tou_llist* elem);
tou_llist* tou_llist_pop(tou_llist* elem);
void tou_llist_free_element(tou_llist* elem);
tou_llist* tou_llist_get_head(tou_llist* list);
tou_llist* tou_llist_get_tail(tou_llist* list);
void tou_llist_iter(tou_llist* list, tou_funcptr cb);
tou_llist* tou_llist_find_key
(
	tou_llist* list,
	void* dat1
// #ifndef TOU_LLIST_SINGLE_ELEM
// 	, void* dat2
// #endif
);
tou_llist* tou_llist_find_func
(
	tou_llist* list,
	tou_funcptr cb
);

void tou_llist_destroy(tou_llist* list);


char* tou_find_string_start(char* ptr, char* kwd);

char* tou_trim_string(char** str);
char* tou_trim_front(char** str);
void tou_trim_back(char** str);

void tou_trim_string_pure(char* str, char** start, char** end);
char* tou_trim_front_pure(char* str);
char* tou_trim_back_pure(char* str);

char* tou_read_fp_in_blocks(FILE* fp, size_t* read_len, size_t blocksize);
char* tou_read_file_in_blocks(const char* filename, size_t* read_len);

size_t tou_strlcpy(char* dst, const char* src, size_t size);
size_t tou_strlcat(char* dst, const char* src, size_t size);
tou_llist* tou_split(char* str, char* delim);


// ==============================================================
// ||                         FUNCTIONS                        ||
// ==============================================================


#ifdef TOU_IMPLEMENTATION
#if defined(TOU_IMPLEMENTATION_DONE)
#pragma error "TOU_IMPLEMENTATION already defined somewhere!"
#else

#define TOU_IMPLEMENTATION_DONE

/*
	Find start function
	has two pointers corresponding to current pos in source string
	and current pos in keyword string that was matched up current char
	
	@param[in] ptr Source string
	@param[in] kwd Which keyword to search for
	@return Pointer to the beginning of the keyword in the text or NULL
*/
char* tou_find_string_start(char* ptr, char* kwd)
{
	TOU_DEBUG( printf(_TOU_DEBUG_PREFIX"RECEIVED PTR :: %s\n", ptr) );
	// dont worry its quite stupid
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


/*
	Trims whitespaces from front and back of the string
	!! This is a destructive operation since trim_back is destructive. !!

	@param[in] str Pointer to the string to be trimmed
	@return Pointer to the new start in string
*/
char* tou_trim_string(char** str)
{
	/*char* ptr = (*str + strlen(*str) - 1);
	while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r') {
		*ptr = '\0';
		ptr--;
	}

	ptr = *str;
	while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r')
		ptr++;

	*str = ptr;*/

	tou_trim_back(str);
	return tou_trim_front(str);
}


/*
	Trims whitespaces only from the front of the string

	@param[in] str Pointer to the string to be trimmed
	@return Pointer to the new start in string
*/
char* tou_trim_front(char** str)
{
	if (str == NULL || *str == NULL)
		return NULL;

	char* ptr = *str;
	// while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r')
	while (TOU_IS_BLANK(*ptr))
		ptr++;
	*str = ptr;
	return ptr;
}


/*
	Trims whitespaces only from the back of the string
	!! This is a destructive operation !!

	@param[in] str Pointer to the string to be trimmed
*/
void tou_trim_back(char** str)
{
	if (str == NULL || *str == NULL)
		return;

	char* ptr = (*str + strlen(*str) - 1);
	// while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r') {
	while (TOU_IS_BLANK(*ptr)) {
		*ptr = '\0';
		ptr--;
	}
}


/*
	Returns pointers to the first and last character that
	are not whitespaces

	@param[in] str Pointer to the string to be trimmed
	@param[out] start Pointer to the start of contents
	@param[out] end Pointer to the byte after the contents (or NUL)
*/
void tou_trim_string_pure(char* str, char** start, char** end)
{
	/*char* ptr = (*str + strlen(*str) - 1);
	while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r') {
		*ptr = '\0';
		ptr--;
	}

	ptr = *str;
	while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r')
		ptr++;

	*str = ptr;*/

	char* back = tou_trim_back_pure(str);
	char* front = tou_trim_front_pure(str);

	*end = back;
	*start = front;
}


/*
	Returns pointer to the first character from
	the front that isn't a whitespace

	@param[in] str Pointer to the string to be trimmed
	@return Pointer to first non-whitespace char
*/
char* tou_trim_front_pure(char* str)
{
	if (str == NULL)
		return NULL;

	while (*str != '\0' && TOU_IS_BLANK(*str))
		str++;
	return str;
}


/*
	Returns pointer to the first byte AFTER the
	contents of the string, or the NUL byte.

	@param[in] str Pointer to the string to be trimmed
	@return Pointer to the first trimmed byte (or NUL)
*/
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


/*
	Reads file in blocks
	Automatically allocates memory and
	optionally returns amount read (may be set to null)

	@param[in] filename Either file name or ""/"stdin" to read from stdin
	@param[out] read_len Optional pointer to where to store file size
	@param[in] blocksize Size in bytes; set to 0 to use default (TOU_DEFAULT_BLOCKSIZE)
	@return Pointer to loaded file or NULL if error
*/
char* tou_read_fp_in_blocks(FILE* fp, size_t* read_len, size_t blocksize)
{
	// Clamp blocksize to TOU_DEFAULT_BLOCKSIZE if not in range [1, 0xFFFFFF>
	if (blocksize < 1 || blocksize > 0xFFFFFF) {
		blocksize = TOU_DEFAULT_BLOCKSIZE;
		TOU_DEBUG( printf(_TOU_DEBUG_PREFIX "clamping blocksize to " TOU_MSTR(TOU_DEFAULT_BLOCKSIZE)"\n") );
	}

	size_t curr_bufsize = blocksize;
	char blockbuf[blocksize];
	char* filebuf = NULL;
	if ((filebuf = malloc(curr_bufsize)) == NULL) {
		TOU_DEBUG( printf(_TOU_DEBUG_PREFIX "error allocating initial %zu bytes\n", curr_bufsize) );
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
		if (newbuf) {
			filebuf = newbuf;
		} else {
			TOU_DEBUG( printf(_TOU_DEBUG_PREFIX "error allocating %d bytes\n", curr_bufsize) );
			if (fp != stdin) fclose(fp);
			return NULL;
		}
		strcat(filebuf, blockbuf);
	}

	if (fp != stdin)
		fclose(fp);

	if (read_len)
		*read_len = rlen;
	return filebuf;
}


/*
	Reads file in blocks
	Automatically allocates memory and
	optionally returns amount read (may be set to null)

	@param[in] filename Either file name or ""/"stdin" to read from stdin
	@param[out] read_len Optional pointer to where to store file size
	@return Pointer to loaded file or NULL if error
*/
char* tou_read_file_in_blocks(const char* filename, size_t* read_len)
{
	FILE* fp;
	if (!filename || strlen(filename) == 0 || strcmp("stdin", filename) == 0) { // stdin
		fp = stdin;
	} else if ((fp = fopen(filename, "r")) == NULL) { // file
		TOU_DEBUG( printf(_TOU_DEBUG_PREFIX "cannot open '%s'\n", filename) );
		return NULL;
	}

	TOU_DEBUG( printf(_TOU_DEBUG_PREFIX "reading from: %s\n", (fp==stdin) ? "STDIN" : filename) );
	char* read = tou_read_fp_in_blocks(fp, read_len, -1); // default block size

	if (fp != stdin)
		fclose(fp);
	return read;
}


#ifndef TOU_LLIST_SINGLE_ELEM
	/*
		Appends a brand new element to the llist and updates the pointer in place
		Also returns pointer to the newly created element

		@param[in] list Address of first element (tou_llist**)
		@param[in] dat1 Data 1
		@param[in] dat2 Data 2
		@param[in] dat1_is_dynalloc Should data1 be free()'d when destroying list?
		@param[in] dat2_is_dynalloc Should data2 be free()'d when destroying list?
		@return tou_llist* Pointer to the newly created element
	*/
	tou_llist* tou_llist_append(tou_llist** list, void* dat1, void* dat2, char dat1_is_dynalloc, char dat2_is_dynalloc)
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
		return new_head;
	}
#else
	/*
		Appends a brand new element to the llist and updates the pointer in place
		Also returns pointer to the newly created element

		@param[in] list Address of first element (tou_llist**)
		@param[in] dat1 Data 1
		@param[in] dat1_is_dynalloc Should data1 be free()'d when destroying list?
		@return tou_llist* Pointer to the newly created element
	*/
	tou_llist* tou_llist_append(tou_llist** list, void* dat1, char dat1_is_dynalloc)
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
		return new_head;
	}
#endif


/*
	Removes an element from llist and deallocates it

	@param[in] elem Pointer to the element to be removed
*/
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


/*
	Removes an element from llist but does not deallocate it
	
	@param[in] elem Pointer to the element to be removed
	@return tou_llist* Pointer to the element popped from llist
*/
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


/*
	Frees a standalone "element". Do NOT use on an element
	still associated with list; only on a pop()'d elem for example

	@param[in] elem Pointer to the element to be freed
*/
void tou_llist_free_element(tou_llist* elem)
{
	if (!elem) return;

	if (elem->destroy_dat1) free(elem->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
	if (elem->destroy_dat2) free(elem->dat2);
#endif
	free(elem);
}


/*
	Traverses the whole list through ->next and returns the last element
	that was added into list (where ->next is NULL)

	@param[in] list Element of type tou_llist*
	@return tou_llist* Pointer to the head element
*/
tou_llist* tou_llist_get_head(tou_llist* list)
{
	if (!list) return NULL;
	while (list->next)
		list = list->next;
	return list;
}


/*
	Traverses the whole list through ->prev and returns the first element
	that was added to list (where ->prev is NULL)

	@param[in] list Element of type tou_llist*
	@return tou_llist* Pointer to the tail element
*/
tou_llist* tou_llist_get_tail(tou_llist* list)
{
	if (!list) return NULL;
	while (list->prev)
		list = list->prev;
	return list;
}


/*
	Iterate through and call the specified function for each element
	Automatically checks whether given element is head or tail and 
	iterates accordingly.
	If given function returns a value different from 0 the iteration
	terminates early.

	@param[in] list Head or tail of the list
	@param[in] cb Function to be called for each element
*/
void tou_llist_iter(tou_llist* list, tou_funcptr cb)
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
}


/*
	Iterate through and call the specified function for each element
	Automatically checks whether given element is head or tail and 
	iterates accordingly.
	If given function returns a value different from 0 the iteration
	terminates early.

	[[WIP]] Manipulate elements from function during iteration ?

	@param[in] list ADDRESS of the Head or tail of the list
	@param[in] cb Function to be called for each element
*/
/* void tou_llist_iterex(tou_llist** list, tou_funcptr cb)
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


/*
	Traverses the list starting from the passed element and tries
	to find the first matching element by simple equals-comparison
	using both parameters at the same time.

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@param[in] dat2 Comparison value (if enabled)
	@return tou_llist* Found element or NULL
*/
#ifndef TOU_LLIST_SINGLE_ELEM
	tou_llist* tou_llist_find_exact(tou_llist* list, void* dat1, void* dat2)
	{
		if (!list)
			return NULL;

		while (list) {
			if (list->dat1 == dat1) return list;
			if (list->dat2 == dat2) return list;
			list = list->prev;
		}

		return NULL;
	}
#else
	tou_llist* tou_llist_find_exact(tou_llist* list, void* dat1)
	{
		if (!list)
			return NULL;

		while (list) {
			if (list->dat1 == dat1) return list;
			list = list->prev;
		}

		return NULL;
	}
#endif


/*
	Traverses the list starting from the passed element and tries
	to find the first matching element with the following conditions:
		- if dat1 is not null, tries to match it as a nul-terminated string
		- if dat1 is null, tries to match dat2 exactly (if enabled)

	@param[in] list Element to start the search from
	@param[in] dat1 Comparison value
	@param[in] dat2 Comparison value (if enabled)
	@return tou_llist* Found element or NULL
*/
tou_llist* tou_llist_find_key
(
	tou_llist* list,
	void* dat1
// #ifndef TOU_LLIST_SINGLE_ELEM
// 	, void* dat2
// #endif
){
	if (!list)
		return NULL;

	#ifndef TOU_LLIST_SINGLE_ELEM
	#else
		if (!dat1) return NULL;
	#endif

	while (list) {
		if (strcmp(list->dat1, dat1) == 0) return list;
// #ifndef TOU_LLIST_SINGLE_ELEM
// 		if (list->dat2 == dat2) return list;
// #endif
		list = list->prev;
	}

	return NULL;
}


/*
	Traverses the list starting from the passed element and tries
	to find the first matching element by checking the return value
	of the given callback comparison function. If value equals 0
	element is considered matched, otherwise the search continues.

	@param[in] list Element to start the search from
	@param[in] cb Custom comparison function
	@return tou_llist* Found element or NULL
*/
tou_llist* tou_llist_find_func
(
	tou_llist* list,
	tou_funcptr cb
){
	if (!list || !cb)
		return NULL;

	while (list) {
		if ((size_t) cb(list) == 0)
			return list;
		list = list->prev;
	}

	return NULL;
}


/*
	Traverses elements using data->prev and frees each one
	including copied category string

	@param[in] data Element of type tou_llist*; accepts either head or tail
*/
void tou_llist_destroy(tou_llist* list)
{
	if (!list)
		return;

	// We have at least one element

	if (list->prev != NULL) {

		tou_llist *prev, *curr = list; // copy so we can later destroy it separately
		while (curr != NULL) {
			prev = curr->prev;

			if (curr->destroy_dat1) free(curr->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
			if (curr->destroy_dat2) free(curr->dat2);
#endif
			free(curr);
			curr = prev;
		}

	} else if (list->next != NULL) {

		tou_llist *next, *curr = list;
		while (curr != NULL) {
			next = curr->next;

			if (curr->destroy_dat1) free(curr->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
			if (curr->destroy_dat2) free(curr->dat2);
#endif
			free(curr);
			curr = next;
		}
	}

	// then this is not needed since its also destroyed in loop?
	// guess i may have been delulu all this time?
	/*
	// destroy passed element separately
	if (list == NULL)
		return;
	if (list->destroy_dat1) free(list->dat1);
#ifndef TOU_LLIST_SINGLE_ELEM
	if (list->destroy_dat2) free(list->dat2);
#endif
	free(list);
	*/
}


/* Copies string SRC to DST.  If SRC is longer than SIZE - 1
	characters, only SIZE - 1 characters are copied.  A null
	terminator is always written to DST, unless SIZE is 0.
	Returns the length of SRC, not including the null terminator.
	
	strlcpy() is not in the standard C library, but it is an
	increasingly popular extension.  See
	http://www.courtesan.com/todd/papers/strlcpy.html for
	information on strlcpy(). */
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


/* Concatenates string SRC to DST.  The concatenated string is
   limited to SIZE - 1 characters.  A null terminator is always
   written to DST, unless SIZE is 0.  Returns the length that the
   concatenated string would have assuming that there was
   sufficient space, not including a null terminator.

   strlcat() is not in the standard C library, but it is an
   increasingly popular extension.  See
   http://www.courtesan.com/todd/papers/strlcpy.html for
   information on strlcpy(). */
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


/*
*/
//strndup(pos_start, pos_delim-pos_start)


/*
	Splits string using a delimiter that may be more than one character

	@param[in] str String to be split
	@param[in] delim Delimiter string
	@return Linked list containing tokens
*/
tou_llist* tou_split(char* str, char* delim)
{
	if (!str || !delim)
		return NULL;

	TOU_PRINTD("STR_LEN :: %d\n", strlen(str));

	size_t delim_len = strlen(delim);
	tou_llist* list = NULL;
	char* pos_start = str;
	char* pos_delim = tou_find_string_start(str, delim);
	
	while (pos_delim) {
		// bruh i dont got my strndup..
		//tou_llist_append(&list, strndup(pos_start, pos_delim-pos_start), NULL, 1, 0);

		char* buf = malloc(pos_delim-pos_start + 1);
		// strncpy(buf, pos_start, pos_delim-pos_start);
		tou_strlcpy(buf, pos_start, pos_delim-pos_start + 1);
		TOU_PRINTD("BUF: %s\n", buf);
		tou_llist_append(&list, buf, NULL, 1, 0);

		// Find next occurence
		pos_start = pos_delim + delim_len;
		pos_delim = tou_find_string_start(pos_start/*pos_delim + delim_len*/, delim);
	}

	// TOU_PRINTD("pos_start: |%p|\n", pos_start);
	// TOU_PRINTD("pos_delim: |%p|\n", pos_delim);
	// TOU_PRINTD("str: |%p|\n", str);
	// TOU_PRINTD("str+strlen: |%p|\n", str+strlen(str));
	// TOU_PRINTD("str+strlen-2 == 0: %d |%c|\n", (*(str+strlen(str)-2) == 0), *(str+strlen(str)-2) );
	// TOU_PRINTD("str+strlen-1 == 0: %d |%c|\n", (*(str+strlen(str)-1) == 0), *(str+strlen(str)-1) );
	// TOU_PRINTD("str+strlen == 0: %d |%c|\n", (*(str+strlen(str)) == 0), *(str+strlen(str)) );
	// TOU_PRINTD("str+strlen+1 == 0: %d |%c|\n", (*(str+strlen(str)+1) == 0), *(str+strlen(str)+1) );
	// TOU_PRINTD("str+strlen+2 == 0: %d |%c|\n", (*(str+strlen(str)+2) == 0), *(str+strlen(str)+2) );
	// TOU_PRINTD("(str+strlen)-pos_start: |%d|\n", (str+strlen(str))-pos_start);
	// TOU_PRINTD("\n",0);

	size_t len = strlen(pos_start);
	TOU_PRINTD("FINAL LEN :: %d\n", len);
	if (len > 0) {
		// Append last part till the end
		char* buf = malloc(len + 1);
		tou_strlcpy(buf, pos_start, len + 1);
		TOU_PRINTD("BUF: %s\n", buf);
		tou_llist_append(&list, buf, NULL, 1, 0);
	}

	return tou_llist_get_tail(list);
}


/*
	Parses given fp as .INI file and constructs structured data.

	Parser supports using both semicolon (;) and hashtag (#) for comments,
	however they must be on a separate line by themselves.
	Specifying the same section name more than once is not supported (that
	may change in the future where the definitions would merge and "flatten"
	into one combined section).
	When specifying properties both keys and values may have whitespaces in
	them but not at the start and at the end (of course that also means that
	such keys will be sensitive to those whitespaces when using the data).
	Whitespaces may be added between sections, properties and section name
	brackets for better readability.

	@param[in] fp File pointer to where to read the .INI data from
	@return Pointer to the allocated structured data
*/
void* tou_ini_parse_fp(FILE* fp)
{
	/* size_t bufsize = 256+1;
	char* linebuf = malloc(bufsize);
	while (getline(&linebuf, &bufsize, fp) > -1) {
	free(linebuf);
	*/

	tou_llist* inistruct = NULL;

	char linebuf[256+1];
	char* line = linebuf;
	while (fgets(line, 256+1, fp)) {
		printf("Line read: (%d) |%s|\n", strlen(line), line);

		tou_trim_front(&line);

		/*int len = strlen(line);
		if (len < 1) // Empty line
			continue;*/
		if (! *line)
			continue;

		if (*line == ';' || *line == '#') {
			// Comment line
			continue;
		}

		if (*line == '[') {
			// Section indication
			tou_trim_back(&line);
			line++;
			line[strlen(line) - 1] = '\0';

			//tou_llist_append(&inistruct, );
		}
		// else check property otherwise invalid

		tou_trim_back(&line);

		// int seplen;
		// char* sep;
		// sep = tou_find_string_start(line, " = ");
		// seplen = 3;
		// if (!sep) {
		// 	sep = tou_find_string_start(line, "=");
		// 	seplen = 1;
		// 	if (!sep) {
		// 		// invalid line
		// 		printf("invalid (%s)\n", line);
		// 		return 0;
		// 	}
		// }
		// *sep = '\0';
		// strcpy(key, bufptr);
		// strcpy(val, sep+seplen);
	}

	return NULL;
}


/*
	Parses given buffer as .INI file and returns structured data

	@param[in] contents Buffer containing raw .INI data
	@return Pointer to the allocated structured data
*/
void* tou_ini_parse(char* contents)
{
	// Raise notimplemented
	return NULL;
}


#endif // TOU_IMPLEMENTATION_DONE
#endif // TOU_IMPLEMENTATION

#endif // __TOU_H_