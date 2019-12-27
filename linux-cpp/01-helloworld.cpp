#include <iostream>

using namespace std;

int main(){
#ifdef MY_DEBUG
	std::cout<<"hello world"<<endl;
#endif
}
