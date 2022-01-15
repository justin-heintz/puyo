#pragma once
using namespace std;

class grid {
	vector<vector<int>> board;
	public:
		grid() {
			vector<vector<int>> board(6, vector<int>(12));
		}
		int getId(int x, int y) {
			return board[x][y];
		}
		void insertId(int x, int y, int id) {
			board[x][y] = id;
		}
		void removeId(int x, int y) {
			board[x][y] = 0;
		}
};
