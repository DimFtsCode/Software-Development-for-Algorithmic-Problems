#include "my_header.h"

Clustering::Clustering(const vector<pair<int, vector<unsigned int>*>>& dataset,const string& configuration_file)
    : vectors(dataset)
{
    // Set the default values
    this->K = -1; // This is mandatory in the cluster.config, has to be changed
    this->Lsh_L = 3;
    this->Lsh_K = 4;
    this->Cube_M = 10;
    this->Cube_K = 3;
    this->Cube_Probes = 2;
    this->ClusterPerVector = vector<int>(vectors.size(), -1);
    // read the configuration_file
    readConfig(configuration_file);  
    this->VectorsPerCluster = vector<vector<unsigned int>>(this->K); 
    centroidInitiation();
    cout << "Clustering started." << endl;
}

const vector<pair<int, vector<unsigned int>*>>& Clustering::getCentroids() const {
    return centroids;
}

void Clustering::initialize_ClustersWithVectorIDs(){
    for(size_t j = 0; j < ClusterPerVector.size(); j++) {
        VectorsPerCluster[ClusterPerVector[j]].push_back(j);
    }
}

vector<vector<unsigned int>> Clustering::get_VectorsPerCluster(){
    return VectorsPerCluster;
}

void Clustering::readConfig(const string& filename){
    ifstream configfile(filename);
    string line;

    while (getline(configfile, line)){
        istringstream iss(line);
        string key;
        int value;
        if (!(iss >> key >> value))
            continue; // To skip invalid lines

        if (key == "number_of_clusters:")
            this->K = value;
        else if (key == "number_of_vector_hash_tables:")
            this->Lsh_L = value;
        else if (key == "number_of_vector_hash_functions:")
            this->Lsh_K = value;
        else if (key == "max_number_M_hypercube:")
            this->Cube_M = value;
        else if (key == "number_of_hypercube_dimensions:")
            this->Cube_K = value;
        else if (key == "number_of_probes:"){
            this->Cube_Probes = value;
        }
    }

    if (this->K == -1){
        cerr << "number_of_clusters is a mandatory parameter in the config file!" << endl;
        exit(1);
    }
    configfile.close();
}

 void Clustering::print() const {
    std::cout << "K: " << K << std::endl;
    std::cout << "Lsh_L: " << Lsh_L << std::endl;
    std::cout << "Lsh_K: " << Lsh_K << std::endl;
    std::cout << "Cube_M: " << Cube_M << std::endl;
    std::cout << "Cube_K: " << Cube_K << std::endl;
    std::cout << "Cube_Probes: " << Cube_Probes << std::endl;
}


void Clustering::centroidInitiation()
{   
    cout << "Initializing centroids.." << endl;
    int vectorSize = vectors.size();

    int firstCentroidIdx = rand() % vectorSize;
    centroids.push_back(vectors[firstCentroidIdx]);
    cout << "Centroid 1 found." << endl;

    for(int c=1; c < K; c++)
    {   
        vector<double> dists(vectorSize);
        double totalSquaredDist = 0.0; // Συνολική τετραγωνική απόσταση

        for (int i = 0; i < vectorSize; i++) {
            dists[i] = std::numeric_limits<double>::max();

            for (int j = 0; j < c; j++) {
                double tempDist = Functions::distance1(*vectors[i].second,*centroids[j].second);
                if (tempDist < dists[i]) {
                    dists[i] = tempDist;
                }
            }
            totalSquaredDist += dists[i] * dists[i]; // Προσθέτουμε το τετράγωνο της απόστασης στη συνολική τετραγωνική απόσταση
        }

        double rnd = ((double)rand() / RAND_MAX) * totalSquaredDist; // Χρησιμοποιούμε τη συνολική τετραγωνική απόσταση για την τυχαία επιλογή
        for (int i = 0; i < vectorSize; i++) {
            if (rnd <= dists[i] * dists[i]) { // Συγκρίνουμε με το τετράγωνο της απόστασης
                centroids.push_back(vectors[i]);
                break;
            }
            rnd -= dists[i] * dists[i]; // Αφαιρούμε το τετράγωνο της απόστασης
        }
        cout << "Centroid " << c+1 << " found." << endl;
    }
}


void Clustering::printCentroids() const {
    for (const auto& centroid : centroids) {
        cout << "ID: " << centroid.first << endl;
        Functions::printImage(*centroid.second);
        cout << "----------------------------------" << endl; // Διαχωριστική γραμμή μεταξύ των κεντροειδών
    }
}


