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

// Σταθερές για το μέγεθος των εικόνων
const int IMAGE_SIZE = 28;
const int IMAGE_BYTES = IMAGE_SIZE * IMAGE_SIZE;

// Συνάρτηση για την αντιστροφή των byte (big-endian σε little-endian)
//uint32_t reverseInt(uint32_t i);
//vector<vector<unsigned int>> readImages(const string& filename); // not used at the moment?

//                              ***LSH***
class HashTable{
    private:
        int tableSize;                                                      // amount of buckets
        int k;                                                              // amount of h functions to use
        vector<list<pair<int, const vector<unsigned int>*>>> table;               // A vector holding the buckets, each bucket pairs of the vectors and their index
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
    LSH(const vector<pair<int, vector<unsigned int>*>>& dataset, int k = 4, int l = 5); // constructor 
    ~LSH();
    void print(int amount = 10);         // amount is for how many vectors to print. If 0 is given, print ALL    
    const vector<pair<int, vector<unsigned int>*>>& getVectors() const; // public method to get the dataset, without giving permission for changes
    set<pair<double, pair<int, const vector<unsigned int>*>>> approximateKNN(const vector<unsigned int>& query_vec, int n);         // n is for how many neighbours to find
    set<pair<double, pair<int, const vector<unsigned int>*>>> exhaustiveSearch(const vector<unsigned int>& query_vec, int n);
    vector<pair<int, const vector<unsigned int>*>> approximateRangeSearch(const vector<unsigned int>& query_vec, double r);
};

//                          ***HYPERCUBE***
class HashTableCube{
    private:
        int tableSize;
        int k;
        vector<list<pair<int, const vector<unsigned int>*>>> table;               // A vector holding the buckets, each bucket pairs of the vectors and their index
        vector<vector<double>> randomVectors;                               // (v) random vector (used in h function)
        vector<int> w;                                                      // w does not change. (used in h function)
        vector<int> t;
    public:
        HashTableCube(int k,size_t d);
        int hashFunction(const vector<unsigned int>* query_vec);
        void printValues();
        bool addToTable(int index, const vector<unsigned int>* vec_ptr);
        const list<pair<int, const vector<unsigned int>*>>& getBucket(int hash) const;
};

class Cube{
    private:
        int K;                  // The dimension of the cube
        int M;                  // max potential neighbors (images) to be checked
        int Probes;             // max probes to be checked
            
        vector<pair<int, vector<unsigned int>*>> vectors;
        HashTableCube* hashtable;
        void initiation();                   // populates the Hashtables with the vectors
        vector<int> neighboringBuckets(int index, int max_probes);  // returns the indexes (hash values) of "max_probes" neighbouring probes 

    public:
        Cube(const vector<pair<int, vector<unsigned int>*>>& dataset, int k = 14, int m = 10, int probes = 2);
        ~Cube();
        void print(int amount = 10);
        set<pair<double, pair<int, const vector<unsigned int>*>>> exhaustiveSearch(const vector<unsigned int>& query_vec, int n);
        set<pair<double, pair<int, const vector<unsigned int>*>>> approximateKNN(const vector<unsigned int>& query_vec, int n);
        vector<pair<int, const vector<unsigned int>*>> approximateRangeSearch(const vector<unsigned int>& query_vec, double r);
};

//                          ***CLUSTERING***
class Clustering{
    private:
        int K;                      //clusters (default 10) 
        int Lsh_L;
        int Lsh_K;
        int Cube_M;
        int Cube_K;
        int Cube_Probes;
        double Silhouette;
        vector<pair<int, vector<unsigned int>*>> vectors;
        vector<pair<int, vector<unsigned int>*>> vectors_original;
        vector<pair<int, vector<unsigned int>*>> centroids;
        vector<int> ClusterPerVector;                           // An int for each vector, representing the index of the cluster it belongs to
        vector<vector<unsigned int>> VectorsPerCluster;               // A collection of integers representing the images assigned to each cluster
        LSH* lsh; 
        Cube* cube;  
        void readConfig(const string& filename);                // Function to read the configuration file
        void initialize_ClustersWithVectorIDs();
        double silhouetteCoefficient(size_t i) const;           //Function for Silhouette EveryPoint
        void centroidInitiation();                              // Initializes the centroids with the KMeans++ method

        
    public:
        Clustering(const vector<pair<int, vector<unsigned int>*>>& dataset,const vector<pair<int, vector<unsigned int>*>>& dataset_original, const string& configuration_file);
        const vector<pair<int, vector<unsigned int>*>>& getCentroids() const;
        void print() const;
        void printCentroids() const;
        void lloydsAlgorithm();
        void reverseApproachAssignment(const string method);
        void MacQueenLogic(const string method);
        vector<vector<unsigned int>> get_VectorsPerCluster();
        vector<double> averageSilhouette();  //Function for average
        void reverseApproachAssignment();
};


class Functions {
private:
    static unsigned int squareDifference(double a, double b);

public:
    static uint32_t reverseInt(uint32_t num);
    static double dotProduct(const vector<unsigned int>& queryvec, const vector<double>& randomvec); 
    static double distance(const vector<unsigned int>& vec1, const vector<unsigned int>& vec2);
    static double distance1(const vector<unsigned int>& vec1, const vector<unsigned int>& vec2);            // Return the distance squared
    static vector<double> generateRandomVector(size_t d);
    static void printImage(const vector<unsigned int>& vec);                                                // Print the vector as an image
    static void readImages(const string& filename, vector<pair<int, vector<unsigned int>*>>& destination); // Read the file and put the vectors in the destination
    static void printClusters(const map<int, vector<int>>& clusters);
    static void printSilhouettes(const vector<double>& silhouettes);
};

#endif