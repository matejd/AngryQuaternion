all:
	clang DualQuaternionTests.cpp -o DualQuaternionTests -g3 -Wall -std=c++11 -I. -I../../extensions/include/ -L./gtest/ -lgtest -lstdc++ -lpthread -lm
