#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOU_IMPLEMENTATION
#define TOU_DBG 1
/* #define TOU_LLIST_SINGLE_ELEM */
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
	return (void*) 1;
} */

/* void* cbblock3(void* data, void* len, void* userdata)
{
	printf("Block\n=====\n%s\n===== (%zu)\n", (char*)data, (size_t)len);
	return (void*) TOU_CONTINUE;
} */


int main(int argc, char const* argv[])
{
	// File read test //
/*
	char* contents = tou_read_file("testfile.txt", NULL);
	printf("tou_read_file contents:\n%s", contents);
	free(contents); contents = NULL;

	FILE* fptr = fopen("testfile.txt", "r");
	contents = tou_read_fp_in_blocks(fptr, 0,0,cbblock3,0);
	fclose(fptr);
	printf("\ntou_read_fp_in_blocks contents:\n%s", contents);
	free(contents); contents = NULL;
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
	elem = tou_llist_find_func(&head, cb,NULL);
	#ifdef TOU_LLIST_SINGLE_ELEM
		printf("Found: %s\n", (char*) (*elem)->dat1);
	#else
		printf("Found: %s, %d\n", (char*) (*elem)->dat1, (int)(size_t) (*elem)->dat2);
	#endif

	// Pop it
	tou_llist* poppedelem = tou_llist_pop(*elem);

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
	// char* inicontents = tou_read_file_in_blocks("testini.ini", NULL, NULL, NULL);
	// printf("File contents:\n|%s|\n", inicontents);
	// void* parsed = tou_ini_parse(inicontents);

	// free(inicontents);

//printf("BEFORE disabled stdout ...\n");
//int oldstdout = tou_disable_stdout();

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
	printf("\n");

	FILE* fpout = fopen("testini.out.ini", "w");
	int retval = tou_ini_save_fp(inicontents, fpout);
	// FILE* fpout = fopen("testini.out.json", "w");
	// int retval = tou_ini_save_fp_json(inicontents, fpout);
	fclose(fpout);
	printf("Ini save return val: %d\n\n", retval);

	// Obliterate 
	tou_ini_destroy(inicontents);

//tou_enable_stdout(oldstdout);
//printf("AFTER enabled stdout again...\n");

*/

	// Server test (WIP) //
/*
	tou_server* serv = tou_spinup_server(servcb);
	while (1) {
		;
	}
	tou_destroy_server(serv);
*/

/*
	//extern tou_llist* testfunc();

	tou_llist* tst = NULL;//testfunc();
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


/*
	typedef struct {
		char name[31+1];
		char surname[31+1];
		int age;
		void* extra_data;
	} payload_example;
	printf("sss %d\n", sizeof(payload_example));

	typedef struct {
		int id;
		char free;
		// "3" more bytes free here inbetween
		size_t size;
	} memfrag;

	#define tou_sll_new(varname, n, sizeofone) \
		char _##varname##__ [(n)*(sizeof(tou_sll)+(sizeofone))] = {0}; \
		tou_sll* varname = (tou_sll*) _##varname##__;

	// tou_sll_new(tst, 10, sizeof(memfrag));
	// -> is actually:  tou_sll varname[(n)*((sizeofone)+sizeof(tou_sll))] = {0};
	// -> or:           tou_sll tst[10 * (sizeof(memfrag) + sizeof(tou_sll))] = {0};

	printf("sizeof(tou_sll) = %d, sizeof(memfrag) = %d\n\n", sizeof(tou_sll), sizeof(memfrag));

	#define LEN_SLL 10
	//char tst[LEN_SLL * (sizeof(tou_sll) + sizeof(memfrag))] = {0};
	tou_sll_new(tst, 10, sizeof(memfrag));
	const size_t SIZE_SLL_ELEM = sizeof(tou_sll) + sizeof(memfrag);

	#define SLL_FROM_MEM(mem, idx) ((tou_sll*)((char*)(mem) + (idx)*SIZE_SLL_ELEM + 0))
	#define FRG_FROM_MEM(mem, idx) ((memfrag*)((char*)(mem) + (idx)*SIZE_SLL_ELEM + sizeof(tou_sll)))
	#define FRG_FROM_SLL(sll)      ((memfrag*)(((char*)(sll)) + sizeof(tou_sll)))

	for (size_t i = 0; i < LEN_SLL; i++) {
		tou_sll* sll_elem = SLL_FROM_MEM(tst, i); //(tou_sll*)(tst + i*SIZE_SLL_ELEM + 0);
		memfrag* mfr_elem = FRG_FROM_MEM(tst, i); //(memfrag*)(tst + i*SIZE_SLL_ELEM + sizeof(tou_sll));

		printf("tou_sll* sll_elem = %p\nmemfrag* mfr_elem = %p\n", sll_elem, mfr_elem);

		if (i > 0) {
			sll_elem->prev = SLL_FROM_MEM(tst, i-1);
		} else {
			sll_elem->prev = NULL;
		}

		if (i < LEN_SLL-1) {
			sll_elem->next = SLL_FROM_MEM(tst, i+1);
		} else {
			sll_elem->next = NULL;
		}

		mfr_elem->id = i;
		mfr_elem->free = 1;
		mfr_elem->size = 0;
	}

	printf("\nAlright, time to try reading data as list.\n");
	tou_sll* sll_elem = SLL_FROM_MEM(tst, 0);
	memfrag* mfr_elem = FRG_FROM_SLL(sll_elem);//FRG_FROM_MEM(tst, 0);

	while (sll_elem) {
		printf("SLL elem: (%p) id=%d  [%p]\n", sll_elem, FRG_FROM_SLL(sll_elem)->id, sll_elem->prev);
		sll_elem = sll_elem->next;
	}
	
	//printf("1) %p, 2) %p\n", tst->prev, tst->next);//, tst->)
*/


	printf("\nDone.\n");
	return 0;
}
