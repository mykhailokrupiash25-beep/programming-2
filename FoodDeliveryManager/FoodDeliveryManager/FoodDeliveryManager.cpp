#include "FoodDeliveryManager.h"
#include <utility>

/* Загальна складність системи: O(W + R * U), де:
 W - кількість активних подій у 45-хвилинному вікні (зберігається у std::deque).
 R - кількість унікальних ресторанів у вікні.
 U - кількість унікальних клієнтів для кожного ресторану.
 Завдяки очищенню, програма не зберігає зайві дані, тому пам'ять оптимальна.
 */

FoodDeliveryManager::FoodDeliveryManager()
    : current_time_(0) {
}

void FoodDeliveryManager::Order(
    int64_t time,
    std::string restaurant_name,
    int user_id,
    int amount
) {
    // Час O(1) Коли функція завершиться, деструктор lock_guard автоматично звільнить його.
    std::lock_guard<std::mutex> lock(mtx_);

    // Час O(1). Проста перевірка та присвоєння.
    if (time > current_time_) {
        current_time_ = time;
    }

    //Амортизована складність за час O(log R + log U) на одну подію.
    AdjustWindow(current_time_);

    // Час O(log R) для пошуку/вставки в масив map 
    revenue_by_restaurant_[restaurant_name] += amount;

    // Час O(log R) для пошуку ресторану + O(log U) для вставки користувача.
    user_counts_by_restaurant_[restaurant_name][user_id]++;

    // Час O(1). Додавання в кінець deque.
    // move перетворює копіювання рядка з O(L) (де L - довжина рядка) 
    // на миттєве перенесення вказівника за O(1).
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

    // Час O(log R). Пошук по дереву у мапі ресторанів.
    auto it = user_counts_by_restaurant_.find(restaurant_name);
    if (it == user_counts_by_restaurant_.end()) {
        return 0;
    }

    // Час O(1). 
    return static_cast<int>(it->second.size());
}

int FoodDeliveryManager::ComputeRevenue(
    const std::string& restaurant_name
) {
    std::lock_guard<std::mutex> lock(mtx_);

    // Час O(log R). Пошук по дереву у мапі ресторанів.
    auto it = revenue_by_restaurant_.find(restaurant_name);
    if (it == revenue_by_restaurant_.end()) {
        return 0;
    }

    // Час O(1). Повернення знайденого значення.
    return it->second;
}

void FoodDeliveryManager::AdjustWindow(int64_t current_time) {
    // Цикл while обробляє старі події. Хоча він може зробити багато ітерацій за раз,
    // кожна подія додається і видаляється з черги лише 1 раз. 
    // Тому амортизований час на одну подію залишається малим.
    while (
        !events_.empty() &&
        // Час O(1). Доступ до початку std::deque.
        events_.front().time <= current_time - window_size_
        ) {
        // Час O(1). Читання посилання на перший елемент.
        const OrderEvent& old = events_.front();

        // Час O(log R). Пошук ресторану для зменшення виручки.
        auto rev_it = revenue_by_restaurant_.find(old.restaurant_name);
        if (rev_it != revenue_by_restaurant_.end()) {
            rev_it->second -= old.amount;

            // Час O(log R) у найгіршому випадку або константа на видалення вузла.
            // Це рятує нас від витоків пам'яті.
            if (rev_it->second == 0) {
                revenue_by_restaurant_.erase(rev_it);
            }
        }

        // Час O(log R) на пошук ресторану.
        auto outer_it = user_counts_by_restaurant_.find(old.restaurant_name);
        if (outer_it != user_counts_by_restaurant_.end()) {
            auto& umap = outer_it->second;

            // Час O(log U) на пошук клієнта.
            auto user_it = umap.find(old.user_id);
            if (user_it != umap.end()) {
                // Час O(1) на декремент. 
                // Якщо 0 - O(log U) або константа на видалення (erase).
                if (--user_it->second == 0) {
                    umap.erase(user_it);
                }
            }

            // Якщо для ресторану немає активних клієнтів, видаляємо ресторан (O(log R)).
            if (umap.empty()) {
                user_counts_by_restaurant_.erase(outer_it);
            }
        }

        // Час O(1). Швидке видалення першого елемента з std::deque.
        events_.pop_front();
    }
}