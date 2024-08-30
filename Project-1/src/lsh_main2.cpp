#include "my_header.h"

#define QUERY_VECS_NUM 10   // Put here the number of query vectors to be examined


double generateOutputFile(    // takes an ofstream reference and writes the results
    ofstream& file,         // The allready opened file
    int querynumber,        // number of q in the test file
    const set<pair<double, pair<int, const vector<unsigned int>*>>>& approximateResults,
    const set<pair<double, pair<int, const vector<unsigned int>*>>>& trueResults,
    double tLSH,            // time of approximateResults                   
    double tTrue,           // time of true results
    const vector<pair<int, const vector<unsigned int>*>>& rangeSearchResults
);


int main(int argc, char *argv[]) {
    string dataset_file;
    string query_file;
    string output_file = "lsh_output.txt";
    int k = 4;              // amount of h_i functions
    int L = 5;              // amount of Hash Tables
    int N = 1;              // amount of NN to search
    double R = 2500.00;    // Radious of range search

    // Error checking of the arguments
    for (int i=1; i<argc; i++){
        if (strcmp(argv[i], "-d") == 0){
            if (argc > i +1) // To avoid segmentation fault
                dataset_file = argv[++i];
            else {
                cerr << "You did not provide a file after  the -d flag. Try again." << endl;
                return 1;
            } 
        } else if (strcmp(argv[i], "-q") == 0){
            if (argc > i +1)
                query_file = argv[++i];
            else {
                cerr << "You did not provide a file after the -q flag. Try again" << endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-k") == 0){
            if (argc > i +1)
                k = atoi(argv[++i]);
            else {
                cerr << "You did not provide a number after the -k flag." << endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-L") == 0){
            if (argc > i +1)
                L = atoi(argv[++i]);
            else {
                cerr << "You did not provide a number after the -L flag" << endl;
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
        } else if (strcmp(argv[i], "-R") == 0){
            if (argc > i +1)
                R = stod(argv[++i]);
            else {
                cerr << "You did not provide a number after the -R flag." << endl;
                return 1;
            }
        }

    }

    // final check 
    if (k<=0 || L <=0 || N<=0 || R<=0){
        cout << "Wrong input. Please try again." << endl;
        return 1;
    }

    if (dataset_file.empty()){
        cout << "You have not provided an input file. Please give a filename: ";
        cin >> dataset_file;
    }
    if (query_file.empty()){
        cout << "You have not provided a query file. Please give a filename: ";
        cin >> query_file;
    }

    while (1){   // Repeat the cycle until the user wants to exit
        // Read the dataset
        vector<pair<int, vector<unsigned int>*>> dataset;  
        Functions::readImages(dataset_file, dataset);
        
        LSH lsh(dataset, k, L);
        //lsh.print(4);  // print 4 images

        // open the test set and read the images
        vector<pair<int, vector<unsigned int>*>> query_vecs;
        Functions::readImages(query_file, query_vecs);

        // open the output file for append.
        ofstream output(output_file, ios_base::app);
        if (!output.is_open()) {
            cout << "File cannot be opened." << endl;
            break;
        }
        unsigned int testImg = -1;  
        double maxRatio[QUERY_VECS_NUM];     // Put a random number of an image to print its results (Or -1)
        for (size_t i=0; i< QUERY_VECS_NUM; i++){ // for every image in the test set
            
            if (i == testImg){
                cout << "Test image: " << endl;
                Functions::printImage(*query_vecs[3].second);

                cout << "Calculating approximate Neighbours..." << endl;
            }
            // perform the approximateNN 
            auto LSHstart = chrono::high_resolution_clock::now();
            auto approximateNeighbours = lsh.approximateKNN(*query_vecs[i].second, N);
            auto LSHend = chrono::high_resolution_clock::now();
            auto LSHtime = chrono::duration_cast<chrono::microseconds>(LSHend - LSHstart);
            double tLSH = LSHtime.count() / 1e6; // convert microseconds to seconds

            if (i == testImg){
                cout << "Nearest Neighbours (Time elapsed: "<< tLSH << "): "<< endl;
                for(const auto& pair : approximateNeighbours){
                    auto distance = pair.first;
                    auto vec_indx = pair.second.first;
                    auto vec_ptr = pair.second.second;
                    cout << "distance: " << distance << ", Image no: " << vec_indx << endl;
                    Functions::printImage(*vec_ptr);
                    cout << endl << endl;
                }
            
            cout << "Calcuating TRUE Nearest Neighbours..." << endl;
            }
            // perform the exhaustive search
            auto Truestart = chrono::high_resolution_clock::now();
            auto TrueNeighbours = lsh.exhaustiveSearch(*query_vecs[i].second, N);
            auto Trueend = chrono::high_resolution_clock::now();
            auto Truetime = chrono::duration_cast<chrono::microseconds>(Trueend - Truestart);
            double tTrue = Truetime.count() / 1e6; // convert microseconds to seconds

            if (i == testImg){
                cout << "True Neighbours (Time elapsed: "<< tTrue << "): "<< endl;;
                for(const auto& pair : TrueNeighbours){
                    auto distance = pair.first;
                    auto vec_indx = pair.second.first;
                    auto vec_ptr = pair.second.second;
                    cout << "distance: " << distance << ", Image no: " << vec_indx << endl;
                    Functions::printImage(*vec_ptr);
                    cout << endl << endl;
                }
            }
            // perform the range nearest search
            auto rangeNeighbours = lsh.approximateRangeSearch(*query_vecs[i].second, R);
            // write down the results of the current query
            maxRatio[i] = generateOutputFile(output, query_vecs[i].first, approximateNeighbours, TrueNeighbours, tLSH, tTrue, rangeNeighbours);
            cout << "Finished search for query " << query_vecs[i].first << endl;
        }

        double maxElement = maxRatio[0]; // Αρχικοποιείτε το maxElement με την πρώτη τιμή του πίνακα

        for (int i = 1; i < QUERY_VECS_NUM; i++) {
            if (maxRatio[i] > maxElement) {
                maxElement = maxRatio[i]; // Ενημερώνετε το maxElement αν βρείτε μια μεγαλύτερη τιμή
            }
        }
        output << "MAF: " << maxElement << endl;
        output.close();
        // delete the space that readImages() allocated for the dataset and the query set
        for (auto vec : dataset)
            delete vec.second;
        for (auto vec : query_vecs)
            delete vec.second;

        string answer;
        cout << "Search complete. The results are saved in \""<< output_file << "\". Do you want to restart for another test set?[y/n]: ";
        cin >> answer;
        if (answer == "y" || answer == "Y"){
            cout << "Please provide the name of the test file: ";
            cin >> query_file;
        } else {
            cout << "Goodbye.";
            break;
        }
    }
    
    

    return 0;
}

double generateOutputFile( 
    ofstream& file,
    int querynumber, 
    const set<pair<double, pair<int, const vector<unsigned int>*>>>& approximateResults, 
    const set<pair<double, pair<int, const vector<unsigned int>*>>>& trueResults,
    double tLSH,  
    double tTrue,          
    const vector<pair<int, const vector<unsigned int>*>>& rangeSearchResults
){
    double maxRatio = 0.0;
    file << "Query " << querynumber << endl;
    /* We use two iterators simultaneously
        1 for approxResults and 1 for trueResults*/
    auto trueResult = trueResults.begin();
    int i=1;
    for (const auto& approxResult : approximateResults){
        double approxDist = approxResult.first;
        int approxImgNumb = approxResult.second.first;
        double trueDist = trueResult->first;

        double currentRatio = trueDist / approxDist; // Υπολογισμός του τρέχοντος λόγου

        if (currentRatio > maxRatio) {
            maxRatio = currentRatio; // Ενημέρωση του μέγιστου λόγου αν το τρέχον είναι μεγαλύτερο
        }

        file << "Nearest neighbour-" << i << ": " << approxImgNumb << endl;
        file << "distanceLSH: " << approxDist << endl;
        file << "distanceTrue: " << trueDist << endl;
        trueResult++;
        i++;
    }

    file << "tLSH: " << tLSH << endl << "tTrue: " << tTrue << endl;


    /*file << "R-near neighbours: " << endl;
    for (const auto& rangeResult : rangeSearchResults){
        file << rangeResult.first << endl;
    }*/
    file << "--------------------------------------------------------------" << endl;
    return maxRatio;
}

