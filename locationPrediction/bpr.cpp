#include "bpr.h"

#ifdef __DEBUG__
#include <iostream>
#endif // __DEBUG__

shochuAlgorithm::BPR::Bpr_impl::Bpr_impl(const int* data, const unsigned int users, const unsigned int items) {
	this->add(data, users, items);
}

void shochuAlgorithm::BPR::Bpr_impl::add(const int* data, const unsigned int users,const unsigned int items) {
	assert(this->isAlreadAdd == false && "already add data");
	unsigned int perUesrAlreadAdd = 0;
	unsigned int perUserRecordMax = this->times * items;
	this->users = users;
	this->items = items;
	
	for (unsigned int u = 0; u < users; ++u) {
		for (unsigned int i = 0; i < items; ++i) {
			for (unsigned int j = 0; j < items; ++j) {
				if (perUesrAlreadAdd < perUserRecordMax) {
					if (data[u*items + i] && !data[u*items + j]) {
                        this->trainSet.emplace_back(u, i, j);
						++perUesrAlreadAdd;
					}
				}
				else {
					goto userAddRecordFinish;
				}
			}
		}
	userAddRecordFinish:
		perUesrAlreadAdd = 0;
	}
#ifdef __DEBUG__
	std::cout << "train set size:" << this->trainSet.size() << std::endl;
#endif // __DEBUG__
	this->isAlreadAdd = true;
}

void shochuAlgorithm::BPR::Bpr_impl::add(const Triad& a) {
	this->trainSet.push_back(a);
}

void shochuAlgorithm::BPR::Bpr_impl::add(Triad&& a) {
	this->trainSet.push_back(a);
}

void shochuAlgorithm::BPR::Bpr_impl::add(unsigned int u,unsigned int i,unsigned int j) {
    this->trainSet.emplace_back(u, i, j);
}

void shochuAlgorithm::BPR::Bpr_impl::add(Triad a) {
    this->trainSet.push_back(a);
}

void shochuAlgorithm::BPR::Bpr_impl::setTrainSetSize(const unsigned int a) {
	this->times = a;
}

float shochuAlgorithm::BPR::Bpr_impl::predictUserItem(const unsigned int user, const unsigned int item) const{
	float ret = 0.0;
	for (unsigned int i = 0; i < this->k; ++i) {
		ret += (this->w.get())[user*this->k + i] * (this->h.get())[i*this->k + item];
	}
	return ret;
}

void shochuAlgorithm::BPR::Bpr_impl::train(const float lambda, const float learnRate, const unsigned int step, const unsigned int k, const float threshold) {
	unsigned int runStep = 0;
	unsigned int count = 0;
	float loss[3] = { 0,0,0 };
	unsigned int len = this->trainSet.size();
	this->k = k;
	//随机初始化w h
	std::default_random_engine engine(static_cast<unsigned int>(time(0)));
	std::normal_distribution<float> uniformfloatRand(0.0, 1.0);
	std::uniform_int_distribution<unsigned int> uniformIntRand(0, len-1);
	this->w = makeUnique(this->users * this->k);
	this->h = makeUnique(this->k * this->items);
	this->x = makeUnique(this->users * this->items);
	for (unsigned int i = 0; i < this->users*this->k; ++i) {
		(this->w.get())[i] = uniformfloatRand(engine);
	}
	for (unsigned int i = 0; i < this->k*this->items; ++i) {
		(this->h.get())[i] = uniformfloatRand(engine);
	}
	
	while (runStep++ < step)
	{
#ifdef __DEBUG__
		std::cout << runStep << std::endl;
#endif // __DEBUG__
		loss[0] = 0.0;
		loss[1] = 0.0;
		loss[2] = 0.0;
		for (count = 0; count < len; ++count) {
			//auto idx = this->trainSet[uniformIntRand(engine)];
            auto ii = this->trainSet.begin();
            std::advance(ii, uniformIntRand(engine));
            Triad idx = *(ii);

			float x_ui = this->predictUserItem(idx.u, idx.i);
			float x_uj = this->predictUserItem(idx.u, idx.j);

			float sigmoid = 1.0 / (1 + exp(x_ui - x_uj));

			for (unsigned int l = 0; l < this->k; ++l) {
				float w_ul = (this->w.get())[idx.u*this->k + l];
				float h_il = (this->h.get())[l*this->k + idx.i];
				float h_jl = (this->h.get())[l*this->k + idx.j];

				(this->w.get())[idx.u*this->k + l] = w_ul - learnRate*(sigmoid * (h_jl - h_il) + lambda * w_ul);
				(this->h.get())[l*this->k + idx.i] = h_il - learnRate*(sigmoid * (-w_ul) + lambda * h_il);
				(this->h.get())[l*this->k + idx.j] = h_jl - learnRate*(sigmoid * ( w_ul) + lambda * h_jl);

				loss[0] += abs(sigmoid * (h_jl - h_il) + lambda * w_ul);
				loss[1] += abs(sigmoid * (-w_ul) + lambda * h_il);
				loss[2] += abs(sigmoid * ( w_ul) + lambda * h_jl);
			}
		}
		loss[0] /= len;
		loss[1] /= len;
		loss[2] /= len;
#ifdef __DEBUG__
		std::cout << loss[0] << ' ' << loss[1] << ' ' << loss[2] << std::endl;
#endif // __DEBUG__
		if (*std::max_element(loss,loss+3) < threshold) {
			break;
		}
	}
#ifdef __DEBUG__
	std::cout << "train number:" << runStep - 1 << std::endl;
#endif // __DEBUG__
	//计算x
	for (unsigned int i = 0; i < users; ++i) {
		for (unsigned int j = 0; j < items; ++j) {
			(this->x.get())[i*this->items + j] = 0;
			for (unsigned int l = 0; l < this->k; ++l) {
				(this->x.get())[i*this->items + j] += (this->w.get())[i*this->k + l] * (this->h.get())[l*this->k + j];
			}
		}
	}
}

