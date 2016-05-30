#include "KMP.h"

vector<int> computePrefix(string pattern)
{
	unsigned int size = pattern.length();
	vector<int> pi(size + 1);

	pi[0] = -1;
	int k = -1;
	for(int q = 0; q < size; q++)
	{
		while(k > 0 && (pattern[k] != pattern[q]))
			k = pi[k - 1];

		if(pattern[k] == pattern[q])
			++k;

		pi[q] = k;
	}

	return pi;
}

int KMPMatcher(string text, string pattern, const vector<int> &pi)
{
	int n = text.size();
	int m = pattern.size();

	int q = 0;
	int maxQ = 0;

	for(int i = 0; i < n; i++)
	{
		while(q > 0 && (pattern[q] != text[i]))
		{
			q = pi[q];
		}
		if(pattern[q] == text[i])
			++q;
		if(q > maxQ)
			maxQ = q;
		if(q == m && n == m)
		{
			return -1;
		}

	}

	return maxQ;
}
