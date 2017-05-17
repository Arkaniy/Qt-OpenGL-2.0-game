#include <queue>
#include "gameplay.h"

Map::Map() // constructor
{
	srand(time(0));
	newMap();
}

void Map::newMap() {
	for (int i = 0; i < WORLD_MAX.i; i++)
		for (int j = 0; j < WORLD_MAX.j; j++)
		{
			world[i][j].state = 0;
			world[i][j].wasHere = false;
		}
	rooms.clear();
	//create rooms
	for (int i = 0; i < NROOMS; i++)
		rooms.push_back(createRoom());

	//deleteWasteWalls();

	//create passage
	for (int i = 0; i < NROOMS - 1; i++)
		generatePassage(rooms[i], rooms[i + 1]);
	deleteWasteWalls();
	//add walls
	int ways[8][2] = { { 0, -1 }, { 0, 1 }, { 1, -1 }, { 1, 1 }, { 1, 0 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
	for (int i = 1; i < WORLD_MAX.i - 1; i++)
		for (int j = 1; j < WORLD_MAX.j - 1; j++) {
			int count = 0;
			for (int k = 0; k < 8; k++) {
				if (world[i][j].state == 1 && world[i + ways[k][0]][j + ways[k][1]].state == 0)
					world[i + ways[k][0]][j + ways[k][1]].state = 2;
				if (world[i][j].state == 2 && world[i + ways[k][0]][j + ways[k][1]].state == 1)
					count++;
			}
			if (count == 8) {
				world[i][j].state = 1;
			}

			if (world[i][j].state == 2 && world[i][j + 1].state == 1 && world[i][j - 1].state == 1)
				world[i][j].state = 1;
			if (world[i][j].state == 2 && world[i + 1][j].state == 1 && world[i - 1][j].state == 1)
				world[i][j].state = 1;
		}
}

Map::Room Map::createRoom()
{
	Room temp;

	// find free space
	do
	{
		//////////////////////////////////////////////////////////////////////////////////////
		temp = { 0, 0, 0, 0, 0, 0 };                           //							//
		temp.i = rand() % (WORLD_MAX.i - 10) + 1;               //							//
		temp.j = rand() % (WORLD_MAX.j - 10) + 1;               //   Dont touch!!!!		    //
		temp.h = 5 + rand() % ((WORLD_MAX.i - temp.i) / 6);    //	  It works with Magic!	//
		temp.w = 5 + rand() % ((WORLD_MAX.j - temp.j) / 6);    //							//
		//////////////////////////////////////////////////////////////////////////////////////
	} while (isIntersection(temp));

	// mark inside room
	for (int i = temp.i + 1; i < temp.h + temp.i; i++)
		for (int j = temp.j + 1; j < temp.w + temp.j; j++)
			world[i][j].state = 1;

	// mark walls
	for (int i = 0; i < temp.h; i++)
	{
		world[i + temp.i][temp.j].state = 2;
		world[i + temp.i][temp.j + temp.w].state = 2;
	}
	for (int j = 0; j <= temp.w; j++)
	{
		world[temp.i][j + temp.j].state = 2;
		world[temp.i + temp.h][j + temp.j].state = 2;
	}

	// find middle point
	temp.ic = temp.h / 2 + temp.i;
	temp.jc = temp.w / 2 + temp.j;

	return temp;
}

bool Map::isIntersection(Room &inRoom)
{
	for (auto room : rooms)
		if (inRoom.i <= (room.i + room.h) && (inRoom.i + inRoom.h) >= room.i && inRoom.j <= (room.j + room.w) && (inRoom.j + inRoom.w) >= room.j)
			return true;

	return false;
}

void Map::deleteWasteWalls() {
	for (auto room : rooms) { // for each room
		for (int i = room.i + 1; i < room.i + room.h; i++) { // look at vertical walls
			if (world[i][room.j + room.w + 1].state == 2) { // look at near right point
				world[i][room.j + room.w + 1].state = 1;
				world[i][room.j + room.w].state = 1;
			}
		}
		for (int j = room.j + 1; j < room.j + room.w; j++) { // look at gorizontal walls
			if (world[room.i + room.h + 1][j].state == 2) { // look at near bottom point
				world[room.i + room.h + 1][j].state = 1;
				world[room.i + room.h][j].state = 1;
			}
		}
	}
}

void Map::generatePassage(const Room &first, const Room &last)
{
	struct Point
	{
		int x, y, cost;
		bool operator==(const Point &p) const
		{
			return x == p.x && y == p.y;
		}

		bool operator<(const Point &p) const
		{
			return cost > p.cost;
		}

		int calcCost(Point point)
		{
			cost += abs(y - point.y) + abs(x - point.x); // manhetten distance
			if (world[y][x].state == 1)
				return cost;
			else return cost + 25;
		}
	};

	Point start, finish;
	start.x = first.jc + rand() % first.w - 2; start.y = first.ic + rand() % first.h - 2; start.cost = 0;
	finish.x = last.jc + rand() % last.w - 2; finish.y = last.ic + rand() % last.h - 2; finish.cost = 0;

	// make matrix with parents for search way
	int parents[H / CELL * NDISPLAYS * W / CELL * NDISPLAYS];
	for (int &pare : parents)
		pare = -1;

	std::priority_queue<Point> active;
	active.push(start);

	static const int directions[4][2] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };
	while (!active.empty())
	{
		const Point point = active.top();
		active.pop();

		if (point == finish)
			break;

		for (int i = 0; i < 4; ++i)
		{
			Point p = { point.x - directions[i][0], point.y - directions[i][1], 0 };
			if (p.x < 0 || p.y < 0 || p.x >= WORLD_MAX.j || p.y >= WORLD_MAX.i)
				continue;

			if (parents[p.x + p.y * WORLD_MAX.j] < 0)
			{
				p.cost = p.calcCost(finish);
				active.push(p);
				parents[p.x + p.y * WORLD_MAX.j] = i;
			}
		}
	}
	int n = rand() % 5;
	while (!(finish == start))
	{
		const int *directon = directions[parents[finish.x + finish.y * WORLD_MAX.j]];

		if (world[finish.y][finish.x].state != 1)
		{
			world[finish.y][finish.x].state = 1;

			for (int i = 0; i < n; i++) {
				if (world[finish.y + directon[0]][finish.x + i * directon[1]].state == 0)
					world[finish.y + directon[0]][finish.x + i * directon[1]].state = 1;

				if (world[finish.y + directon[0]][finish.x - i * directon[1]].state == 0)
					world[finish.y + directon[0]][finish.x - i * directon[1]].state = 1;

				if (world[finish.y - directon[0]][finish.x + i * directon[1]].state == 0)
					world[finish.y - directon[0]][finish.x + i * directon[1]].state = 1;

				if (world[finish.y - directon[0]][finish.x - i * directon[1]].state == 0)
					world[finish.y - directon[0]][finish.x - i * directon[1]].state = 1;
			}


			if (world[finish.y + directon[0]][finish.x + n *directon[1]].state == 0)
				world[finish.y + directon[0]][finish.x + n *directon[1]].state = 2;

			if (world[finish.y + directon[0]][finish.x - n *directon[1]].state == 0)
				world[finish.y + directon[0]][finish.x - n * directon[1]].state = 2;

			if (world[finish.y - directon[0]][finish.x + n * directon[1]].state == 0)
				world[finish.y - directon[0]][finish.x + n * directon[1]].state = 2;

			if (world[finish.y - directon[0]][finish.x - n * directon[1]].state == 0)
				world[finish.y - directon[0]][finish.x - n * directon[1]].state = 2;

		}

		finish.x += directon[0];
		finish.y += directon[1];
	}
}

Map::~Map() {
    for (int i = 0; i < WORLD_MAX.i; i++) {
		for (int j = 0; j < WORLD_MAX.j; j++) {
			world[i][j].state = 0;
			world[i][j].wasHere = false;
            delete world[i][j].object;
            world[i][j].object = nullptr;
		}
    }
}
