//word count with mapreduce


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <string.h>
#include <utility>
#include <vector>	
#include <algorithm>
#define StringIntPair pair<string, int>
using namespace std;

ifstream inFile;
vector<string> word_vector; // holds input item: vector of words parsed from input file
vector<StringIntPair> pair_vector;	// holds initial mapped pairs
vector<vector<StringIntPair>> dup_vector;	// vector of vector!! wohoo! holds duplicate pairs to be reduced as distinct vectors
vector<StringIntPair> reduced_vector;	// holds the final reduced pairs for word_count output

bool compFuncDescending(const StringIntPair a, const StringIntPair b)
{
    return (a.second > b.second);
}

// input function
vector<string> inputReadFunc(vector<string> &word_vector){
	string word;
	while (inFile >> word){
		word_vector.push_back(word);
	}
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
StringIntPair reduceFunc(const vector<StringIntPair> &dup_vector){
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

int main(){

	// STEP 1: read in file
	inFile.open("words.txt");
	if(!inFile.is_open()){
		cout << "open failed" << endl; 
		return 1;
	}
	word_vector = inputReadFunc(word_vector);

	inFile.close();
    
    // STEP 2: map
  

	//form dup_vector structure with separate vectors for all unique words
	for (int i = 0; i < pair_vector.size(); i++)
    {
	    int j =0;
	    for (j = 0; j < dup_vector.size(); j++)
	    {
            if (pair_vector[i].first == dup_vector[j][0].first)
            {
                dup_vector[j].push_back(pair_vector[i]);
                break;
            }
        }
        if(j >= dup_vector.size())
        {
            vector<StringIntPair> temp;
            temp.push_back(pair_vector[i]);
            dup_vector.push_back(temp);
        }
    }

    //STEP3: reduce


    // STEP 3.5: sort the reduced vector in descending
    std::sort(reduced_vector.begin(), reduced_vector.end(), compFuncDescending);

    // STEP 4: output
    outputFunc(reduced_vector);

    return 0;
}