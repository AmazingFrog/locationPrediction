/**
*  实例化该类前先对命名空间中的featureNum赋值
*
*
*
*
*
*
*/
#ifndef _LOGISTICS_REGRESSION_H_
#define _LOGISTICS_REGRESSION_H_

#include <cassert>
#include <cmath>
#include <vector>
#include <initializer_list>

namespace shochuAlgorithm {
	namespace LogisticsRegression {
		extern unsigned int featureNum;

		struct TrainNode {
			double trueVal;
			std::vector<double> feature;

			TrainNode() = default;
			TrainNode(double _predictVal, std::initializer_list<double> featureIL);

			void setPredictVal(double val) {
				this->trueVal = val;
			}
			void setFeatureVal(std::initializer_list<double> il) {
				assert(shochuAlgorithm::LogisticsRegression::featureNum != 0 && "shochuAlgorithm::LogisticsRegression::featureNum must not be 0");
				assert(shochuAlgorithm::LogisticsRegression::featureNum < il.size() && "too few parameter to initlize TrainNode");
				this->feature.assign(il);
				this->feature.resize(shochuAlgorithm::LogisticsRegression::featureNum);
			}
			void setFeatureVal(const std::vector<double>& _featureVal) {
				assert(shochuAlgorithm::LogisticsRegression::featureNum != 0 && "shochuAlgorithm::LogisticsRegression::featureNum must not be 0");
				assert(shochuAlgorithm::LogisticsRegression::featureNum < _featureVal.size() && "too few parameter to initlize TrainNode");
				this->feature = _featureVal;
			}
		};

		class LogisticsRegression_impl {
		public:
			LogisticsRegression_impl();
			LogisticsRegression_impl(std::vector<TrainNode>&& a);
			LogisticsRegression_impl(const std::vector<TrainNode>& a);

			/**
			* @brief 添加数据
			*/
			void add(TrainNode&& a);
			void add(const TrainNode& a);
			void add(std::vector<TrainNode>&& a);
			void add(const std::vector<TrainNode>& a);
			
			/**
			* @beief 训练
			*/
			std::vector<double>& train(double learnRate = 0.01);

			/**
			* @brief 预测
			*/
			double predict(const TrainNode& a);

		private:
			std::vector<TrainNode> trainSet;
			std::vector<double> model;

			void init();
		};
	}
}

#endif // _LOGISTICS_REGRESSION_H_
