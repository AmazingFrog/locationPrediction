inline void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::add(const TrainNode& a) {
    this->trainSet.push_back(a);
}

inline void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::add(TrainNode&& a) {
    this->trainSet.push_back(a);
}

inline void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::add(std::list<TrainNode>&& a) {
    this->trainSet.insert(this->trainSet.end(), a.begin(), a.end());
}

inline void shochuAlgorithm::LogisticsRegression::LogisticsRegression_impl::add(const std::list<TrainNode>& a) {
    this->trainSet.insert(this->trainSet.end(), a.begin(), a.end());
}
