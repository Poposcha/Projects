#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

int find_pair_index(vector<pair<int, int>>& verts, pair<int, int>& p) {
    auto it = find_if(verts.begin(), verts.end(), [&p](const pair<int, int>& p_check) {
        return p.first == p_check.first && p.second == p_check.second;
        });

    return it - verts.begin();
}

void get_verts(vector<pair<int, int>>& verts, string line) {
    stringstream stream(line);
    int x1, x2, y1, y2;
    stream >> x1 >> y1 >> x2 >> y2;
    pair<int, int> p1(x1, y1), p2(x2, y2);

    int index1 = find_pair_index(verts, p1);
    if (index1 == verts.size())
        verts.push_back(p1);

    int index2 = find_pair_index(verts, p2);
    if (index2 == verts.size())
        verts.push_back(p2);
}

vector<vector<int>> read_file_to_matrix(string filename, vector<pair<int, int>>& verts) {
    string line;
    bool flag = true;
    int N;

    ifstream in(filename);
    if (in.is_open())
        while (getline(in, line))
            if (flag) {
                 N = stoi(line);
                flag = false;
            }
            else
                get_verts(verts, line);
    in.close();

    vector<vector<int>> matrix(N, vector<int>(N, 0));

    in.open(filename);
    if (in.is_open()) {
        getline(in, line);
        while (getline(in, line)) {
            stringstream stream(line);
            int x1, x2, y1, y2;
            stream >> x1 >> y1 >> x2 >> y2;
            pair<int, int> p1(x1, y1), p2(x2, y2);


            int index1 = find_pair_index(verts, p1);
            int index2 = find_pair_index(verts, p2);

            matrix[index1][index2] = matrix[index2][index1] = 1;
        }
    }
    in.close();


    return matrix;
}
template<class T>
void print_matrix(vector<vector<T>> const& mat) {
    for (vector<T> row : mat)
    {
        for (T val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
}

void BFS_Visit(int s, vector<int>& state, vector<int>& pred, vector<vector<int>> matrix) {
    queue<int> q;
    q.push(s);
    int v;

    while (!q.empty()) {
        v = q.front();
        q.pop();
        for (int u = 0; u < matrix[0].size(); u++) 
            if (matrix[v][u] != 0)
                if (state[u] == -1) {
                    state[u] = 0;
                    pred[u] = v;
                    q.push(u);
                }   
        state[v] = 1;
            
    }
}
vector<int> merge_ways(const vector<int>& predV, int v, const vector<int>& predU, int u);
vector<int> find_loop(int s, vector<vector<int>>& predList, vector<int>& state, vector<int>& pred, vector<vector<int>> matrix);

vector<int> BFS(vector<vector<int>> matrix) {
    /* -1 --unvisited
        0 -- visited
        1 -- processed */
    vector<vector<int>> predList(matrix[0].size());
    vector<int> result;
    int N = matrix[0].size();
    vector<int> state(N, -1);
    vector<int> pred(N, -1);

    for (int i = 0; i < N; i++)
        if (state[i] == -1) {
            //BFS_Visit(i, state, pred, matrix);
            result = find_loop(i, predList, state, pred, matrix);
            if (result.size() != 0)
                return result;
        }

    return result;
}

vector<int> find_loop(int s, vector<vector<int>>& predList, vector<int>& state, vector<int>& pred, vector<vector<int>> matrix) {
    queue<int> q;
    q.push(s);
    int v;
    vector<int> result;

    while (!q.empty()) {
        v = q.front();
        q.pop();
        for (int u = 0; u < matrix[0].size(); u++)
            if (matrix[v][u] != 0)
                if (state[u] == -1) {
                    state[u] = 0;
                    pred[u] = v;
                    q.push(u);

                    int index = pred[u];
                    predList[u].push_back(index);
                    while (pred[index] != -1) {
                        index = pred[index];
                        predList[u].push_back(index);
                    }
                }
                else if (pred[v] != u){
                    result = merge_ways(predList[v], v, predList[u], u);
                    break;
                }
        state[v] = 1;
        if (result.size() != 0)
            break;

    }

    return result;



}

vector<int> merge_ways(const vector<int>& predV, int v, const vector<int>& predU, int u) {
    vector<int> result;
    int index1 = predV.size() -1;
    int index2 = predU.size() -1;

    while (index1 >= 0 && index2 >= 0 && predV[index1] == predU[index2]) {
        index1--;
        index2--;
    }
    result.push_back(v);
    for (int i = 0; i <= index1+1; ++i)
        result.push_back(predV[i]);
    
    for (int i = index2; i >= 0; --i)
        result.push_back(predU[i]);
    result.push_back(u);
    
    return result;

}

int main(int argc, char* argv[]) {
    string input, output;

    if (argc > 1) {
        input = argv[1];
        output = argv[2];
        vector<pair<int, int>> verts;
        auto matrix = read_file_to_matrix(input, verts);
        vector<int> loop = BFS(matrix);
        ofstream out;
        out.open(output);
        out << loop.size() << "\n";
        for (int i = 0; i < loop.size(); i++)
            out << verts[loop[i]].first << " " << verts[loop[i]].second << "\n";

    }
        return 0;
}
