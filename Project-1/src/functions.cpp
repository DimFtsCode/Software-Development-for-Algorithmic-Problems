#include "my_header.h"
/* defines several functions used by the classes */

uint32_t Functions::reverseInt(uint32_t num) {
        // Υλοποιήστε τη συνάρτηση εδώ
        return ((num & 0xFF) << 24) | 
               ((num & 0xFF00) << 8) |
               ((num & 0xFF0000) >> 8) |
               ((num & 0xFF000000) >> 24);
 }


unsigned int Functions::squareDifference(double a, double b) {
    return pow(a - b, 2);
}

double Functions::distance(const vector<unsigned int>& vec1, const vector<unsigned int>& vec2) {
    if (vec1.size() != vec2.size()) {
        cerr << "Διανύσματα έχουν διαφορετικές διαστάσεις!" << endl;
        return UINT_MAX; // Επιστροφή ενός σφάλματος
    }

    double sum = 0.0;
    for (size_t i = 0; i < vec1.size(); ++i) {
        sum += squareDifference(static_cast<double>(vec1[i]), static_cast<double>(vec2[i]));
    }
    return sqrt(sum);
}

double Functions::distance1(const vector<unsigned int>& vec1, const vector<unsigned int>& vec2) {
    if (vec1.size() != vec2.size()) {
        cerr << "Διανύσματα έχουν διαφορετικές διαστάσεις!" << endl;
        return UINT_MAX; // Επιστροφή ενός σφάλματος
    }

    double sum = 0.0;
    for (size_t i = 0; i < vec1.size(); ++i) {
        sum += squareDifference(static_cast<double>(vec1[i]), static_cast<double>(vec2[i]));
    }
    return sum;
}

double Functions::dotProduct(const vector<unsigned int>& queryvec, const vector<double>& randomvec){
    if (queryvec.size() != randomvec.size()) {
        cerr << "The vectors are not the same dimension!" << endl;
        return -1; // Επιστροφή ενός σφάλματος
    }

    signed int sum = 0.0;
    for (size_t i = 0; i < queryvec.size(); ++i) {
        sum += queryvec[i] * randomvec[i];
    }
    return sum;
}

vector<double> Functions::generateRandomVector(size_t d) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> dist(0, 1); 

    vector<double> vec(d);
    //cout << "d: " << d << endl; 
    for (size_t i = 0; i < d; ++i) 
        vec[i] = dist(gen); 

    return vec;
}

void Functions::printImage(const vector<unsigned int>& vec){
    for (size_t i = 0; i < vec.size(); i++){
            // every number has a width of 4, for better readability
            cout << std::left << std::setw(4) << static_cast<unsigned int>(vec[i]);
        
            if ((i + 1) % 28 == 0)   // Change line after 28 pixels
                cout << endl;
        }
}

void Functions::readImages(const string& filename, vector<pair<int, vector<unsigned int>*>>& destination){
    
    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        cout << "File cannot be opened." << endl;
        exit(1);
    }

    uint32_t magicNumber, numberOfImages, numberOfRows, numberOfColumns;
    file.read(reinterpret_cast<char*>(&magicNumber), sizeof(magicNumber));
    magicNumber = reverseInt(magicNumber);

    file.read(reinterpret_cast<char*>(&numberOfImages), sizeof(numberOfImages));
    numberOfImages = reverseInt(numberOfImages);

    file.read(reinterpret_cast<char*>(&numberOfRows), sizeof(numberOfRows));
    numberOfRows = reverseInt(numberOfRows);

    file.read(reinterpret_cast<char*>(&numberOfColumns), sizeof(numberOfColumns));
    numberOfColumns = reverseInt(numberOfColumns);

    uint32_t imageSize = numberOfRows * numberOfColumns;
    destination.resize(numberOfImages);

    for (uint32_t i = 0; i < numberOfImages; ++i) {
        destination[i].first = i + 1;
        destination[i].second = new vector<unsigned int>(imageSize);
        for (uint32_t j = 0; j < imageSize; ++j) {
            unsigned char byte;
            file.read(reinterpret_cast<char*>(&byte), sizeof(unsigned char));
            (*destination[i].second)[j] = static_cast<unsigned int>(byte);
        }
    }
    file.close();
}

void Functions::printClusters(const map<int, vector<int>>& clusters) {
    for (const auto& cluster : clusters) {
        cout << "Cluster " << cluster.first + 1 << " includes vectors with indices: ";
        for (const auto& index : cluster.second) {
            cout << index << " ";
        }
        cout << endl;
    }
}

void Functions::printSilhouettes(const vector<double>& silhouettes) {
    cout << "Silhouette: [";
    if (!silhouettes.empty()) {
        for (size_t i = 0; i < silhouettes.size() - 1; ++i) { 
            cout << silhouettes[i];
            cout << ", ";
        }
        
        cout << silhouettes.back(); 
    }
    cout << "]\n"; 
    if (!silhouettes.empty()) {
        cout << "[" << silhouettes.back() << "]" << endl; 
    }
}