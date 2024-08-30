#include "my_header.h"

kNNGraph::kNNGraph(const vector<pair<int, vector<unsigned int>*>>& dataset, int k,int e,int r,int n, int lsh_hash_functions, int lsh_hastables)
    : K(k),R(r),E(e),N(n),lsh(new LSH(dataset, lsh_hash_functions, lsh_hastables)),vectors(dataset){
    this->innerEdges.reserve(vectors.size());  
    //for every vector/index create the innerEdges

    cout << "Constructing kNNGraph..." << endl;
    for(const auto& pairs : vectors){
        int index = pairs.first;
        const vector<unsigned int>* vecPtr = pairs.second;
        set<pair<double, pair<int, const vector<unsigned int>*>>> resultSet = this->lsh->approximateKNN(*vecPtr, this->K);
        vector<int> neighbors;
        for (const auto& result : resultSet) {
            neighbors.push_back(result.second.first);       // keep only the indexes of the neighbors
        }

        innerEdges.push_back(make_pair(index, neighbors));
    }
    cout << "Graph initialized" << endl;
};

kNNGraph::kNNGraph(const vector<pair<int, vector<unsigned int>*>>& dataset, const vector<pair<int ,vector<int>>>& existingGraph, int k,int e,int r,int n)
: K(k), R(r), E(e), N(n), lsh(nullptr), vectors(dataset), innerEdges(existingGraph){
    cout << "kNNGraph initialized using existing graph" << endl;
}


kNNGraph::~kNNGraph() {
        delete lsh;
    }

void kNNGraph::printEdges(){
    cout << "First 10 elements:" << endl;
    for (int i = 0; i < 10; ++i) {
        cout << "Index: " << innerEdges[i].first << " - Neighbors: ";
        for (int neighbor : innerEdges[i].second) {
            cout << neighbor << " ";
        }
        cout << endl;
        cout <<"Size: " << innerEdges[i].second.size() << endl; 
    }

    cout << "\nLast 10 elements:" << endl;
    for (int i = static_cast<int>(innerEdges.size()) - 10; i < static_cast<int>(innerEdges.size()); ++i) {
        cout << "Index: " << innerEdges[i].first << " - Neighbors: ";
        for (int neighbor : innerEdges[i].second) {
            cout << neighbor << " ";
        }
        cout << endl;
        cout <<"Size: " << innerEdges[i].second.size() << endl; 
    }
};

const vector<pair<int, vector<int>>>& kNNGraph::getInnerEdges() const {
        return innerEdges;
    }

set<pair<double,int>> kNNGraph::GnnsMethod(vector<unsigned int>* queryVec){
    
    set<pair<double,int>> S;  //Set which we are using for keeping all the points that we have visited
    
    // Initialize random number generator
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, vectors.size() - 1);
    

    for(int i=0; i < this->R; i++)
    {

        int randomIndex = distrib(gen);                                             // The index of the random starting node
        //cout << "randomIndex: " << randomIndex << endl;
        vector<unsigned int>* randomVec  = this->vectors[randomIndex].second;       // get the random vector
        double distanceToQuery = Functions::distance(*randomVec,*queryVec); 
        S.insert(make_pair(distanceToQuery, randomIndex));                          // insert it to S
        
        int currentNearestIndex = randomIndex;
        int currentNearestDistance = distanceToQuery;
        int greedySteps = 0;
        for(int j=0; j < this->E; j++)
        {
            vector<int> currentNeighbors = this->innerEdges[currentNearestIndex].second;    // Get the neighbors of the random vector in the graph
            set<pair<double,int>> currentSet;                                                        //a Set for the current neighbors and their distances to queryVec
            
            for(auto neighs : currentNeighbors){
                vector<unsigned int>* neighborVec = vectors[neighs-1].second;
               if (!neighborVec) 
                    cout << "Null vector at index: " << neighs << endl;
                double distanceToQuery = Functions::distance(*neighborVec,*queryVec);

                
                currentSet.insert(make_pair(distanceToQuery,neighs));   //We put the new element in both sets
                S.insert(make_pair(distanceToQuery,neighs));            //Why do we put all the neighbors and not only the closest in S?
            }
            
            auto nearestNeighbor = *currentSet.begin();                  //find the element with smallest distance to query
            
            /*  Local optimal condtion. Without it, the results are a lot better
                Local optimal is mostly found at greedy steps 1 or 2 */
            if (currentNearestDistance < nearestNeighbor.first){
                //cout << "Found local optimal at greedy steps " << j << endl;             // the current node was the closest to the query from all its neighbors (local minimum)
                break;
            }
            
            currentNearestIndex = nearestNeighbor.second;  // else, change the current node to the new one
            currentNearestDistance =nearestNeighbor.first;
            greedySteps++;
        }
        cout << "Greedy steps: " << greedySteps << endl;
    }
    
    //remove all the elements in S we dont need
    auto iter = std::next(S.begin(), this->N);  // iterator to the (N+1) element
    S.erase(iter, S.end());                     // Erase all elements from iter to end
    return S;
}; 


set<pair<double, pair<int, const vector<unsigned int>*>>> kNNGraph::exhaustiveSearch(const vector<unsigned int>& query_vec, int n){
    set<pair<double, std::pair<int, const std::vector<unsigned int>*>>> sortedNeighbours;
    
    for (const auto& vec_pair : this->vectors){ // for every vector in the dataset
        const auto vec_indx = vec_pair.first;
        const auto vec_ptr = vec_pair.second;
        double currentDistance = Functions::distance(query_vec, *vec_ptr);

        if (sortedNeighbours.size() < (unsigned int)n) // if there aren't k neighbours yet
            // just insert the nieghbour
            sortedNeighbours.insert({currentDistance, {vec_indx, vec_ptr}});
        else {
            // Check if the currentDistance is smaller than that of our furthest neighbour
            if (currentDistance < sortedNeighbours.rbegin()->first) {
                sortedNeighbours.erase(--sortedNeighbours.end());
                sortedNeighbours.insert({currentDistance, {vec_indx, vec_ptr}});
            }
        }
    }
    return sortedNeighbours;
}