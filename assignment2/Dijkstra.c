// Dijkstra API implementation
// COMP2521 Assignment 2
// James Teng z5361442
// August 2021
// Implementation of a variation of Dijkstra's algorithm to find all possible
// shortest paths from a given source node to all other nodes in the graph
//   *if there are multiple shortest paths from the source node to another node
//   *the program keeps track of all of them using a linked list of predecessors.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Dijkstra.h"
#include "Graph.h"
#include "PQ.h"

//******************************FUNCTION DECLARATIONS***************************
static PredNode *create_prednode(Vertex v);
static void free_predlist(PredNode *pred);
//******************************************************************************

ShortestPaths dijkstra(Graph g, Vertex src) {
	ShortestPaths sps;
	sps.numNodes = GraphNumVertices(g); 
	sps.src = src;
	// dynamically allocate dist array based on number of vertices
	sps.dist = malloc(sps.numNodes * sizeof(int));
	// dynamically allocate pred array of linked lists based on number of vertices
	sps.pred = malloc(sps.numNodes * sizeof(struct PredNode));
	PQ v_set = PQNew();

	//set all values in pred array to NULL and all values in dist array to
	//INFINITY
	for (int i = 0; i < GraphNumVertices(g); i++) {
		sps.dist[i] = INFINITY;
		sps.pred[i] = NULL;
	}
	sps.dist[src] = 0;
	// queue the source vertex
	PQInsert(v_set, src, 0);

	// runs while the priority queue is not empty
	while (!PQIsEmpty(v_set)) {
		int vertex = PQDequeue(v_set);
		// create adjacency list representation of outlinks from dequeued vertex
		AdjList out = GraphOutIncident(g, vertex);

		// loop through AdjList of out edges from the vertex
		while (out != NULL) {
			// perform edge relaxation (update arrays if we find a new min path)
			if (sps.dist[vertex] + out->weight < sps.dist[out->v]) {
				// clear the pred_nodes list (as they are not the minimum cost)
				if (sps.pred[out->v] != NULL) {
					free_predlist(sps.pred[out->v]);
				}
				// update dist to the new minimum distance
				sps.dist[out->v] = sps.dist[vertex] + out->weight;
				// add the current dequeued vertex to the predecessor list of the
				// current "out" vertex
				sps.pred[out->v] = create_prednode(vertex);
				PQInsert(v_set, out->v, out->weight);
			}
			// if we have found a different path of the same minimum distance
			else if (sps.dist[vertex] + out->weight == sps.dist[out->v]) {
				// add new path to the head of the adjacency list of predecessors
				PredNode *curr = sps.pred[out->v];
				PredNode *new_head = create_prednode(vertex);
				// link new node to the head
				new_head->next = curr;
				// replace head with new node
				sps.pred[out->v] = new_head;
			}
			out = out->next;
		}
	}
	PQFree(v_set);
	return sps;
}

// helper function to return a newly malloc'd PredNode
static PredNode *create_prednode(Vertex v) {
	PredNode *new_node = malloc(sizeof(struct PredNode));
	new_node->v = v;
	new_node->next = NULL;
	return new_node;
}

// helper function to free a PredNodes linked list
static void free_predlist(PredNode *pred) {
	PredNode *temp;
	// iterate through list
	while (pred != NULL) {
		temp = pred;
		pred = pred->next;
		free(temp);
	}
}

// frees all memory associated with the given ShortestPaths structure.
void freeShortestPaths(ShortestPaths sps) {
	// free dist array
	free(sps.dist);
	int i = 0;
	// iterate through array of linked list and free all linked lists
	while (i < sps.numNodes) {
		PredNode *temp = sps.pred[i];
		free_predlist(temp);
		i++;
	}
	// free the array of linked lists
	free(sps.pred);
}


void showShortestPaths(ShortestPaths sps) {

}