void Clustering::lloydsAlgorithm() {
    cout << "Loyd's algorithm starting" << endl;
    int imagesChanged = INT_MAX;
    int iter = 0;
    /* Loop until convergence: 
        Consider convergence when less that 1 % of the dataset, (600 images) have changed cluster*/
    while(imagesChanged > vectors.size() * 0.01) {
        imagesChanged = 0;
        //1: Assign each vector to the nearest centroid
        vector<vector<pair<int, vector<unsigned int>*>>> assignments(centroids.size());
        
        for(size_t vecIdx = 0; vecIdx < vectors.size(); vecIdx++) {
            const auto& vec = vectors[vecIdx];
            double minDist = std::numeric_limits<double>::max();
            int nearestCentroidIndex = -1;

            for(size_t i = 0; i < centroids.size(); i++) {
                double dist = Functions::distance1(*vec.second, *centroids[i].second);
                
                if(dist < minDist) {
                    minDist = dist;
                    nearestCentroidIndex = i;
                }
            }

            assignments[nearestCentroidIndex].push_back(vec);

            // check if the assignment has changed
            if (ClusterPerVector[vecIdx] != nearestCentroidIndex){
                ClusterPerVector[vecIdx] = nearestCentroidIndex;
                imagesChanged++; // An image has changed cluster
            }
        }

        //2: Update each centroid to be the average of the vectors assigned to it
        for(size_t i = 0; i < centroids.size(); i++) {
            vector<unsigned int> newCentroid(centroids[i].second->size(), 0);
            
            for(const auto& vec : assignments[i]) {
                for(size_t j = 0; j < vec.second->size(); j++) {
                    newCentroid[j] += (*vec.second)[j];
                }
            }

            if (assignments[i].size() > 0){ // To avoid division by zero
                for(size_t j = 0; j < newCentroid.size(); j++) {
                    newCentroid[j] /= assignments[i].size();
                }
            }

            for (size_t j=0; j < centroids[i].second->size(); j++) // update the centroids
                (*centroids[i].second)[j] = newCentroid[j];
        }
        cout << "Loyd's iterations: " << ++iter << ", images re-arranged: " << imagesChanged <<  endl;
    }
}

void Clustering::reverseApproachAssignment(const string method){
    cout << "reverseApproachAssignment starting.." << endl;
    //1. Calculate the initial radius as half the minimum distance between centroids
    double initial_radius = DBL_MAX;
    for (int i = 0; i < K - 1; i++) {
        for (int j = i + 1; j < K; j++) {
            double dist = Functions::distance(*centroids[i].second, *centroids[j].second);
            initial_radius = std::min(initial_radius, dist);
        }
    }
    initial_radius /= 2;
    cout << "Initial radius in reverse approach: " << initial_radius << endl;
    //2. Assign vectors to each centroid 
    int doublesNumber = 0;                  // To count the times that the balls have been doubled
    int firstTimeAssignments = 0;           // To count how many vectors that had no centroid get assigned
    int totalAssignments = 0;               // To count the total assignments that have happened
    int threshold = vectors.size() * 0.8;   // If more than 80% of the points have been assigned, stop
    int MaxIteration = 40;                  // Set a maximum of total iterations
    while (MaxIteration){
        int pointsThatChangeCluster = 0;
        for (int i = 0; i < K; i++) {

            vector<pair<int, const vector<unsigned int>*>> nearbyPoints;
            if (method == "LSH")
                nearbyPoints = lsh->approximateRangeSearch(*centroids[i].second, initial_radius);
            else if (method == "Hypercube")
                nearbyPoints = cube->approximateRangeSearch(*centroids[i].second, initial_radius);
            // cout << "count of the points returned by approximateRangeSearch: " << nearbyPoints.size() << endl;
            for (auto& point : nearbyPoints) { // for every nearby Point
                int vectorIndex = point.first;
                // if the point is not assigned to any centroids, or if this centroid is closer, assign it now
                if (ClusterPerVector[vectorIndex] == -1 ||
                    Functions::distance(*centroids[ClusterPerVector[vectorIndex]].second, *point.second) > Functions::distance(*centroids[i].second, *point.second)){
                    if (ClusterPerVector[vectorIndex] != i){        // If the point is not already assigned to the same centroid
                        if (ClusterPerVector[vectorIndex] == -1){    
                            firstTimeAssignments++;
                            totalAssignments++;
                        } else pointsThatChangeCluster++;

                        ClusterPerVector[vectorIndex] = i;
                    }
                }        
            }
        }

        cout << "first time assignments: " << firstTimeAssignments <<", total assignments: " << totalAssignments << ". changed cluster: " << pointsThatChangeCluster <<  endl;
        
        // Exit if no new points get assigned, or less than 0.01 of the dataset has remained unassigned
        if (/*firstTimeAssignments == 0 ||*/ totalAssignments > threshold ){ 
            break;
        }

        // Update each centroid to be the average of the vectors assigned to it
        for (int i = 0; i < K; i++) {
            vector<unsigned int> sum(vectors[0].second->size(), 0);
            int pointsInCluster = 0;
            for (size_t j = 0; j < vectors.size(); j++) {
                if (ClusterPerVector[j] == i) {
                    pointsInCluster++;
                    for (size_t dim = 0; dim < vectors[j].second->size(); dim++) {
                        sum[dim] += (*vectors[j].second)[dim];
                    }
                }
            }
            // Update the centroid if it has assigned points
            if (pointsInCluster > 0) {
                for (size_t dim = 0; dim < sum.size(); dim++) {
                    (*centroids[i].second)[dim] = sum[dim] / pointsInCluster;
                }
            }
        }


        initial_radius *= 2;
        doublesNumber++;
        firstTimeAssignments = 0;
        //cout << "Centroid range doubled " << doublesNumber << " times, now is " << initial_radius << endl;
        MaxIteration--;
    }

    

    //3. For every unassigned point, assign it to the closet centroid
    int unassignedPoints = 0;
    for (size_t i = 0; i < vectors.size(); i++) {
        if (ClusterPerVector[i] == -1) {
            unassignedPoints++;
            double min_distance = DBL_MAX;
            int closest_cluster = -1;

            for (int j = 0; j < K; j++) {
                double dist = Functions::distance(*vectors[i].second, *centroids[j].second);
                if (dist < min_distance) {
                    min_distance = dist;
                    closest_cluster = j;
                }
            }

            ClusterPerVector[i] = closest_cluster;
        }
    }
    cout << "Unassigned points after using " << method << " range search: " << unassignedPoints << endl;

}

