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
    int value = 1;
    StringIntPair key_val_pair;
    key_val_pair.first = word;
    key_val_pair.second = value;
    return key_val_pair;
}

//reduce function
StringIntPair reduceFunc(StringIntPair* dupArray, int dup_size){
    StringIntPair combined_pair;
    combined_pair.first = dupArray[0].first;
    combined_pair.second = dup_size;
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
    vector<StringIntPair> dup_vector;   // holds duplicate pairs to be reduced
    vector<StringIntPair> used_vector;  // keeps track of which pairs have been tested for duplicity
    vector<StringIntPair> reduced_vector;   // holds the final reduced pairs for word_count output
    string wordArray[1000];
    StringIntPair pairArray[1000];
    StringIntPair dupArray[1000];
    StringIntPair usedArray[1000];
    StringIntPair reducedArray[1000];
    


    // STEP 1: read in file
    string* filled_wordArray = inputReadFunc(wordArray);

    // STEP 2: map
    #pragma omp parallel for 
    for (int i = 0; i<num_words; i++){
        pairArray[i] = mapFunc(wordArray[i]);
    }

/*
    // FOR NOW: go back to vectors for reduction phase
    for (int i = 0; i<num_words; i++){
    	pair_vector.push_back(pairArray[i]);
    }
*/

    // STEP 3: reduce: send vectors of duplicates to reduce function
    //#pragma omp critical
    int num_used = 0;
    int dup_size = 0;
    for(int i = 0; i< num_words; i++){
    	for(int j = 0; j<num_used; j++){
    		if (usedArray[j] == pairArray[i])
    			continue; 
    	}
    	dupArray[i] = pairArray[i];
    	dup_size++;
    	usedArray[i] = pairArray[i];
    	for(int k = i+1; k<num_words; k++){
    		for (int l = 0; l<dup_size; l++){
    			if (dupArray[l] == pairArray[k]){
    				dupArray[k] = pairArray[k];
    				dup_size++;
    			}
    		}
    	}
    	reducedArray[i] = reduceFunc(dupArray, dup_size);
    	dup_size = 0;
    }
    cout << reducedArray[0].first << endl;

/*
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
*/

    // STEP 3.5: sort the reduced vector in descending
    sort(reduced_vector.begin(), reduced_vector.end(), compFuncDescending);

    // STEP 4: output
    outputFunc(reduced_vector);

    return 0;

}