#include "EngineFactory.hpp"

std::unique_ptr<Engine> EngineFactory::createEngine() {
    ChessEngine engine;
    return std::make_unique<ChessEngine>(std::move(engine));
}
