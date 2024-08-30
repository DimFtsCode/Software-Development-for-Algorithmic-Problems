#include "my_header.h"

#define QUERY_VECS_NUM 10   // Put here the number of query vectors to be examined





int main(int argc, char *argv[]) {
    string dataset_file ="dataset";
    string datasetRed_file ="dataset_reduced";
    string query_file = "test_set";
    string queryRed_file = "queryset_reduced";

    string output_file = "Combined_cube_output.txt";
    int k = 10;                           
    int M = 100;
    int Probes = 5;  
    int N = 1;              // amount of NN to search
    
    vector<pair<int, vector<unsigned int>*>> dataset; // contruct Lsh for dataset  
    Functions::readImages(dataset_file, dataset);
    Cube cube(dataset,k,M,Probes);

    vector<pair<int, vector<unsigned int>*>> datasetRed; // contruct Lsh for dataset_Red 
    Functions::readImages(datasetRed_file, datasetRed);
    Cube cubeRed(datasetRed,k,M,Probes);
    
    vector<pair<int, vector<unsigned int>*>> query_vecs; // read query file
    Functions::readImages(query_file, query_vecs);
    
    vector<pair<int, vector<unsigned int>*>> queryRed_vecs; // read queryReduced file
    Functions::readImages(queryRed_file, queryRed_vecs);

    ofstream output(output_file, ios_base::app);
    if (!output.is_open()) {
        cout << "File cannot be opened." << endl;
        return 1;
    }

    // unsigned int testImg = -1;  
    // double maxRatio[QUERY_VECS_NUM]; 
    for (size_t i=0; i< QUERY_VECS_NUM; i++){ // for every image in the test set  
        auto CUBEstart = chrono::high_resolution_clock::now();// 
        auto approximateNeighbours = cube.approximateKNN(*query_vecs[i].second, N);
        auto CUBEend = chrono::high_resolution_clock::now();
        auto CUBEtime = chrono::duration_cast<chrono::microseconds>(CUBEend - CUBEstart);
        double tCUBE = CUBEtime.count() / 1e6; // convert microseconds to seconds
        auto shortest_range =*(approximateNeighbours.begin());
        int RealDistance = shortest_range.first;
        // cout << "Lsh shortest Range in Real dimension: " << smlRange << endl;
        // auto Check = dataset[0].first;

       

        auto CUBEstartRed = chrono::high_resolution_clock::now();
        auto approximateNeighboursRed = cubeRed.approximateKNN(*queryRed_vecs[i].second, N);
        auto CUBEendRed = chrono::high_resolution_clock::now();
        auto CUBEtimeRed = chrono::duration_cast<chrono::microseconds>(CUBEendRed - CUBEstartRed);
        double tCUBERed = CUBEtimeRed.count() / 1e6;
        auto smallest_element =*(approximateNeighboursRed.begin());
       
        int vecIndex  = smallest_element.second.first; //find the Index
        //The first element of the matrix has index 1 so for matrix we use index -1
        // cout << "Index: " << vecIndex << endl;

        double ReducedRealDistance = Functions::distance(*query_vecs[i].second,*dataset[vecIndex-1].second);
        double factor = (ReducedRealDistance / RealDistance);

        // cout << "Real Distance Between reduced solution and vector: " << RedRealDistance << endl;
        output << "Query: " << i+1 << endl;
        output << "tCUBE: " << tCUBE << "    ||" << " tLCUBE reduced: " << tCUBERed << endl; 
        output << "DistanceCUBE: "<< RealDistance << " ||" << " DistanceCUBE Reduced: " << ReducedRealDistance << endl;
        output << "Μεσο Κλασμα Προσεγγισης: " <<factor << endl; 
    }

    output.close();

    
}     
