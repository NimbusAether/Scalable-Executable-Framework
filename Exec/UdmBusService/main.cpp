#include<string>
#include<iostream>
#include "UdmService.h"

#ifdef VLD_DEBUG
#include <vld.h>
#endif
int main(int argc, char* argv[])
{
	std::cout << "running test." << std::endl;
	int i = 0;
	int *j = new int;
	do {
		//std::cout << "ÇëÊäÈë: 'exit' ÍË³ö³ÌÐò";
		//std::string in;
		//std::cin >> in;
		//if (in.compare("exit") == 0)
		//{
		//	break;
		//}
		//_sleep(50);
	} while (i++ < 60);
	return 0;
}