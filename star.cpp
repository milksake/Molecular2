// #define _GLIBCXX_DEBUG 1
#include <bits/stdc++.h>
using namespace std;

#define int long long
#define ll long long
#define rep(i, a, b) for (int i = a; i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
#define printv(x) for (int iz = 0; iz < sz(x); iz++) cout << (x)[iz] << ((iz == sz(x)-1) ? '\n' : ' ')
#define printvv(x) for (int jz = 0; jz < sz(x); jz++) printv((x)[jz])

typedef pair<int, int> pii;
typedef vector<int> vi;
typedef vector<vi> vvi;

const int inf = 1e15;

int neddlemanWunsch(const string& s, const string& t, vvi& dp)
{
    dp.assign(sz(s)+1, vi(sz(t)+1, -inf));
    dp[0][0] = 0;

    rep(i, 0, sz(s)+1)
    {
        rep(j, 0, sz(t)+1)
        {
            if (i)
                dp[i][j] = dp[i-1][j] - 2;
            if (j)
                dp[i][j] = max(dp[i][j], dp[i][j-1] - 2);
            if (i&&j)
                dp[i][j] = max(dp[i][j], dp[i-1][j-1] + (s[i-1] == t[j-1] ? 1 : -1));
        }
    }

    return dp[sz(s)][sz(t)];
}

pair<string, string> neddlemanWunschRecoverOne(const string& s, const string& t, const vvi& dp)
{
    string sp, tp;

    for (int i = sz(s), j = sz(t); true; )
    {
        if (i>0 && dp[i-1][j] - 2 == dp[i][j])
        {
            tp.push_back('-');
            sp.push_back(s[i-1]);
            i--;
        }
        else if (j>0 && dp[i][j-1] - 2 == dp[i][j])
        {
            sp.push_back('-');
            tp.push_back(t[j-1]);
            j--;
        }
        else if (i>0 && j>0 && dp[i-1][j-1] + (s[i-1] == t[j-1] ? 1 : -1) == dp[i][j])
        {
            sp.push_back(s[i-1]);
            tp.push_back(t[j-1]);
            i--;
            j--;
        }
        else
            break;
    }

    reverse(all(sp));
    reverse(all(tp));

    return {sp, tp};
}

void neddlemanWunschRecoverAll(const string& s, const string& t, const vvi& dp, vector<pair<string, string>>& out)
{
    out.clear();

    queue<pair<pair<int, int>, pair<string, string>>> q;
    q.push({{sz(s), sz(t)}, {"", ""}});

    while (!q.empty())
    {
        auto [i, j] = q.front().first;
        auto [sp, tp] = q.front().second;
        q.pop();
        
        while (true)
        {
            bool processed = false;
            int ci = i, cj = j;
            string csp = sp, ctp = tp;

            if (i>0 && dp[i-1][j] - 2 == dp[i][j])
            {
                sp.push_back(s[i-1]);
                tp.push_back('-');
                i--;
                processed = true;
            }
            if (j>0 && dp[i][j-1] - 2 == dp[i][j])
            {
                if (!processed)
                {
                    sp.push_back('-');
                    tp.push_back(t[j-1]);
                    j--;
                    processed = true;
                }
                else
                    q.push({{ci, cj-1}, {csp + "-", ctp + t.substr(cj-1, 1)}});
            }
            if (i>0 && j>0 && dp[i-1][j-1] + (s[i-1] == t[j-1] ? 1 : -1) == dp[i][j])
            {
                if (!processed)
                {
                    sp.push_back(s[i-1]);
                    tp.push_back(t[j-1]);
                    i--;
                    j--;
                    processed = true;
                }
                else
                    q.push({{ci-1, cj-1}, {csp + s.substr(i-1, 1), ctp + t.substr(j-1, 1)}});
            }

            if (!processed)
                break;
        }

        reverse(all(sp));
        reverse(all(tp));
        out.push_back({sp, tp});
    }
}

