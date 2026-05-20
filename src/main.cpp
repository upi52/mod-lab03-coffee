// Copyright 2026 upi52
#include <iostream>

#include "Automata.h"

int main() {
  Automata coffee_station;

  std::cout << "=== Сценарий работы автомата ===\n";
  std::cout << "Стартовое состояние: " << toString(coffee_station.getState())
            << "\n\n";

  coffee_station.on();
  std::cout << coffee_station.etMenu() << '\n';

  std::cout << "Состояние после включения: "
            << toString(coffee_station.getState()) << '\n';
  coffee_station.coin(50);
  coffee_station.coin(50);
  std::cout << "Внесено денег: " << coffee_station.getCash() << " руб.\n";
  std::cout << "Состояние: " << toString(coffee_station.getState()) << '\n';

  coffee_station.choice(3);
  std::cout << "Выбран напиток №3, состояние: "
            << toString(coffee_station.getState()) << '\n';
  std::cout << "Проверка суммы: "
            << (coffee_station.check() ? "достаточно" : "недостаточно")
            << '\n';

  coffee_station.cook();
  std::cout << "Готовится: " << coffee_station.getLastDrink() << '\n';
  std::cout << "Состояние: " << toString(coffee_station.getState()) << '\n';

  const int change = coffee_station.finish();
  std::cout << "Выдана сдача: " << change << " руб.\n";
  std::cout << "Состояние после обслуживания: "
            << toString(coffee_station.getState()) << '\n';

  coffee_station.coin(40);
  std::cout << "Новый пользователь передумал, возврат: "
            << coffee_station.cancel() << " руб.\n";

  coffee_station.off();
  std::cout << "Финальное состояние: " << toString(coffee_station.getState())
            << '\n';
  return 0;
}
