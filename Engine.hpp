#ifndef CHESS_ENGINE_ENGINE_HPP
#define CHESS_ENGINE_ENGINE_HPP

#include "PrincipalVariation.hpp"
#include "Board.hpp"
#include "TimeInfo.hpp"

#include <string>
#include <optional>
#include <cstddef>

struct HashInfo {
    std::size_t defaultSize;
    std::size_t minSize;
    std::size_t maxSize;
};

class Engine {
public:

    virtual ~Engine() = default;

    virtual std::string name() const = 0;

    virtual std::string version() const = 0;

    virtual std::string author() const = 0;

    virtual void newGame() = 0;

    virtual PrincipalVariation pv(
            const Board &board,
            const TimeInfo::Optional &timeInfo = std::nullopt
    ) = 0;

    virtual std::optional<HashInfo> hashInfo() const;

    virtual void setHashSize(std::size_t size);
};

typedef struct LINE {
    int cMove{};
    Move argmove[256];
} LINE;

class ChessEngine : public Engine {
public:

    using U64 = uint64_t;

    [[nodiscard]] std::string name() const override;

    [[nodiscard]] std::string version() const override;

    [[nodiscard]] std::string author() const override;

    void newGame() override;

    PrincipalVariation pv(
            const Board &board,
            const TimeInfo::Optional &timeInfo = std::nullopt
    ) override;

    PrincipalVariation iterativeDeepening(const Board &board, const TimeInfo::Optional &timeInfo);

    long negamax(const Board &board, int depth, int ply, long alpha, long beta, int color, LINE* pline);

    [[nodiscard]] std::chrono::milliseconds moveTime(PieceColor turn, TimeInfo ti) const;

    void setNewGame(bool newGame);

private:
    std::string name_ = "penguin";
    std::string version_ = "19.8.4";
    std::string author_ = "c0mplex";

    Board board_;

    std::map<U64, int> boardStates;

    bool isInitialBoard = true;
};

#endif
