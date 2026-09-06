//#include <iostream>
//#include <string>
//#include <string_view>
//#include <vector>
//#include <algorithm>
//
//void printCharGrid(std::string_view str) {
//	for (int i{}; i < 10; ++i) {
//		for (int j{}; j < str.length(); ++j) {
//			std::cout << std::string(20, str[j]);
//		}
//		std::cout << '\n';
//	}
//}
//
//int main()
//{
//	std::string chars{ 33 };
//	int p{};
//
//	for (char i{ 34 }; i < 127; ++i) {
//		int p = 0;
//		bool accepted{};
//		while (!accepted) {
//			std::string str;
//			if (p - 1 >= 0) str.append(1, chars[p - 1]);
//			str.append(1, i);
//			if (p <= chars.size()) str.append(1, chars[p]);
//			printCharGrid(str);
//
//			std::cout << "Place charater [" << i << "]";
//			char c{};
//			std::cin >> c;
//
//			if (c == 'a') {
//				p = p == 0 ? 0 : p - 1;
//			}
//			else if (c == 'd') {
//				p = p == chars.size() ? p : p + 1;
//			}
//			else if (c == 'x') {
//				accepted = true;
//			}
//			else if (c == 'w') {
//				if (p == chars.size()) {
//					chars.append(1, i);
//				}
//				else {
//					chars.insert(p, 1, i);
//				}
//				accepted = true;
//			}
//		}
//	}
//
//	std::cout << "\n\n\n" << chars;
//
//	return 0;
//}