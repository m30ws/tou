#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOU_IMPLEMENTATION
#define TOU_DBG 1
/* #define TOU_LLIST_SINGLE_ELEM */
#include "tou.h"

/* void* cb(void* ptr, void* _userdata)
{
	tou_llist_t* elem = (tou_llist_t*) ptr;
	return (void*)(size_t) strcmp(elem->dat1, "test07");
} */

void* cb_ini(void* ptr, void* userdata)
{
	tou_llist_t* elem = (tou_llist_t*) ptr;
	printf("CB_INI got: %s, %s\n", elem->dat1, elem->dat2);
	return (void*)(size_t) strcmp(elem->dat1, userdata);
}

void* cb_ini2(void* ptr, void* userdata)
{
	tou_llist_t* elem = (tou_llist_t*) ptr;
	printf("CB_INI2 got: %s, %s\n", elem->dat1, elem->dat2);
	return (void*)(size_t) strcmp(elem->dat1, userdata);
}

/* void* cb_tok(void* ptr)
{
	printf("Token <%s>\n", ((tou_llist_t*)ptr)->dat1);
	return (void*) 1;
} */

/* void* cb_fileread(void* blockdata, void* len, void* userdata)
{
	char* block = (char*) blockdata;
	size_t size = (size_t) len;
	char** filedata = (char**) userdata;

	size_t currsize = strlen(*filedata);

	if (size > 0) {
		char* new_buffer = realloc(*filedata, currsize + size);

		if (!new_buffer){ 
			TOU_PRINTD("cannot realloc memory\n");
			return (void*) TOU_BREAK;
		}

		strcpy(new_buffer + currsize, block);
		*filedata = new_buffer;
	}
	
	return (void*) TOU_CONTINUE;
} */


