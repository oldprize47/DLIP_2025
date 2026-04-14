#include "../../../Include/TU_DLIP.h"
//#include "TU_DLIP.h"

void main()
{
	proj_A::MyNum mynum1(1, 2, 3);
	proj_B::MyNum mynum2(4, 5, 6);

	mynum1.print();
	mynum2.print();

	system("pause");
}

//#include <iostream>
//
//using namespace std;
//
//int add_value(int A = 10, int B = 5) {
//    int result = A + B;
//
//    cout << "result = " << result << endl;
//    return result;
//}
//
//int main() {
//
//    add_value();
//
//	system("pause");
//    return 0;
//}