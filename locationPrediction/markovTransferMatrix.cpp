#include "MarkovTransferMatrix.h"

//二维矩阵
shochuAlgorithm::MarkovTransferMatrix_2DMatrix::MarkovTransferMatrix_2DMatrix(const unsigned int newN) : MarkovTransferMatrix(newN) {
	this->create(newN);
}

shochuAlgorithm::MarkovTransferMatrix_2DMatrix::MarkovTransferMatrix_2DMatrix(const MarkovTransferMatrix_2DMatrix& a) : MarkovTransferMatrix(a.n) {
	*this = static_cast<shochuAlgorithm::MarkovTransferMatrix>(a);
}

shochuAlgorithm::MarkovTransferMatrix_2DMatrix& shochuAlgorithm::MarkovTransferMatrix_2DMatrix::create(const unsigned int _n) {
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

shochuAlgorithm::MarkovTransferMatrix_2DMatrix& shochuAlgorithm::MarkovTransferMatrix_2DMatrix::operator=(MarkovTransferMatrix& aa) {
    shochuAlgorithm::MarkovTransferMatrix_2DMatrix& a = dynamic_cast<shochuAlgorithm::MarkovTransferMatrix_2DMatrix&>(aa);
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
    return *this;
}

float* shochuAlgorithm::MarkovTransferMatrix_2DMatrix::operator[](const unsigned int row) const {
	return &((*(this->mat))[row*this->n]);
}

void shochuAlgorithm::MarkovTransferMatrix_2DMatrix::copyTo(MarkovTransferMatrix& dest) const {
    MarkovTransferMatrix_2DMatrix& dst = dynamic_cast<MarkovTransferMatrix_2DMatrix&>(dest);
    dst.n = this->n;
	if (dst.mat == nullptr) {
		try {
			dst.mat = std::make_unique<float*>(new float[this->n*this->n]);
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
	memcpy(*(dst.mat), *this->mat, this->n*this->n * sizeof(float));
}

shochuAlgorithm::MarkovTransferMatrix shochuAlgorithm::MarkovTransferMatrix_2DMatrix::power(const unsigned int nn) const {
	MarkovTransferMatrix_2DMatrix ret(this->n);
	if (nn == 0) {
		return ret;
	}
	if (nn == 1) {
		this->copyTo(ret);
		return ret;
	}
	
	ret = static_cast<shochuAlgorithm::MarkovTransferMatrix>(*this);
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

void shochuAlgorithm::MarkovTransferMatrix_2DMatrix::mul(const shochuAlgorithm::MarkovTransferMatrix_2DMatrix& a) {
	assert(this->n == a.n && "n must be the same");
	MarkovTransferMatrix_2DMatrix res(this->n);

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


//稀疏矩阵
