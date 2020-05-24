#include <algorithm>
#include <array>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <random>
#include <spdlog/spdlog.h>
#include <string>

std::default_random_engine& global_urng()
{
    static std::default_random_engine u{};
    return u;
}

void randomize()
{
    static std::random_device rd{};
    global_urng().seed(rd());
}

int pick_a_number(int from, int thru)
{
    static std::uniform_int_distribution<> d{};
    using parm_t = decltype(d)::param_type;
    return d(global_urng(), parm_t{from, thru});
}

constexpr int length(const char* str)
{
    if (str == nullptr)
        return 0;
    return *str ? 1 + length(str + 1) : 0;
}

class DNA
{
public:
    DNA()
    {
        std::generate(genes.begin(), genes.end(), []() {
            return pick_a_number(32, 126);
        });
    }

    auto countCharEqualAtTheSamePosition() const
    {
        const auto charEqualAtTheSamePosition = [pos =
                                                     0](const char c) mutable {
            auto i = pos++;
            return c == DNA::target[i];
        };

        return std::count_if(
            genes.begin(), genes.end(), charEqualAtTheSamePosition);
    }

    auto fitness()
    {
        const auto score = countCharEqualAtTheSamePosition();
        fitnes = static_cast<float>(score) / length(target);
    }

    static constexpr const char* target = "to be or not to be";
    std::array<char, length(target)> genes;
    float fitnes{0};
};

int main()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("target: \t{}\n", DNA::target);

    randomize();

    std::array<DNA, 100> population;

    std::for_each(
        population.begin(), population.end(), [](auto& dna) { dna.fitness(); });

    int i = 0;
    for (const auto& p : population)
        spdlog::debug(
            "gene {}: \t{} -> {}", i++, fmt::join(p.genes, ""), p.fitnes);

    std::vector<DNA*> matingPool;
    matingPool.reserve(population.size() * 100);

    std::for_each(population.begin(), population.end(), [&matingPool](auto& dna) {
        const auto n = static_cast<int>(dna.fitnes);
        matingPool.insert(matingPool.end(), n, &dna);
    });

    matingPool.shrink_to_fit();

    // https://natureofcode.com/book/chapter-9-the-evolution-of-code/
    // EX. 9.2: use the Monte Carlo method
    // EX. 9.3: change fitness score with the ordinals of scoring
    // EX. mine: use std::discrete_distribution
}
