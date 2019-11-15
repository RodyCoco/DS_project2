#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

template<class T>
class Queue {
public:
	Queue(int queueCapacity = 100) :capacity(queueCapacity), front(-1), rear(-1) { queue = new T[capacity]; }
	~Queue() { delete queue; }
	bool isempty() { return rear == front; }
	T& Front() { return queue[front]; }
	T& Rear() { return queue[(rear-1)% capacity]; }
	void push(const T& x) {
		if (isempty()) {
			front = 0;
			rear = 1;
			queue[0] = x;
		}
		else {
			if ((rear + 1) % capacity == front) {
				T* tmp = new T[2 * capacity];
				for (int i = 0; i < capacity - 1; i++) {
					tmp[i] = queue[(i + front) % capacity];
				}
				rear = capacity - 1;
				capacity = 2 * capacity;
				delete queue;
				queue = tmp;
			}
			queue[rear] = x;
			rear = (rear + 1) % capacity;
		}
	}
	void pop() {
		queue[front].~T();
		front = (front + 1) % capacity;
	}
	void popRear() {
		rear = (rear - 1) % capacity;
		queue[rear].~T();
	}
private:
	T* queue;
	int front, rear;
	int capacity;
};


struct pos {
	pos(int R = 0, int C = 0) :r(R), c(C) {}
	int r;
	int c;
};
pos battery;
Queue<pos> to_be_update;
Queue<pos> path(1000);

struct floormap {
	int type;//-1:R  -2:cleaned
};
floormap map[1000][1000];
bool visited[1000][1000];
int BFScal[1000][1000];
int BFS_battery[1000][1000];

int count_step = 0;//the step robot use
int R, C, B, cur_battery, unclean = 0;
ifstream inputFile;
fstream outputFile;



pos findunclean(pos cur) {
	pos tmp = cur;
	Queue<pos> Q;
	Q.push(tmp);


	BFScal[tmp.r][tmp.c] = 0;
	visited[tmp.r][tmp.c] = true;
	to_be_update.push(tmp);
	while (true) {
		tmp = Q.Front();
		Q.pop();
		if (tmp.c + 1 < C) {
			if (map[tmp.r][tmp.c + 1].type != 1 && visited[tmp.r][tmp.c + 1] == false) {
				BFScal[tmp.r][tmp.c + 1] = BFScal[tmp.r][tmp.c] + 1;
				visited[tmp.r][tmp.c + 1] = true;
				to_be_update.push(pos(tmp.r, tmp.c + 1));
				if (map[tmp.r][tmp.c + 1].type == 0) {
					tmp.c++;
					return tmp;
				}
				Q.push(pos(tmp.r, tmp.c + 1));
			}
		}
		if (tmp.c - 1 >= 0) {
			if (map[tmp.r][tmp.c - 1].type != 1 && visited[tmp.r][tmp.c - 1] == false) {
				BFScal[tmp.r][tmp.c - 1] = BFScal[tmp.r][tmp.c] + 1;
				visited[tmp.r][tmp.c - 1] = true;
				to_be_update.push(pos(tmp.r, tmp.c - 1));
				if (map[tmp.r][tmp.c - 1].type == 0) {
					tmp.c--;
					return tmp;
				}
				Q.push(pos(tmp.r, tmp.c - 1));
			}
		}
		if (tmp.r + 1 < R) {
			if (map[tmp.r + 1][tmp.c].type != 1 && visited[tmp.r + 1][tmp.c] == false) {
				BFScal[tmp.r + 1][tmp.c] = BFScal[tmp.r][tmp.c] + 1;
				visited[tmp.r + 1][tmp.c] = true;
				to_be_update.push(pos(tmp.r+1, tmp.c ));
				if (map[tmp.r + 1][tmp.c].type == 0) {
					tmp.r++;
					return tmp;
				}
				Q.push(pos(tmp.r + 1, tmp.c));
			}
		}
		if (tmp.r - 1 >= 0) {
			if (map[tmp.r - 1][tmp.c].type != 1 && visited[tmp.r - 1][tmp.c] == false) {
				BFScal[tmp.r - 1][tmp.c] = BFScal[tmp.r][tmp.c] + 1;
				visited[tmp.r - 1][tmp.c] = true;
				to_be_update.push(pos(tmp.r - 1, tmp.c));
				if (map[tmp.r - 1][tmp.c].type == 0) {
					tmp.r--;
					return tmp;
				}
				Q.push(pos(tmp.r - 1, tmp.c));
			}
		}
	}
}

