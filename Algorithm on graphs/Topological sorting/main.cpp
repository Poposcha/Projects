#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

vector<string> get_lines(size_t& N, string in_filename) {
    string line;
    bool flag = true;
    vector<string> words;

    ifstream in(in_filename);
    if (in.is_open())
        while (getline(in, line))
            if (flag) {
                N = stoi(line);
                flag = false;
            }
            else words.push_back(line);
    in.close();
    return words;
}

vector<string> get_verts(vector<string> words) {
    string buf;
    vector<string> verts;
    for (size_t i = 0; i < words.size(); i++)
        for (size_t j = 0; j < words[i].size(); j++) {
            buf = words[i][j];
            if (find(verts.begin(), verts.end(), buf) == verts.end()) {
                verts.push_back(buf);
            }
        }
    return verts;
}


vector<vector<string>> get_matrix(vector<pair<string, string>> edges, vector<string> verts) {
    size_t index;
    vector<vector<string>> matrix(verts.size(), vector<string>(verts.size(), "0"));
    for (int i = 0; i < verts.size(); ++i) {
        for (int j = 0; j < edges.size(); ++j) {
            if ((verts[i] == edges[j].first))
                if ((verts[i] == edges[j].first)) {
                    vector<string>::iterator it = find(verts.begin(), verts.end(), edges[j].second);
                    index = distance(verts.begin(), it);
                    matrix[i][index] = "1";
                }
                else {
                    vector<string>::iterator it = find(verts.begin(), verts.end(), edges[j].first);
                    index = distance(verts.begin(), it);
                    matrix[i][index] = "1";
                }
        }
    }
    return matrix;
}

enum class States {
    unvisited,
    visited,
    processed
};

void top_sort_visit(size_t v, vector<vector<string>> matrix, vector<States>& state, int& cur_time, vector<int>& time_in, vector<int>& pred, vector<int>& time_out, vector<int>& top_num, int& cur_top_num) {
    state[v] = States::visited;
    cur_time++;
    time_in[v] = cur_time;

    for (size_t u = 0; u < matrix[v].size(); ++u) {
        if (matrix[v][u] == "1")
            if (state[u] == States::unvisited) {
                pred[u] = v;
                top_sort_visit(u, matrix, state, cur_time, time_in, pred, time_out, top_num, cur_top_num);
            }
    }
    state[v] = States::processed;
    cur_time++;
    time_out[v] = cur_time;
    top_num[v] = cur_top_num;
    cur_top_num--;  

}

string top_sort(vector<string> verts, vector<pair<string, string>> edges, vector<vector<string>> matrix) {
    vector<States> state(verts.size(), States::unvisited);
    vector<int> pred(verts.size(), NULL);
    vector<int> time_in(verts.size(), NULL);
    vector<int> time_out(verts.size(), NULL);
    vector<int> top_num(verts.size(), NULL);

    int cur_time = 0;
    int cur_top_num = verts.size();

    for (size_t v = 0; v < verts.size(); ++v) {
        if (state[v] == States::unvisited)
            top_sort_visit(v, matrix, state, cur_time, time_in, pred, time_out, top_num, cur_top_num);
    }

    //for (auto i : top_num)
    //    cout << i << " ";
    //cout << endl;
    //
    //for (auto i : verts)
    //    cout << i << " ";
    //cout << endl;

    //for (auto i : edges)
    //    cout << i.first << "->" << i.second << "\n";
    //cout << endl;

    //for (int i = 0; i < matrix.size(); ++i)
    //{
    //    for (int j = 0; j< matrix[i].size(); ++j)
    //        cout << matrix[i][j] << " ";
    //    cout << endl;
    //}

    string result = "";
    for (size_t i = 0; i < verts.size(); ++i) {
        for (size_t j = 0; j < verts.size(); ++j) {
            if (top_num[j] == i + 1) {
                result += verts[j];
                break;
            }
        }
    }


    return result;
}

void Nital(string in_filename, string out_filename) {
    size_t N;
    vector<string> words = get_lines(N, in_filename);
    vector<string> verts = get_verts(words);
    vector<vector<string>> matrix;
    vector<pair<string, string>> edges;
    string buf1, buf2;

    for (size_t i = 0; i < words.size() - 1; ++i) {
        for (size_t j = 0; j < min(words[i].size(), words[i + 1].size()); ++j) {
            if (words[i][j] != words[i + 1][j]) {
                buf1 = words[i][j];
                buf2 = words[i + 1][j];
                pair<string, string> p(buf1, buf2);
                edges.push_back(p);
                break;
            }
        }
    }

    matrix = get_matrix(edges, verts);
    
    ofstream f_out(out_filename);
    f_out << top_sort(verts, edges, matrix);
    f_out.close();
   

}


int main(int argc, char* argv[]) {
    string input, output;

    if (argc > 1) {
        input = argv[1];
        output = argv[2];
    }
    Nital(input, output);
    return 0;
}
