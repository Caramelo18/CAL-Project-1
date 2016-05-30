#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * Pre processes a pattern prefix
 * @param pattern pattern to process
 * @return vector containing the result of the pre process
 */
vector<int> computePrefix(string pattern);

/**
 * Applies the Knuth Morris Pratt algorithm
 * @param text text to analyze
 * @param pattern pattern to search for
 * @param pi result of the pre process of the pattern
 * @return returns the max of occurrences of patterns' char in text, -1 if it is the same word
 */
int KMPMatcher(string text, string pattern, const vector<int> &pi);
