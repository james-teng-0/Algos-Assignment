// Centrality Measures API implementation
// COMP2521 Assignment 2
// James Teng z5361442
// August 2021
// Implements closeness centrality, betweeeness centrality and normalised 
// betweeness centrality for a given directed weighted graph.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "Dijkstra.h"
#include "PQ.h"

//***********************FUNCTION DECLARATIONS**********************************
static double closeness_formula(int dist_sum, int N, int n);
static double paths_count(ShortestPaths sps, PredNode *curr);
static double normal_formula(int num_nodes, double value);
static double in_path_check(ShortestPaths sps, PredNode *curr, int v);
//******************************************************************************

//************************CLOSENESS CENTRALITY FUNCTIONS************************

NodeValues closenessCentrality(Graph g) {
	int vertices_num = GraphNumVertices(g);
	NodeValues nvs = {0};
	// dynamically allocate memory for values array based on number of vertices
	nvs.values = malloc(vertices_num*sizeof(double));
	nvs.numNodes = vertices_num;

	for (int i = 0; i < vertices_num; i++) {
		// finds the shortest paths to all vertices reachable from vertex i
		ShortestPaths sps = dijkstra(g, i);

		double dist_sum = 0;
		int reachable_num = 0;
		// loop through all vertices in graph
		for (int j = 0; j < vertices_num; j++) {
			// runs if vertex j is reachable from vertex i
			if (sps.dist[j] != 0 && sps.dist[j] != INFINITY) {
				dist_sum += sps.dist[j];
				reachable_num++;
			}
		}
		// node counts itself as a reachable node
		reachable_num++;
		// if node is not reachable, closeness value is 0
		if (dist_sum == 0) {
			nvs.values[i] = 0;
		}
		else {
			double value = closeness_formula(dist_sum, vertices_num, reachable_num);
			// update nvs values array with the calculated closeness centrality
			nvs.values[i] = value;
		}
		freeShortestPaths(sps);
	}
	return nvs;
}

// formula to return the closeness centrality for a node given amount of 
// reachable nodes and total min distance
static double closeness_formula(int dist_sum, int N, int n) {
	double result = 1.0*(n - 1)*(n - 1)/(N - 1);
	result = 1.0*result/dist_sum;
	return result;
}
//******************************************************************************

//***********************BETWEENESS CENTRALITY FUNCTIONS************************

NodeValues betweennessCentrality(Graph g) {
	int vertices_num = GraphNumVertices(g);
	NodeValues nvs = {0};
	nvs.values = malloc(vertices_num*sizeof(double));
	nvs.numNodes = vertices_num;

	// loop through v vertices 
	for (int v = 0; v < vertices_num; v++) {
		double result = 0;

		// loop through all possible src vertices
		for (int src = 0; src < vertices_num; src++) {
			if (v == src) continue;
			// finds the shortest paths to all vertices reachable from src vertex
			ShortestPaths sps = dijkstra(g, src);

			// loop through all possible dest vertices
			for (int dest = 0; dest < vertices_num; dest++) {
				if (dest == v || dest == src) continue;
				// find number of paths from src to dest
				double path_num = paths_count(sps, sps.pred[dest]);
				// find amount of times v appears in path from src to dest
				double in_path_num = in_path_check(sps, sps.pred[dest], v);
				
				if (path_num != 0 && in_path_num != 0) {
					result+=(in_path_num)/(path_num);
				}
			}
			freeShortestPaths(sps);
		}
		// update nvs values array with betweeness centrality value
		nvs.values[v] = result;
	}
	return nvs;
}

// path counter - counts amount of paths from src to dest
static double paths_count(ShortestPaths sps, PredNode *curr) {
	// if we reached the end of the list
	if (curr == NULL) {	
		return 0;	
	}
	// if we have reached the start
	else if (curr->v == sps.src) {
		return 1 + paths_count(sps, curr->next);
	}
	else {
		return paths_count(sps, curr->next) + paths_count(sps, sps.pred[curr->v]);
	}
}

// counts amount of times vertex v is in the path from src to dest
static double in_path_check(ShortestPaths sps, PredNode *curr, int v) {
	// if we reached the end of the list
	if (curr == NULL) {
		return 0;
	}
	// if current vertex is the src vertex
	else if (curr->v == sps.src) {
		return in_path_check(sps, curr->next, v);
	}
	// if current vertex is the vertex v (then the path passes through v)
	else if (curr->v == v) {
		return paths_count(sps, sps.pred[curr->v]) + in_path_check(sps, curr->next, v);
	}
	else {
		return in_path_check(sps, curr->next, v) + in_path_check(sps, sps.pred[curr->v], v);
	}
}
//******************************************************************************

//****************NORMALISED BETWEENESS CENTRALITY FUNCTIONS********************
NodeValues betweennessCentralityNormalised(Graph g) {
	NodeValues nvs = betweennessCentrality(g);
	int num_nodes = nvs.numNodes;
	for (int i = 0; i < num_nodes; i++) {
		// update nvs values array with the normalised value
		nvs.values[i] = normal_formula(num_nodes, nvs.values[i]);
	}
	return nvs;
}

// formula to find the normalised centrality of a node and return the result
static double normal_formula(int num_nodes, double value) {
	int n = num_nodes;
	double result = 1.0*1/(n - 1)*1/(n-2);
	result = result*value;
	return result;
}

//******************************************************************************

void showNodeValues(NodeValues nvs) {
	
}

//******************************************************************************

void freeNodeValues(NodeValues nvs) {
	free(nvs.values);
}

