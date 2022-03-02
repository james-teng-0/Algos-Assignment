// Lance-Williams Algorithm for Hierarchical Agglomerative Clustering
// COMP2521 Assignment 2
// James Teng z5361442
// August 2021
// Implementation of the Lance-Williams HAC algorithm, using the single linkage 
// and complete linkage method of the Lance-Williams formula.

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"
#include "LanceWilliamsHAC.h"

#define INFINITY DBL_MAX
#define PAIR 2

static int *smallest_dist(int vertices_num, Dendrogram *cluster, double **dist);
static void combine_clusters(Dendrogram *cluster, int *vertex_index);
static void lance_williams(double **dist, int vertices_num, int *vertex_index, int method);
static double find_max_weight(Graph g, Vertex i, Vertex j);

/**
 * Generates  a Dendrogram using the Lance-Williams algorithm (discussed
 * in the spec) for the given graph  g  and  the  specified  method  for
 * agglomerative  clustering. The method can be either SINGLE_LINKAGE or
 * COMPLETE_LINKAGE (you only need to implement these two methods).
 * 
 * The function returns a 'Dendrogram' structure.
 */
Dendrogram LanceWilliamsHAC(Graph g, int method) {
	int vertices_num = GraphNumVertices(g);
	// create a 2D visited array
	double **dist = malloc(vertices_num * sizeof(*dist));
	for (int i = 0; i < vertices_num; i++) {
		dist[i] = malloc(vertices_num * sizeof(double));
		for (int j = 0; j < vertices_num; j++) {
			if (i == j) {
				dist[i][j] = -1;
			}
			// runs if a direct edge exists between vertices i and j
			else if (GraphIsAdjacent(g, i, j) || GraphIsAdjacent(g, j, i)) {
				// find max weight of edges between vertex i and j
				double max_weight = find_max_weight(g, i, j);
				dist[i][j] = 1.0/max_weight;
			}
			// if there is no edge between vertices i and j
			else {
				dist[i][j] = INFINITY;
			}
		}
	}
	// create array of dendrograms with a dendrogram "cluster" for each vertex
	Dendrogram *cluster = malloc(vertices_num * sizeof(Dendrogram));
	for (int i = 0; i < vertices_num; i++) {
		// initialise each cluster and update its values
		cluster[i] = malloc(sizeof(*cluster[i]));
		cluster[i]->vertex = i;
		cluster[i]->right = NULL;
		cluster[i]->left = NULL;
	}
	// loop to keep merging clusters until there is one cluster left
	for (int i = 0; i < vertices_num - 1; i++) {
		// store index of pair of clusters with the smallest distance
		int *vertex_index = smallest_dist(vertices_num, cluster, dist);
		combine_clusters(cluster, vertex_index);
		// recalculate distances between new cluster and the other clusters
		lance_williams(dist, vertices_num, vertex_index, method);
		free(vertex_index);
	}
	// after merging all clusters, the final dendrogram will be in index 0 
	Dendrogram final_cluster = cluster[0];

	// free individual dist matrix cells and clusters
	for (int i = 0; i < vertices_num; i++) {
		free(dist[i]);
	}
	free(dist);
	free(cluster);

	return final_cluster;
}

//find pair of cluster with smallest distance between them
// returns index of the pair of cluster in the malloc'd vertex_index array
static int *smallest_dist(int vertices_num, Dendrogram *cluster, double **dist) {
	int *vertex_index = malloc(PAIR * sizeof(int)); 

	double min_dist = INFINITY;
	// loop through all possible combinations of vertices
	for (int i = 0; i < vertices_num; i++) {
		for (int j = 0; j < vertices_num; j++) {
			
			// update dist array if distance between i and j is less than the 
			// minimum distance
			if (dist[i][j] > 0 && dist[i][j] < min_dist) {
				min_dist = dist[i][j];
				// update array with the index of the vertices
				vertex_index[0] = i;
				vertex_index[1] = j;
			}
		}
	}
	return vertex_index;
}

