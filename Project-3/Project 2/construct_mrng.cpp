#include "my_header.h"

int main(int argc, char *argv[]){
    // set the default values
    string dataset_file = "test_set";
    string output_file = "MRNGraph_file";
    int I = 20;
    int N = 1;

    // Error checking of the arguments
    for (int i=1; i<argc; i++){
        if (strcmp(argv[i], "-d") == 0){
            if (argc > i +1) // To avoid segmentation fault
                dataset_file = argv[++i];
            else {
                cerr << "You did not provide a file after  the -d flag. Try again." << endl;
                return 1;
            } 
        } else if (strcmp(argv[i], "-I") == 0){
            if (argc > i +1)
                I = atoi(argv[++i]);
            else {
                cerr << "You did not provide a number after the -I flag" << endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-o") == 0){
            if (argc > i +1)
                output_file = argv[++i];
            else {
                cerr << "You did not provide a file after the -o flag. Try again" << endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-N") == 0){
            if (argc > i +1)
                N = atoi(argv[++i]);
            else {
                cerr << "You did not provide a number after the -N flag." << endl;
                return 1;
            }
        } 
    }

    if (I<=0 || N<=0 || I<N){
        cout << "Wrong input. Please try again." << endl;
        return 1;
    }

    if (dataset_file.empty()){
        cout << "You have not provided an input file. Please give a filename: ";
        cin >> dataset_file;
    }
    if(output_file.empty()){
        cout << "You have not provided a output file. Please give a filename: ";
        cin >> output_file;
    }

    // read the images
    vector<pair<int, vector<unsigned int>*>> dataset;  
    Functions::readImages(dataset_file, dataset);

    // initialize the class
    MRNGraph Graph(dataset, I, N);
    const vector<pair<int, vector<int>>> innerEdges = Graph.getInnerEdges();

    ofstream output(output_file, ios_base::trunc);
    if (!output.is_open()) {
        cout << "File cannot be opened." << endl;
        return 0;
    }

    // write the innerEdges to the file
    for (const auto& edge : innerEdges){
        int node = edge.first;
        const vector<int>& neighbors = edge.second;

        output << node << ": ";
        for (const int neighbor: neighbors){
            output << neighbor << " ";
        }
        output << endl;
    }
    output.close();
    cout << "Graph edges written to " << output_file << endl;
}