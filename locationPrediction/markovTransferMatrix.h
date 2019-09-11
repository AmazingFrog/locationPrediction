#ifndef _MARKOV_TRANSFER_MATRIX_H_
#define _MARKOV_TRANSFER_MATRIX_H_

#define FLOAT_THRESOULT 0.00001f

#include <cmath>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <functional>
#include <unordered_map>

namespace shochuAlgorithm {
    struct hashVal {
        size_t operator()(const std::pair<unsigned int, unsigned int>& a) const{
            std::hash<unsigned int> uintHash;
            return uintHash(uintHash(a.first) + uintHash(a.second));
        }
    };

    struct hashValEqual {
        bool operator()(const std::pair<unsigned int, unsigned int>& a, const std::pair<unsigned int, unsigned int>& b) const{
            return (a.first == b.first) && (a.second == b.second);
        }
        
    };

    //所有的虚函数都应该重写
    class MarkovTransferMatrix {
    public:
        MarkovTransferMatrix() :n(-1) {}
        MarkovTransferMatrix(const unsigned int newN) :n(newN) {}
        virtual ~MarkovTransferMatrix() {}
        
        virtual MarkovTransferMatrix& operator=(const MarkovTransferMatrix& a) { this->n = a.n; return *this; }
        virtual void copyTo(MarkovTransferMatrix& dest) const {}
        virtual MarkovTransferMatrix& create(const unsigned int _n) { return *this; }

        /**
        * @brief   自乘nn次
        * @param   nn 自乘的次数
        * @return  自乘的结果
        */
        virtual MarkovTransferMatrix& power(const unsigned int nn) { return *this; }

        /**
        * @brief   返回矩阵在x行y列的值(0-base)
        * @param   x y
        * @return  [x][y]
        */
        virtual float& at(const unsigned int x, const unsigned int y) { return this->stub; }
        virtual const float& at(const unsigned int x, const unsigned int y) const { return this->stub; }
        virtual void clear() {}
        
        void setN(unsigned int _n) { this->n = _n; }
        unsigned int getN() const { return this->n; }
    protected:
        unsigned int n;
        float stub;
    };

    //二维矩阵 
    class MarkovTransferMatrix_2DMatrix : public MarkovTransferMatrix {
    public:
        MarkovTransferMatrix_2DMatrix() = default;
        MarkovTransferMatrix_2DMatrix(const unsigned int newN);
        MarkovTransferMatrix_2DMatrix(const MarkovTransferMatrix_2DMatrix& a);
        ~MarkovTransferMatrix_2DMatrix() { this->clear(); }
        MarkovTransferMatrix_2DMatrix& operator=(const MarkovTransferMatrix& a) override;
        MarkovTransferMatrix_2DMatrix& create(const unsigned int _n) override;
        void copyTo(MarkovTransferMatrix& dest) const override;
        float& at(const unsigned int x, const unsigned int y) override;
        const float& at(const unsigned int x, const unsigned int y) const override;
        MarkovTransferMatrix_2DMatrix& power(const unsigned int nn) override;
        void clear() override;

        float* operator[](const unsigned int row) const;
    private:
        //矩阵 n*n
        //元素默认值为1 
        std::unique_ptr<float[]> mat;

        void mul(const MarkovTransferMatrix& a);
        static std::unique_ptr<float[]> makeUnique(const unsigned int nn);
    };

    //稀疏矩阵 
    class MarkovTransferMatrix_spzrseMatrix : public MarkovTransferMatrix {
    public:
        MarkovTransferMatrix_spzrseMatrix() = default;
        MarkovTransferMatrix_spzrseMatrix(const unsigned int newN):MarkovTransferMatrix(newN){}
        MarkovTransferMatrix_spzrseMatrix(const MarkovTransferMatrix_spzrseMatrix& a);
        ~MarkovTransferMatrix_spzrseMatrix(){}
        MarkovTransferMatrix_spzrseMatrix& operator=(const MarkovTransferMatrix& a) override;
        MarkovTransferMatrix& create(const unsigned int _n) override;
        void copyTo(MarkovTransferMatrix& dest) const override;
        MarkovTransferMatrix_spzrseMatrix& power(const unsigned int nn) override;
        float& at(const unsigned int x, const unsigned int y) override;
        const float& at(const unsigned int x, const unsigned int y) const override;
        void clear() override;

        void cleanInvaildVal();
	private:
		std::unordered_map<std::pair<unsigned int, unsigned int>, 
                            float, 
                            shochuAlgorithm::hashVal,
                            shochuAlgorithm::hashValEqual
        > mat;
        void mul(const MarkovTransferMatrix& a);
    };
}

#endif // _MARKOV_TRANSFER_MATRIX_H_

