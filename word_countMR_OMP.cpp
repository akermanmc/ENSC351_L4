//word count with mapreduce and omp


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <vector>   
#include <algorithm>
#include <omp.h>

#define StringIntPair pair<string, int>
using namespace std;

int num_words = 0;

// input function
string* inputReadFunc(string wordArray[]){
	string word;
	ifstream inFile;

    inFile.open("words.txt");
    if(!inFile.is_open()){
        cout << "open failed, word_vector empty" << endl; 
        return wordArray;
    }
	while (inFile >> word){
		wordArray[num_words] = word;
		num_words++;
	}
	inFile.close();
	return wordArray;
}

// map function
StringIntPair mapFunc(const string word){
	//cout << "in map" << endl;
    int value = 1;
    StringIntPair key_val_pair;
    key_val_pair.first = word;
    key_val_pair.second = value;
    return key_val_pair;
}

//reduce function
StringIntPair reduceFunc(vector<StringIntPair> dup_vector){
	//cout << "in reduce" << endl;
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
    vector<StringIntPair> pair_vector;  // holds initial mapped pairs
    vector<vector<StringIntPair> > dup_vector;   // holds duplicate pairs to be reduced
    vector<StringIntPair> used_vector;  // keeps track of which pairs have been tested for duplicity
    vector<StringIntPair> reduced_vector;   // holds the final reduced pairs for word_count output
    string wordArray[1000];
    StringIntPair pairArray[1000];

    // STEP 1: read in file
    string* filled_wordArray = inputReadFunc(wordArray);

    // STEP 2: map
    #pragma omp parallel for 
    for (int i = 0; i<num_words; i++){
        pairArray[i] = mapFunc(wordArray[i]);
    }

    // FOR SIMPLICITY: go to vectors for reduction phase
    for (int i = 0; i<num_words; i++){
    	pair_vector.push_back(pairArray[i]);
    }

    // STEP 3: reduce: send vectors of duplicates to reduce function
    for (int i = 0; i < pair_vector.size(); i++){
    	int j = 0;
	    for (j = 0; j < dup_vector.size(); j++){
            if (pair_vector[i].first == dup_vector[j][0].first){
                dup_vector[j].push_back(pair_vector[i]);
                break;
            }
        }
        if(j >= dup_vector.size()){
            vector<StringIntPair> temp;
            temp.push_back(pair_vector[i]);
            dup_vector.push_back(temp);
        }
    }

    #pragma omp declare reduction (merge: vector<pair<string, int> >: omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))
    #pragma omp parallel for reduction (merge: reduced_vector)
    for (int i = 0; i < dup_vector.size(); i++){
    	reduced_vector.push_back(reduceFunc(dup_vector[i]));
    }

    // STEP 3.5: sort the reduced vector in descending order
    sort(reduced_vector.begin(), reduced_vector.end(), compFuncDescending);

    // STEP 4: output
    outputFunc(reduced_vector);

    return 0;
}