std::list<std::pair<int,float> > shochuAlgorithm::BPR::Bpr_impl::predict(const unsigned int user,const unsigned int n) const{
	assert(n <= this->items && "n must be less than or equal to items");
	std::list<std::pair<int,float> > ret;
	std::unique_ptr<float*> t = std::make_unique<float*>(new float[this->items]);
	memcpy(*t, this->x.get() + user*this->items, this->items * sizeof(float));
	for (unsigned int i = 0; i < this->items; ++i) {
        ret.emplace_back(i, (*t)[i]);
	}
	//std::sort(ret.begin(), ret.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)->bool {return a.second > b.second; });
    ret.sort([](const std::pair<int, float>& a, const std::pair<int, float>& b)->bool {return a.second > b.second; });
    ret.resize(n);
	return ret;
}

void shochuAlgorithm::BPR::Bpr_impl::loadData(const char* fileName){
	std::fstream f;
	std::unique_ptr<int[]> data;
	f.open(fileName, std::ios::in);
	assert(f.is_open() && "file don't find");

	f >> this->users >> this->items;
	data = std::unique_ptr<int[]>(new int[this->users*this->items]);
	for (unsigned int i = 0; i < this->users; ++i) {
		for (unsigned int j = 0; j < this->items; ++j) {
			f >> (data.get())[i*this->items + j];
		}
	}
	this->add(data.get(), users, this->items);
	f.close();
}

void shochuAlgorithm::BPR::Bpr_impl::loadData(const std::string& fileName) {
	this->loadData(fileName.c_str());
}

void shochuAlgorithm::BPR::Bpr_impl::saveModel(const char* fileName) const{
	std::fstream f;
	f.open(fileName, std::ios::out);
	assert(f.is_open() && "file don't find");

	f << this->users << ' ' << this->items << ' ' << this->k << ' ';
	for (unsigned int i = 0; i < this->users; ++i) {
		for (unsigned int j = 0; j < this->k; ++j) {
			f << (this->w.get())[i*this->k + j] << ' ';
		}
	}
	for (unsigned int i = 0; i < this->k; ++i) {
		for (unsigned int j = 0; j < this->items; ++j) {
			f << (this->h.get())[i*this->items + j] << ' ';
		}
	}
	f.close();
}

void shochuAlgorithm::BPR::Bpr_impl::saveModel(const std::string& fileName) const{
	this->saveModel(fileName.c_str());
}

void shochuAlgorithm::BPR::Bpr_impl::loadModel(const char* fileName) {
	std::fstream f;
	f.open(fileName, std::ios::in);
	assert(f.is_open() && "file don't find");

	f >> this->users >> this->items >> this->k;
	
	this->w = makeUnique(this->users * this->k);
	this->h = makeUnique(this->k * this->items);
	this->x = makeUnique(this->users * this->items);
	
	for (unsigned int i = 0; i < this->users; ++i) {
		for (unsigned int j = 0; j < this->k; ++j) {
			f >> (this->w.get())[i*this->k + j];
		}
	}
	
	for (unsigned int i = 0; i < this->k; ++i) {
		for (unsigned int j = 0; j < this->items; ++j) {
			f >> (this->h.get())[i*this->items + j];
		}
	}
	for (unsigned int i = 0; i < this->users; ++i) {
		for (unsigned int j = 0; j < this->items; ++j) {
			(this->x.get())[i*this->items + j] = this->predictUserItem(i,j);
		}
	}
	this->isAlreadAdd = true;
	f.close();
}

void shochuAlgorithm::BPR::Bpr_impl::loadModel(const std::string& fileName) {
	this->loadModel(fileName.c_str());
}
