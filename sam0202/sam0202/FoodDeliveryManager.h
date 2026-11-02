#pragma once
#include <string>
#include <deque>
#include <map>
#include <mutex>
#include <cstdint>

// Структура для зберігання інформації про замовлення
struct OrderEvent {
    int64_t time;
    std::string restaurant_name;
    int user_id;
    int amount;
};

class FoodDeliveryManager {
public:
    FoodDeliveryManager();
    ~FoodDeliveryManager() = default;

    // Метод для фіксації замовлення (має бути потокобезпечним)
    void Order(int64_t time, std::string restaurant_name, int user_id, int amount);

    // Методи отримання статистики
    int ComputeUsersCount(const std::string& restaurant_name);
    int ComputeRevenue(const std::string& restaurant_name);

private:
    // Очищення старих запитів (старших за 45 хвилин)
    void AdjustWindow(int64_t current_time);

    int64_t current_time_;
    const int64_t window_size_ = 2700; // 45 хвилин у секундах

    std::mutex mtx_; // Захист від стану гонки (Лекція 9)
    std::deque<OrderEvent> events_; // Історія замовлень (Лекція 7)

    // Словник: Ресторан -> Загальна виручка
    std::map<std::string, int> revenue_by_restaurant_;

    // Словник: Ресторан -> (Клієнт -> Кількість замовлень у вікні)
    std::map<std::string, std::map<int, int>> user_counts_by_restaurant_;
};