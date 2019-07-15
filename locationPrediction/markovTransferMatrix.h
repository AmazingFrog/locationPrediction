#ifndef _MARKOV_TRANSFER_MATRIX_H_
#define _MARKOV_TRANSFER_MATRIX_H_

#include <memory>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>

class MarkovTransferMatrix {
public:
	MarkovTransferMatrix() = default;
	MarkovTransferMatrix(const unsigned int& newN);
	MarkovTransferMatrix(const MarkovTransferMatrix& a);

	MarkovTransferMatrix& operator=(const MarkovTransferMatrix& a);

	MarkovTransferMatrix& create(const unsigned int _n);

	void copyTo(MarkovTransferMatrix& dest) const;

	float* operator[](const unsigned int row) const;
	float* operator[](const int row) const;

	/**
	* @brief   自乘nn次
	* @param   nn 自乘的次数
	* @return  自乘的结果
	*/
	MarkovTransferMatrix power(const unsigned int nn) const;
private:
	//矩阵 n*n
	//元素默认值为1
	std::unique_ptr<float*> mat;

	//每行个数
	unsigned int n = 0;

	void mul(const MarkovTransferMatrix& a);
};

#endif // _MARKOV_TRANSFER_MATRIX_H_

