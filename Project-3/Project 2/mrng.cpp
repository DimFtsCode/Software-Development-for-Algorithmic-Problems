#include "my_header.h"

MRNGraph::MRNGraph(const vector<pair<int, vector<unsigned int>*>>& dataset, int candidateSetSize, int numOfNeighbors)
    : I(candidateSetSize), N(numOfNeighbors), vectors(dataset){
        this->innerEdges.resize(vectors.size());
        cout << "Constructing MRNGraph..." << endl;
        constructGraph();
        cout << "Calculating Navigation Node..." << endl;
        vector<unsigned int> centroid = calculateCentroid();
        navigatingNode = findNNBruteForce(centroid);
        cout << "Navigation Node caluclated." << endl;
    }

MRNGraph::MRNGraph(const vector<pair<int, vector<unsigned int>*>>& dataset,const vector<pair<int ,vector<int>>>& existingGraph, int candidateSetSize, int numOfNeighbors)
    :  I(candidateSetSize), N(numOfNeighbors), vectors(dataset),innerEdges(existingGraph){
        cout << "MRNGraph initialized using existing graph." << endl;
        
        cout << "Calculating Navigation Node..." << endl;
        vector<unsigned int> centroid = calculateCentroid();
        navigatingNode = findNNBruteForce(centroid);
        cout << "Navigation Node caluclated." << endl;
    }

void MRNGraph::printEdges(){
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

const vector<pair<int, vector<int>>>& MRNGraph::getInnerEdges() const {
    return innerEdges;
}

vector<unsigned int> MRNGraph::calculateCentroid(){
    vector<unsigned int> centroid(vectors[0].second->size(), 0); // a vector to store the values of the centroid
    for (const auto & p : vectors){
        for (size_t i = 0; i< p.second->size(); i++)
            centroid[i] += (*p.second)[i];
    }
    for (size_t i = 0; i<centroid.size(); i++)
        centroid[i] /=  vectors.size();
    return centroid;
}

int MRNGraph::findNNBruteForce(const vector<unsigned int>& queryVec){
    double minDist = numeric_limits<double>::max();
    int nearestIndex = -1;

    for (const auto& p : vectors){
        double dist = Functions::distance(*p.second, queryVec);
        if (dist < minDist){
            minDist = dist;
            nearestIndex = p.first;
        }
    }
    return nearestIndex;
}

bool MRNGraph::isLongestEdge(int p, int r, int t){
    /* check if pr is the longest edge int the triangle prt
        If not, then this edge can be added*/
    
    pair<int ,int> pair_pr = make_pair(min(p, r), max(p, r));
    pair<int, int> pair_pt = make_pair(min(p, t), max(p, t));
    pair<int ,int> pair_rt = make_pair(min(r, t), max(r, t));
    double dist_pr = 0.0;
    double dist_pt = 0.0;
    double dist_rt = 0.0;

    // Get the distances from the distanceMap (pr and pt will be already calculated, rt maybe not)
    dist_pr = distanceMap[pair_pr];
    dist_pt = distanceMap[pair_pt];
    //dist_rt = distanceMap[make_pair(min(r, t), max(r, t))];
    // distance rt 
    if (distanceMap.find(pair_rt) == distanceMap.end()){
        dist_rt = Functions::distance(*vectors[r-1].second, *vectors[t-1].second);
        distanceMap[pair_rt] = dist_rt;
    } else dist_rt = distanceMap[pair_rt];
    

    // return true if pr is the longest edge
    return dist_pr > dist_pt && dist_pr > dist_rt; 
}

void MRNGraph::constructGraph(){

    for (const auto& p: vectors){  // iterate through every vector
        cout << "vec: " << p.first << endl;
        set<pair<double, int>> sortedDistances; // A set to keep the distances from vector p (Rp)

        for (const auto& q: vectors){
            if (p.first != q.first){ // to avoid distance with itself
                // make a pair of the 2 points for the distanceMap
                pair<int, int> pointPair = make_pair(min(p.first, q.first), max(p.first, q.first));
                // Calculate the distance if not already calculated
                if (distanceMap.find(pointPair) == distanceMap.end()){
                    double dist = Functions::distance(*p.second, *q.second);
                    distanceMap[pointPair] = dist;
                    sortedDistances.insert({dist,q.first});
                } else { 
                    // distance already calculated
                    sortedDistances.insert({distanceMap[pointPair],q.first});
                }
                
            }
        }

        // Initialize Lp as the set of nodes with the minimum distance to p
        set<int> Lp;
        auto it = sortedDistances.begin();
        double minDist = it->first;
        innerEdges[p.first - 1].first = p.first; // set the index of the p in the innerEdges vector.
        while (it != sortedDistances.end() && it->first == minDist){
            Lp.insert(it->second);
            // add the edge from p to this nearest neighbor
            innerEdges[p.first - 1].second.push_back(it->second);
            it++;
        }

        for (const auto& dist : sortedDistances){   
            int r = dist.second;                // For every r in Rp,
            if (Lp.find(r) == Lp.end()){        // that is not in Lp
                bool condition = true;
                for(int t : Lp){                // and for every t in Lp
                    if (isLongestEdge(p.first, r, t)){
                        condition = false;
                        break;
                    }
                }
                if(condition){
                    Lp.insert(r);   // Add r to Lp
                    innerEdges[p.first-1].second.push_back(r); // and the edge pr to graph
                }
            }
        }
    }
}

set<pair<double, int>> MRNGraph::search(const vector<unsigned int>& queryVec){
    set<pair<double, int>> R;               // Candidate set R as pairs of (distance to query, node index)
    set<int> R_indexes;                     // only the indexes 
    set<pair<double, int>> unchecked;       // Set of the unchecked nodes sorted by distance to always take the closer
    // add the navigatingNode to R
    double navigating_dist = Functions::distance(queryVec, *vectors[navigatingNode - 1].second);
    R.insert({navigating_dist, navigatingNode});
    R_indexes.insert(navigatingNode);
    unchecked.insert({navigating_dist, navigatingNode});

    int i=0;
    while (i<this->I){
        // Get the node index with the smallest distance
        auto it = unchecked.begin();
        int p = it->second;
        unchecked.erase(it); // remove it from the unchecked set (mark as checked)

        // for every neighbor n of p
        for (int n : innerEdges[p-1].second){
            // if n is not in R
            if (R_indexes.find(n) == R_indexes.end()){
                double dist = Functions::distance(queryVec, *vectors[n-1].second);
                R.insert({dist, n});
                R_indexes.insert(n);
                unchecked.insert({dist, n});
                i++;
            }
        }
    }

    // Return the first N nodes in R
    set<pair<double, int>> neighbors;
    auto it = R.begin();
    for (int i=0; i<this->N && (unsigned int)i<R.size(); i++, it++){ 
        neighbors.insert(*it);
    }

    return neighbors;
}


set<pair<double, pair<int, const vector<unsigned int>*>>> MRNGraph::exhaustiveSearch(const vector<unsigned int>& query_vec, int n){
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