#include "logisticsRegression.h"

unsigned int shochuAlgorithm::LogisticsRegression::featureNum = 0;

shochuAlgorithm::LogisticsRegression::TrainNode::TrainNode(double _predictVal, std::initializer_list<double> featureIL) : trueVal(_predictVal){
    assert(shochuAlgorithm::LogisticsRegression::featureNum != featureIL.size() && "shochuAlgorithm::LogisticsRegression::featureNum must be same with initlize_lise.size()");
    this->setFeatureVal(featureIL);
}

shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::LogisticsRegression_impl() {
	this->init();
}
shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::LogisticsRegression_impl(std::list<TrainNode>&& a) :trainSet(a){
	this->init();
}
shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::LogisticsRegression_impl(const std::list<TrainNode>& a) :trainSet(a){
	this->init();
}
void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::init(){
	assert(shochuAlgorithm::LogisticsRegression::featureNum != 0 && "shochuAlgorithm::LogisticsRegression::featureNum must not be 0");
	this->model.assign(shochuAlgorithm::LogisticsRegression::featureNum, 0.0);
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

bool shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::save(const std::string& path) {
    return this->save(path.c_str());
}
bool shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::save(const char* path) {
    std::fstream fout(path,std::ios::out);
    if (!fout.is_open()) {
        return false;
    }
    fout << this->model.size() << std::endl;
    for (auto i = this->model.begin(); i != this->model.end(); ++i) {
        fout << *i << std::endl;
    }
    fout.close();
    return true;
}

bool shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::load(const std::string& path) {
    return this->load(path.c_str());
}
bool shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::load(const char* path) {
    std::fstream fin(path, std::ios::in);
    if (!fin.is_open()) {
        return false;
    }
    fin >> shochuAlgorithm::LogisticsRegression::featureNum;
    while (fin.peek() != EOF){
        double t = 0;
        fin >> t;
        this->model.emplace_back(t);
    }
    fin.close();
    return true;
}
