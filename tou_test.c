#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOU_IMPLEMENTATION
#define TOU_DBG 0
/* #define TOU_LLIST_SINGLE_ELEM */
#include "tou.h"

 void* cb(void* ptr, void* _userdata)
{
	tou_llist_t* elem = (tou_llist_t*) ptr;
	return (void*)(size_t) strcmp(elem->dat1, "test07");
} 

void* cb_ini(void* ptr, void* userdata)
{
	tou_llist_t* elem = (tou_llist_t*) ptr;
	// printf("- (cb_ini) received: %s, %s\n", elem->dat1, elem->dat2);
	return (void*)(size_t) strcmp(elem->dat1, userdata);
}

void* cb_ini2(void* ptr, void* userdata)
{
	tou_llist_t* elem = (tou_llist_t*) ptr;
	// printf("- (cb_ini2) received: %s, %s\n", elem->dat1, elem->dat2);
	return (void*)(size_t) strcmp(elem->dat1, userdata);
}

 void* cb_tok(void* ptr)
{
	tou_llist_t* elem = (tou_llist_t*) ptr;
	printf("- (cb_tok) Token <%s>\n", elem->dat1);
	return (void*) TOU_CONTINUE;
} 

 void* cb_fileread(void* blockdata, void* len, void* userdata)
{
	char* block = (char*) blockdata;
	size_t size = (size_t) len;
	char** filedata = (char**) userdata;

	size_t currsize = *filedata ? tou_strlen(*filedata) : 0;

	if (size > 0) {
		char* new_buffer = realloc(*filedata, currsize + size + 1);

		if (!new_buffer){ 
			TOU_PRINTD("cannot realloc memory\n");
			return (void*) TOU_BREAK;
		}

		// strcpy(new_buffer + currsize, block);
		memcpy(new_buffer + currsize, block, size);
		*(new_buffer + currsize + size) = '\0';
		*filedata = new_buffer;
	}
	
	return (void*) TOU_CONTINUE;
} 


