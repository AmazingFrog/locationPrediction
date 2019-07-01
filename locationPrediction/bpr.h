#ifndef _BPR_H_
#define _BRP_H_
#define __DEBUG__

#include <cmath>
#include <ctime>
#include <string>
#include <random>
#include <vector>
#include <memory>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <algorithm>

#define BPR_CHICK_IN_NO       0
#define BPR_CKICK_IN_YES      1


namespace bpr {
	struct triad {
		int u;
		int i;
		int j;
		triad() = default;
		triad(int _u,int _i,int _j):u(_u),i(_i),j(_j){}
	};


	class bpr_impl {
	public:
		bpr_impl() = default;

		/**
		* @brief            创建bpr实例,该构造函数与add函数仅调用其中之一
		* @para data        指向数据的指针,data[i*items+j]为BPR_CKICK_IN_YES表示用户i chick in 了j物品
		* @para users       用户数量
		* @para items       物品数量
		*/
		bpr_impl(const int* data, const unsigned int users, const unsigned int items);
		void add(const int* data, const unsigned int users, const unsigned int items);

		/**
		* @brief        添加数据
		* @para triad   u用户,i物品标号,j物品标号,表示用户u喜欢i物品大于喜欢j物品
		*/
		void add(const triad a);

		/**
		* @brief          训练数据
		* @para lambda    模型参数,正则化参数
		* @para learnRate 学习速率
		* @para step      迭代次数
		* @para k         分解维度
		* @para threshold 收敛阈值
		*/
		void train(const double lambda, const double learnRate = 0.01, const unsigned int step = 100, const unsigned int k = 20,const double threshold = 0.00001);
		
		/**
		* @brief      推荐用户uesr前n个物品
		* @para user  用户编号
		* @para n     推荐的物品个数
		* @return     pair.first为物品标号,pair.second为用户user为物品的打分
		*/
		std::vector<std::pair<int,double> > predict(const unsigned int user,const unsigned int n);

		/**
		* @brief      预测uesr对item的打分
		* @para user  用户标号
		* @para item  物品标号
		*/
		double predictUserItem(const unsigned int user, const unsigned int item);

		/**
		* @brief   设置每个用户的训练集大小,
		* @para a  新的大小
		*/
		void setTrainSetSize(const unsigned int a);

		/**
		* @brief         从文件初始化users-items矩阵
		* @para fileName 读取的文件名
		*/
		void loadData(const char* fileName);
		void loadData(const std::string& fileName);

		/**
		* @brief         读取w h矩阵
		* @para fileName 读取的文件名
		*/
		void loadModel(const char* fileName);
		void loadModel(const std::string& fileName);

		/**
		* @brief         保存w h矩阵到文件
		* @para fileName 保存的文件名
		*/
		void saveModel(const char* fileName);
		void saveModel(const std::string& fileName);

	private:
		bool isAlreadAdd = false;
		std::vector<triad> trainSet;

		//用户 users*k
		std::unique_ptr<double*> w;
		//物品 k*items
		std::unique_ptr<double*> h;
		//w*h users*items
		std::unique_ptr<double*> x;
		//每个用户的训练集最大为 times*items
		//总的训练集大小为 users*items*times
		unsigned int times = 50;
		//用户数量
		unsigned int users;
		//物品数量
		unsigned int items;
		//隐藏维度
		unsigned int k;
	};
}
#endif // !_BPR_H_
