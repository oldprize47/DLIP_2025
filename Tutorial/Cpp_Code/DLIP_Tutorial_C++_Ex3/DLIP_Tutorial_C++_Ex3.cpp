#include "../../IncludeDLIP/TU_DLIP.h"
//#include "TU_DLIP.h"
using namespace proj_A;

void main()
{
	MyNum mynum1(1, 2, 3);
	proj_B::MyNum mynum2(4, 5, 6);

	mynum1.print();
	mynum2.print();

	system("pause");
}