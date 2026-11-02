#include "FoodDeliveryManager.h"
#include <utility>

FoodDeliveryManager::FoodDeliveryManager()
	: current_time_(0) {
}

void FoodDeliveryManager::Order(
	int64_t time,
	std::string restaurant_name,
	int user_id,
	int amount
) {
	std::lock_guard<std::mutex> lock(mtx_);

	if (time > current_time_) {
		current_time_ = time;
	}

	AdjustWindow(current_time_);

	revenue_by_restaurant_[restaurant_name] += amount;
	user_counts_by_restaurant_[restaurant_name][user_id]++;

	events_.push_back({
		time,
		std::move(restaurant_name),
		user_id,
		amount
		});
}

int FoodDeliveryManager::ComputeUsersCount(
	const std::string& restaurant_name
) {
	std::lock_guard<std::mutex> lock(mtx_);

	auto it = user_counts_by_restaurant_.find(restaurant_name);
	if (it == user_counts_by_restaurant_.end()) {
		return 0;
	}
	return static_cast<int>(it->second.size());
}

int FoodDeliveryManager::ComputeRevenue(
	const std::string& restaurant_name
) {
	std::lock_guard<std::mutex> lock(mtx_);

	auto it = revenue_by_restaurant_.find(restaurant_name);
	if (it == revenue_by_restaurant_.end()) {
		return 0;
	}
	return it->second;
}

void FoodDeliveryManager::AdjustWindow(int64_t current_time) {
	while (
		!events_.empty() &&
		events_.front().time <= current_time - window_size_
		) {	
		const OrderEvent& old = events_.front();

		// Зменшуємо виручку
		auto rev_it = revenue_by_restaurant_.find(old.restaurant_name);
		if (rev_it != revenue_by_restaurant_.end()) {
			rev_it->second -= old.amount;
			if (rev_it->second == 0) {
				revenue_by_restaurant_.erase(rev_it);
			}
		}

		// Зменшуємо лічильник клієнта
		auto outer_it = user_counts_by_restaurant_.find(old.restaurant_name);
		if (outer_it != user_counts_by_restaurant_.end()) {
			auto& umap = outer_it->second;
			auto user_it = umap.find(old.user_id);
			if (user_it != umap.end()) {
				if (--user_it->second == 0) {
					umap.erase(user_it);
				}
			}
			if (umap.empty()) {
				user_counts_by_restaurant_.erase(outer_it);
			}
		}

		events_.pop_front();
	}
}