#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOU_IMPLEMENTATION
#define TOU_DBG 0
// #define TOU_LLIST_SINGLE_ELEM
#include "tou.h"

/* void* cb(void* ptr) {
	tou_llist* elem = (tou_llist*) ptr;
	return (void*)(size_t) strcmp(elem->dat1, "test07");
} */

/* void* cbtok(void* ptr)
{
	printf("Token <%s>\n", ((tou_llist*)ptr)->dat1);
	return 0;
} */

int main(int argc, char const* argv[])
{
	// File read test //
/*	
	char* contents = tou_read_file_in_blocks("testfile.txt", NULL);
	printf("%s", contents);
	free(contents);
*/

	// Function pointer test //
/*
	tou_funcptr fptr = cb;
	fptr(NULL);
*/

	// Linked list test //
/*
	printf("sizeof: %d\n", sizeof(tou_llist));

	tou_llist* head = NULL;
	tou_llist* elem;
	// Append
	for (int i = 0; i < 12; i++) {
		char* txt = malloc(16+1);
		sprintf(txt, "test%02d", i);

		#ifdef TOU_LLIST_SINGLE_ELEM
			elem = tou_llist_append(&head, txt, 1);
		#else
			elem = tou_llist_append(&head, txt, (void*)(size_t) i, 1, 0);
		#endif
	}

	// Get tail
	tou_llist* firstelem = tou_llist_get_tail(elem);
	while (firstelem) {

		#ifdef TOU_LLIST_SINGLE_ELEM
			printf("elem: %s\n", (char*) firstelem->dat1);
		#else
			printf("elem: %s, %d\n", (char*) firstelem->dat1, (int)(size_t) firstelem->dat2);
		#endif

		firstelem = firstelem->next;
	}

	printf("\n");

	// Get head
	tou_llist* lastelem = tou_llist_get_head(head);
	while (lastelem) {

		#ifdef TOU_LLIST_SINGLE_ELEM
			printf("elem: %s\n", (char*) lastelem->dat1);
		#else
			printf("elem: %s, %d\n", (char*) lastelem->dat1, (int)(size_t) lastelem->dat2);
		#endif

		if (!lastelem->prev) break;
		lastelem = lastelem->prev;
	}

	printf("\nFind test:\n");

	// Find using callback function
	elem = tou_llist_find_func(head, cb);
	#ifdef TOU_LLIST_SINGLE_ELEM
		printf("Found: %s\n", (char*) elem->dat1);
	#else
		printf("Found: %s, %d\n", (char*) elem->dat1, (int)(size_t) elem->dat2);
	#endif

	// Pop it
	elem = tou_llist_pop(elem);

	// Get tail
	firstelem = tou_llist_get_tail(elem);
	while (firstelem) {

		#ifdef TOU_LLIST_SINGLE_ELEM
			printf("elem: %s\n", (char*) firstelem->dat1);
		#else
			printf("elem: %s, %d\n", (char*) firstelem->dat1, (int)(size_t) firstelem->dat2);
		#endif

		firstelem = firstelem->next;
	}

	// Free manually because elem was only popped
	tou_llist_free_element(elem);

	printf("\n");

	// Find using dat1 as string
	elem = tou_llist_find_key(head, "test05");
	#ifdef TOU_LLIST_SINGLE_ELEM
		printf("Found: %s\n", (char*) elem->dat1);
	#else
		printf("Found: %s, %d\n", (char*) elem->dat1, (int)(size_t) elem->dat2);
	#endif

	firstelem = tou_llist_get_tail(elem);
	
	// Remove the other element, this function also deallocates it
	tou_llist_remove(elem);

	while (firstelem) {

		#ifdef TOU_LLIST_SINGLE_ELEM
			printf("elem: %s\n", (char*) firstelem->dat1);
		#else
			printf("elem: %s, %d\n", (char*) firstelem->dat1, (int)(size_t) firstelem->dat2);
		#endif

		firstelem = firstelem->next;
	}

	printf("\n");

	// Find using dat1 as string
	elem = tou_llist_find_exact(head, -1, 2);
	#ifdef TOU_LLIST_SINGLE_ELEM
		printf("Found: %s\n", (char*) elem->dat1);
	#else
		printf("Found: %s, %d\n", (char*) elem->dat1, (int)(size_t) elem->dat2);
	#endif

	firstelem = tou_llist_get_tail(elem);
	tou_llist_remove(elem);

	while (firstelem) {
		#ifdef TOU_LLIST_SINGLE_ELEM
			printf("elem: %s\n", (char*) firstelem->dat1);
		#else
			printf("elem: %s, %d\n", (char*) firstelem->dat1, (int)(size_t) firstelem->dat2);
		#endif

		firstelem = firstelem->next;
	}

	// Delete whole list and all elements
	tou_llist_destroy(head);

	printf("\n");
*/

	// Server test (WIP) //
/*
	tou_server* serv = tou_spinup_server(servcb);
	while (1) {
		;
	}
	tou_destroy_server(serv);
*/

	// Trim test //
/*
	char buf[] = "  ke ey = vvaalluuee   ";
	char key[32+1], c, val[32+1];

	printf("Contents before: |%s|\n", buf);
	
	char *start, *end;
	tou_trim_string_pure(buf, &start, &end);
	printf("Char by char contents:\n|");
	while (start < end) {
		printf("%c", *start);
		start++;
	}
	printf("|\nEnd contents.\n");

	char* bufptr = buf;
	tou_trim_string(&bufptr);
	printf("Contents after: |%s|\n", bufptr); 

	char* sep;
	int seplen;
	sep = tou_find_string_start(bufptr, " = ");
	seplen = 3;
	if (!sep) {
		sep = tou_find_string_start(bufptr, "=");
		seplen = 1;
	}

	if (!sep) {
		// section declaration or invalid
		printf("invalid (%s)\n", bufptr);
		return 0;
	}
	*sep = '\0';
	strcpy(key, bufptr);
	strcpy(val, sep+seplen);
	
	printf("Key: |%s|  Val: |%s|\n", key, val);

	printf("\n");
*/

	// String split & Iter test //
/*
	char str[] = "abc = 123,def = 456;";
	char delim[] = " = ";

	tou_llist* splitted = tou_split(str, delim);
	tou_llist* iter = splitted;
	
	printf("Iterating by hand:\n");
	while (iter) {
		printf("Token <%s>\n", iter->dat1);
		iter = iter->next;
	}

	printf("Iterating using _llist_iter():\n");
	iter = splitted; // tou_llist_get_head(splitted);
	tou_llist_iter( iter, cbtok );

	tou_llist_destroy(splitted);
*/

	// .INI test //

	// char* inicontents = tou_read_file_in_blocks("testini.ini", NULL);
	// printf("File contents:\n|%s|\n", inicontents);
	// void* parsed = tou_ini_parse(inicontents);

	// free(inicontents);

	FILE* fp = fopen("testini.ini", "r");
	void* parsed = tou_ini_parse_fp(fp);
	fclose(fp);

	printf("Parsed ptr: %p", parsed);
	free(parsed);


	printf("\nDone.\n");
	return 0;
}