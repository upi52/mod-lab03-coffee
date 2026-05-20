// Copyright 2026 upi52

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "Automata.h"

TEST(Automata, StartsInOffState) {
  Automata machine;
  EXPECT_EQ(machine.getState(), STATES::OFF);
  EXPECT_EQ(machine.getCash(), 0);
}

TEST(Automata, TurnsOnAndWaitsForClient) {
  Automata machine;
  EXPECT_TRUE(machine.on());
  EXPECT_EQ(machine.getState(), STATES::WAIT);
  EXPECT_FALSE(machine.on());
}

TEST(Automata, TurnsOffOnlyFromWaitState) {
  Automata machine;
  machine.on();
  EXPECT_TRUE(machine.off());
  EXPECT_EQ(machine.getState(), STATES::OFF);
}

TEST(Automata, AcceptsSeveralCoins) {
  Automata machine;
  machine.on();
  EXPECT_TRUE(machine.coin(25));
  EXPECT_TRUE(machine.coin(50));
  EXPECT_EQ(machine.getState(), STATES::ACCEPT);
  EXPECT_EQ(machine.getCash(), 75);
}

TEST(Automata, RejectsInvalidCoins) {
  Automata machine;
  machine.on();
  EXPECT_FALSE(machine.coin(0));
  EXPECT_FALSE(machine.coin(-10));
  EXPECT_EQ(machine.getCash(), 0);
  EXPECT_EQ(machine.getState(), STATES::WAIT);
}

TEST(Automata, ChoiceMovesToCheckState) {
  Automata machine({"Мокко", "Зелёный чай"}, {90, 40});
  machine.on();
  machine.coin(100);
  EXPECT_TRUE(machine.choice(1));
  EXPECT_EQ(machine.getState(), STATES::CHECK);
  EXPECT_EQ(machine.getSelectedPrice(), 40);
}

TEST(Automata, ChoiceRejectsWrongIndex) {
  Automata machine({"Кофе"}, {50});
  machine.on();
  machine.coin(100);
  EXPECT_FALSE(machine.choice(5));
  EXPECT_EQ(machine.getState(), STATES::ACCEPT);
}

TEST(Automata, CheckDetectsNotEnoughMoney) {
  Automata machine({"Горячий шоколад"}, {110});
  machine.on();
  machine.coin(60);
  machine.choice(0);
  EXPECT_FALSE(machine.check());
  EXPECT_FALSE(machine.cook());
  EXPECT_EQ(machine.getState(), STATES::CHECK);
}

TEST(Automata, CooksSelectedDrinkWhenMoneyIsEnough) {
  Automata machine({"Капучино"}, {80});
  machine.on();
  machine.coin(100);
  machine.choice(0);
  EXPECT_TRUE(machine.check());
  EXPECT_TRUE(machine.cook());
  EXPECT_EQ(machine.getState(), STATES::COOK);
  EXPECT_EQ(machine.getCash(), 20);
  EXPECT_EQ(machine.getLastDrink(), "Капучино");
}

TEST(Automata, FinishReturnsChangeAndReturnsToWait) {
  Automata machine({"Американо"}, {70});
  machine.on();
  machine.coin(100);
  machine.choice(0);
  machine.cook();
  EXPECT_EQ(machine.finish(), 30);
  EXPECT_EQ(machine.getState(), STATES::WAIT);
  EXPECT_EQ(machine.getCash(), 0);
  EXPECT_EQ(machine.getLastRefund(), 30);
}

TEST(Automata, CancelRefundsMoneyBeforeCooking) {
  Automata machine({"Чёрный чай"}, {35});
  machine.on();
  machine.coin(50);
  machine.choice(0);
  EXPECT_EQ(machine.cancel(), 50);
  EXPECT_EQ(machine.getState(), STATES::WAIT);
  EXPECT_EQ(machine.getCash(), 0);
}

TEST(Automata, OffIsBlockedDuringActiveSession) {
  Automata machine;
  machine.on();
  machine.coin(50);
  EXPECT_FALSE(machine.off());
  EXPECT_EQ(machine.getState(), STATES::ACCEPT);
}

TEST(Automata, MenuValidationRejectsIncorrectData) {
  EXPECT_THROW(Automata({}, {}), std::invalid_argument);
  EXPECT_THROW(Automata({"Чай", "Кофе"}, {10}), std::invalid_argument);
  EXPECT_THROW(Automata({"Чай"}, {0}), std::invalid_argument);
}

TEST(Automata, MenuTextContainsDrinkNamesAndPrices) {
  Automata machine({"Раф", "Матча"}, {120, 95});
  const std::string menu = machine.etMenu();
  EXPECT_NE(menu.find("Раф"), std::string::npos);
  EXPECT_NE(menu.find("95"), std::string::npos);
  EXPECT_EQ(machine.getMenu().size(), 2U);
}
