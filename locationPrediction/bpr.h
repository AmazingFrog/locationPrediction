#ifndef _BPR_H_
#define _BPR_H_
#define __DEBUG__

#include <vector>
#include <list>
#include <cmath>
#include <ctime>
#include <string>
#include <random>
#include <memory>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <algorithm>

#define BPR_CHICK_IN_NO       0
#define BPR_CKICK_IN_YES      1


namespace shochuAlgorithm {
	namespace BPR {
		struct Triad {
			int u;
			int i;
			int j;
			Triad() = default;
			Triad(int _u, int _i, int _j) :u(_u), i(_i), j(_j) {}
		};


		class Bpr_impl {
		public:
			Bpr_impl() = default;

			/**
			* @brief             创建bpr实例,该构造函数与add函数仅调用其中之一
			* @param data        指向数据的指针,data[i*items+j]为BPR_CKICK_IN_YES表示用户i chick in 了j物品
			* @param users       用户数量
			* @param items       物品数量
			*/
			Bpr_impl(const int* data, const unsigned int users, const unsigned int items);
			void add(const int* data, const unsigned int users, const unsigned int items);

            void setUser(unsigned int user) {
                this->users = user;
            }
            void setItem(unsigned int items) {
                this->items = items;
            }
            void setUserItem(unsigned int users, unsigned int items) {
                this->setUser(users);
                this->setItem(items);
            }

			/**
			* @brief         添加数据
			* @param triad   u用户,i物品标号,j物品标号,表示用户u喜欢i物品大于喜欢j物品
			*/
			void add(const Triad& a);
			void add(Triad&& a);
            void add(Triad a);
			void add(const unsigned int u, const unsigned int i, const unsigned int j);

			/**
			* @brief           训练数据
			* @param lambda    模型参数,正则化参数
			* @param learnRate 学习速率
			* @param step      迭代次数
			* @param k         分解维度
			* @param threshold 收敛阈值
			*/
			void train(const float lambda, const float learnRate = 0.01, const unsigned int step = 100, const unsigned int k = 20, const float threshold = 0.00001);

			/**
			* @brief       推荐用户uesr前n个物品
			* @param user  用户编号
			* @param n     推荐的物品个数
			* @return      pair.first为物品标号,pair.second为用户user为物品的打分
			*/
			std::list<std::pair<int, float> > predict(const unsigned int user, const unsigned int n) const;

			/**
			* @brief       预测uesr对item的打分
			* @param user  用户标号
			* @param item  物品标号
			*/
			float predictUserItem(const unsigned int user, const unsigned int item) const;

			/**
			* @brief    设置每个用户的训练集大小,
			* @param a  新的大小
			*/
			void setTrainSetSize(const unsigned int a);

			/**
			* @brief          浠庢枃浠跺垵濮嬪寲users-items鐭╅樀
			* @param fileName 璇诲彇鐨勬枃浠跺悕
			*/
			void loadData(const char* fileName);
			void loadData(const std::string& fileName);

			/**
			* @brief          从文件初始化users-items矩阵
			* @param fileName 读取的文件名
			*/
			void loadModel(const char* fileName);
			void loadModel(const std::string& fileName);

			/**
			* @brief          读取w h矩阵
			* @param fileName 读取的文件名
			*/
			void saveModel(const char* fileName) const;
			void saveModel(const std::string& fileName) const;

		private:
			bool isAlreadAdd = false;
			std::list<Triad> trainSet;

			//用户 users*k
			std::unique_ptr<float[]> w;
			//物品 k*items
            std::unique_ptr<float[]> h;
			//w*h users*items
            std::unique_ptr<float[]> x;
			//每个用户的训练集最大为 times*items
			//总的训练集大小为 users*items*times
            unsigned int times = 50;
			//用户数量
            unsigned int users;
			//物品数量
            unsigned int items;
			//隐藏维度
            unsigned int k;
            
            std::unique_ptr<float[]> makeUnique(unsigned int len) {
                return std::unique_ptr<float[]>(new float[len]);
            }
		};
	}
}


#endif // !_BPR_H_