void do_one_clean(pos des) {
	Queue<pos> Q;
	pos tmp = des;
	while (BFScal[tmp.r][tmp.c] != 0) {
		Q.push(tmp);
		if (tmp.c + 1 < C) {
			if (BFScal[tmp.r][tmp.c + 1] == BFScal[tmp.r][tmp.c] - 1) {
				tmp.c++;
				continue;
			}

		}
		if (tmp.c - 1 >= 0) {
			if (BFScal[tmp.r][tmp.c - 1] == BFScal[tmp.r][tmp.c] - 1) {
				tmp.c--;
				continue;
			}

		}
		if (tmp.r + 1 < R) {
			if (BFScal[tmp.r + 1][tmp.c] == BFScal[tmp.r][tmp.c] - 1) {
				tmp.r++;
				continue;
			}
		}
		if (tmp.r - 1 >= 0) {
			if (BFScal[tmp.r - 1][tmp.c] == BFScal[tmp.r][tmp.c] - 1) {
				tmp.r--;
				continue;
			}
		}
	}
	//¥i¥HÀu¤Æ
	while (!Q.isempty()) {
		pos p = Q.Rear();
		Q.popRear();
		count_step++;
		cur_battery--;
		path.push(p);
	}
}

void back_to_charge(pos cur) {
	pos tmp = cur;
	while (BFS_battery[tmp.r][tmp.c] != 0) {
		if (map[tmp.r][tmp.c].type == 0) {
			map[tmp.r][tmp.c].type = -2;
			unclean--;
		}

		if (tmp.c + 1 < C) {
			if (BFS_battery[tmp.r][tmp.c + 1] == BFS_battery[tmp.r][tmp.c] - 1) {
				tmp.c++;
				path.push(tmp);
				count_step++;
				continue;
			}
		}
		if (tmp.c - 1 >= 0) {
			if (BFS_battery[tmp.r][tmp.c - 1] == BFS_battery[tmp.r][tmp.c] - 1) {
				tmp.c--;
				path.push(tmp);
				count_step++;
				continue;
			}

		}
		if (tmp.r + 1 < R) {
			if (BFS_battery[tmp.r + 1][tmp.c] == BFS_battery[tmp.r][tmp.c] - 1) {
				tmp.r++;
				path.push(tmp);
				count_step++;
				continue;
			}
		}
		if (tmp.r - 1 >= 0) {
			if (BFS_battery[tmp.r - 1][tmp.c] == BFS_battery[tmp.r][tmp.c] - 1) {
				tmp.r--;
				path.push(tmp);
				count_step++;
				continue;
			}
		}
	}
	cur_battery = B;
}

void back_to_origin(pos cur) {
	pos tmp = cur;
	while (BFS_battery[tmp.r][tmp.c] != 0) {
		if (tmp.c + 1 < C) {
			if (BFS_battery[tmp.r][tmp.c + 1] == BFS_battery[tmp.r][tmp.c] - 1) {
				tmp.c++;
				path.push(tmp);
				count_step++;
				continue;
			}
		}
		if (tmp.c - 1 >= 0) {
			if (BFS_battery[tmp.r][tmp.c - 1] == BFS_battery[tmp.r][tmp.c] - 1) {
				tmp.c--;
				path.push(tmp);
				count_step++;
				continue;
			}

		}
		if (tmp.r + 1 < R) {
			if (BFS_battery[tmp.r + 1][tmp.c] == BFS_battery[tmp.r][tmp.c] - 1) {
				tmp.r++;
				path.push(tmp);
				count_step++;
				continue;
			}
		}
		if (tmp.r - 1 >= 0) {
			if (BFS_battery[tmp.r - 1][tmp.c] == BFS_battery[tmp.r][tmp.c] - 1) {
				tmp.r--;
				path.push(tmp);
				count_step++;
				continue;
			}
		}
	}

}

