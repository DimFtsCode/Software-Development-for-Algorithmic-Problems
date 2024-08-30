#ifndef MY_HEADER_H
#define MY_HEADER_H

#include <random>
#include <iostream>
#include <cstring>
#include <cfloat>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <climits>
#include <list>
#include <set>
#include <utility>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <map>


using namespace std;

class Functions {
private:
    static unsigned int squareDifference(double a, double b);
public:
    static uint32_t reverseInt(uint32_t num);
    static double dotProduct(const vector<unsigned int>& queryvec, const vector<double>& randomvec); 
    static double distance(const vector<unsigned int>& vec1, const vector<unsigned int>& vec2);            
    static vector<double> generateRandomVector(size_t d);
    static void printImage(const vector<unsigned int>& vec);                                                    // Print the vector as an image
    static void readImages(const string& filename, vector<pair<int, vector<unsigned int>*>>& destination);      // Reads the file and puts the vectors in the destination
    static bool readInnerEdges(const string& filename, vector<pair<int, vector<int>>>& innerEdges);                     // Reads the file of an existing graph
};



//                              ***LSH***

class HashTable{
    private:
        int tableSize;                                                      // amount of buckets
        int k;                                                              // amount of h functions to use
        vector<list<pair<int, const vector<unsigned int>*>>> table;         // A vector holding the buckets, each bucket pairs of the vectors and their index
        vector<vector<double>> randomVectors;                               // (v) random vector (used in h function)
        vector<int> w;                                                      // w does not change. (used in h function)
        vector<int> t;                                                      // real number (used in h function)
        unsigned int M;                                                     // 2^32 - 5
        unsigned int Rh;                                                    // r. The multiplier of h
        // unsigned int calculate_ID(const vector<unsigned int>* query_vec);   // Gia to trick stin selida 22
    public:
        HashTable(int size,int k,size_t d);                                 // constructor
        int hashFunction(const vector<unsigned int>* query_vec);            // return the hash key
        bool addToTable(int index, const vector<unsigned int>* vec_ptr);    // public method to add a vector to the Hash table.
        const list<pair<int, const vector<unsigned int>*>>& getBucket(int hash) const; // returns the list of vectors of a bucket
        void printValues();             // do we need this
        void printTable();              // and this

};


class LSH {
private:
    int K; //number of h functions used in g
    int L; // number of HastTables
    vector<pair<int, vector<unsigned int>*>> vectors;
    vector<HashTable*> hashTables;
    void initiation();                   // populates the Hashtables with the vectors

public:
    LSH(const vector<pair<int, vector<unsigned int>*>>& dataset, int k = 10, int l = 11); // constructor 
    ~LSH();
    void print(int amount = 10);         // amount is for how many vectors to print. If 0 is given, print ALL    
    const vector<pair<int, vector<unsigned int>*>>& getVectors() const; // public method to get the dataset, without giving permission for changes
    set<pair<double, pair<int, const vector<unsigned int>*>>> approximateKNN(const vector<unsigned int>& query_vec, int n);         // n is for how many neighbours to find
    set<pair<double, pair<int, const vector<unsigned int>*>>> exhaustiveSearch(const vector<unsigned int>& query_vec, int n);
    vector<pair<int, const vector<unsigned int>*>> approximateRangeSearch(const vector<unsigned int>& query_vec, double r);
};

//                              ***kNN Graph - GNNS.CPP file***

class kNNGraph{
private:
    int K;      //neighs of each node
    int R;      //random starts of GnnsMethod
    int E;      // greedy Steps in GnnsMethod 
    int N;      //amount of neighbours to return
    LSH* lsh;
    vector<pair<int, vector<unsigned int>*>> vectors;       // pointers to the vectors of the dataset
    vector<pair<int ,vector<int>>> innerEdges;              // indexes of the nodes (vectors) and their neighbors in the graph    
public:
    // 2 constructors. 1 that constructs the graph, and 1 that takes an existing graph to save time.
    kNNGraph(const vector<pair<int, vector<unsigned int>*>>& dataset, int k,int e,int r,int n, int lsh_hash_functions = 10, int lsh_hastables = 11);
    kNNGraph(const vector<pair<int, vector<unsigned int>*>>& dataset, const vector<pair<int ,vector<int>>>& existingGraph, int k,int e,int r,int n);
    ~kNNGraph(); // destructor to handle the allocated memory for lsh
    const vector<pair<int, vector<int>>>& getInnerEdges() const;
    void printEdges();
    set<pair<double,int>> GnnsMethod(vector<unsigned int>* queryVec);
    set<pair<double, pair<int, const vector<unsigned int>*>>> exhaustiveSearch(const vector<unsigned int>& query_vec, int n);
};

//                              ***MRNGraph - mrng.cpp file***

class MRNGraph{
private:
    int I;                                                          // candidate set size
    int N;                                                          // amount of neighbors to return
    int navigatingNode;                                             // the index of the Navigating Node
    vector<pair<int, vector<unsigned int>*>> vectors;               // pointers to the vectors of the dataset
    vector<pair<int ,vector<int>>> innerEdges;                      // indexes of the nodes (vectors) and their neighbors in the graph 
    map<pair<int, int>, double> distanceMap;                        // Store here all the pre-caluclated distances
    void constructGraph();                                          // method to construct the MRNG edges
    void constructGraph1();
    vector<unsigned int> calculateCentroid();                       // method to calculate the centroid of the dataset
    bool isLongestEdge(int p, int r, int t);                        // method that decides if edge pr is the longest in the triangle prt 
    int findNNBruteForce(const vector<unsigned int>& queryVec);     // method to find the nearest vector (for the Navigating Node)
public:
    // 2 constructors. 1 that constructs the graph, and 1 that takes an existing graph to save time.
    MRNGraph(const vector<pair<int, vector<unsigned int>*>>& dataset, int candidateSetSize = 20, int numOfNeighbors = 1);
    MRNGraph(const vector<pair<int, vector<unsigned int>*>>& dataset,const vector<pair<int ,vector<int>>>& existingGraph, int candidateSetSize = 20, int numOfNeighbors = 1);
    void printEdges();
    const vector<pair<int, vector<int>>>& getInnerEdges() const;
    set<pair<double, int>> search(const vector<unsigned int>& queryVec);       // The generic search-on-graph algorithm
    set<pair<double, pair<int, const vector<unsigned int>*>>> exhaustiveSearch(const vector<unsigned int>& query_vec, int n);
};

#endif