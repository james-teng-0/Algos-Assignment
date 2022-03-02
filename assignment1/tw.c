// COMP2521 21T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File
// z5361442 James Teng - written in July 2021
/* This file parses and reformats words from text-file and inserts words into a 
BST implementation. Then prints out words and their frequencies from highest to 
lowest. */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "stemmer.h"
#include "WFreq.h"

#define MAXLINE 1000
#define MAXWORD 100
#define STOPWORDS 654

#define isWordChar(c) (isalnum(c) || (c) == '\'' || (c) == '-')

// ***************************FUNCTION PROTOTYPES ******************************
void create_array(char stopword_array[STOPWORDS][MAXWORD]);
int stopword_search(char stopword_array[STOPWORDS][MAXWORD], char search_word[MAXWORD]);
void tokenise(char line[MAXLINE]);
void bookwords_to_BST(char *fileName, Dict d, char stopword_array[STOPWORDS][MAXWORD]);
// ***************************MAIN FUNCTION ************************************
int main(int argc, char *argv[]) {
	int   nWords;    // number of top frequency words to show
	char *fileName;  // name of file containing book text

	// process command-line args
	switch (argc) {
		case 2:
			nWords = 10;
			fileName = argv[1];
			break;
		case 3:
			nWords = atoi(argv[1]);
			if (nWords < 10) nWords = 10;
			fileName = argv[2];
			break;
		default:
			fprintf(stderr,"Usage: %s [Nwords] File\n", argv[0]);
			exit(EXIT_FAILURE);
	}

	//declare array to store stopwords
    char stopword_array[STOPWORDS][MAXWORD];
	// adds stopwords into the "stopword_array"
	create_array(stopword_array);
	
	Dict d = DictNew();
	// converts text to words which are stored in a binary search tree
	bookwords_to_BST(fileName, d, stopword_array);

	WFreq wfs[15000];	
	int i = 0;
	// read words into wfs array sorted by highest frequency to lowest frequency
	// DictFindTopN returns the amount of words stored in the wfs array
	int loop = DictFindTopN(d, wfs, nWords);
	while (i < loop) {
		printf("%d %s\n", wfs[i].freq, wfs[i].word);
		i++;
	}
	// frees the "dictionary" (BST that stored the words) memory
	DictFree(d);
}


//**************************FUNCTIONS ******************************************
//creates an array for stopwords
void create_array(char stopword_array[STOPWORDS][MAXWORD]) {
	FILE *fp = fopen("stopwords", "r");
	// error handling if there is no stopwords file
	if (fp == NULL) {
		fprintf(stderr, "Can't open %s\n", "stopwords");
		exit(EXIT_FAILURE);
	}
	char line[MAXLINE + 1];

	int counter = 0;
	//reads in all lines in the text document
	while (fgets(line, MAXLINE + 1, fp) != NULL) {
		// replaces \n with \0
		line[strcspn(line, "\n")] = '\0';
		// copies the line that was read in, into the stopword_array
		strcpy(stopword_array[counter], line);
		counter++;
	}
	fclose(fp);
}

//binary search algorithm in an array, to check if a given word is a stopword
int stopword_search(char stopword_array[STOPWORDS][MAXWORD], char search_word[MAXWORD]) {
	int low = 0;
	int high = STOPWORDS - 1;
	int compare = 0;

	while (low <= high) {
		// halves the middle index to narrow our search
		int mid = (high + low) / 2;
		// compares the middle (current) word in the array to the search word
		compare = strcmp(stopword_array[mid], search_word);
		if (compare > 0) {
			high = mid - 1;
		}
		else if (compare < 0) {
			low = mid + 1;
		}
		// if word has been matched
		else {
			return mid;
		}
	}
	// word not found
	return -1;
}

// replaces non-valid characters with space and turns capital letters into 
// lowercase
void tokenise(char line[MAXLINE]) {
	int i = 0;
	// iterate through line
	while (line[i] != '\0') {
		if (!isWordChar(line[i])) {
			// replace non-valid characters with space
			line[i] = ' ';
		}
		// replaces capital letters with lower-case letters
		line[i] = tolower(line[i]);
		i++;
	}
	
}

// reads in a file and converts the text into formatted words which are then 
// stored in a binary search tree
void bookwords_to_BST(char *fileName, Dict d, char stopword_array[STOPWORDS][MAXWORD]) {
	// create a file pointer and open selected file
	FILE *fp = fopen(fileName, "r");
	// error handling if file name on command-line is non-existent/unreadable
	if (fp == NULL) {
		fprintf(stderr, "Can't open %s\n", fileName);
		exit(EXIT_FAILURE);
	}
	
	char line[MAXLINE];
	
	// declare starting string
	char *begin = "*** START OF";
	int begin_length = strlen(begin);
	// declare ending string
	char *end = "*** END OF";
	int end_length = strlen(end);
	int found_start = 0;
	int found_end = 0;
	
	//reads in all lines in the text document
	while (fgets(line, MAXLINE, fp) != NULL) {
		// checks if the current line matches the starting string
		if (found_start == 0 && strncmp(line, begin, begin_length) == 0) {
			found_start = 1;
		}
		// checks if the current line matches the ending string
		else if (found_end == 0 && strncmp(line, end, end_length) == 0) {
			found_end = 1;
			break;
		}
		// runs after the starting string is found
		else if (found_start == 1) {
			tokenise(line);
			// extract words using space as a delimiter 
			char* token = strtok(line, " ");
			// extracts all words in the line string
			while (token != NULL) {
				// runs if word is more than one character
				if (strlen(token) > 1) {
					// runs if word is not a stopword
					if (stopword_search(stopword_array, token) == -1) {
						// stem word
						stem(token, 0, strlen(token) - 1);
						// insert word into binary search tree
						DictInsert(d, token);
					}
				}
				token = strtok(NULL, " ");
			}
		}
	}
	// error handling if can't find the "*** START OF" line
	if (found_start == 0) {
		fprintf(stderr, "Not a Project Gutenberg book\n");
		exit(EXIT_FAILURE);
	}
	// error handling if EOF encountered before "*** END OF" line
	else if (found_end == 0) {
		fprintf(stderr, "Not a Project Gutenberg book\n");
		exit(EXIT_FAILURE);
	}
	//closes file 
	fclose(fp);
}

