#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <algorithm>

using namespace std;

const int targetSum = 2024;
const int GRID_SIZE = 6;

char gridSymbols[GRID_SIZE][GRID_SIZE] = {
        {'A', 'A', 'A', 'B', 'B', 'C'},
        {'A', 'A', 'A', 'B', 'B', 'C'},
        {'A', 'A', 'B', 'B', 'C', 'C'},
        {'A', 'A', 'B', 'B', 'C', 'C'},
        {'A', 'B', 'B', 'C', 'C', 'C'},
        {'A', 'B', 'B', 'C', 'C', 'C'}
};

pair<int, int> convertPosToCoord(const string& pos) {
    int x = pos[0] - 'a';
    int y = stoi(pos.substr(1)) - 1;
    return {x, y};
}

string convertCoordToPos(const pair<int, int>& coord) {
    string pos = "";
    pos += static_cast<char>('a' + coord.first);
    pos += to_string(coord.second + 1);
    return pos;
}

vector<pair<int, int>> getKnightMoves(int x, int y) {
    vector<pair<int, int>> potentialMoves = {
            {x + 1, y + 2}, {x + 1, y - 2},
            {x - 1, y + 2}, {x - 1, y - 2},
            {x + 2, y + 1}, {x + 2, y - 1},
            {x - 2, y + 1}, {x - 2, y - 1}
    };

    vector<pair<int, int>> validMoves;
    for(auto &[mx, my] : potentialMoves) {
        if(mx >= 0 && mx < GRID_SIZE && my >= 0 && my < GRID_SIZE)
            validMoves.emplace_back(mx, my);
    }
    return validMoves;
}

vector<vector<pair<int, int>>> findAllPaths(const pair<int, int>& start, const pair<int, int>& end, int maxDepth=6) {
    vector<vector<pair<int, int>>> allPaths;
    queue<vector<pair<int, int>>> pathQueue;
    pathQueue.push({start});

    while(!pathQueue.empty()) {
        vector<pair<int, int>> currentPath = pathQueue.front();
        pathQueue.pop();
        pair<int, int> currentPos = currentPath.back();

        if(currentPos == end) {
            allPaths.push_back(currentPath);
            continue;
        }

        if(currentPath.size() > maxDepth)
            continue;

        vector<pair<int, int>> moves = getKnightMoves(currentPos.first, currentPos.second);
        for(auto &move : moves) {
            if(find(currentPath.begin(), currentPath.end(), move) == currentPath.end()) {
                vector<pair<int, int>> newPath = currentPath;
                newPath.push_back(move);
                pathQueue.push(newPath);
            }
        }
    }

    return allPaths;
}

long long calculateScore(const vector<pair<int, int>>& path, const unordered_map<char, int>& values, int A_val) {
    if(path.empty()) return 0;
    long long score = A_val;
    char prevChar = gridSymbols[path[0].second][path[0].first];

    for(int i = 1; i < path.size(); ++i) {
        char currentChar = gridSymbols[path[i].second][path[i].first];
        if(currentChar != prevChar) {
            score *= values.at(currentChar);
        }
        else {
            score += values.at(currentChar);
        }
        prevChar = currentChar;
        if(score > targetSum) break;
    }
    return score;
}

int main(){
    string start1 = "a1";
    string end1 = "f6";
    string start2 = "a6";
    string end2 = "f1";

    pair<int, int> coordStart1 = convertPosToCoord(start1);
    pair<int, int> coordEnd1 = convertPosToCoord(end1);
    pair<int, int> coordStart2 = convertPosToCoord(start2);
    pair<int, int> coordEnd2 = convertPosToCoord(end2);

    vector<vector<pair<int, int>>> paths1 = findAllPaths(coordStart1, coordEnd1, 6);
    vector<vector<pair<int, int>>> paths2 = findAllPaths(coordStart2, coordEnd2, 6);

    for(int A = 1; A < 50; ++A){
        for(int B = 1; B < 50; ++B){
            if(B == A) continue;
            for(int C = 1; C < 50; ++C){
                if(C == A || C == B) continue;
                if(A + B + C >= 50) continue;

                unordered_map<char, int> letterValues = {
                        {'A', A},
                        {'B', B},
                        {'C', C}
                };

                vector<vector<pair<int, int>>> validPaths1;
                for(auto &path : paths1){
                    long long score = calculateScore(path, letterValues, A);
                    if(score == targetSum){
                        validPaths1.push_back(path);
                    }
                }

                vector<vector<pair<int, int>>> validPaths2;
                for(auto &path : paths2){
                    long long score = calculateScore(path, letterValues, A);
                    if(score == targetSum){
                        validPaths2.push_back(path);
                    }
                }

                if(!validPaths1.empty() && !validPaths2.empty()){
                    string output = to_string(A) + "," + to_string(B) + "," + to_string(C);
                    for(auto &[x, y] : validPaths1[0]){
                        output += "," + convertCoordToPos({x, y});
                    }
                    for(auto &[x, y] : validPaths2[0]){
                        output += "," + convertCoordToPos({x, y});
                    }
                    cout << output << "\n";

                    return 0;
                }
            }
        }
    }

    cout << "No solution found.\n";
    return 0;
}