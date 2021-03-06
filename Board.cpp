#include "Board.h"


Board::~Board()
{

	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
		{
			if (cells[i][j] != nullptr)
				delete cells[i][j];
		}
	for (Soldier*& i : soldiers)
	{
		if (i != nullptr)
			delete i;
	}
	//clearScreen();
}
void Board::SetSpecialCells()
{
	//initialize the FR cells
	int FR[][2] = { { 3,4 },{ 3,5 },{ 3,6 },{ 3,7 },{ 2,7 },{ 1,7 },{ 1,8 },{ 4,6 } ,{ 4,7 } ,{ 4,8 },{ 4,9 } };
	for (auto& i : FR)
	{
		int x = i[0] - 1;
		int y = i[1] - 1;
		cells[x][y] = new Cell(x, y, Type::FR);
		if (!TheOption.GetQuiet()) cells[x][y]->Draw();
	}

	//initialize the SEA cells
	int SEA[][2] = { { 8, 6 },{ 9,5 },{ 9,6 },{ 10,4 },{ 10,5 },{ 10,6 },{ 10,7 },{ 10,8 },{ 10,9 },{ 10,10 },{ 11,7 },{ 11,8 },{ 11,9 },{ 12,8 } };

	for (auto& i : SEA)
	{
		int x = i[0] - 1;
		int y = i[1] - 1;
		cells[x][y] = new Cell(x, y, Type::SEA);
		if (!TheOption.GetQuiet()) cells[x][y]->Draw();

	}
	//initialize the SEA cells

	//FlgA: 11, 1
	cells[10][0] = new Cell(10, 0, Type::FlgA);
	if (!TheOption.GetQuiet()) cells[10][0]->Draw();

	//FlgB: 2,13

	cells[1][12] = new Cell(1, 12, Type::FlgB);
	if (!TheOption.GetQuiet()) cells[1][12]->Draw();

}

void Board::RandomSoldiers()
{

	int cur = 1;
	int x, y;
	int AsoldierNumber[] = { 1, 2, 3 };
	srand(unsigned int(time(time_t(0))));

	soldiers[0] = new Soldier(1, Side::SideA, false, false);
	soldiers[1] = new Soldier(2, Side::SideA, true, true);
	soldiers[2] = new Soldier(3, Side::SideA, true, false);
	soldiers[3] = new Soldier(7, Side::SideB, true, true);
	soldiers[4] = new Soldier(8, Side::SideB, false, true);
	soldiers[5] = new Soldier(9, Side::SideB, false, false);

	for (int i = 0; i < 6; i++)
	{
		bool isFinised = false;
		do {
			x = rand() % 13;
			y = (rand() % 5) + (i >= 3 ? 8 : 0);
			if (cells[x][y] == nullptr)
			{
				isFinised = true;
				cells[x][y] = new Cell(x, y, soldiers[i]);
				cells[x][y]->SetType((Type)((int)Type::S1 + i));
				soldiers[i]->SetSoldierCell(cells[x][y]);
				if (!TheOption.GetQuiet()) cells[x][y]->Draw();
			}
		} while (isFinised != true);
	}
}

bool Board::IsDeadArmy(Side _side)
{
	int counter = 0;
	int i = (_side == Side::SideA ? 0 : 3);
	for (int j = i; j < i + 3; j++)
	{
		if (soldiers[j] == nullptr)
			counter++;
	}
	return counter == 3 ? true : false;
}

Side Board::IsFlagCatched()
{
	if ((cells[FlgAPos.first][FlgAPos.second]->GetSoldier() != nullptr) && cells[FlgAPos.first][FlgAPos.second]->GetSoldier()->GetSoliderSide() == Side::SideB)
	{

		return Side::SideB;
	}
	else if ((cells[FlgBPos.first][FlgBPos.second]->GetSoldier() != nullptr) && cells[FlgBPos.first][FlgBPos.second]->GetSoldier()->GetSoliderSide() == Side::SideA)
	{

		return Side::SideA;
	}
	return Side::NoSide;
}
Side Board::isAnySideWon()
{
	Side result = Side::NoSide;
	if ((result = IsFlagCatched()) != Side::NoSide) {}
	else if (IsDeadArmy(Side::SideA)) result = Side::SideB;
	else if (IsDeadArmy(Side::SideB)) result = Side::SideA;

	return result;
}
void Board::MoveSoldierHelper(Soldier*& sol, int sourceX, int sourceY, int targetX, int targetY)
{
	//if move to new cell secsseeded then return true
	Cell *& TargetCell = cells[targetX][targetY];
	Cell *& sourceCell = cells[sourceX][sourceY];

	Move soldier_input = Move::NoMove;

	if (TargetCell == nullptr)
	{
		TargetCell = new Cell(targetX, targetY, Type::Regular);
		soldier_input = sol->MoveSoldier(TargetCell);
	}
	else
		switch (TargetCell->GetType())
		{
		case Type::SEA:
			if (sol->GetIsOnWater() == true)
				soldier_input = sol->MoveSoldier(TargetCell);
			break;
		case Type::FR:
			if (sol->GetIsInForest() == true)
				soldier_input = sol->MoveSoldier(TargetCell);
			break;
		case Type::FlgA:
			if (sol->GetSoliderSide() == Side::SideB)
				soldier_input = sol->MoveSoldier(TargetCell);
			break;
		case Type::FlgB:
			if (sol->GetSoliderSide() == Side::SideA)
				soldier_input = sol->MoveSoldier(TargetCell);
			break;
		case Type::Regular:
		case Type::S1:
		case Type::S2:
		case Type::S3:
		case Type::S7:
		case Type::S8:
		case Type::S9:
			soldier_input = sol->MoveSoldier(TargetCell);
			break;
		}




	if (soldier_input != Move::NoMove && soldier_input != Move::Failed)
	{
		if (!TheOption.GetQuiet()) sourceCell->Draw();
		if (!TheOption.GetQuiet()) TargetCell->Draw();
	}

	if (soldier_input != Move::OK && soldier_input != Move::Failed && soldier_input != Move::NoMove) //dead soldier 
	{
		int index = ((int)soldier_input < 6) ? ((int)soldier_input - 1) : ((int)soldier_input - 4);
		//if (sol->GetSoliderNumber() == soldiers[index]->GetSoliderNumber())
		//sol = nullptr;
		delete soldiers[index];
		soldiers[index] = nullptr;
	}

}
void Board::PlayerMove(Soldier *& sol, Direction dir)
{
	int x = sol->GetSoliderCell()->GetX();
	int y = sol->GetSoliderCell()->GetY();
	switch (dir)
	{
	case Direction::UP:
		if (0 < y) {
			MoveSoldierHelper(sol, x, y, x, y - 1);
		}
		break;
	case Direction::DOWN:
		if (y < 12) {
			MoveSoldierHelper(sol, x, y, x, y + 1);
		}
		break;
	case Direction::LEFT:
		if (0 < x) {
			MoveSoldierHelper(sol, x, y, x - 1, y);
		}
		break;
	case Direction::RIGHT:
		if (x < 12) {
			MoveSoldierHelper(sol, x, y, x + 1, y);
		}
		break;
	}
}