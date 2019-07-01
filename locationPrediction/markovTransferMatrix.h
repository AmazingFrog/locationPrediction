#ifndef _MARKOV_TRANSFER_MATRIX_H_
#define _MARKOV_TRANSFER_MATRIX_H_

#include <cstdlib>
#include <memory>
#include <cassert>
#include <iostream>

class markovTransferMatrix {
private:
	//矩阵 n*n
	//元素默认值为1
	std::unique_ptr<float*> mat = nullptr;

	//每行个数
	unsigned int n = 0;

	void copyTo(markovTransferMatrix& dest) const;
	void mul(const markovTransferMatrix& a);
public:
	markovTransferMatrix() = default;
	markovTransferMatrix(const unsigned int& newN);

	float* operator[](const unsigned int& row) const;
	float* operator[](const int& row) const;

	/**
	* @brief   自乘nn次
	* @para    nn 自乘的次数
	* @return  自乘的结果
	*/
	markovTransferMatrix power(const unsigned int& nn) const;
};

#endif // _MARKOV_TRANSFER_MATRIX_H_

