#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOU_IMPLEMENTATION
#define TOU_DBG 1
// #define TOU_LLIST_SINGLE_ELEM
#include "tou.h"

/* void* cb(void* ptr, void* _userdata)
{
	tou_llist* elem = (tou_llist*) ptr;
	return (void*)(size_t) strcmp(elem->dat1, "test07");
} */

/* void* cb_ini(void* ptr, void* userdata)
{
	tou_llist* elem = (tou_llist*) ptr;
	printf("CB_INI got: %s, %s\n", elem->dat1, elem->dat2);
	return (void*)(size_t) strcmp(elem->dat1, userdata);
}

void* cb_ini2(void* ptr, void* userdata)
{
	tou_llist* elem = (tou_llist*) ptr;
	printf("CB_INI2 got: %s, %s\n", elem->dat1, elem->dat2);
	return (void*)(size_t) strcmp(elem->dat1, userdata);
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
	tou_func fptr = cb;
	fptr(NULL);
*/

	// Linked list test //
/*
	printf("Sizeof tou_llist: %d\n", sizeof(tou_llist));

	tou_llist* head = tou_llist_new(); //= NULL;
	tou_llist** elem;
	// Append
	for (int i = 0; i < 12; i++) {
		char* txt = malloc(16+1);
		sprintf(txt, "test%02d", i);

		#ifdef TOU_LLIST_SINGLE_ELEM
			elem = tou_llist_append(&head, txt, 1);
		#else
			elem = tou_llist_append(&head, txt, (void*)(size_t) i, 1,0);
		#endif
	}

	// Get tail
	tou_llist* firstelem = tou_llist_get_tail(*elem);
	while (firstelem) {

		#ifdef TOU_LLIST_SINGLE_ELEM
			printf("Elem: val=%s\n", (char*) firstelem->dat1);
		#else
			printf("Elem: key=%s, val=%d\n", (char*) firstelem->dat1, (int)(size_t) firstelem->dat2);
		#endif

		firstelem = firstelem->next;
	}

	printf("\n");

	// Get head
	tou_llist* lastelem = tou_llist_get_head(head);
	while (lastelem) {

		#ifdef TOU_LLIST_SINGLE_ELEM
			printf("Elem: val=%s\n", (char*) lastelem->dat1);
		#else
			printf("Elem: key=%s, val=%d\n", (char*) lastelem->dat1, (int)(size_t) lastelem->dat2);
		#endif

		if (! lastelem->prev) break;
		lastelem = lastelem->prev;
	}

	printf("\nFind test:\n");

	// Find using callback function
	elem = tou_llist_find_func(&head, cb,0);
	#ifdef TOU_LLIST_SINGLE_ELEM
		printf("Found: %s\n", (char*) (*elem)->dat1);
	#else
		printf("Found: %s, %d\n", (char*) (*elem)->dat1, (int)(size_t) (*elem)->dat2);
	#endif

	// Pop it
	tou_llist* poppedelem = tou_llist_pop(*elem);

	// Get tail
	firstelem = tou_llist_get_tail(head);//*elem);
	while (firstelem) {

		#ifdef TOU_LLIST_SINGLE_ELEM
			printf("Elem: val=%s\n", (char*) firstelem->dat1);
		#else
			printf("Elem: key=%s, val=%d\n", (char*) firstelem->dat1, (int)(size_t) firstelem->dat2);
		#endif

		firstelem = firstelem->next;
	}

	// Free manually because the elem was only pop()'d, not remove()'d
	tou_llist_free_element(poppedelem);

	printf("\n");

	// Find using dat1 as string
	elem = tou_llist_find_key(&head, "test05");
	printf("FIND KEY\n");
	#ifdef TOU_LLIST_SINGLE_ELEM
		printf("Found: %s\n", (char*) (*elem)->dat1);
	#else
		printf("Found: %s, %d\n", (char*) (*elem)->dat1, (int)(size_t) (*elem)->dat2);
	#endif

	firstelem = tou_llist_get_tail(*elem);
	
	// Remove the other element, this function also deallocates it
	tou_llist_remove(*elem);

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
//	#pragma GCC diagnostic ignored "-Wint-conversion"
	elem = tou_llist_find_exact(&head, (void*)-1, (void*)2);
//	#pragma GCC diagnostic warning "-Wint-conversion"

	#ifdef TOU_LLIST_SINGLE_ELEM
		printf("Found: %s\n", (char*) (*elem)->dat1);
	#else
		printf("Found: %s, %d\n", (char*) (*elem)->dat1, (int)(size_t) (*elem)->dat2);
	#endif

	firstelem = tou_llist_get_tail(*elem);
	tou_llist_remove(*elem);

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
/*
	// char* inicontents = tou_read_file_in_blocks("testini.ini", NULL);
	// printf("File contents:\n|%s|\n", inicontents);
	// void* parsed = tou_ini_parse(inicontents);

	// free(inicontents);

	FILE* fp = fopen("testini.ini", "r");
	tou_llist* inicontents = tou_ini_parse_fp(fp);
	fclose(fp);
	printf("Parsed ptr: %p\n", inicontents);
	printf("\n");

	printf("Printing before:\n");
	tou_ini_print(inicontents);
	printf("\n\n\n");

	// strcpy(
	// 	tou_llist_find_key(tou_llist_find_key(&inicontents, "first")->dat2, "surname")->dat2,
	// 	"Snow");

	// strcpy(
	// 	(*tou_ini_get_property(&inicontents, "first", "name"))->dat2, "Ante" );

	// strcpy(
	// 	tou_ini_get(&inicontents, "first", "name"), "Anto" );

	// tou_ini_print(inicontents);

	tou_ini_set(&inicontents, "first", "surname", "Gledic");
	tou_ini_print(inicontents);
	tou_ini_set(&inicontents, "first", "surname", "Bilus");
	tou_ini_print(inicontents);
	tou_ini_set(&inicontents, "first", "rizz", "12");
	tou_ini_print(inicontents);
	tou_ini_set(&inicontents, "first", "rizz", "255");
	tou_ini_print(inicontents);

	printf("Ini get first,surname: %s\n", tou_ini_get(&inicontents, "first", "surname"));

	tou_llist* prop = *(tou_ini_get_property(&inicontents, "first", "surname"));
	printf("Ini get property first,surname: %s, %s\n\n", prop->dat1, prop->dat2);
	
	printf("Ini set first,name,Giuseppe: %s\n", tou_ini_set(&inicontents, "first", "name", "Giuseppe"));

	printf("Reading prop after setting: %s, %s\n", prop->dat1, prop->dat2);

	printf("\n\n\n");
	printf("Printing whole inistruct after:\n");
	tou_ini_print(inicontents);

	// Test _llist_find_func()
	printf("Finding using llist_find_func cb_ini,'first': \n");
	tou_llist** ret = tou_llist_find_func(&inicontents, cb_ini,"first");
	if (ret == NULL) {
		printf(" @ ret = null  RAAAAGHHHH no section 'first' found\n");
	} else {
		printf(" $ Found: %s, %s\n", (*ret)->dat1, (*ret)->dat2);
		printf("Finding using llist_find_func cb_ini2,'name': \n");
		tou_llist** ret2 = tou_llist_find_func(TOU_LLIST_DAT_ADDR(ret, dat2), cb_ini2,"name");
		if (ret2 == NULL) {
			printf(" @ ret2 = null  RAAAAAGGHHHH no property 'name' found\n");
		} else {
			printf(" $ Found: %s, %s\n", (*ret2)->dat1, (*ret2)->dat2);
		}
	}

	// Obliterate 
	tou_ini_destroy(inicontents);
*/

	printf("\nDone.\n");
	return 0;
}