// TURGUT CAN AYDINALEV 150120001 AoA2 PROJECT 1
#include <iostream>
#include <cstring>
#include <vector>
#include <list>
#include <bitset>
#include <chrono>
#include <ctime>
#include <cstdlib>

#define FAR 0
#define CAR 1
#define RAB 2
#define FOX 3

using namespace std;

class path_node { // for solution path. parent nodes are needed for backtracking
public:
	int index;
	int parent;
	path_node(int i, int p) { index = i; parent = p; }
};

class node { // for each state
public:
	node(int i);
	vector <int> edges; // edges
	string side; // binary representation of the positions
	bool safeCheck(); // is the state safe or not 
	bool isTarget(); // is the state target or not
	int nodeno(); // it returns integer value of the binary number for easy indexing
	void print();
};

class graph {
public:
	vector <node> states;
	graph();
	void dfs();
	void bfs();
	void print_moves(int n1, int n2); // printing moves
};

void graph::print_moves(int n1, int n2) {
	bitset<4> t1(states[n1].side);
	bitset<4> t2(states[n2].side);
	string str = (t1^=t2).to_string();
	char farmer_side = states[n1].side[FAR];
	cout << "(Farmer";
	if (str[FOX] == '1') cout << ", Fox, ";
	else if (str[RAB] == '1') cout << ", Rabbit, ";
	else if (str[CAR] == '1') cout << ", Carrot, ";
	else cout << ", " ;
	if (farmer_side == '0') cout << "> )";
	else cout << "< )";
	cout << endl;
}

graph::graph() {
	for(int i =0; i < 16; i++) { // 16 states
		states.push_back(node(i));
	}
	for(int i =0; i < 16; i++) { // for each state
		for(int j=1; j<4;j++) { // for each item
			if(states[i].side[FAR] == states[i].side[j]) { // check farmer can pick up or not
				bitset<4> temp(states[i].side);
				temp.flip(3-FAR); // index reversed due to string operations
				temp.flip(3-j);
				states[i].edges.push_back(stoi(temp.to_string(),nullptr,2));
			}
		}
		// farmer can freely change side
		bitset<4> temp(states[i].side);
		temp.flip(3-FAR);
		states[i].edges.push_back(stoi(temp.to_string(),nullptr,2));
	}
}

void graph::bfs() {
	cout << "Algorithm: BFS" << endl;
	auto start = chrono::system_clock::now();
	vector <path_node> path;
	bool disc[16];
	int max = 0;
	for (int i=0;i < 16;i++) disc[i] = false; // discovered bool array
	path.push_back(path_node(0,-1));
	list <int> q; // list for bfs
	disc[0] = true; // start node is index 0.
	q.push_back(0);
	int visited_node_count=1;
	
	while(!q.empty()) {
		int cur = q.front();
		q.pop_front();
		
		if (states[cur].isTarget()) { // reached the last node

			cout << "Number of the visited nodes: " << visited_node_count << endl << "Maximum number of nodes kept in the memory: " << max <<  endl;

			int searching = 15; // last node
			for(int i=path.size()-1;i > -1 ;i--) {
				if (path[i].index == searching) {
					searching = path[i].parent;
				}
				else {
					path.erase(path.begin() + i);
				}
			}
			auto end = chrono::system_clock::now();
			cout << "Running time: " << double(chrono::duration_cast<chrono::nanoseconds>(end-start).count()) / 1000000000 << " seconds" << endl;
			cout << "Solution move count: " << path.size() - 1 << endl << endl;
			for (int i =0; i < path.size(); i++) {
				states[path[i].index].print();
				if (i != path.size()-1) print_moves(path[i].index,path[i+1].index);
			}
			
			return;
		}
		for(int i = 0; i < states[cur].edges.size(); i++) {
			visited_node_count++;
			if (disc[states[cur].edges[i]] == false && states[states[cur].edges[i]].safeCheck()) {
				q.push_back(states[cur].edges[i]); // queue
				path.push_back(path_node(states[cur].edges[i],states[cur].nodeno()));
				if (max < q.size()) max = q.size();
				disc[states[cur].edges[i]] = true;
			}
		}
	}
	cout << "Solution not found!" << endl;
	return;
}

