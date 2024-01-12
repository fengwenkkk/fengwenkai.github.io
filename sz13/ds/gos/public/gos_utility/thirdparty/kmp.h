/*******************************************************************************
 * ALGORITHM IMPLEMENTAIONS
 *
 *  /\  |  _   _  ._ o _|_ |_  ._ _   _
 * /--\ | (_| (_) |  |  |_ | | | | | _>
 *         _|
 *
 * KNUTH-MORRIS-PRATT ALGORITHMS
 *
 * Features:
 * Complexity is O(n + k), where n is the target string length,
 * and k is the pattern length
 *
 * http://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
 *
 ******************************************************************************/

#ifndef ALGO_KMP_H__
#define ALGO_KMP_H__
#include <string.h>
#include <string>

namespace gos
{
/**
 * build a table for the word to be searched
 * eg:
 * i        0   1   2   3   4   5   6
 * W[i]     A   B   C   D   A   B   D
 * T[i]     -1  0   0   0   0   1   2
 */
inline void kmp_table(const char *W, int *T, int len)
{
    int pos = 2;  // the current position we are computing in T
    int cnd = 0;  // the next character of the current candidate substring
    T[0] = -1;
    T[1] = 0;

    while (pos < len)
    {
        // first case: the substring continues
        if (W[pos - 1] == W[cnd])
        {
            cnd++;
            T[pos] = cnd;
            pos++;
        }
        else if (cnd > 0)
        {  // second case: it doesn't, but we can fall back
            cnd = T[cnd];
        }
        else
        {  // third case: we have run out of candidates.  Note cnd = 0
            T[pos] = 0;
            pos++;
        }
    }
}

/**
 * S -> the text to be searched
 * W -> the word to search
 * offset -> search begin position
 * return the position where W is found S
 */
inline size_t kmp_search(const char *S, const char *W, int offset = 0)
{
    int LEN_S = S ? strlen(S) : 0;
    int LEN_W = W ? strlen(W) : 0;
    if (!S || !W || LEN_S == 0 || LEN_S < LEN_W || offset < 0)
    {
        return std::string::npos;
    }

    if (LEN_W == 0)
    {
        return 0;
    }

    int m = offset;
    int i = 0;
    int *T = new int[LEN_W + 1];

    kmp_table(W, T, LEN_W);

    while (m + i < LEN_S && i < LEN_W)
    {
        if (W[i] == S[m + i])
        {
            if (i == LEN_W - 1)
            {
                delete[] T;
                return static_cast<size_t>(m);
            }
            i++;
        }
        else
        {
            m = m + i - T[i];
            if (T[i] > -1)
            {
                i = T[i];
            }
            else
            {
                i = 0;
            }
        }
    }
    delete[] T;
    return std::string::npos;
}

}  // namespace gos

#endif
