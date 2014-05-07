/*
 * test_unordered_map.cpp
 *
 *  Created on: 7 May 2014
 *      Author: ncl
 */

#include <unordered_map>

int main(int argc, char* argv[]){
	std::unordered_map<int,int> m;

	m.insert(std::pair<int,int>(1, 1));
}
