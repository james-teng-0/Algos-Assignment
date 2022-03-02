// COMP2521 21T2 Assignment 1
// Dict.c ... implementation of the Dictionary ADT
// z5361442 James Teng - written in July 2021
/* This file provides functions for a Binary Search Tree implementation. 
It also provides functions to add nodes in a BST to a temporary array, then 
sorts this array and inserts the first n entries in this array into the given 
"wfs" array */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "WFreq.h"

// you may define your own structs here
struct DictRep {
	char *data;
	int word_count;
	Dict left;
	Dict right;
};

// ************************ function prototypes ********************************
static void show_BST_node(Dict d);
Dict return_insert(Dict d, char *word);

int cmp_func(const void * a, const void * b);
void tree_to_array(Dict d, WFreq *all_words, int *i);
int order_sorter(const void *object_1, const void *object_2);
// ************************ end of function prototypes *************************
// Creates a new Dictionary
Dict DictNew(void) {
	Dict d = malloc(sizeof(struct DictRep));
	d->left = NULL;
	d->right = NULL;
	d->word_count = 0;
	return d;
}

// Frees the given Dictionary
void DictFree(Dict d) {
	if (d == NULL) {
		return;
	}
	if (d->left != NULL) {
		DictFree(d->left);
	}
	if (d->right != NULL) {
		DictFree(d->right);
	}
	free(d->data);
	free(d);
}

// Inserts an occurrence of the given word into the Dictionary
void DictInsert(Dict d, char *word) {
    // if root node is empty
    if (d->word_count == 0) {
        d->data = strdup(word);
        d->word_count = 1;
        return;
    }
	return_insert(d, word);
}

// helper function for the DictInsert function
Dict return_insert(Dict d, char *word) {
    if (d == NULL) {
        //create new node and store word within this node
		Dict new_node = DictNew();
        new_node->data = strdup(word);
		new_node->word_count = 1;
		return new_node;
	}
	int compare = strcmp(word, d->data);
	// if word is lexicographically smaller than node's word
	if (compare < 0) {
		d->left = return_insert(d->left, word);
	}
	// if word is lexicographically larger than node's word
	else if (compare > 0) {
		d->right = return_insert(d->right, word);
	}
	// if word is already in dictionary
	else if (compare == 0) {
		d->word_count++;
		return d;
	}
	return d;
}

// Returns the occurrence count of the given word. Returns 0 if the word
// is not in the Dictionary.
int DictFind(Dict d, char *word) {
	// if word is not found
	if (d == NULL) {
		return 0;
	}
	int compare = strcmp(word, d->data);
	// if word is lexicographically smaller than node's word
	if (compare < 0) {
		return DictFind(d->left, word);
	}
	// if word is lexicographically larger than node's word
	else if (compare > 0) {
		return DictFind(d->right, word);
	}
	// if word has been found in dictionary
	else if (compare == 0) {
		return d->word_count;
	}
	return 0;
}

// Finds  the top `n` frequently occurring words in the given Dictionary
// and stores them in the given  `wfs`  array  in  decreasing  order  of
// frequency,  and then in increasing lexicographic order for words with
// the same frequency. Returns the number of WFreq's stored in the given
// array (this will be min(`n`, #words in the Dictionary)) in  case  the
// Dictionary  does  not  contain enough words to fill the entire array.
// Assumes that the `wfs` array has size `n`.
int DictFindTopN(Dict d, WFreq *wfs, int n) {
	// declare a temporary array to hold all words in the BST
	WFreq all_words[50000];
	int j = 0;
	// reads all words in the BST into the all_words array
	tree_to_array(d, all_words, &j);
	// sorting algorithm to sort all_words array from largest frequency 
	//to lowest frequency
	qsort(all_words, j, sizeof(WFreq), order_sorter);
	int i = 0;
	// update the wfs array with the top n words from the all_words array
	while (i < j && i < n) {
		wfs[i].word = all_words[i].word;
		wfs[i].freq = all_words[i].freq;
		i++;
	}
	return i;
}

// ******************START OF HELPER FUNCTIONS FOR DictFindTopN*****************
// compare function for qsort
// determines the order to sort the array 
int order_sorter(const void *object_1, const void *object_2) {
	WFreq *node_1 = (WFreq *)object_1;
	WFreq *node_2 = (WFreq *)object_2;

    // if new node is higher frequency than current node, then we insert 
	// new_node in front of the current node
    if (node_1->freq > node_2->freq) {
        return 0;
    }
    // if new node is lower frequency than current node, then we insert new_node
    // behind the current node
    else if (node_1->freq < node_2->freq) {
        return 1;
    }
    // if new node has the same frequency as the current node, then we need to 
	// sort alphabetically
    else if (node_1->freq == node_2->freq) {
        int compare = strcmp(node_1->word, node_2->word);
        // if new node is lexicographically smaller than curr node, then we 
        // insert new_node before the current node
        if (compare < 0) {
            return 0;
        }
        // if new node is lexicographically bigger than curr node, then we 
		// insert after the current node
        else if (compare > 0) {
            return 1;
        }
    }
    return -1;
}

// traverses the tree recursively, to store nodes in the tree into a given array
void tree_to_array(Dict d, WFreq *all_words, int *j) {
	if (d == NULL) {
		return;
	}
	tree_to_array(d->left, all_words, j);
	tree_to_array(d->right, all_words, j);
	// stores word in array
	all_words[*j].word = d->data;
	// stores frequency of word in array
	all_words[*j].freq = d->word_count;
	(*j)++;
}

// ******************END OF HELPER FUNCTIONS FOR DictFindTopN*****************

// Displays the given Dictionary. This is purely for debugging purposes,
// so  you  may  display the Dictionary in any format you want.  You may
// choose not to implement this.
void DictShow(Dict d) {
	if (d == NULL) return;

    DictShow(d->left);
    show_BST_node(d);
    DictShow(d->right);
}

static void show_BST_node(Dict d) {
    if (d == NULL) return;
    printf("%s ", d->data);
}