#include "my_header.h"

HashTable::HashTable(int size,int k,size_t d){
    this->tableSize = size;
    this->k = k; 
    this->table = vector<list<pair<int, const vector<unsigned int>*>>>(size);
    for (int i = 0; i < this->k; ++i) {
        
        int random_w =  50 + rand() % 101; // Τυχαίος αριθμός από 50 μέχρι 150
        w.push_back(random_w); 
        
        int random_t = rand() % (random_w + 1); // Τυχαίος αριθμός από 0 έως w
        t.push_back(random_t); 

        randomVectors.push_back(Functions::generateRandomVector(d));
    }
    //this->M = UINT_MAX - 5;     // for 2^32 -5
    this->M = (1 << 16) - 5;    // for 2^16 -5
    //this->M = (1 << 12) - 5;    // for 2^12 -5

    random_device rd;  
    mt19937 gen(rd());
    uniform_int_distribution<unsigned int> dis(1, UINT_MAX);
    this->Rh = dis(gen);
}

bool HashTable::addToTable(int index, const vector<unsigned int>* vec_ptr){
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

const list<pair<int, const vector<unsigned int>*>>& HashTable::getBucket(int hash) const{
    return table[hash];
}

int HashTable::hashFunction(const vector<unsigned int>* query_vec){
    // calculate g
    unsigned int resultSum = 0;
    for(int i=0; i < k; i++)    // for every r_i*h_i(p)
    {   
        // calculate h
        double dot_result = Functions::dotProduct(*query_vec, randomVectors[i]);
        unsigned int dot_normal = static_cast<unsigned int>(dot_result * dot_result);  // dot product must always be positive
        unsigned int h_result = floor((dot_normal + (this->t[i]))/static_cast<double>(this->w[i])); //h(p)
        // [r_1*h_1(p) + r_2*h_2(p)] mod M == [(r_1*h_1(p) mod M) + (r_2*h_2(p) mod M)] mod M
        unsigned int result3 = (h_result * this->Rh) % this->M; // (r_1*h_1(p) mod M)
        resultSum += result3; 
    }
    resultSum = resultSum % this->M; // the final mod M
    return resultSum % this->tableSize;
}


void HashTable::printValues() {
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
    cout << "M: " << this->M <<endl;
    cout << "Rh: " << this->Rh << endl;

}

LSH::LSH(const vector<pair<int, vector<unsigned int>*>>& dataset, int k, int l)
: K(k), L(l), vectors(dataset) {
    cout << "Initializing LSH..." << endl;

    // Initialize the Hashtables.
    size_t Dimensions = vectors[0].second->size();
    size_t numberOfImages = vectors.size();
    for (int i = 0; i < this->L; ++i) {
        HashTable* ht = new HashTable(numberOfImages/8,K,Dimensions);
        hashTables.push_back(ht);
    }

    // populate the hash tables
    initiation();

}

void LSH::print(int amount) {
    cout << "=== LSH ===" << endl;
    cout << "K: " << K << endl;
    cout << "L: " << L << endl;
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

void LSH::initiation() {
    // Pass a pointer of every vector to every hash table
    for (auto& image : vectors) {   
        for (auto& hashTable : hashTables) { 
            hashTable->addToTable(image.first, image.second); 
        }
    }
    cout << "Done Initiation. The hashtables have been populated." << endl;
}


LSH::~LSH(){
        for (auto ht : hashTables)
            delete ht;
    }

const vector<pair<int, vector<unsigned int>*>>& LSH::getVectors() const{
    return vectors;
}

set<pair<double, pair<int, const vector<unsigned int>*>>> LSH::approximateKNN(const vector<unsigned int>& query_vec, int n){
    
    set<pair<double, pair<int, const vector<unsigned int>*>>> sortedNeighbours;

    for (int i = 0; i < L; i++) { // for every hash table
        int hash = hashTables[i]->hashFunction(&query_vec);
        const auto& bucket = hashTables[i]->getBucket(hash);

        for (const auto& vec_pair : bucket) { // for every vector in the bucket
            const auto vec_indx = vec_pair.first;
            const auto vec_ptr = vec_pair.second;

            //See if they are the same
            if (*vec_ptr == query_vec) {
                continue;
            }

            double currentDistance = Functions::distance(query_vec, *vec_ptr);

            if (sortedNeighbours.size() < (unsigned int)n) {
                sortedNeighbours.insert({currentDistance, {vec_indx, vec_ptr}});
            } else {
                if (currentDistance < sortedNeighbours.rbegin()->first) {
                    sortedNeighbours.erase(--sortedNeighbours.end());
                    sortedNeighbours.insert({currentDistance, {vec_indx, vec_ptr}});
                }
            }
        }
    }
    
    return sortedNeighbours;
}

set<pair<double, pair<int, const vector<unsigned int>*>>> LSH::exhaustiveSearch(const vector<unsigned int>& query_vec, int n){
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

vector<pair<int, const vector<unsigned int>*>> LSH::approximateRangeSearch(const vector<unsigned int>& query_vec, double r){
    vector<pair<int, const vector<unsigned int>*>> rangeResult;

    for (int i=0; i<L; i++){ // for every hash table 
        // Get the vectors of the bucket
        int hash = hashTables[i]->hashFunction(&query_vec);
        const auto& bucket = hashTables[i]->getBucket(hash);
        for (const auto& vec_pair : bucket){ // for every vector in the bucket
            const auto vec_indx = vec_pair.first;
            const auto vec_ptr = vec_pair.second;
            double currentDistance = Functions::distance(query_vec, *vec_ptr);
            if (currentDistance < r) {
                //cout << "Found a vec with distance " << currentDistance << " < " << r << "(search ball)" << endl;
                rangeResult.push_back({vec_indx, vec_ptr});
            }
            
            // early return
            if (rangeResult.size() > static_cast<size_t>(20 * L)){
                return rangeResult;
            }
        }
    }

    return rangeResult;
}