int SPScore(const vector<string>& s)
{
    int n = sz(s);
    int ans = 0;
    rep(k, 0, sz(s[0]))
    {
        rep(i, 0, n)
        {
            rep(j, i+1, n)
            {
                char const* p1 = &s[i][k];
                char const* p2 = &s[j][k];

                if (*p1 > *p2)
                    swap(p1, p2);

                if (*p1 == '-')
                {
                    if (*p2 != '-')
                        ans -= 2;
                    continue;
                }
                ans += (*p1 == *p2) ? 1 : -1;
            }
        }

        // cout << '\t' << ans << endl;
    }
    return ans;
}

void MSAStar(const vector<string>& s, vector<string>& ans)
{
    int n = sz(s);
    vvi dp;

    vi vals(n);
    rep(i, 0, n)
    {
        rep(j, i+1, n)
        {
            int v = neddlemanWunsch(s[i], s[j], dp);
            vals[i] += v;
            vals[j] += v;
        }
    }

    int c = max_element(all(vals)) - vals.begin();

    ans.assign(n, "");
    ans[c] = s[c];

    rep(i, 0, n)
    {
        if (i == c)
            continue;

        neddlemanWunsch(s[c], s[i], dp);
        auto [asc, asi] = neddlemanWunschRecoverOne(s[c], s[i], dp);
        ans[i] = asi;

        vi addPrev;
        vi addCurr;

        int k, j;
        for (k = 0, j = 0; k < sz(asc) && j < sz(ans[c]); )
        {
            if (asc[k] == ans[c][j])
            {
                j++; k++;
                continue;
            }
            if (asc[k] == '-')
            {
                k++;
                addPrev.push_back(j);
                continue;
            }
            if (ans[c][j] == '-')
            {
                j++;
                addCurr.push_back(k);
                continue;
            }
        }

        for (; k < sz(asc); k++)
            addPrev.push_back(sz(ans[c]));

        for (; j < sz(ans[c]); j++)
            addCurr.push_back(sz(asc));

        reverse(all(addPrev));
        reverse(all(addCurr));
        for (auto pos : addPrev)
        {
            rep(j, 0, i)
            {
                if (j != c)
                    ans[j].insert(pos, 1, '-');
            }
            ans[c].insert(pos, 1, '-');
        }
        for(auto pos : addCurr)
            ans[i].insert(pos, 1, '-');

        // for (auto& s : ans)
        // {
        //     cout << s << endl;
        // }
        // cout << "-----" << endl;
    }
}

void input(vector<string>& f, vector<string>& r)
{
    f.assign(6, "");
    r.assign(6, "");

    string tmp;
    rep(i, 0, 6)
    {
        cin >> tmp;
        cin >> tmp;
        cin >> tmp;
        cin >> tmp;
        f[i] = tmp;
        f[i] = f[i].substr(5, sz(f[i])-10);
        cin >> tmp;
        cin >> tmp;
        r[i] = tmp;
        r[i] = r[i].substr(5, sz(r[i])-10);

        // cout << f[i] << ' ' << r[i] << endl;
    }
}

void solve()
{
    vector<string> ans;
    vector<string> example = {
        "ATTGCCATT",
        "ATGGCCATT",
        "ATCCAATTTT",
        "ATCTTCTT",
        "ACTGACC"
    };
    MSAStar(example, ans);

    cout << "Example:\n";
    for (auto& s : example)
        cout << s << '\n';
    cout << '\n';
    for (auto& s : ans)
        cout << s << '\n';

    cout << "Score: " << SPScore(ans) << "\n\n";

    vector<string> f;
    vector<string> r;
    input(f, r);

    MSAStar(f, ans);    

    cout << "Forward:\n";
    for (auto& s : f)
        cout << s << '\n';
    cout << '\n';
    for (auto& s : ans)
        cout << s << '\n';
    
    cout << "Score: " << SPScore(ans) << "\n\n";
    
    MSAStar(r, ans);    
    
    cout << "Reverse:\n";
    for (auto& s : r)
        cout << s << '\n';
    cout << '\n';
    for (auto& s : ans)
        cout << s << '\n';

    cout << "Score: " << SPScore(ans) << "\n\n";
}

signed main()
{
    cin.tie(0)->sync_with_stdio(0);
    cin.exceptions(cin.failbit);

    // int t;
    // cin >> t;
    // while (t--)
        solve();
    
}