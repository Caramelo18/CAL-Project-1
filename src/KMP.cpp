#include "KMP.h"

vector<int> computePrefix(string pattern)
{
	int size = pattern.size();
	vector<int> pi;
	pi.resize(size);

	pi[0] = 0;
	int k = 0;
	for(int q = 1; q <= size; q++)
	{
		while(k > 0 && (pattern[k] != pattern[q]))
			k = pi[k - 1];

		if(pattern[k] == pattern[q])
			++k;

		pi[q] = k;
	}

	/*for(int i = 0; i < size; i++)
		cout << pi[i] << " ";
	cout << endl;*/

	return pi;
}

int KMPMatcher(string text, string pattern)
{
	int n = text.size();
	int m = pattern.size();
	vector<int> pi = computePrefix(pattern);

	int q = 0;
	int maxQ = 0;

	for(int i = 0; i <= n; i++)
	{
		while(q > 0 && (pattern[q] != text[i]))
			q = pi[q];
		if(pattern[q] == text[i])
			++q;
		if(q > maxQ)
					maxQ = q;
		if(q == m)
		{
			//cout << "Pattern occurs with shift " << i - m << endl;
			q = pi[q];
		}

	}
//	cout << maxQ << " " << text << " " << pattern << endl;
	return maxQ;
}