void graph::dfs() {
	cout << "Algorithm: DFS" << endl;
	auto start = chrono::system_clock::now();

	vector <path_node> path;
	int max = 0;
	bool disc[16];
	for (int i=0;i < 16;i++) disc[i] = false; // discovered bool array
	path.push_back(path_node(0,-1));
	list <int> q; // list for dfs
	disc[0] = true; // start node is index 0.
	q.push_back(0); // start node
	int visited_node_count=1;

	while(!q.empty()) {
		int cur = q.front();
		q.pop_front();
		
		if (states[cur].isTarget()) { // reached the last node

			cout << "Number of the visited nodes: " << visited_node_count << endl << "Maximum number of nodes kept in the memory: " << max <<  endl;

			int searching = 15; // last node
			for(int i=path.size()-1;i > -1 ;i--) {
				if (path[i].index == searching) {
					searching = path[i].parent;
				}
				else {
					path.erase(path.begin() + i);
				}
			}
			auto end = chrono::system_clock::now();
			cout << "Running time: " << double(chrono::duration_cast<chrono::nanoseconds>(end-start).count()) / 1000000000 << " seconds" << endl;
			cout << "Solution move count: " << path.size() - 1 << endl << endl;
			for (int i =0; i < path.size(); i++) {
				states[path[i].index].print();
				if (i != path.size()-1) print_moves(path[i].index,path[i+1].index);			
			
			}

			return;
		}
		for(int i = 0; i < states[cur].edges.size(); i++) {
			visited_node_count++;
			if (disc[states[cur].edges[i]] == false && states[states[cur].edges[i]].safeCheck()) { // add safe edges into list
				q.push_front(states[cur].edges[i]); // stack
				path.push_back(path_node(states[cur].edges[i],states[cur].nodeno())); // for solution
				if (max < q.size()) max = q.size(); // max count
				disc[states[cur].edges[i]] = true; // discovered
			}
		}
	}
	cout << "Solution not found!" << endl;
	return;
}


node::node(int i) {
	side = bitset<4>(i).to_string();
}

void node::print() { // 0 ->left side, 1 -> right side
	string left;
	string right;
	if (side[CAR] == '0') left = left + "Carrot ";
	else right = right + "Carrot ";

	if (side[RAB] == '0') left = left + "Rabbit ";
	else right = right + "Rabbit ";

	if (side[FOX] == '0') left = left + "Fox ";
	else right = right + "Fox ";

	if (side[FAR] == '0') left = left + "Farmer ";
	else right = right + "Farmer ";
	cout << left <<" ||  " << right;
	cout << endl;
}

int node::nodeno() {
	return stoi(side,nullptr,2);
}

bool node::isTarget() { 
	if (side[CAR] == '1' && side[RAB] == '1' && side[FOX] == '1' && side[FAR] == '1') return true;
	else return false;
}

bool node::safeCheck() {
	if (side[FAR] == '1') { // not safe conditions according to position of the farmer
		if ((side[CAR] == '0' && side[RAB] == '0') || (side[RAB] == '0' && side[FOX] == '0')) return false;
		else return true;
	}
	else {
		if ((side[CAR] == '1' && side[RAB] == '1') || (side[RAB] == '1' && side[FOX] == '1')) return false;
		else return true;
	}
}

int main(int argc, char *argv[]) {

	graph g;
	if (argc == 2) {
		if (strcmp(argv[1],"dfs") == 0) {
			g.dfs();
		}
		else if (strcmp(argv[1],"bfs") == 0) {
			g.bfs();
		}
		else {
			cout << "Wrong input" << endl;
			return -1;
		}
	}
	else {
			cout << "Wrong input" << endl;
			return -1;
		}

	return 0;

}