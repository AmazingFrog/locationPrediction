#ifndef _MARKOV_TRANSFER_MATRIX_H_
#define _MARKOV_TRANSFER_MATRIX_H_

#include <memory>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <unordered_map>


namespace shochuAlgorithm {
    //所有的虚函数都应该重写
    class MarkovTransferMatrix {
    public:
        MarkovTransferMatrix() = default;
        MarkovTransferMatrix(const unsigned int newN) :n(newN) {}
        virtual ~MarkovTransferMatrix() {};
        
        virtual MarkovTransferMatrix& operator=(MarkovTransferMatrix& a) { return *this; };
        virtual void copyTo(MarkovTransferMatrix& dest) const {};
        virtual MarkovTransferMatrix& create(const unsigned int _n) {};

        /**
        * @brief   自乘nn次
        * @param   nn 自乘的次数
        * @return  自乘的结果
        */
        virtual MarkovTransferMatrix power(unsigned int nn) const { return *this; };

        /**
        * @brief   返回矩阵在x行y列的值(0-base)
        * @param   x y
        * @return  [x][y]
        */
        virtual double at(const unsigned int x, const unsigned int y) { return 0.0; };

        unsigned int getN() { return this->n; }
    protected:
        unsigned int n;
    };


    //二维矩阵
    class MarkovTransferMatrix_2DMatrix : public MarkovTransferMatrix{
    public:
        MarkovTransferMatrix_2DMatrix() = default;
        MarkovTransferMatrix_2DMatrix(const unsigned int newN);
        MarkovTransferMatrix_2DMatrix(const MarkovTransferMatrix_2DMatrix& a);
        MarkovTransferMatrix_2DMatrix& operator=(MarkovTransferMatrix& a) override;

        MarkovTransferMatrix_2DMatrix& create(const unsigned int _n)override;

        void copyTo(MarkovTransferMatrix& dest) const override;

        float* operator[](const unsigned int row) const;
        
        MarkovTransferMatrix power(const unsigned int nn) const override;
    private:
        //矩阵 n*n
        //元素默认值为1
        std::unique_ptr<float*> mat;

        void mul(const MarkovTransferMatrix_2DMatrix& a);
    };

    //稀疏矩阵
    class MarkovTransferMatrix_spzrseMatrix : public MarkovTransferMatrix{
    public:
        MarkovTransferMatrix_spzrseMatrix() = default;
        MarkovTransferMatrix_spzrseMatrix(const unsigned int newN):MarkovTransferMatrix(newN){}
        MarkovTransferMatrix_spzrseMatrix(const MarkovTransferMatrix_spzrseMatrix& a);
        MarkovTransferMatrix_spzrseMatrix& operator=(MarkovTransferMatrix& a) override;

        MarkovTransferMatrix& create(const unsigned int _n);
        void copyTo(MarkovTransferMatrix& dest) const override;

        MarkovTransferMatrix power(unsigned int nn) const override;

        double at(const unsigned int x, const unsigned int y) override;
	private:
		std::unordered_map<std::pair<unsigned int, unsigned int>, double> mat;
    };
}

#endif // _MARKOV_TRANSFER_MATRIX_H_

