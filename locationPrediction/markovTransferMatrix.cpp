#include "MarkovTransferMatrix.h"

MarkovTransferMatrix::MarkovTransferMatrix(const unsigned int& newN) {
	this->create(newN);
}

MarkovTransferMatrix::MarkovTransferMatrix(const MarkovTransferMatrix& a) {
	*this = a;
}

MarkovTransferMatrix& MarkovTransferMatrix::create(const unsigned int _n) {
	this->n = _n;

	//检查内存是否分配成功
	try {
		this->mat = std::make_unique<float*>(new float[this->n*this->n]);
        memset(*this->mat, 0, this->n*this->n * sizeof(float));
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

MarkovTransferMatrix& MarkovTransferMatrix::operator=(const MarkovTransferMatrix& a) {
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

float* MarkovTransferMatrix::operator[](const unsigned int row) const {
	return &((*(this->mat))[row*this->n]);
}

float* MarkovTransferMatrix::operator[](const int row) const {
	return this->operator[](static_cast<const unsigned int>(row));
}

void MarkovTransferMatrix::copyTo(MarkovTransferMatrix& dest) const {
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

MarkovTransferMatrix MarkovTransferMatrix::power(const unsigned int nn) const {
	MarkovTransferMatrix ret(this->n);
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

void MarkovTransferMatrix::mul(const MarkovTransferMatrix& a) {
	assert(this->n == a.n && "n must be the same");
	MarkovTransferMatrix res(this->n);

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
