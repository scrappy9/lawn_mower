#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include <SFML/Graphics.hpp>

#include <stack>
#include <unordered_set>
#include <set>

#include "Tile.h"
#include "strategies.h"

class Mower : public sf::Drawable {
    sf::CircleShape circle;
    std::weak_ptr<Tile> current_tile;
    unsigned int steps = 0;


    std::unique_ptr<ExploreationStrategy> strategy;

public:

    Mower(std::unique_ptr<ExploreationStrategy> strategy) : strategy(std::move(strategy)) {       
        circle.setRadius(14);
        circle.setFillColor(sf::Color::Yellow);
    }

    void place(std::weak_ptr<Tile> start_tile) {
        moveTo(start_tile);
        strategy->startTile(start_tile);
    }
    
    void moveTo(std::weak_ptr<Tile> new_tile) {
        if (!new_tile.lock()) throw std::runtime_error("The next tile is NULLPTR");
        
        new_tile.lock()->cut();
        ++steps;
        current_tile = new_tile;
        circle.setPosition(sf::Vector2f(new_tile.lock()->getPosition()));
    }

    void step() {
        auto neighbours = current_tile.lock()->getNeighbours();
        auto chosen_tile = strategy->chooseTile(neighbours);
        if(chosen_tile.lock()) moveTo(chosen_tile);
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(circle);
    }
};