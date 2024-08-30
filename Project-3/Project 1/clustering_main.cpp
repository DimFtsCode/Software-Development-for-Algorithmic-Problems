#include "my_header.h"


int main(int argc, char *argv[]) {

    string data_file;
    string conf_file;
    string output_file;
    string method;
    int CompleteFlag = 0;

    for (int i=1; i<argc; i++){
        if (strcmp(argv[i], "-i") == 0){
            if (argc > i +1) // To avoid segmentation fault
                data_file = argv[++i];
            else {
                cerr << "You did not provide a file after  the -i flag. Try again." << endl;
                return 1;
            } 
        } else if (strcmp(argv[i], "-c") == 0){
            if (argc > i +1)
                conf_file = argv[++i];
            else {
                cerr << "You did not provide a file after the -c flag. Try again" << endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-o") == 0){
            if (argc > i +1)
                output_file = argv[++i];
            else {
                cerr << "You did not provide a file after the -o flag. Try again" << endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-m") == 0){
            if (argc > i +1){
                i++;
                if (strcmp(argv[i],"Classic") == 0 || strcmp(argv[i],"LSH") == 0 || strcmp(argv[i],"Hypercube") == 0){
                    method = argv[i];
                } else {
                    cerr << "Available methods are: Classic - LSH - Hypercube" << endl;
                }
            }
            else {
                cerr << "You did not provide a method after the -m flag. (Classic, LSH or Hypercube)" << endl;
                return 1;
            }
        }else if (strcmp(argv[i], "-complete") == 0){
            CompleteFlag = 1;
        } else{
            cout << "Unknown flag \"" << argv[i] << "\"" << endl;
            return 1;
        }

    }

    if (data_file.empty()){
        cout << "You have not provided an input file. Please give a filename: ";
        cin >> data_file;
    }
    if (conf_file.empty()){
        cout << "You have not provided a configuration file. Please give a filename: ";
        cin >> conf_file;
    }
    if (output_file.empty()){
        cout << "You have not provided an output file. Please give a filename: ";
        cin >> output_file;
    }
    if (method.empty()){
        cout << "Please provide the method (Classic - LSH - Hypercube): ";
        cin >> method; 
    }
    

    // read the dataset
    vector<pair<int, vector<unsigned int>*>> dataset;
    Functions::readImages(data_file, dataset);
    
    Clustering Cl(dataset, conf_file);
    //Cl.print();
    auto ClusterStart = chrono::high_resolution_clock::now();
    Cl.MacQueenLogic(method);
    auto ClusterEnd = chrono::high_resolution_clock::now();
    auto ClusterT = chrono::duration_cast<chrono::microseconds>(ClusterEnd - ClusterStart);
    auto ClusterTime = ClusterT.count() / 1e6;
    vector<double> Sil = Cl.averageSilhouette();
    vector<vector<unsigned int>> clusters = Cl.get_VectorsPerCluster();

    string methodDescription;

    if (method == "Classic") {
        methodDescription = "Classic";
    } else if (method == "LSH") {
        methodDescription = "Range Search LSH";
    } else if (method == "Hypercube") {
        methodDescription = "Range Search Hypercube";
    }

    vector<pair<int, vector<unsigned int>*>> centroids = Cl.getCentroids();

    ofstream output(output_file, ios_base::app);
    if (!output.is_open()) {
        cout << "File cannot be opened." << endl;
        return 1;
    }

    output << "Algorithm: " << methodDescription << endl;
    int i = 1;
    for (const auto& centroidPair : centroids) {
        output << "CLUSTER-" << i << " {size: " << clusters[i-1].size() << ", centroid: [";
        for (size_t j = 0; j < centroidPair.second->size(); ++j) {
            output << (*centroidPair.second)[j];
            if (j < centroidPair.second->size() - 1) output << ", ";
        }
        output << "]}" << endl;
        ++i;
    }
    
    output << "clustering_time: " << ClusterTime << " seconds" << endl;

    output << "Silhouette: [";
    for (size_t i = 0; i < Sil.size(); ++i) {
        output << Sil[i];
        if (i < Sil.size() - 1) {
            output << ", ";
        }
    }
    output << "]" << endl;
    

   
    if(CompleteFlag){
        int k = 1;
        for (const auto& centroidPair : centroids) {
            output << "CLUSTER-" << k << " {size: " << clusters[k-1].size() << ", centroid: [";
            for (size_t j = 0; j < centroidPair.second->size(); ++j) {
                output << (*centroidPair.second)[j];
                if (j < centroidPair.second->size() - 1) output << ", ";
            }
            output << "]}" << endl;

            output << "images: [";
            for (size_t j = 0; j < clusters[k-1].size(); ++j) {
                output << clusters[k-1][j];
                if (j < clusters[k-1].size() - 1) output << ", ";
            }
            output << "]" << endl;
            k++;

        }
        
    }
    output.close();
    //Functions::printClusters(clusters);
    //Functions::printSilhouettes(Sil);
    // Delete the space that readImages() allocated
    for (auto vec : dataset)
        delete vec.second;
               
}