void Clustering::MacQueenLogic(const string method){


    if (method == "LSH"){
        // Initiate an lsh instance
        lsh = new LSH(vectors, Lsh_K, Lsh_L);
        // Run the assignment algorithm
        reverseApproachAssignment(method);
        // destroy the LSH instance
        delete lsh;
    } else if (method == "Hypercube"){
        // Initiate a hypercube instance
        cube = new Cube(vectors, Cube_K, Cube_M, Cube_Probes);
        // Run the assignment algorithm
        reverseApproachAssignment(method);
        // destroy the cube instance
        delete cube;
    } else if (method == "Classic"){
        lloydsAlgorithm();
    } else {
        cerr << "Wrong method input." << endl;
        exit(1);
    }
}

double Clustering::silhouetteCoefficient(size_t i) const {
    double a_i = 0.0;
    double b_i = numeric_limits<double>::max();
    size_t cluster_i = ClusterPerVector[i];


    //find the distances in the same cluster
    int sameClusterCount = 0;
    for (size_t idx : VectorsPerCluster[cluster_i]) {
        if (i == idx) continue;

        double dist = Functions::distance(*vectors[i].second, *vectors[idx].second);
        a_i += dist;
        sameClusterCount++;
    }
    a_i /= sameClusterCount;


    //find the nearest cluster
    int closestCluster = -1;
    double minClusterDist = numeric_limits<double>::max();
    for (size_t idx = 0; idx < centroids.size(); idx++) {
        if(idx == cluster_i) continue;
        double dist = Functions::distance(*vectors[i].second, *centroids[idx].second);
        if (dist < minClusterDist) {
            minClusterDist = dist;
            closestCluster = idx;
        }
    }


    //find the distances between our points and nearest cluster points
    double sumDistancesToClosestCluster = 0.0;
    for (size_t idx : VectorsPerCluster[closestCluster]) {
        double dist = Functions::distance(*vectors[i].second, *vectors[idx].second);
        sumDistancesToClosestCluster += dist;
    }

    b_i = sumDistancesToClosestCluster / VectorsPerCluster[closestCluster].size();

    //return the proper function
    return (b_i - a_i) / max(a_i, b_i); 
}


//calculate the above for every function
vector<double> Clustering::averageSilhouette(){
    cout << "AverageSilhouette started. " << endl;
    initialize_ClustersWithVectorIDs();

    vector<double> silhouettes;
    double sum = 0.0;

    for (size_t cluster_indx = 0; cluster_indx < (unsigned int)this->K; cluster_indx++){  // for every cluster
        cout << "Calculating for cluster " << cluster_indx << endl;
        double clusterSilhouette = 0.0;
        for (size_t i = 0; i < VectorsPerCluster[cluster_indx].size(); i++){    // iterate through the vectors of that cluster and calculate the silhouette
            double silhouetteVal = silhouetteCoefficient(VectorsPerCluster[cluster_indx][i]);
            clusterSilhouette += silhouetteVal;
            sum += silhouetteVal;
        }
        clusterSilhouette = clusterSilhouette / VectorsPerCluster[cluster_indx].size();
        silhouettes.push_back(clusterSilhouette);
        
    }
    /*
    for (size_t i = 0; i < vectors.size(); i++) {
        double silhouetteVal = silhouetteCoefficient(i);
        silhouettes.push_back(silhouetteVal);
        sum += silhouetteVal;
    }
    */
    double average = sum / vectors.size(); // The average of all points
    silhouettes.push_back(average);

    return silhouettes;
}



        