pos clean(pos cur) {
	pos des = findunclean(cur);
	Queue<pos> Q;
	while (!to_be_update.isempty()) {
		pos tmp = to_be_update.Front();
		to_be_update.pop();
		visited[tmp.r][tmp.c] = false;
		Q.push(tmp);
	}

	if (BFScal[des.r][des.c] + BFS_battery[des.r][des.c] > cur_battery) {
		back_to_charge(cur);
		return battery;
	}
	else {
		do_one_clean(des);
		map[des.r][des.c].type = -2;
		while (!Q.isempty()) {
			pos tmp = Q.Front();
			Q.pop();
			BFScal[tmp.r][tmp.c] = -1;
		}
		unclean--;
		return des;
	}
}

void initail_BFS_battery() {
	BFS_battery[battery.r][battery.c] = 0;
	pos tmp(battery.r, battery.c);
	Queue<pos> Q;
	Q.push(tmp);
	visited[tmp.r][tmp.c] = true;
	while (!Q.isempty()) {
		tmp = Q.Front();
		Q.pop();
		//cout << tmp.r << " " << tmp.c << " " << BFS_battery[tmp.r][tmp.c]<<endl;

		if (tmp.c + 1 < C) {
			if (map[tmp.r][tmp.c + 1].type != 1 && visited[tmp.r][tmp.c + 1] == false) {
				BFS_battery[tmp.r][tmp.c + 1] = BFS_battery[tmp.r][tmp.c] + 1;
				visited[tmp.r][tmp.c + 1] = true;
				Q.push(pos(tmp.r, tmp.c + 1));
			}
		}
		if (tmp.c - 1 >= 0) {
			if (map[tmp.r][tmp.c - 1].type != 1 && visited[tmp.r][tmp.c - 1] == false) {
				BFS_battery[tmp.r][tmp.c - 1] = BFS_battery[tmp.r][tmp.c] + 1;
				visited[tmp.r][tmp.c - 1] = true;
				Q.push(pos(tmp.r, tmp.c - 1));
			}
		}
		if (tmp.r + 1 < R) {
			if (map[tmp.r + 1][tmp.c].type != 1 && visited[tmp.r + 1][tmp.c] == false) {
				BFS_battery[tmp.r + 1][tmp.c] = BFS_battery[tmp.r][tmp.c] + 1;
				visited[tmp.r + 1][tmp.c] = true;
				Q.push(pos(tmp.r + 1, tmp.c));
			}
		}
		if (tmp.r - 1 >= 0) {
			if (map[tmp.r - 1][tmp.c].type != 1 && visited[tmp.r - 1][tmp.c] == false) {
				BFS_battery[tmp.r - 1][tmp.c] = BFS_battery[tmp.r][tmp.c] + 1;
				visited[tmp.r - 1][tmp.c] = true;
				Q.push(pos(tmp.r - 1, tmp.c));
			}
		}
	}

}

int main() {


	inputFile.open("floor.data");
	if (!inputFile)
		cout << "Fail to open 'floor.data'" << endl;
	outputFile.open("final.path.output", ios::out);
	if (!outputFile)
		cout << "Fail to open 'floor.output'" << endl;
	inputFile >> R >> C >> B;
	cur_battery = B;




	for (int i = 0; i < R; i++) {
		for (int j = 0; j < C; j++) {
			char c;
			inputFile >> c;
			if (c == 'R') {
				map[i][j].type = -1;
				battery.r = i;
				battery.c = j;
			}
			else if (c == '0') {
				map[i][j].type = c - '0';
				unclean++;
			}
			else
				map[i][j].type = c - '0';
		}
	}

	path.push(battery);
	memset(visited, false, 1000 * 1000 * sizeof(bool));
	memset(BFScal, -1, 1000 * 1000 * sizeof(int));
	memset(BFS_battery, -1, 1000 * 1000 * sizeof(int));
	initail_BFS_battery();
	memset(visited, false, 1000 * 1000 * sizeof(bool));

	pos cur = battery;
	while (unclean > 0) {
		cur = clean(cur);
	}
	back_to_origin(cur);
	outputFile << count_step << endl;
	while (!path.isempty()) {
		pos tmp = path.Front();
		path.pop();
		outputFile << tmp.r << " " << tmp.c << endl;
	}

	inputFile.close();
	outputFile.close();
	return 0;
}

