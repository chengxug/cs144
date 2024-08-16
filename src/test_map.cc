#include <iostream>
#include <map>

int main(){
    std::map<int,char> test {{1, 'a'}, {2, 'b'}, {3, 'c'}, {4, 'd'}, {5, 'e'}};
    auto iter = test.find(6);
    if (iter == test.end()) {
    iter = std::prev(test.end());  
}

do {
    std::cout << iter->first << " " << iter->second << std::endl;
    // 当iter已经指向begin时，退出循环
    if (iter == test.begin()) {
        break;
    }
    --iter;
} while (true);
    return 0;
}