int main(int argc, char const* argv[])
{
	////////////////////
	// File read test //
	////////////////////
printf("\n\n");
printf("========================================\n"
       "|            FILE READ TEST            |\n"
       "========================================\n");
printf("\n");

	size_t siz;
	FILE* fptr;
	char* contents;

	// 1. Read file simply using filename //
	contents = tou_read_file("testfile.txt", &siz);
	printf("1.) tou_read_file contents:\n|%s|\nSize: %zu, %d\n", contents, siz, strlen(contents));
	free(contents); contents = NULL;

	// 2. Read FILE* using custom function //
	contents = tou_strdup(""); // malloc with just '\0' since we're using strlen in the function

	fptr = fopen("testfile.txt", "rb");
	siz = tou_read_fp_in_blocks(fptr, TOU_DEFAULT_BLOCKSIZE, cb_fileread, &contents);
	fclose(fptr); fptr = NULL;

	printf("\n2.) tou_read_fp_in_blocks contents (%zu):\n%s", siz, contents);
	free(contents); contents = NULL;

	// 3. Use tou-defined func+struct for reading FILE* (this is how tou_read_file() is implemented) //
	tou_block_store_struct my_data_buf = {NULL, 0};
	
	fptr = fopen("testfile.txt", "rb");
	tou_read_fp_in_blocks(fptr, -1, tou_block_store_cb, &my_data_buf); // default block size, without function
	fclose(fptr); fptr = NULL;
	
	printf("3.) I have read (%zu):\n%s\n", my_data_buf.size, my_data_buf.buffer);
	
	free(my_data_buf.buffer); my_data_buf.buffer = NULL;

	// 4. Retrieve only file size //
	fptr = fopen("testfile.txt", "rb");
	printf("4.) File size: %zu\n", tou_read_fp_in_blocks(fptr, 0,0,0));
	fclose(fptr); fptr = NULL;


printf("\n\n");
printf("========================================\n"
       "|         FUNCTION POINTER TEST        |\n"
       "========================================\n");
printf("\n");

	tou_func2 funcptr = cb;
	printf("Calling funcptr (func `cb`) for .dat1=test07 :: %d\n",
		funcptr(
			&((tou_llist_t){0,0,"test07",0,0,0}),
			NULL) );


printf("\n\n");
printf("========================================\n"
       "|           LINKED LIST TEST           |\n"
       "========================================\n");
printf("\n");

	printf("Sizeof tou_llist_t: %d\n", sizeof(tou_llist_t));

	tou_llist_t* head = tou_llist_new(); //= NULL;

// Append some elements to the list //
	printf("\n=== Append some elements to the list:\n");
	for (int i = 0; i < 12; i++) {
		char* txt = malloc(16+1);
		sprintf(txt, "test%02d", i);

		#ifdef TOU_LLIST_SINGLE_ELEM
			tou_llist_append(&head, txt, 1);
		#else
			tou_llist_append(&head, txt, (void*)(size_t) i, 1,0);
		#endif
	}

// Print appended elements
	printf("== Displaying the list:\n");
	tou_llist_print(head, "%s", "%d");
	printf("\n");


// Retrieve and print from tail //
	printf("\n=== Retrieve and print from tail:\n");
	tou_llist_t* tail_elem = tou_llist_get_tail(head);
	while (tail_elem)
	{
#ifdef TOU_LLIST_SINGLE_ELEM
		printf("- val=%s\n", (char*)tail_elem->dat1);
#else
		printf("- key=%s, val=%d\n",
			(char*)tail_elem->dat1, (int)(size_t)tail_elem->dat2);
#endif
		tail_elem = tail_elem->next;
	}
	printf("\n");

// Retrieve and print from head //
	printf("\n=== Retrieve and print by hand from head:\n");
	tou_llist_t* head_elem = tou_llist_get_head(head);
	while (head_elem)
	{
#ifdef TOU_LLIST_SINGLE_ELEM
		printf("- val=%s\n", (char*)head_elem->dat1);
#else
		printf("- key=%s, val=%d\n",
			(char*)head_elem->dat1, (int)(size_t)head_elem->dat2);
#endif
		if (head_elem->prev == NULL)
			break;
		head_elem = head_elem->prev;
	}
	printf("\n");


// Find using callback function //
	printf("\n=== Find using callback function 'cb' :\n");

	tou_llist_t* cb_found_elem = tou_llist_find_func(head, cb, NULL);
#ifdef TOU_LLIST_SINGLE_ELEM
	printf("Found: %s\n", (char*)cb_found_elem->dat1);
#else
	printf("Found: %s, %d\n",
		(char*)cb_found_elem->dat1, (int)(size_t)cb_found_elem->dat2);
#endif
	printf("\n");


// Append & prepend new elements //

	// Append to head, tail, middle
	printf("\n=== Appending new elements to the head, tail and the found element in the middle...\n");

	/*tou_llist_t* happend_res =*/ tou_llist_append(&head, "APPENDED TO HEAD", /* (void*)(size_t) */ 25, 0, 0);
	tou_llist_t* tail = tou_llist_get_oldest(head);
	tou_llist_append(&tail, "APPENDED TO TAIL", /* (void*)(size_t) */ 26, 0, 0);
	// Append to some inner element
	printf("Appending to elem: %s\n", (char*)cb_found_elem->dat1);
	tou_llist_append(&cb_found_elem, "APPENDED TO INNER ELEMENT", /* (void*)(size_t) */ 27, 0, 0);

	// Prepend to head, tail, middle //
	printf("\n=== Prepending new elements to the head, tail and the found element in the middle...\n");
	tou_llist_prepend(&head, "PREPENDED TO HEAD", /* (void*)(size_t) */ 28, 0, 0);
	/*
	tail = tou_llist_get_oldest(head); // We can still use the previous value of tail from when
	                                   // appending since it hasn't changed which wouldn't be the
	                                   // case if we had prepended first and then appended
	*/
	tou_llist_prepend(&tail, "PREPENDED TO TAIL", /* (void*)(size_t) */ 29, 0, 0);
	tou_llist_prepend(&cb_found_elem, "PREPENDED TO INNER ELEMENT", /* (void*)(size_t) */ 30, 0, 0);

	// Print list after appending
	printf("\nShowing list after appending & prepending, before popping:\n");
	tou_llist_print(head, "%s", "%d");
	printf("\n");

// Pop that element //
	printf("\n=== Popping element (%s)...\n\n", cb_found_elem->dat1);
	tou_llist_t* popped_elem = tou_llist_pop(cb_found_elem);


// Print from tail one more time to see the result //
	printf("\n== Showing that the element was popped (from tail):\n");
	tou_llist_print_tail(head, "%s", "%d");

	// Free manually because the elem was only pop()'d, not remove()'d
	printf("\n=== Freeing element (%s)...\n", (char*)popped_elem->dat1);
	tou_llist_free_element(popped_elem);
	printf("\n");

// Find using dat1 as 'key' (which we know is a string in this case) //
	printf("\n=== Searching for element where .dat1 = 'test05'...\n");
	tou_llist_t* key_found_elem = tou_llist_find_key(head, "test05");
#ifdef TOU_LLIST_SINGLE_ELEM
	printf("Found: %s\n", (char*)key_found_elem->dat1);
#else
	printf("Found: %s, %d\n", (char*)key_found_elem->dat1, (int)(size_t)key_found_elem->dat2);
#endif

// Remove the element, this function also deallocates it. //
	printf("\n=== Removing element...\n");
	tou_llist_remove(key_found_elem);

	// Find element which contains number is 2 (its .dat2 param) //
	// by using find_exact and assuming that .dat1 (string) will //
	// not be the value "(void *)-1" in any of the elements.     //
	// Won't of course work with TOU_LLIST_SINGLE_ELEM defined.  //
	printf("\n=== Searching for element where .dat2 = 2...\n");

	// #pragma GCC diagnostic ignored "-Wint-conversion"
	tou_llist_t* exact_found_elem = tou_llist_find_exact(head, (void*)-1, (void*) 2);
	// #pragma GCC diagnostic warning "-Wint-conversion"

	printf("Found: %s, %d\n",
		(char*)exact_found_elem->dat1, /* (int)(size_t) */ exact_found_elem->dat2);

	/* tou_llist_t* */ tail_elem = tou_llist_get_tail(exact_found_elem);
	tou_llist_remove(exact_found_elem);

// Show after remove()'ing
	printf("\n=== Contents after remove() was used:\n");
	while (tail_elem)
	{
#ifdef TOU_LLIST_SINGLE_ELEM
		printf("- %s\n", (char*)tail_elem->dat1);
#else
		printf("- %s, %d\n", (char*)tail_elem->dat1, (int)(size_t)tail_elem->dat2);
#endif
		tail_elem = tail_elem->next;
	}

	// Delete whole list and all elements
	tou_llist_destroy(head);


// Gather test //
	tou_llist_t* gathertst = tou_llist_new();
	size_t len;
	void** arr;
	tou_llist_append(&gathertst, tou_strdup("str1"), /* (void*)(size_t) */ 33, 1,0);
	tou_llist_append(&gathertst, tou_strdup("str2"), /* (void*)(size_t) */ 44, 1,0);

// Gather .dat1 //
	printf("\n=== Gathered .dat1 fields:\n");
	arr = tou_llist_gather_dat1(gathertst, &len); // arr is malloc'd array
	for (size_t i = 0; i < len; i++)
		printf("- arr[%d] = %s\n", i, (char*) arr[i]);
	free(arr);

// Gather .dat2 //
	printf("\n=== Gathered .dat2 fields:\n");
	arr = tou_llist_gather_dat2(gathertst, &len); // arr is malloc'd array
	for (size_t i = 0; i < len; i++)
		printf("- arr[%d] = %d\n", i, (char*) arr[i]);
	free(arr);

	tou_llist_destroy(gathertst);


printf("\n\n");
printf("========================================\n"
       "|               TRIM TEST              |\n"
       "========================================\n");
printf("\n");

	char buf[] = "  ke ey = vvaalluuee   ";
	printf("Original contents to trim: |%s|\n\n", buf);

// Retrieve pointers to start and end of "trimmed" area //
	char *trim_start, *trim_end;
	tou_trim_string_pure(buf, &trim_start, &trim_end);
	printf("Characters between *trim_start and *trim_end (original array untouched; trim_string_pure):\n|");
	while (trim_start < trim_end)
		printf("%c", *trim_start++);
	printf("|\nEnd contents.\n");

// Trim given string by modifying original //
	char* buf_ptr = buf;
	tou_trim_string(&buf_ptr);
	printf("Contents after trimming both ends in place (trim_string):\n|");

// Find substring in string //
	char* sep;
	int sep_len;
	printf("== Trying to find ' = ' in '%s'...\n", buf_ptr);
	sep = tou_sfind(buf_ptr, " = ");
	sep_len = 3;
	if (!sep) {
		// First string ' = ' not found, try '='
		printf("== Trying to find '=' in '%s'...\n", buf_ptr);
		sep = tou_sfind(buf_ptr, "=");
		sep_len = 1;
	}

	if (!sep) {
		printf("Neither substring found.\n");
	} else {
		printf("Substring '%.*s' found at %d.\n", sep_len, sep, (sep - buf_ptr));

		// Cut up in some sort of "key-value" pairs
		printf("Using it as a key-value separator:\n");
		char key[32+1], val[32+1];
		*sep = '\0';
		strcpy(key, buf_ptr);
		strcpy(val, sep+sep_len);
		printf("Key: |%s|  Val: |%s|\n", key, val);
	}


printf("\n\n");
printf("========================================\n"
       "|       STRING SPLIT & ITER TEST       |\n"
       "========================================\n");
printf("\n");

	char str[] = "abc = 123,def = 456;";
	char delim[] = " = ";
	printf("Original string: |%s|\n", str);

// Split string by a string delimiter //
	printf("== Splitting '%s' by '%s'...\n", str, delim);
	tou_llist_t* splitted = tou_split(str, delim);
	tou_llist_t* iter = splitted;
	
	printf("== Tokens found:\n");
	while (iter) {
		printf("- Token <%s>\n", (char*)iter->dat1);
		iter = iter->/*next*/prev;
	}

	printf("== Iterating using llist_iter():\n");
	iter = splitted; // tou_llist_get_head(splitted);
	tou_llist_iter(iter, cb_tok);

	tou_llist_destroy(splitted);


	// Character/string replacing test //
printf("\n\n");
printf("========================================\n"
       "|   CHARACTER/STRING REPLACING TEST    |\n"
       "========================================\n");
printf("\n");

	char repl_ss[] = "aeiouoiea/1/2/3";
	printf("\noriginal = %s\n", repl_ss);
	printf("strchr(i) = %s\n", tou_strchr(repl_ss, 'i'));
	printf("strrchr(i) = %s\n", tou_strrchr(repl_ss, 'i'));
	printf("strchr(a) = %s\n", tou_strchr(repl_ss, 'a'));
	printf("strrchr(a) = %s\n", tou_strrchr(repl_ss, 'a'));
	printf("strchr(\\0) = %s\n", tou_strchr(repl_ss, '\0'));
	printf("strrchr(\\0) = %s\n", tou_strrchr(repl_ss, '\0'));
	printf("strchr(/) = %s\n", tou_strchr(repl_ss, '/'));
	printf("strrchr(/) = %s\n", tou_strrchr(repl_ss, '/'));
	printf("original after = %s\n", repl_ss);

	printf("replace_ch o -> F:\n");
	tou_replace_ch(repl_ss, 'o', 'F');
	printf("original at the end = %s\n", repl_ss);


	// .INI test //
printf("\n\n");
printf("========================================\n"
       "|      .INI FILE OPERATIONS TEST       |\n"
       "========================================\n");
printf("\n");


// Load .INI from memory //
	printf("\n=== Parsing INI from buffer...\n");
	char* newbuf = tou_strdup("\n\r\n  ; abc = def \n\r\n[buffer test]\n sect1prop1 = testttyyy\r\n\n[stillbuffertest]\n      oycaramba = nice test :)))  \n  ");
	tou_llist_t* newini = tou_ini_parse_buffer(newbuf);
	free(newbuf);

	printf("\n=== Printing parsed contents:\n");
	tou_ini_print(newini);
	free(newini);

// Load .INI from FILE* //
	printf("\n=== Load using ini_parse_fp:\n");
	FILE* fp = fopen("testini.ini", "r");
	tou_llist_t* inicontents = tou_ini_parse_fp(fp);
	fclose(fp);

	printf("\n=== Printing parsed contents:\n");
	tou_ini_print(inicontents);

	printf("\n== Setting [first] surname = Gledic\n");
	tou_ini_set(&inicontents, "first", "surname", "Gledic");
	// tou_ini_print(inicontents);
	printf("\n== Setting [first] surname = Bilus\n");
	tou_ini_set(&inicontents, "first", "surname", "Bilus");
	// tou_ini_print(inicontents);
	printf("\n== Setting [first] rizz = 12\n");
	tou_ini_set(&inicontents, "first", "rizz", "12");
	// tou_ini_print(inicontents);
	printf("\n== Setting [first] rizz = 255\n");
	tou_ini_set(&inicontents, "first", "rizz", "255");
	// tou_ini_print(inicontents);

	printf("\n=== Calling ini_get [first] surname is :: %s\n",
		(char*)tou_ini_get(inicontents, "first", "surname"));

	tou_llist_t* prop = tou_ini_get_property(inicontents, "first", "name");
	printf("\n=== Dereferenced retrieved ini_get_property of [first] name :: %s = %s\n",
		(char*)prop->dat1, (char*)prop->dat2);
	
	/*
	tou_llist_t* new_prp = malloc(sizeof(tou_llist_t));
	new_prp->prev = (*prop)->prev;
	new_prp->next = (*prop)->next;
	new_prp->dat1 = tou_strdup("name");
	new_prp->destroy_dat1 = 1;
	new_prp->dat2 = tou_strdup("BBBBBBBBBBBBBBBBB");
	new_prp->destroy_dat2 = 1;
	free((*prop)->dat1);
	free((*prop)->dat2);
	memcpy(*prop, new_prp, sizeof(tou_llist_t));

	prop = tou_ini_get_property(inicontents, "first", "name");
	printf("\nDereferenced retrieved ini_get_property of [first] name :: %s = %s\n",
		prop->dat1, prop->dat2);
	*/

	printf("\n=== Updating using ini_set [first] name = Giuseppe :: returns '%s'\n",
		(char*)tou_ini_set(&inicontents, "first", "name", "Giuseppe") );
	printf("\n=== Reading the same property reference ([first] name) after updating with ini_set :: %s = %s\n",
		(char*)prop->dat1, (char*)prop->dat2);

	printf("\nPrinting updated inistruct:\n");
	tou_ini_print(inicontents);

// Find .INI element by hand using llist_find_func with custom comparison function //
	printf("\n=== Searching for .INI elem by hand using llist_find_func with cb_ini,'first':\n");

	// First search inicontents to retrieve section
	tou_llist_t* section = tou_llist_find_func(inicontents, cb_ini, "first");

	if (section == NULL) {
		printf("No matching section found :(\n");
	} else {
		printf("Section found: [%s], property .dat2(llist) = ptr %p\n", (char*)section->dat1, section->dat2);
		printf("\n=== Searching section for property using llist_find_func with cb_ini2,'name': \n");
		tou_llist_t* section2 = tou_llist_find_func(
			section->dat2, cb_ini2,"name");

		if (section2 == NULL) {
			printf(" No matching property found :(\n");
		} else {
			printf(" -> Property found: %s = %s\n", (char*)section2->dat1, (char*)section2->dat2);
		}
	}
	printf("\n");

	FILE* fpout = NULL;
	// int retval = 0;
	
	fpout = fopen("testini.out.ini", "w");
	/* retval = */ tou_ini_save_fp(inicontents, fpout);
	// TOU_PRINTD("INI save .ini return: %d\n\n", retval);
	printf("=== Saved updated inistruct as 'testini.out.ini'.\n");
	fclose(fpout);

	fpout = fopen("testini.out.json", "w");
	/* retval = */ tou_ini_save_fp_json(inicontents, fpout);
	// TOU_PRINTD("INI save .json return: %d\n\n", retval);
	printf("=== Saved updated inistruct as 'testini.out.json'.\n");
	fclose(fpout);

	fpout = fopen("testini.out.xml", "w");
	/* retval = */ tou_ini_save_fp_xml(inicontents, fpout);
	// TOU_PRINTD("INI save .xml return: %d\n\n", retval);
	printf("=== Saved updated inistruct as 'testini.out.xml'.\n");
	fclose(fpout);

	// Obliterate 
	tou_ini_destroy(inicontents);


// Enable/disable STDOUT test //
	printf("\n\n");
printf("========================================\n"
       "|       STDOUT ENABLE-DISABLE TEST     |\n"
       "========================================\n");
printf("\n");

	printf("=== Disabling STDOUT and trying to print something...\n");
	int old_stdout = tou_disable_stdout();

	printf("I\n");
	printf("AM\n");
	printf("PRINTING\n");
	printf("SOMETHING\n");

	tou_enable_stdout(old_stdout);
	printf("=== Re-enabling STDOUT and trying to print once more...\n");

	printf("Now I can print things again.\n");


// Stack & Queue test //
	printf("\n\n");
printf("========================================\n"
       "|           STACK & QUEUE TEST         |\n"
       "========================================\n");
printf("\n");

/* =============================== */
printf("== DumbTest(TM): STACK\n\n");
/* =============================== */

	tou_stack* stack = tou_stack_new();
	
	int* firstelem = calloc(1, sizeof *firstelem);
	int* secondelem = calloc(1, sizeof *secondelem);
	char* thirdelem = NULL;

	*firstelem = 123;
	*secondelem = 456;
	thirdelem = tou_strdup("Aj em da elementtt");
	
	// First push //
	tou_stack_push(&stack, firstelem);
#ifndef TOU_LLIST_SINGLE_ELEM
	stack->dat2 = "Some first const str";
#endif
	printf("Pushed 1st: (%p) %d\n", firstelem, *firstelem);

	// Second push //
	tou_stack_push(&stack, secondelem);
#ifndef TOU_LLIST_SINGLE_ELEM
	stack->dat2 = "Some second const str";
#endif
	printf("Pushed 2nd: (%p) %d\n", secondelem, *secondelem);
	
	// Third push //
	tou_stack_push(&stack, thirdelem);
#ifndef TOU_LLIST_SINGLE_ELEM
	stack->dat2 = "Some third const str";
#endif
	printf("Pushed 3rd: (%p) %s\n", thirdelem, thirdelem);
	

	tou_llist_print(stack, "%p", "%s" /* gets ignored if TOU_LLIST_SINGLE_ELEM */);
	printf("\n");

	// printf("Stack ptr is: %p\n");

	// First pop //
#ifndef TOU_LLIST_SINGLE_ELEM
	printf("Stack_top->dat2: %s\n", stack->dat2);
#endif
	char* val1 = tou_stack_pop(&stack);
	printf("Popped 1st: %s\n", val1);
	// printf("Stack ptr is: %p\n");
	

	// Second pop //
#ifndef TOU_LLIST_SINGLE_ELEM
	printf("Stack_top->dat2: %s\n", stack->dat2);
#endif
	int* val2 = tou_stack_pop(&stack);
	printf("Popped 2nd: %d\n", *val2);
	// printf("Stack ptr is: %p\n");


	// Third pop //
#ifndef TOU_LLIST_SINGLE_ELEM
	printf("Stack_top->dat2: %s\n", stack->dat2);
#endif
	int* val3 = tou_stack_pop(&stack);
	printf("Popped 3rd: %d\n", *val3);
	// printf("Stack ptr is: %p\n");


	// Try to pop from empty //
	printf("Stack ptr is: %p\n", stack);
	tou_stack_pop(&stack);
	printf("Stack ptr is: %p\n", stack);

	tou_stack_destroy(stack); stack = NULL;

	free(firstelem); firstelem = NULL;
	free(secondelem); secondelem = NULL;
	free(thirdelem); thirdelem = NULL;

/* =============================== */
printf("\n\n== DumbTest(TM): QUEUE\n\n");
/* =============================== */

	tou_queue* queue = tou_queue_new();
	
	firstelem = calloc(1, sizeof *firstelem);
	secondelem = calloc(1, sizeof *secondelem);
	thirdelem = NULL;

	*firstelem = 123;
	*secondelem = 456;
	thirdelem = tou_strdup("Aj em da elementtt");
	

	// First push //
	tou_queue_push(&queue, firstelem);
#ifndef TOU_LLIST_SINGLE_ELEM
	tou_llist_get_oldest(queue)->dat2 = "Some first const str";
#endif
	printf("Pushed 1st: (%p) %d\n", firstelem, *firstelem);

	
	// Second push //
	tou_queue_push(&queue, secondelem);
#ifndef TOU_LLIST_SINGLE_ELEM
	tou_llist_get_oldest(queue)->dat2 = "Some second const str";
#endif
	printf("Pushed 2nd: (%p) %d\n", secondelem, *secondelem);
	

	// Third push //
	tou_queue_push(&queue, thirdelem);
#ifndef TOU_LLIST_SINGLE_ELEM
	tou_llist_get_oldest(queue)->dat2 = "Some third const str";
#endif
	printf("Pushed 3rd: (%p) %s\n", thirdelem, thirdelem);
	

	tou_llist_print(queue, "%p", "%s" /* gets ignored if TOU_LLIST_SINGLE_ELEM */);
	printf("\n");

	// printf("Queue ptr is: %p\n");


	// First pop //
#ifndef TOU_LLIST_SINGLE_ELEM
	printf("Queue_top->dat2: %s\n", queue->dat2);
#endif
	val1 = tou_queue_pop(&queue);
	printf("Popped 1st: %d\n", *val1);
	// printf("Queue ptr is: %p\n");
	

	// Second pop //
#ifndef TOU_LLIST_SINGLE_ELEM
	printf("Queue_top->dat2: %s\n", queue->dat2);
#endif
	val2 = tou_queue_pop(&queue);
	printf("Popped 2nd: %d\n", *val2);
	// printf("Queue ptr is: %p\n");


	// Third pop //
#ifndef TOU_LLIST_SINGLE_ELEM
	printf("Queue_top->dat2: %s\n", queue->dat2);
#endif
	val3 = tou_queue_pop(&queue);
	printf("Popped 3rd: %s\n", val3);
	// printf("Queue ptr is: %p\n");


	// Try to pop from empty //
	printf("queue ptr is: %p\n", queue);
	tou_queue_pop(&queue);
	printf("queue ptr is: %p\n", queue);

	tou_queue_destroy(queue); queue = NULL;

	free(firstelem); firstelem = NULL;
	free(secondelem); secondelem = NULL;
	free(thirdelem); thirdelem = NULL;



	// Server test (WIP) //
/*
	tou_server* serv = tou_spinup_server(servcb);
	while (1) {
		;
	}
	tou_destroy_server(serv);
*/

	printf("\nDone.\n");
	return 0;
}
