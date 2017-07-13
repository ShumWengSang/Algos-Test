#include "TowerOfHanoi.hpp"

void main()
{
	int number_of_disks = 5;
	TowerOfHanoi tower(number_of_disks);
	tower.Solve_Hanoi();
}