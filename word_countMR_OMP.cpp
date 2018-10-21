//word count with mapreduce and omp


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <vector>   
#include <algorithm>

#define StringIntPair pair<string, int>
using namespace std;

// input function
vector<string> inputReadFunc(vector<string> &word_vector){
	string word;
	ifstream inFile;

    inFile.open("words.txt");
    if(!inFile.is_open()){
        cout << "open failed, word_vector empty" << endl; 
        return word_vector;
    }
	while (inFile >> word){
		word_vector.push_back(word);
	}
	inFile.close();
	return word_vector;
}

// map function
StringIntPair mapFunc(const string &word){
    int value = 1;
    StringIntPair key_val_pair;
    key_val_pair.first = word;
    key_val_pair.second = value;
    return key_val_pair;
}

//reduce function
StringIntPair reduceFunc(vector<StringIntPair> dup_vector){
    StringIntPair combined_pair;
    combined_pair.first = dup_vector[0].first;
    combined_pair.second = dup_vector.size();
    return combined_pair;
}

//output function
void outputFunc(const vector<StringIntPair> &reduced_vector){
	for (int i = 0; i<reduced_vector.size(); i++){
		cout << reduced_vector[i].first << "," << reduced_vector[i].second << endl;
	}
}

bool compFuncDescending(const StringIntPair a, const StringIntPair b)
{
    return (a.second > b.second);
}

int main(){
    vector<string> word_vector; // holds input item: vector of words parsed from input file
    vector<StringIntPair> pair_vector;  // holds initial mapped pairs
    vector<StringIntPair> dup_vector;   // holds duplicate pairs to be reduced
    vector<StringIntPair> used_vector;  // keeps track of which pairs have been tested for duplicity
    vector<StringIntPair> reduced_vector;   // holds the final reduced pairs for word_count output

    // STEP 1: read in file
    word_vector = inputReadFunc(word_vector);

    // STEP 2: map
    for (int i = 0; i<word_vector.size(); i++){
        pair_vector.push_back(mapFunc(word_vector[i]));
    }

    // STEP 3: reduce: send vectors of duplicates to reduce function
    for (int i= 0; i<pair_vector.size(); i++){
        if (find(used_vector.begin(), used_vector.end(), pair_vector[i]) != used_vector.end()){
            continue;
        }
        dup_vector.push_back(pair_vector[i]);
        used_vector.push_back(pair_vector[i]);
        for (int j = i+1; j<pair_vector.size(); j++){
            if (find(dup_vector.begin(), dup_vector.end(), pair_vector[j]) != dup_vector.end()){
                dup_vector.push_back(pair_vector[j]);
            }
        }
        reduced_vector.push_back(reduceFunc(dup_vector));
        dup_vector.clear();
    }

    // STEP 3.5: sort the reduced vector in descending
    sort(reduced_vector.begin(), reduced_vector.end(), compFuncDescending);

    // STEP 4: output
    outputFunc(reduced_vector);

    return 0;

}