#ifndef RANDOM_H
#define RANDOM_H
#include <limits>
#include <random>

class [[nodiscard]] Random {
public:
  explicit Random(int min, int max)
  : verteilung(min, max)
  {}

  explicit Random(int max = std::numeric_limits<int>::max())
  : verteilung(0, max)
  {}

  void setSeed(unsigned int newseed) { generator.seed(newseed); }

  [[nodiscard]] int operator()()
  {
    return verteilung(generator);    // Pseudo-Zufallszahl zwischen min und max
  }
private:
  std::mt19937 generator;
  std::uniform_int_distribution<> verteilung;   // <>: Vorgabe ist int
};
#endif // RANDOM_H
