#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <set>
#include <float.h>

using namespace std;


vector<string> get_tops(string line) {
    vector<string> result;
    stringstream data(line);
    string str;
    while (getline(data, str, ' '))
        result.push_back(str);
    return result;
}


vector<vector<pair<double,double>>> get_graph_info(string filename) {
    string line;
    bool flag = true;
    bool flag2 = true;
    vector<string> tops;

    ifstream in(filename);
    string v1, v2, w;
    double weight;
    vector<vector<pair<double, double>>> matrix;

    if (in.is_open()) {
        getline(in, line);
        tops = get_tops(line);
        matrix = vector<vector<pair<double, double>>>(tops.size(), vector<pair<double, double>>(tops.size(), make_pair(0.0, 0.0)));
        while (getline(in, line)) {
            if (line != "") {
                stringstream s(line);
                s >> v1;
                s >> v2;
                s >> w;
                weight = stod(w);
                auto it = find(tops.begin(), tops.end(), v1);
                auto it2 = find(tops.begin(), tops.end(), v2);
                int index1 = distance(tops.begin(), it);
                int index2 = distance(tops.begin(), it2);
                matrix[index1][index2].first = weight;
                matrix[index1][index2].second = 0.0;
            }
        }
    }
    in.close();
    return matrix;
}   

void ford_falkerson(vector<vector<pair<double, double>>>& matrix){
    int s = 0; // исток
    int t = matrix.size() - 1; // сток

    set<int> S;
    S.insert(t);

    double δ;
    vector<int> p;


    while (S.find(t) != S.end()) {
        S = set<int>();
        S.insert(s);
        δ = DBL_MAX;
        p = vector<int>(matrix.size(), -1);
        bool flag = true;

        while (flag){
            flag = false;

            for (int u = 0; u < matrix.size(); ++u){
                for (int v = 0; v < matrix.size(); ++v){
                    if (matrix[u][v].first != 0) {
                        // u in S and v not in S and f(e)<c(e);
                        if (S.find(u) != S.end() && S.find(v) == S.end() && matrix[u][v].second < matrix[u][v].first) {
                            S.insert(v);
                            p[v] = u;
                            δ = min(δ, matrix[u][v].first - matrix[u][v].second);
                            flag = true;
                        }
                        // u not in S and v in S and f(e)>0;
                        else if (S.find(u) == S.end() && S.find(v) != S.end() && matrix[u][v].second > 0) {
                            S.insert(u);
                            p[u] = v;
                            δ = min(δ, matrix[u][v].second);
                            flag = true;
                        }
                    }
                }
            }
        }

        if (S.find(t) != S.end()) {
            int v{t }, u{ p[t] };
            while (u != -1) {
                matrix[u][v].second += δ;
                v = u;
                u = p[v];
            }
        }
    }
}


int main(int argc, char* argv[]) {
    //string input, output;
    string input ="Input1.txt", output="output.txt";
    if (argc > 1) {
      //  input = argv[1];
        //output = argv[2];
        vector<vector<pair<double, double>>> matrix;

        matrix = get_graph_info(input);
        ford_falkerson(matrix);

        double res{0.};
        int t{static_cast<int>((matrix.size()) - 1)};
        for (int u = 0; u < matrix.size(); ++u) {
            if (matrix[u][t].first != 0) {
                res += matrix[u][t].second;
            }
        }


        fstream file(output);
        string line, u1, v1;
        int _n;
        file << res << endl;

        getline(file, line);
        auto tops = get_tops(line);
        while (getline(file, line)) {
            if (line != "") {
                stringstream stream(line);
                stream >> u1 >> v1 >> _n;

                auto it = find(tops.begin(), tops.end(), u1);
                auto it2 = find(tops.begin(), tops.end(), v1);
                int index1 = distance(tops.begin(), it);
                int index2 = distance(tops.begin(), it2);

                file << u1 << " " << v1 << " " << matrix[index1][index2].second << endl;
            }
        }
    }

    return 0;
}
