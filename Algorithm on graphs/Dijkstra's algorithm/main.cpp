#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>


using namespace std;

pair<int, int> get_vert(string line) {
    stringstream stream(line);
    int x, y;

    stream >> x >> y;
    pair<int, int> p1(x, y);

    return p1;
}

double norm(pair<int, int> p1, pair<int, int> p2) {
    return sqrt((p2.first - p1.first) * (p2.first - p1.first) + (p2.second - p1.second) * (p2.second - p1.second));
}

vector<vector<double>> read_file_to_matrix(string filename, vector<pair<int,int>>& verts) {
    string line, line1;
    bool flag = true;
    vector<int> vars;
    int N, R;

    ifstream in(filename);
    if (in.is_open())
        while (getline(in, line)) {
            if (flag) {
                stringstream streamData(line);
                const char separator = ' ';

                while (getline(streamData, line1, separator))
                    vars.push_back(stoi(line1));
                N = vars[0]; R = vars[1];
                flag = false;
            }
            else {
                verts.push_back(get_vert(line));
            }
        }
    in.close();

    double dist;
    vector<vector<double>> matrix(N, vector<double>(N, 0.0));
    for (size_t i = 0; i < N; ++i)
        for (size_t j = 0; j < N; ++j) {
            dist = norm(verts[i], verts[j]);
            if (dist < R || abs(dist - R) < 1e-2)
                matrix[i][j] = dist;
        }

    return matrix;
}

typedef priority_queue< pair<double, double>, vector<pair<double, double>>, greater<pair<double, double>>> q_t;

void update(vector<vector<double>> matrix, int v, q_t& p_q, vector<bool>& visited, vector<double>& d, vector<int>& pred) {
    for (size_t u = 0; u < matrix[0].size(); ++u) {
        if (abs(matrix[v][u]) > 0.01 && visited[u] == false && d[u] > d[v] + matrix[v][u]) {
            d[u] = d[v] + matrix[v][u];
            pred[u] = v;
            p_q.push(make_pair(d[u], u));
        }
    }
        
}

double Dijkstra(vector<vector<double>> matrix, vector<int>& pred) {
    vector<double> d(matrix[0].size(), INFINITY);

    q_t p_q;
    vector<bool> visited(matrix[0].size(), false);

    d[0] = 0;
    pred[0] = 0;
    p_q.push(make_pair(d[0], 0));

    int v, u;
    while (!p_q.empty()) {
        v = p_q.top().second;
        p_q.pop();
        visited[v] = true;

        u = pred[v];
        d[v] = d[u] + matrix[u][v];
        pred[v] = u;

        update(matrix, v, p_q, visited, d, pred);
    }
    
    pred[0] = -1;
     
    return ((pred[matrix[0].size() - 1] != -1) ? d[matrix[0].size()-1] : -1);
}

int main(int argc, char* argv[]) {
    string input, output;
    if (argc > 1) {
        input = argv[1];
        output = argv[2];

        vector<vector<double>> matrix;
        vector<pair<int, int>> verts;

        matrix = read_file_to_matrix(input, verts);
        vector<int> pred(verts.size(), -1);
        double min_path = Dijkstra(matrix, pred);
        vector<int> path;
        int index = verts.size() - 1;

        while (pred[index] != -1) {
            path.push_back(index);
            index = pred[index];
        }
        path.push_back(0);

        ofstream out;
        out.open(output);

        out << min_path << endl;
        if (abs(min_path + 1) > 0.01) {
            for (size_t i = path.size(); i > 0; --i) {
                out << verts[path[i - 1]].first << " " << verts[path[i - 1]].second << endl;
            }
        }
        out.close();
    }
    return 0;
}
