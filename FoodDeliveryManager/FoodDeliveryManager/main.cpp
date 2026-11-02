#include <iostream>
#include <vector>
#include <future>
#include <string>
#include "FoodDeliveryManager.h"


using namespace std;


// Функція для перевірки результатів
void AssertEqual(int actual, int expected, const string& hint = "") {
    if (actual != expected) {
        cerr << "Помилка: " << actual << " != " << expected << " підказка: " << hint << endl;
        abort();
    }
}


// Тестування послідовної логіки та 45-хвилинного вікна
void TestSequential() {
    FoodDeliveryManager fdm;

    AssertEqual(fdm.ComputeUsersCount("SushiMaster"), 0, "Початкові клієнти SushiMaster");
    AssertEqual(fdm.ComputeRevenue("SushiMaster"), 0, "Початкова виручка SushiMaster");

    fdm.Order(100, "SushiMaster", 1, 500);
    fdm.Order(200, "BurgerKing", 2, 300);
    fdm.Order(2799, "SushiMaster", 3, 800);

    AssertEqual(fdm.ComputeUsersCount("SushiMaster"), 2, "Клієнти SushiMaster до зсуву вікна");

    // Подія на 2800-й секунді зсуває вікно. Подія на 100-й секунді випадає (2800 - 2700 = 100)
    fdm.Order(2800, "BurgerKing", 3, 450);

    AssertEqual(fdm.ComputeRevenue("SushiMaster"), 800, "Виручка SushiMaster після зсуву (подія на 100 випала)");
    AssertEqual(fdm.ComputeUsersCount("SushiMaster"), 1, "Клієнти SushiMaster після зсуву (подія на 100 випала)");

    cout << "TestSequential OK\n";
}


// Тестування багатопотоковості (Лекція 9)
void TestMultithreading() {
    FoodDeliveryManager fdm;
    vector<future<void>> futures;


    // Симуляція паралельного надходження замовлень з мобільного додатку
    for (int i = 0; i < 900; ++i) {
        futures.push_back(async(launch::async, [&fdm, i]() {
            // 30 унікальних клієнтів роблять замовлення на 50 грн кожне
            fdm.Order(i, "PizzaHut", i % 30, 50);
            }));
    }


    // Очікування завершення всіх потоків
    for (auto& f : futures) {
        f.get();
    }


    AssertEqual(fdm.ComputeUsersCount("PizzaHut"), 30, "Багатопотокові клієнти PizzaHut");
    AssertEqual(fdm.ComputeRevenue("PizzaHut"), 45000, "Багатопотокова виручка PizzaHut (900 * 50)");

    cout << "TestMultithreading OK\n";
}


int main() {
    TestSequential();
    TestMultithreading();
    cout << "Всі тести для Завдання 12 успішно пройдено!" << endl;
    return 0;
}
