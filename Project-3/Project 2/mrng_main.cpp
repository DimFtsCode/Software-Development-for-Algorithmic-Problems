#include "my_header.h"
using namespace std::chrono;
#define QUERY_VECS_NUM 10 

int main(int argc, char* argv[]){
    string dataset_file ="dataset";
    string datasetRed_file ="dataset_reduced";
    string query_file = "test_set";
    string queryRed_file = "queryset_reduced";

    string graph_file = "MRNGraph_file";
    string grahp_file_reduced = "MRNGraph_reduced";

    string output_file = "Combined_MRNG_output";

    // int K = 50;
    // int E = 100;
    // int R = 5;
    int I = 20;
    int N = 10;
    // duration<double> searchDuration(0);
    // duration<double> searchDurationREd(0);

    vector<pair<int, vector<unsigned int>*>> dataset; // contruct Lsh for dataset  
    Functions::readImages(dataset_file, dataset);
    
    vector<pair<int, vector<unsigned int>*>> datasetRed; // contruct Lsh for dataset_Red 
    Functions::readImages(datasetRed_file, datasetRed);

    vector<pair<int, vector<unsigned int>*>> query_vecs; // read query file
    Functions::readImages(query_file, query_vecs);

    vector<pair<int, vector<unsigned int>*>> queryRed_vecs; // read queryReduced file
    Functions::readImages(queryRed_file, queryRed_vecs);

    MRNGraph* mrng;
    vector<pair<int, vector<int>>> existingGraph;
    Functions::readInnerEdges(graph_file, existingGraph);
    mrng = new MRNGraph(query_vecs, existingGraph, I, N);
    MRNGraph* mrng_reduced;
    vector<pair<int, vector<int>>> existingGraphRed;
    Functions::readInnerEdges(grahp_file_reduced, existingGraphRed);
    mrng_reduced = new MRNGraph(queryRed_vecs, existingGraphRed, I, N);


    ofstream output(output_file, ios_base::app);
    if (!output.is_open()) {
        cout << "File cannot be opened." << endl;
        return 1;
    }
    output << "MRNG algorithm with I=" << I << " and N=" << N << endl;
    output << endl;
    output <<"#####################################################################################" << endl;
    output << endl;
    double TotalFactor = 0;
    for (int i = 0; i < QUERY_VECS_NUM; ++i) {
        output << "--------------------------------------------------------------------" << endl;
        output << "Query: " << i+1 << endl;

        
        set<pair<double, int>> graph_results;
        auto MRNGstart = chrono::high_resolution_clock::now();// 
        graph_results = mrng->search(*dataset[i].second);
        auto MRNGend = chrono::high_resolution_clock::now();
        auto MRNGtime = chrono::duration_cast<chrono::microseconds>(MRNGend - MRNGstart);
        double tMRNG = MRNGtime.count() / 1e6;
        auto smallest_pair = *(graph_results.begin());
        int RealDistance = smallest_pair.first;

        set<pair<double, int>> graph_results_reduced;
        auto MRNGstartRed = chrono::high_resolution_clock::now();
        graph_results_reduced = mrng_reduced->search(*datasetRed[i].second);
        auto MRNGendRed = chrono::high_resolution_clock::now();
        auto MRNGtimeRed = chrono::duration_cast<chrono::microseconds>(MRNGendRed - MRNGstartRed);
        double tMRNGred = MRNGtimeRed.count() / 1e6;
        auto smallest_pair_reduced = *(graph_results_reduced.begin());
        auto smallest_pair_index = smallest_pair_reduced.second;
        double ReducedRealDistance = Functions::distance(*query_vecs[i].second,*dataset[smallest_pair_index-1].second);
        double factor = (ReducedRealDistance / RealDistance);


        // output << "--------------------------------------------------------------------" << endl;
        // output << "Query: " << i+1 << endl;
        output << "tMRNG: " << tMRNG << "    ||" << " tMRNG reduced: " << tMRNGred << endl; 
        output << "DistanceMRNG: "<< RealDistance << " ||" << " DistanceMRNG Reduced: " << ReducedRealDistance << endl;
        output << "Κλασμα Προσεγγισης: " <<factor << endl; 
        TotalFactor = TotalFactor + factor;

    }
    output << "--------------------------------------------------------------------" << endl;
    double TheTotalFactor = TotalFactor / 10;
    output << "Μεσο Κλασμα Προσεγγισης: " << TheTotalFactor << endl;
    output.close();
} 