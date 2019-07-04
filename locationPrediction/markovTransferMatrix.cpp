#include "markovTransferMatrix.h"

markovTransferMatrix::markovTransferMatrix(const unsigned int& newN) {
	this->create(newN);
}

markovTransferMatrix::markovTransferMatrix(const markovTransferMatrix& a) {
	*this = a;
}

markovTransferMatrix& markovTransferMatrix::create(const unsigned int _n) {
	this->n = _n;

	//检查内存是否分配成功
	try {
		this->mat = std::make_unique<float*>(new float[this->n*this->n]);
		for (int i = 0; i < this->n; ++i) {
			for (int j = 0; j < this->n; ++j) {
				(*this->mat)[i*this->n + j] = 1;
			}
		}
	}
	catch (std::bad_alloc& error) {
		std::cerr << "memory allocation error" << std::endl;
		std::cerr << error.what() << std::endl;
		abort();
	}
	catch (...) {
		std::cerr << "unknow excpetion" << std::endl;
		abort();
	}
}

markovTransferMatrix& markovTransferMatrix::operator=(const markovTransferMatrix& a) {
	this->n = a.n;

	try {
		this->mat = std::make_unique<float*>(new float[this->n*this->n]);
	}
	catch (std::bad_alloc& error) {
		std::cerr << "memory allocation error" << std::endl;
		std::cerr << error.what() << std::endl;
		abort();
	}
	catch (...) {
		std::cerr << "unknow excpetion" << std::endl;
		abort();
	}

	memcpy(*(this->mat), *(a.mat), this->n*this->n * sizeof(float));
}

float* markovTransferMatrix::operator[](const unsigned int row) const {
	return &((*(this->mat))[row*this->n]);
}

float* markovTransferMatrix::operator[](const int row) const {
	return this->operator[](static_cast<const unsigned int>(row));
}

void markovTransferMatrix::copyTo(markovTransferMatrix& dest) const {
	dest.n = this->n;
	if (dest.mat == nullptr) {
		try {
			dest.mat = std::make_unique<float*>(new float[this->n*this->n]);
		}
		catch (std::bad_alloc& error) {
			std::cerr << "memory allocation error" << std::endl;
			std::cerr << error.what() << std::endl;
			abort();
		}
		catch (...) {
			std::cerr << "unknow excpetion" << std::endl;
			abort();
		}
	}
	memcpy(*(dest.mat), *this->mat, this->n*this->n * sizeof(float));
}

markovTransferMatrix markovTransferMatrix::power(const unsigned int nn) const {
	markovTransferMatrix ret(this->n);
	if (nn == 0) {
		return ret;
	}
	if (nn == 1) {
		this->copyTo(ret);
		return ret;
	}
	
	ret = *this;
	if (nn & 1) {
		ret.mul(ret);
		ret.power(nn / 2);
		ret.mul(*this);
	}
	else {
		ret.mul(ret);
		ret.power(nn / 2);
	}

	return ret;
}

void markovTransferMatrix::mul(const markovTransferMatrix& a) {
	assert(this->n == a.n && "n must be the same");
	markovTransferMatrix res(this->n);

	for (int i = 0; i < this->n; ++i) {
		for (int j = 0; j < this->n; ++j) {
			res[i][j] = 0;
			for (int k = 0; k < this->n; ++k) {
				res[i][j] += ((*this)[i][k] * a[k][j]);
			}
		}
	}

	res.copyTo(*this);
}
