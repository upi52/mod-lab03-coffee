// Copyright 2026 upi52

#include "Automata.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
const int kNoDrinkSelected = -1;
}

Automata::Automata()
    : Automata({"Эспрессо Лаб", "Чай с лимоном", "Какао", "Латте Соло"},
               {60, 45, 70, 85}) {}

Automata::Automata(const std::vector<std::string>& drinks,
                   const std::vector<int>& drink_prices)
    : cash_(0),
      menu_(drinks),
      prices_(drink_prices),
      state_(STATES::OFF),
      selected_index_(kNoDrinkSelected),
      last_refund_(0),
      last_drink_() {
  validateMenu(drinks, drink_prices);
}

void Automata::validateMenu(const std::vector<std::string>& drinks,
                            const std::vector<int>& drink_prices) {
  if (drinks.empty()) {
    throw std::invalid_argument("menu cannot be empty");
  }
  if (drinks.size() != drink_prices.size()) {
    throw std::invalid_argument("menu and prices sizes differ");
  }
  for (std::size_t i = 0; i < drinks.size(); ++i) {
    if (drinks[i].empty()) {
      throw std::invalid_argument("drink name cannot be empty");
    }
    if (drink_prices[i] <= 0) {
      throw std::invalid_argument("drink price must be positive");
    }
  }
}

bool Automata::on() {
  if (state_ != STATES::OFF) {
    return false;
  }
  state_ = STATES::WAIT;
  return true;
}

bool Automata::off() {
  if (state_ != STATES::WAIT || cash_ != 0) {
    return false;
  }
  state_ = STATES::OFF;
  return true;
}

bool Automata::coin(int value) {
  if (value <= 0) {
    return false;
  }
  if (state_ != STATES::WAIT && state_ != STATES::ACCEPT) {
    return false;
  }
  cash_ += value;
  state_ = STATES::ACCEPT;
  return true;
}

std::vector<std::pair<std::string, int>> Automata::getMenu() const {
  std::vector<std::pair<std::string, int>> menu_view;
  menu_view.reserve(menu_.size());
  for (std::size_t i = 0; i < menu_.size(); ++i) {
    menu_view.emplace_back(menu_[i], prices_[i]);
  }
  return menu_view;
}

std::string Automata::etMenu() const {
  std::ostringstream output;
  output << "Меню автомата:\n";
  for (std::size_t i = 0; i < menu_.size(); ++i) {
    output << i << ". " << menu_[i] << " — " << prices_[i] << " руб.\n";
  }
  return output.str();
}

STATES Automata::getState() const {
  return state_;
}

int Automata::getCash() const {
  return cash_;
}

int Automata::getLastRefund() const {
  return last_refund_;
}

int Automata::getSelectedPrice() const {
  if (selected_index_ == kNoDrinkSelected) {
    return 0;
  }
  return prices_[static_cast<std::size_t>(selected_index_)];
}

std::string Automata::getLastDrink() const {
  return last_drink_;
}

bool Automata::choice(std::size_t drink_index) {
  if (state_ != STATES::ACCEPT || drink_index >= menu_.size()) {
    return false;
  }
  selected_index_ = static_cast<int>(drink_index);
  state_ = STATES::CHECK;
  return true;
}

bool Automata::check() const {
  if (state_ != STATES::CHECK || selected_index_ == kNoDrinkSelected) {
    return false;
  }
  return cash_ >= prices_[static_cast<std::size_t>(selected_index_)];
}

int Automata::cancel() {
  if (state_ != STATES::WAIT && state_ != STATES::ACCEPT &&
      state_ != STATES::CHECK) {
    return 0;
  }
  last_refund_ = cash_;
  cash_ = 0;
  selected_index_ = kNoDrinkSelected;
  state_ = STATES::WAIT;
  return last_refund_;
}

bool Automata::cook() {
  if (!check()) {
    return false;
  }
  const std::size_t index = static_cast<std::size_t>(selected_index_);
  cash_ -= prices_[index];
  last_drink_ = menu_[index];
  state_ = STATES::COOK;
  return true;
}

int Automata::finish() {
  if (state_ != STATES::COOK) {
    return 0;
  }
  last_refund_ = cash_;
  cash_ = 0;
  selected_index_ = kNoDrinkSelected;
  state_ = STATES::WAIT;
  return last_refund_;
}

std::string toString(STATES state) {
  switch (state) {
    case STATES::OFF:
      return "OFF";
    case STATES::WAIT:
      return "WAIT";
    case STATES::ACCEPT:
      return "ACCEPT";
    case STATES::CHECK:
      return "CHECK";
    case STATES::COOK:
      return "COOK";
  }
  return "UNKNOWN";
}
