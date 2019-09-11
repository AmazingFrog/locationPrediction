#include "MarkovTransferMatrix.h"

using namespace shochuAlgorithm;

//二维矩阵
MarkovTransferMatrix_2DMatrix::MarkovTransferMatrix_2DMatrix(const unsigned int newN) : MarkovTransferMatrix(newN) {
	this->create(newN);
}

MarkovTransferMatrix_2DMatrix::MarkovTransferMatrix_2DMatrix(const MarkovTransferMatrix_2DMatrix& a) : MarkovTransferMatrix(a.n) {
	*this = static_cast<MarkovTransferMatrix>(a);
}

MarkovTransferMatrix_2DMatrix& MarkovTransferMatrix_2DMatrix::create(const unsigned int _n) {
	this->n = _n;

	//检查内存是否分配成功
	try {
		this->mat = MarkovTransferMatrix_2DMatrix::makeUnique(this->n);
        memset(this->mat.get(), 0, this->n*this->n * sizeof(float));
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

    return *this;
}

MarkovTransferMatrix_2DMatrix& MarkovTransferMatrix_2DMatrix::operator=(const MarkovTransferMatrix& a) {
    this->clear();
    this->n = a.getN();

	try {
		this->mat = MarkovTransferMatrix_2DMatrix::makeUnique(this->n);
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

    for (int i = 0; i < this->n; ++i){
        for (int j = 0; j < this->n; ++j){
            this->mat.get()[i*this->n + j] = a.at(i, j);
        }
    }

    return *this;
}

float* MarkovTransferMatrix_2DMatrix::operator[](const unsigned int row) const {
    return &(this->mat.get()[row * this->n]);
}

void MarkovTransferMatrix_2DMatrix::copyTo(MarkovTransferMatrix& dest) const {
    dest.clear();
    dest.create(this->n);
    for (int i = 0; i < this->n; ++i) {
        for (int j = 0; j < this->n; ++j) {
            dest.at(i, j) = (*this)[i][j];
        }
    }
}

float& MarkovTransferMatrix_2DMatrix::at(const unsigned int x, const unsigned int y) {
    return (*this)[x][y];
}
const float& MarkovTransferMatrix_2DMatrix::at(const unsigned int x, const unsigned int y) const {
    return (*this)[x][y];
}

MarkovTransferMatrix_2DMatrix& MarkovTransferMatrix_2DMatrix::power(const unsigned int nn)  {
	MarkovTransferMatrix_2DMatrix ret(this->n);
	if (nn == 0) {
        unsigned int nn = this->n;
        this->clear();
        return this->create(nn);
	}
	if (nn == 1) {
		this->copyTo(ret);
		return ret;
	}
	
    this->copyTo(ret);
	if (nn & 1) {
		ret.mul(ret);
		ret.power(nn / 2);
		ret.mul(*this);
	}
	else {
		ret.mul(ret);
		ret.power(nn / 2);
	}

    ret.copyTo(*this);
	return *this;
}

void MarkovTransferMatrix_2DMatrix::mul(const MarkovTransferMatrix& a) {
	//assert(this->n == a.getN() && "n must be the same");
	MarkovTransferMatrix_2DMatrix res(this->n);

	for (int i = 0; i < this->n; ++i) {
		for (int j = 0; j < this->n; ++j) {
			res[i][j] = 0;
			for (int k = 0; k < this->n; ++k) {
				res[i][j] += ((*this)[i][k] * a.at(k,j));
			}
		}
	}

	res.copyTo(*this);
}

std::unique_ptr<float[]> MarkovTransferMatrix_2DMatrix::makeUnique(const unsigned int nn){
    return std::unique_ptr<float[]>(new float[nn * nn]);
}

void MarkovTransferMatrix_2DMatrix::clear() {
    if (this->mat.get() != nullptr) {
        delete[] this->mat.release();
    }
    this->n = 0;
}

//稀疏矩阵
MarkovTransferMatrix_spzrseMatrix::MarkovTransferMatrix_spzrseMatrix(const MarkovTransferMatrix_spzrseMatrix& a)
{
    *this = a;
}

MarkovTransferMatrix_spzrseMatrix& MarkovTransferMatrix_spzrseMatrix::operator=(const MarkovTransferMatrix& a)
{
    this->n = a.getN();
    this->clear();

    for (int i = 0; i < this->n; ++i) {
        for (int j = 0; j < this->n; ++j) {
            if (std::fabs(a.at(i, j) - 0) > FLOAT_THRESOULT) {
                this->mat.emplace(std::pair<unsigned int, unsigned int>(i, j), a.at(i, j));
            }
        }
    }

    this->cleanInvaildVal();
    return *this;
}

MarkovTransferMatrix& MarkovTransferMatrix_spzrseMatrix::create(const unsigned int _n){
    this->n = _n;
    return *this;
}

void MarkovTransferMatrix_spzrseMatrix::copyTo(MarkovTransferMatrix& dst) const{
    dst.clear();
    dst.create(this->n);
    for (auto i = this->mat.begin(); i != this->mat.end(); ++i) {
        dst.at(i->first.first, i->first.second) = i->second;
    }
}

float& MarkovTransferMatrix_spzrseMatrix::at(const unsigned int x, const unsigned int y) {
    if (this->mat.find(std::pair<unsigned int, unsigned int>(x, y)) == this->mat.end()) {
        this->mat.emplace(std::pair<unsigned int, unsigned int>(x, y), 0.0);
    }
    return this->mat.at(std::pair<unsigned int, unsigned int>(x, y));
}

const float& MarkovTransferMatrix_spzrseMatrix::at(const unsigned int x, const unsigned int y) const {
    if (this->mat.find(std::pair<unsigned int, unsigned int>(x, y)) == this->mat.end()) {
        return 0.0;
    }
    else {
        return this->mat.at(std::pair<unsigned int, unsigned int>(x, y));
    }    
}

MarkovTransferMatrix_spzrseMatrix& MarkovTransferMatrix_spzrseMatrix::power(const unsigned int nn) {
    MarkovTransferMatrix_spzrseMatrix ret(this->n);

    if (nn == 0) {
        this->clear();
        return *this;
    }
    if (nn == 1) {
        return *this;
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

    *this = ret;
    return *this;
}

void MarkovTransferMatrix_spzrseMatrix::cleanInvaildVal() {
    for (auto i = this->mat.begin(); i != this->mat.end(); ++i) {
        if (std::fabs(i->second - 0.0) <= FLOAT_THRESOULT) {
            this->mat.erase(i);
        }
    }
}

void MarkovTransferMatrix_spzrseMatrix::mul(const MarkovTransferMatrix& a) {
    //assert(this->n == a.getN() && "n must be same.");
    MarkovTransferMatrix_spzrseMatrix ret(this->n);

    for (int i = 0; i < this->n; ++i) {
        for (int j = 0; j < this->n; ++j) {
            for (int k = 0; k < this->n; ++k) {
                ret.at(i, j) += (this->at(i, k) * a.at(k, j));
            }
        }
    }

    ret.cleanInvaildVal();
    ret.copyTo(*this);
}

void MarkovTransferMatrix_spzrseMatrix::clear() {
    this->mat.clear();
}