// create a malloc'd cluster which merges a pair of clusters
static void combine_clusters(Dendrogram *cluster, int *vertex_index) {
	int v1 = vertex_index[0];
	int v2 = vertex_index[1];
	// ensure that v1 is always the lowest index
	if (v1 > v2) {
		int temp = v1;
		v1 = v2;
		v2 = temp;
		vertex_index[0] = v1;
		vertex_index[1] = v2;
  	}
	Dendrogram new_cluster = malloc(sizeof(*new_cluster));
	new_cluster->vertex = -1;
	new_cluster->left = cluster[v1];
	new_cluster->right = cluster[v2];
	// add newly merged cluster to the lowest index of the two "deleted" clusters
	cluster[v1] = new_cluster;
	// set the other "deleted" cluster to NULL
	cluster[v2] = NULL;
}

// implement the lance williams algorithm to readjust values of the dist array
static void lance_williams(double **dist, int vertices_num, int *vertex_index, int method) {
	int v1 = vertex_index[0];
	int v2 = vertex_index[1];
	// index of the newly merged cluster
	int new_cluster = v1;
	// loop through all vertices
	for (int k = 0; k < vertices_num; k++) {
		if ((k != v1) && (k != v2)) {
			// distance from newly merged cluster to vertex k
			double dist1 = dist[v1][k];
			double dist2 = dist[v2][k];
			// if dist1 is INFINITY then we use the dist2 distance
			if (dist1 == INFINITY) {
				dist[new_cluster][k] = dist2;
				dist[k][new_cluster] = dist2;
			}
			// if dist2 is INFINTIY then we use the dist1 distance
			else if (dist2 == INFINITY) {
				dist[new_cluster][k] = dist1;
				dist[k][new_cluster] = dist1;
			}
			// IF LANCE WILLIAMS METHOD IS SINGLE LINKAGE
			else if (method == SINGLE_LINKAGE) {
				if (dist1 < dist2) {
					// update dist for both sides 
					dist[new_cluster][k] = dist1;
					dist[k][new_cluster] = dist1;
				}
				// if dist1 is larger than or equal to dist2
				else {
					dist[new_cluster][k] = dist2;
					dist[k][new_cluster] = dist2;
				}
			}
			// IF LANCE WILLIAMS METHOD IS COMPLETE LINKAGE
			else if (method == COMPLETE_LINKAGE) {
				if (dist1 > dist2) {
					dist[new_cluster][k] = dist1;
					dist[k][new_cluster] = dist1;
				}
				// if dist1 is less than or equal to dist2
				else {
					dist[new_cluster][k] = dist2;
					dist[k][new_cluster] = dist2;
				}
			}
		}
		// v2 is now NULL after merging so we set both distances to INFINITY
		dist[k][v2] = INFINITY;
		dist[v2][k] = INFINITY;
	}
}

// find max weight of edges between vertex i and vertex j
static double find_max_weight(Graph g, Vertex i, Vertex j) {
	double a = 0;
	double b = 0;
	// create adjacency list of vertices coming out from i 
	AdjList i_out = GraphOutIncident(g, i);
	// loop through adjacency list of vertices going out of i and find j
	while (i_out != NULL) {
		if (i_out->v == j) {
			a = i_out->weight;
			break;
		}
		i_out = i_out->next;
	}
	// create adjacency list of vertices going into i 
	AdjList i_in = GraphInIncident(g, i);
	// loop through adjacency list of vertices going into i and find j
	while (i_in != NULL) {
		if (i_in->v == j) {
			b = i_in->weight;
			break;
		}
		i_in = i_in->next;
	}
	// if dist a is not reachable but b is reachable
	if (a == INFINITY && b != INFINITY) {
		return b;
	}
	// if dist b is reachable but a is not reachable
	else if (a != INFINITY && b == INFINITY) {
		return a;
	}
	else if (a > b) {
		return a;
	}
	// if b > a or a == b
	else {
		return b;
	}
}


/**
 * Frees all memory associated with the given Dendrogram structure.
 */
void freeDendrogram(Dendrogram d) {
	if (d != NULL) {
        freeDendrogram(d->left);
        freeDendrogram(d->right);
        free(d);
    }
}
