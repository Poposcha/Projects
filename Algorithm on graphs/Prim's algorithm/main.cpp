#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>

using namespace std;

typedef priority_queue< pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> q_t;

void BFS_Visit(int s, vector<int>& state, vector<int>& pred, vector<vector<int>>& matrix, vector<int>& result) {
    queue<int> q;
    q.push(s);
    int v;

    while (!q.empty()) {
        v = q.front();
        q.pop();
        for (int u = 0; u < matrix[0].size(); u++)
            if (matrix[v][u] != -1)
                if (state[u] == -1) {
                    state[u] = 0;
                    pred[u] = v;
                    q.push(u);
                }
        state[v] = 1;
        result.push_back(v);

    }
}
vector<int> BFS(vector<vector<int>> matrix) {
    /* -1 --unvisited
        0 -- visited
        1 -- processed */

    // Последовательность обхода.
    vector<int> result;

    int N = matrix[0].size();
    vector<int> state(N, -1);
    vector<int> pred(N, -1);

    for (int i = 0; i < N; i++)
        if (state[i] == -1) {
            BFS_Visit(i, state, pred, matrix, result);
        }

    return result;
}


int prim(vector<vector<int>> matrix, vector<int>& visited, vector<int>& pred) {
    // Построение минимального оставного дерева.
    int weight, ind;

    // Массив последовательности загрузки этажей.
    visited = vector<int>(matrix.size(), -1);
    // Массив последовательности загрузок этажей (полная или частичная) для каждого этажа.
    pred = vector<int>(matrix.size(), -1);

    vector<int> C(matrix.size(), INT_MAX);
    vector<int> P(matrix.size(), -1);

    // Массив, проверяющий вершину на вхождение в оставное дерево.
    vector<bool> inMST(matrix.size(), false);
    q_t pq;

    pq.push(make_pair(0, 0));
    C[0] = 0;
    P[0] = 0;


    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();
        
        if (inMST[u] == true)
            continue;

        inMST[u] = true;

        for (int i = 0; i < matrix[u].size(); ++i) {
            if (matrix[u][i] != -1) {
                int v = i;
                int weight = matrix[u][v];

                if (inMST[v] == false && C[v] > weight)
                {
                    C[v] = weight;
                    P[v] = u;
                    pq.push(make_pair(C[v], v));
                }
            }
        }
    }

    for (int i = 0; i < P.size(); ++i) {
        pred[i] = P[i];
    }

    vector<vector<int>> mmatr(matrix.size(), vector<int>(matrix.size(), -1));
    for (int i = 0; i < P.size(); ++i) {
        if (P[i] != -1) {
            mmatr[P[i]][i] = matrix[P[i]][i];
            mmatr[i][P[i]] = matrix[i][P[i]];
        }
    }

    visited = BFS(mmatr);

    int sum = 0;
    for (auto i : mmatr)
        for (auto j : i)
            if (j != -1)
                sum += j;
    sum /= 2;
    return sum;
}

int get_distance(string fl1, string fl2) {
    int count = 0;
    for (int i = 0; i < fl1.size(); ++i)
        if (fl1[i] != fl2[i])
            count++;
    return count;
}

vector<vector<int>> read_file(int& n, int& m, int& k, int& w, vector<string>& floors, string in_filename) {
    string line, line1, line2 = "";
    bool flag = true;
    vector<int> vars;
    int counter = 0;

    ifstream in(in_filename);
    if (in.is_open())
        while (getline(in, line)) {
            if (flag) {
                stringstream streamData(line);
                const char separator = ' ';

                while (getline(streamData, line1, separator))
                    vars.push_back(stoi(line1));
                n = vars[0]; m = vars[1]; k = vars[2]; w = vars[3];
                flag = false;
            }
            else {
                counter++;
                line2 += line;
                if (counter % n == 0) {
                    floors.push_back(line2);
                    line2 = "";
                }
            } 
        }
    in.close();

    vector<vector<int>> matrix(k + 1, vector<int>(k + 1, -1));

    for (int i = 0; i < k + 1; i++)
        for (int j = 0; j < k + 1; j++)
            if ((i == 0 || j == 0) && i != j)
                matrix[i][j] = n * m;
            else if (i != j) {
                matrix[i][j] = get_distance(floors[i-1], floors[j-1]) * w;
            }

    return matrix;
}

int vec_sum(vector<int> v) {
    int sum = 0;
    for (int i = 0; i < v.size(); i++)
        sum += v[i];
    return sum;
}

int main(int argc, char* argv[]) {
    string input, output;
    //string input ="Input3.txt", output="output.txt";
    if (argc > 1) {
        input = argv[1];
        output = argv[2];
        int n, m, k, w;
        vector<string> floors;
        vector<vector<int>> matrix = read_file(n, m, k, w, floors, input);
        vector<int> visited;
        vector<int> pred(k + 1, -1);
        
        int p = prim(matrix, visited, pred);

        ofstream out;
        out.open(output);
        out << p << "\n";
        for (int i = 1; i < visited.size(); i++) {
            int first, second;
            first = visited[i];
            second = pred[visited[i]];

            out << first << " " << second << endl;
        }

   }
}