int main(int argc, char const* argv[])
{
	// File read test //
/*
	size_t siz;
	FILE* fptr;
	char* contents;

	// 1. Read file simply using filename
	contents = tou_read_file("testfile.txt", &siz);
	printf("tou_read_file contents:\n|%s|\nSize: %zu, %d\n", contents, siz, strlen(contents));
	free(contents); contents = NULL;
	
	// 2. Read FILE* using custom function
	contents = strdup(""); // malloc with just '\0' since we're using strlen in the function
	
	fptr = fopen("testfile.txt", "r");
	siz = tou_read_fp_in_blocks(fptr, -1, cb_fileread, &contents);
	fclose(fptr); fptr = NULL;
	
	printf("\ntou_read_fp_in_blocks contents (%zu):\n%s", siz, contents);
	
	free(contents); contents = NULL;

	// 3. Use tou-defined func+struct for reading FILE* (this is how tou_read_file() is implemented)
	tou_block_store_struct my_data_buf = {NULL, 0};
	
	fptr = fopen("testfile.txt", "r");
	tou_read_fp_in_blocks(fptr, -1, tou_block_store_cb, &my_data_buf); // default block size, without function
	fclose(fptr); fptr = NULL;
	
	printf("I have read (%zu):\n%s\n", my_data_buf.size, my_data_buf.buffer);
	
	free(my_data_buf.buffer); my_data_buf.buffer = NULL;

	// 4. Retrieve only file size
	fptr = fopen("testfile.txt", "r");
	printf("File size: %zu\n", tou_read_fp_in_blocks(fptr, 0,0,0));
	fclose(fptr); fptr = NULL;
*/

	// Function pointer test //
/*
	tou_func fptr = cb;
	fptr(NULL);
*/

	// Linked list test //
/*
	printf("Sizeof tou_llist_t: %d\n", sizeof(tou_llist_t));

	tou_llist_t* head = tou_llist_new(); //= NULL;
	tou_llist_t** elem;
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
	tou_llist_t* firstelem = tou_llist_get_tail(*elem);
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
	tou_llist_t* lastelem = tou_llist_get_head(head);
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
	elem = tou_llist_find_func(&head, cb,NULL);
	#ifdef TOU_LLIST_SINGLE_ELEM
		printf("Found: %s\n", (char*) (*elem)->dat1);
	#else
		printf("Found: %s, %d\n", (char*) (*elem)->dat1, (int)(size_t) (*elem)->dat2);
	#endif

	// Pop it
	tou_llist_t* poppedelem = tou_llist_pop(*elem);

	// Get tail
	firstelem = tou_llist_get_tail(head);
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
	elem = tou_llist_find_exact(&head, (void*) -1, (void*) 2);
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

	tou_llist_t* splitted = tou_split(str, delim);
	tou_llist_t* iter = splitted;
	
	printf("Iterating by hand:\n");
	while (iter) {
		printf("Token <%s>\n", iter->dat1);
		iter = iter->next;
	}

	printf("Iterating using _llist_iter():\n");
	iter = splitted; // tou_llist_get_head(splitted);
	tou_llist_iter( iter, cb_tok );

	tou_llist_destroy(splitted);
*/

	// Character/string replacing test //
/*	
	char repl_ss[] = "aeiouoiea/1/2/3";
	printf("original = %s\n\n", repl_ss);
	printf("strchr(i) = %s\n", tou_strchr(repl_ss, 'i'));
	printf("strrchr(i) = %s\n", tou_strrchr(repl_ss, 'i'));
	printf("strchr(a) = %s\n", tou_strchr(repl_ss, 'a'));
	printf("strrchr(a) = %s\n", tou_strrchr(repl_ss, 'a'));
	printf("strchr(\\0) = %s\n", tou_strchr(repl_ss, '\0'));
	printf("strrchr(\\0) = %s\n", tou_strrchr(repl_ss, '\0'));
	printf("strchr(/) = %s\n", tou_strchr(repl_ss, '/'));
	printf("strrchr(/) = %s\n", tou_strrchr(repl_ss, '/'));
	printf("original after = %s\n\n", repl_ss);

	tou_replace_ch(repl_ss, 'o', 'F');
	printf("original at the end = %s\n\n", repl_ss);
*/

	// .INI test //

	// char* inicontents = tou_read_file_in_blocks("testini.ini", NULL, NULL, NULL);
	// printf("File contents:\n|%s|\n", inicontents);
	// void* parsed = tou_ini_parse(inicontents);

	// free(inicontents);

//printf("BEFORE disabled stdout ...\n");
//int oldstdout = tou_disable_stdout();

	FILE* fp = fopen("testini.ini", "r");
	tou_llist_t* inicontents = tou_ini_parse_fp(fp);
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

	tou_llist_t* prop = *(tou_ini_get_property(&inicontents, "first", "surname"));
	printf("Ini get property first,surname: %s, %s\n\n", prop->dat1, prop->dat2);
	
	printf("Ini set first,name,Giuseppe: %s\n", tou_ini_set(&inicontents, "first", "name", "Giuseppe"));

	printf("Reading prop after setting: %s, %s\n", prop->dat1, prop->dat2);

	printf("\n\n\n");
	printf("Printing whole inistruct after:\n");
	tou_ini_print(inicontents);

	// Test _llist_find_func()
	printf("Finding using llist_find_func cb_ini,'first': \n");
	tou_llist_t** ret = tou_llist_find_func(&inicontents, cb_ini,"first");
	if (ret == NULL) {
		printf(" @ ret = null  RAAAAGHHHH no section 'first' found\n");
	} else {
		printf(" $ Found: %s, %s\n", (*ret)->dat1, (*ret)->dat2);
		printf("Finding using llist_find_func cb_ini2,'name': \n");
		tou_llist_t** ret2 = tou_llist_find_func(TOU_LLIST_DAT_ADDR(ret, dat2), cb_ini2,"name");
		if (ret2 == NULL) {
			printf(" @ ret2 = null  RAAAAAGGHHHH no property 'name' found\n");
		} else {
			printf(" $ Found: %s, %s\n", (*ret2)->dat1, (*ret2)->dat2);
		}
	}
	printf("\n");

	FILE* fpout = NULL;
	int retval = 0;
	
	fpout = fopen("testini.out.ini", "w");
	retval = tou_ini_save_fp(inicontents, fpout);
	TOU_PRINTD("INI save .ini return: %d\n\n", retval);
	fclose(fpout);
	fpout = fopen("testini.out.json", "w");
	retval = tou_ini_save_fp_json(inicontents, fpout);
	TOU_PRINTD("INI save .json return: %d\n\n", retval);
	fclose(fpout);
	fpout = fopen("testini.out.xml", "w");
	retval = tou_ini_save_fp_xml(inicontents, fpout);
	TOU_PRINTD("INI save .xml return: %d\n\n", retval);
	fclose(fpout);

	// Obliterate 
	tou_ini_destroy(inicontents);

//tou_enable_stdout(oldstdout);
//printf("AFTER enabled stdout again...\n");



	// Server test (WIP) //
/*
	tou_server* serv = tou_spinup_server(servcb);
	while (1) {
		;
	}
	tou_destroy_server(serv);
*/

/*
	//extern tou_llist_t* testfunc();

	tou_llist_t* tst = NULL;//testfunc();
	tou_llist_append(&tst, tou_strdup("str1"), 0, 1,0);//11 ,1,0);
	tou_llist_append(&tst, tou_strdup("str2"), 0, 1,0);//22 ,1,0);

	size_t len;
	// void** res = tou_llist_gather_dat1(tst, &len);
	// ... or ...
	char** res = (char**)tou_llist_gather_dat1(tst, &len);
	printf("Printing gathered data .dat1:\n");
	for (size_t i = 0; i < len; i++)
		printf("  gathered[%d] = %s\n", i, res[i]);

	void** res2 = tou_llist_gather_dat2(tst, &len);
	printf("Printing gathered data .dat2:\n");
	for (size_t i = 0; i < len; i++)
		printf("  gathered[%d] = %d\n", i, res2[i]);

	free(res);
	free(res2);
	tou_llist_destroy(tst);
*/

	printf("\nDone.\n");
	return 0;
}
