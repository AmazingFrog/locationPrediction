#include "logisticsRegression.h"

unsigned int shochuAlgorithm::LogisticsRegression::featureNum = 0;

shochuAlgorithm::LogisticsRegression::TrainNode::TrainNode(double _predictVal, std::initializer_list<double> featureIL) : trueVal(_predictVal){
    assert(shochuAlgorithm::LogisticsRegression::featureNum != featureIL.size() && "shochuAlgorithm::LogisticsRegression::featureNum must be same with initlize_lise.size()");
    this->setFeatureVal(featureIL);
}

shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::LogisticsRegression_impl() {
	this->init();
}
shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::LogisticsRegression_impl(std::vector<TrainNode>&& a) :trainSet(a){
	this->init();
}
shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::LogisticsRegression_impl(const std::vector<TrainNode>& a) :trainSet(a){
	this->init();
}
void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::init(){
	assert(shochuAlgorithm::LogisticsRegression::featureNum != 0 && "shochuAlgorithm::LogisticsRegression::featureNum must not be 0");
	this->model.assign(shochuAlgorithm::LogisticsRegression::featureNum, 0.0);
}

void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::add(const TrainNode& a) {
	this->trainSet.push_back(a);
}
void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::add(TrainNode&& a) {
	this->trainSet.push_back(a);
}
void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::add(std::vector<TrainNode>&& a) {
	this->trainSet.insert(this->trainSet.end(), a.begin(), a.end());
}
void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::add(const std::vector<TrainNode>& a) {
	this->trainSet.insert(this->trainSet.end(), a.begin(), a.end());
}

std::vector<double>& shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::train(double learnRate) {
	int cnt = 0;
	while (1)
	{
		double error = 0;
		for (int i = 0; i < shochuAlgorithm::LogisticsRegression::featureNum; i++)
		{
			double errorSum = 0;
			for (auto j = this->trainSet.begin(); j != this->trainSet.end(); ++j) {
				double zi = 0;
				for (int k = 0; k < shochuAlgorithm::LogisticsRegression::featureNum; ++k) {
					zi += ((this->model[k]) * (j->feature[k]));
				}
				double hi = 1 / (i + exp(-zi));
				//(h_i - y_i)*x_i
				errorSum = (hi - j->trueVal)*(j->feature[i]);
			}
			this->model[i] -= (learnRate * errorSum);
			error += abs(learnRate * errorSum);
		}
		if (error < 0.0001) {
			break;
		}
		++cnt;
		if (cnt % 50 == 0) {
			learnRate /=  2;
		}
	}
	return this->model;
}

double shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::predict(const TrainNode& a) {
	double ret = 0;
	auto j = a.feature.begin();
	for (auto i = this->model.begin(); i != this->model.end(); ++i,++j) {
		ret += (*i) * (*j);
	}
	return ret;
}
