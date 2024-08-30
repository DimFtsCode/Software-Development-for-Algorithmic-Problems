#include "my_header.h"

#define QUERY_VECS_NUM 10   // Put here the number of query vectors to be examined





int main(int argc, char *argv[]) {
    string dataset_file ="dataset";
    string datasetRed_file ="dataset_reduced";
    string query_file = "test_set";
    string queryRed_file = "queryset_reduced";

    string output_file = "Combined_lsh_output.txt";
    int k = 10;              // amount of h_i functions
    int L = 11;              // amount of Hash Tables
    int N = 1;              // amount of NN to search

    
    vector<pair<int, vector<unsigned int>*>> dataset; // contruct Lsh for dataset  
    Functions::readImages(dataset_file, dataset);
    LSH lsh(dataset, k, L);

    vector<pair<int, vector<unsigned int>*>> datasetRed; // contruct Lsh for dataset_Red 
    Functions::readImages(datasetRed_file, datasetRed);
    LSH lshRed(datasetRed, k, L);
    
    vector<pair<int, vector<unsigned int>*>> query_vecs; // read query file
    Functions::readImages(query_file, query_vecs);
    
    vector<pair<int, vector<unsigned int>*>> queryRed_vecs; // read queryReduced file
    Functions::readImages(queryRed_file, queryRed_vecs);

    ofstream output(output_file, ios_base::app);
    if (!output.is_open()) {
        cout << "File cannot be opened." << endl;
        return 1;
    }
    output << "LSH algorithm with k=" << k << " and L=" << L << endl;
    output << endl;
    output <<"#####################################################################################" << endl;
    output << endl;
    double TotalFactor = 0;
    // unsigned int testImg = -1;  
    // double maxRatio[QUERY_VECS_NUM]; 
    for (size_t i=0; i< QUERY_VECS_NUM; i++){ // for every image in the test set  
        auto LSHstart = chrono::high_resolution_clock::now();// 
        auto approximateNeighbours = lsh.approximateKNN(*query_vecs[i].second, N);
        auto LSHend = chrono::high_resolution_clock::now();
        auto LSHtime = chrono::duration_cast<chrono::microseconds>(LSHend - LSHstart);
        double tLSH = LSHtime.count() / 1e6; // convert microseconds to seconds
         auto shortest_range =*(approximateNeighbours.begin());
        int RealDistance = shortest_range.first;
        // cout << "Lsh shortest Range in Real dimension: " << smlRange << endl;
        // auto Check = dataset[0].first;

       

        auto LSHstartRed = chrono::high_resolution_clock::now();
        auto approximateNeighboursRed = lshRed.approximateKNN(*queryRed_vecs[i].second, N);
        auto LSHendRed = chrono::high_resolution_clock::now();
        auto LSHtimeRed = chrono::duration_cast<chrono::microseconds>(LSHendRed - LSHstartRed);
        double tLSHRed = LSHtimeRed.count() / 1e6;
        auto smallest_element =*(approximateNeighboursRed.begin());
       
        int vecIndex  = smallest_element.second.first; //find the Index
        //The first element of the matrix has index 1 so for matrix we use index -1
        // cout << "Index: " << vecIndex << endl;

        double ReducedRealDistance = Functions::distance(*query_vecs[i].second,*dataset[vecIndex-1].second);
        double factor = (ReducedRealDistance / RealDistance);

        // cout << "Real Distance Between reduced solution and vector: " << RedRealDistance << endl;                output << "--------------------------------------------------------------------" << endl;
        output << "--------------------------------------------------------------------" << endl;
        output << "Query: " << i+1 << endl;
        output << "tLSH: " << tLSH << "    ||" << " tLSH reduced: " << tLSHRed << endl; 
        output << "DistanceLSH: "<< RealDistance << " ||" << " DistanceLSH Reduced: " << ReducedRealDistance << endl;
        output << "Κλασμα Προσεγγισης: " <<factor << endl; 
        TotalFactor = TotalFactor + factor;
    }
    double TheTotalFactor = TotalFactor / 10;
    output << "Μεσο Κλασμα Προσεγγισης: " << TheTotalFactor << endl;
    output.close();

    
}     
