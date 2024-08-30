#include "my_header.h"


HashTableCube::HashTableCube(int k,size_t d){
    this->k = k;
    this->tableSize = (1 << this->k);
    this->table = vector<list<pair<int, const vector<unsigned int>*>>>(this->tableSize);
    for (int i = 0; i < this->k; ++i) {
        
        int random_w =  50 + rand() % 101; // Τυχαίος αριθμός από 50 μέχρι 150
        w.push_back(random_w); // na dokimasoume kai alles times px 2-100
        
        int random_t = rand() % (random_w + 1); // Τυχαίος αριθμός από 0 έως w
        t.push_back(random_t); // na dokimasoume kai alles times

        randomVectors.push_back(Functions::generateRandomVector(d));
    }

}

int HashTableCube::hashFunction(const vector<unsigned int>* query_vec){
    int resultSum = 0;
    int B = 1 << 16; //2^16
    for(int i=0; i < this->k; i++)
    {
        double dot_result = Functions::dotProduct(*query_vec, randomVectors[i]);
        unsigned int dot_normal = static_cast<unsigned int>(dot_result * dot_result);
        unsigned int h_result = floor((dot_normal + (this->t[i]))/static_cast<double>(this->w[i]));
        //cout << "h_result: " <<h_result <<endl;
        int h1_result = h_result % B; // h1 mod B
        //cout << "h1_result: " <<h1_result <<endl;
        double h_normalized = static_cast<double>(h1_result) / B; // (h1 - a) / (b -a) a = 0
        //cout << "h_normalized: " <<h_normalized <<endl;
        int h_norm_round = round(h_normalized); // h_normalized rounded to 0 or 1
        //cout << "h_norm_round: " << h_norm_round <<endl;
        resultSum = resultSum + ((1<<i)*h_norm_round); // creating the sum for example if i = 2 and h_norm 1 we add 4 to sum
    }
    return resultSum;
}

bool HashTableCube::addToTable(int index, const vector<unsigned int>* vec_ptr){
    // Get the hash key..
    int hash = hashFunction(vec_ptr);
    // Push the vector to the hash table
    if (hash>=0 && hash < this->tableSize){
        table[hash].push_back({index, vec_ptr});
        return true;
    } else {
        cerr << "Invalid hash for table: " << hash << endl;
        return false;
    }
}

const list<pair<int, const vector<unsigned int>*>>& HashTableCube::getBucket(int hash) const{
    return table[hash];
}

void HashTableCube::printValues() {
    cout << "size: " << tableSize << endl;
    cout << "k: " << k << endl;
    cout << "Random Vectors:" << endl;
    for (size_t i = 0; i < randomVectors.size(); ++i) {
        for (size_t j = 0; j < randomVectors[i].size(); ++j) {
            cout << randomVectors[i][j] << " ";
        }
        cout << endl;
    }

    cout << "W values:" << endl;
    for (int val : w) {
        cout << val << " ";
    }
    cout << endl;

    cout << "T values:" << endl;
    for (int val : t) {
        cout << val << " ";
    }
    cout << endl;
    

}

Cube::Cube(const vector<pair<int, vector<unsigned int>*>>& dataset, int k, int m, int probes)
: K(k), M(m), Probes(probes),  vectors(dataset){
    cout << "Initializing Hypercube..." << endl;
    
    // Initialize the hashtable
    size_t Dimensions = vectors[0].second->size();
    this->hashtable = new HashTableCube(K,Dimensions);
    // populate the hash tables
    initiation();
    
}
Cube::~Cube(){
        delete this->hashtable;
}


void Cube::print(int amount) {
    cout << "=== LSH ===" << endl;
    cout << "K: " << K << endl;
    cout << "L: " << M << endl;
    cout << "Probes: " << Probes << endl;
    cout << "Number of Vectors: " << vectors.size() << endl;
    
    size_t numOfImages;
    if (amount < 0){
        cout << "Wrong input to print." << endl;
        return;
    }
    else if (amount == 0) // 0 is given, print ALL
        numOfImages = this->vectors.size();
    else 
        numOfImages = amount;

    for (size_t i = 0; i < numOfImages; i++){
        cout << "Image " << i + 1 << ":\n";
        Functions::printImage(*vectors[i].second);
        cout << "-------------\n";  // Line between images
    }
    
}


//Sunartisi pou epistrefei to index me ta geitonika Buckets
vector<int> Cube::neighboringBuckets(int index, int max_probes) {
    vector<int> neighbouring_probes;
    neighbouring_probes.push_back(index); 
    for (int i = 0; i < K; i++) {
        int flippedIndex = index ^ (1 << i);
        neighbouring_probes.push_back(flippedIndex);
        if (neighbouring_probes.size() >= (unsigned int)max_probes)
            break;
    }
    return neighbouring_probes;
}

 void Cube::initiation() {
    // Pass a pointer of every vector to every hash table
    for (auto& image : vectors) {   
        
        this->hashtable->addToTable(image.first, image.second); 
        
    }
    cout << "Done Initiation. The hashtable has been populated." << endl;
}

set<pair<double, pair<int, const vector<unsigned int>*>>> Cube::exhaustiveSearch(const vector<unsigned int>& query_vec, int n){
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


set<pair<double, pair<int, const vector<unsigned int>*>>> Cube::approximateKNN(const vector<unsigned int>& query_vec, int n){    
    set<pair<double, pair<int, const vector<unsigned int>*>>> nearestNeighbours;

    int current_probe = hashtable->hashFunction(&query_vec);
    int neighbors_checked = 0;
    int probes_checked = 0;

    // Get the indexes of the neighbouring probes
    vector<int> neighborProbes = neighboringBuckets(current_probe, M);
    for (int probe : neighborProbes){ // for every probe (First probe being the current probe)
        // Get the vectors in that probe
        const auto& probe_vecs = hashtable->getBucket(probe);

        for (const auto& candidate : probe_vecs){   // for every vector in the probe
            const auto vec_indx = candidate.first;
            const auto vec_ptr = candidate.second;
            double currentDistance = Functions::distance(query_vec, *vec_ptr);
            if (nearestNeighbours.size() < (unsigned int)n){ // if there aren't n neighbours yet
                // just insert the neighbour
                nearestNeighbours.insert({currentDistance, {vec_indx, vec_ptr}});
            } else {
                // We check if the currentDistance is smaller than that of our furthest neighbour
                if (currentDistance < nearestNeighbours.rbegin()->first) { // ->first brings the first element of the pair (dist,vec)
                    nearestNeighbours.erase(--nearestNeighbours.end());
                    nearestNeighbours.insert({currentDistance, {vec_indx, vec_ptr}});
                }
            }
            neighbors_checked++;
            if (neighbors_checked >= M)
                return nearestNeighbours;           
        }
        probes_checked++;
        if (probes_checked >= Probes)
            return nearestNeighbours;
    }
    return nearestNeighbours;
}


vector<pair<int, const vector<unsigned int>*>> Cube::approximateRangeSearch(const vector<unsigned int>& query_vec, double r){
    vector<pair<int, const vector<unsigned int>*>> rangeResult;

    int current_probe = hashtable->hashFunction(&query_vec);
    int neighbors_checked = 0;
    int probes_checked = 0;

    // Get the indexes of the neighbouring probes
    vector<int> neighborProbes = neighboringBuckets(current_probe, M);
    for (int probe : neighborProbes){ // for every probe (First probe being the current probe)
        // Get the vectors in that probe
        const auto& probe_vecs = hashtable->getBucket(probe);

        for (const auto& candidate : probe_vecs){   // for every vector in the probe
            const auto vec_indx = candidate.first;
            const auto vec_ptr = candidate.second;
            double currentDistance = Functions::distance(query_vec, *vec_ptr);
            
            if (currentDistance < r)
                rangeResult.push_back({vec_indx, vec_ptr});

            neighbors_checked++;
            if (neighbors_checked >= M)
                return rangeResult;           
        }
        probes_checked++;
        if (probes_checked >= Probes)
            return rangeResult;
    }
    return rangeResult;
    
}