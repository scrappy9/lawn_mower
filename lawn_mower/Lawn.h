#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

#include "Tile.h"
#include "Mower.h"

class Lawn : public sf::Drawable {
    sf::Vector2i size{ 16, 16 };
    sf::Vector2f tile_size{ 32, 32 }, margins{3,3};
    std::vector<std::shared_ptr<Tile>> tiles;

    std::weak_ptr<Tile> getTile(sf::Vector2i idx) {
        if (idx.x < 0 || idx.y < 0 || idx.x >= size.x || idx.y >= size.y)
            throw std::out_of_range("Tile idx is out of range!");

        return tiles[idx.y * size.x + idx.x];
    }

public:
    Lawn() {
        // create tiles
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {            
                auto pos = sf::Vector2f(x * tile_size.x + margins.x, y * tile_size.y + margins.y);
                auto tile = std::make_shared<Tile>(pos, tile_size-margins);
                tiles.push_back(tile);
            }
        }

        //connect neighbours
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {            
                sf::Vector2i current_pos{ x, y }; // current position
                //std::vector<sf::Vector2i> offsets{ {0,-1}, {1,0}, {-1, 0}, {0,1} }; // up, right, left, down
                std::vector<sf::Vector2i> offsets{ {0,-1}, {1,0}, {-1, 0}, {0,1},   {1,1}, {-1,-1}, {-1,1}, {1,-1} }; // 8 nbrs.

                for (auto offset : offsets) {
                    sf::Vector2i neighbouring_pos = current_pos + offset;                    
                    
                    if (neighbouring_pos.x < 0 || neighbouring_pos.y < 0 || neighbouring_pos.x >= size.x || neighbouring_pos.y >= size.y)
                        continue;

                    getTile(current_pos).lock()->addNeighbour(getTile(neighbouring_pos));
                }
            }
        }

    }

    void disableTilesRandomly(int n) {
        std::vector<std::weak_ptr<Tile>> selected;

        std::sample(
            tiles.begin(),
            tiles.end(),
            std::back_inserter(selected),
            n,
            std::mt19937{ std::random_device{}() }
        );

        for (auto tile : selected) {
            tile.lock()->disable();
        }
    }

    void disableTile(sf::Vector2i idx) {
        getTile(idx).lock()->disable();
    }

    void place(Mower& m) {

        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {            
                auto start_ttile = getTile(sf::Vector2i(x, y));
                if (start_ttile.lock()->getNeighbours().size() > 0) {
                    //m.moveTo(tile);
                    m.place(start_ttile);
                    return;
                }

            }
        }

    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        for (auto tile : tiles) {
            target.draw(*tile);
        }
    }

    void clear() {
        tiles.clear();
    }
};


//namespace utils {
//    sf::Vector2f idxToPos(sf::Vector2i idx) {
//        return sf::Vector2f(idx.x * 16 + 3, idx.y * 16 + 3);
//    }

//    sf::Vector2i posToIdx(sf::Vector2f pos) {
//        pos -= sf::Vector2f(3, 3);
//        return  sf::Vector2i(std::floor(pos.x / 16), std::floor(pos.y / 16));
//    }
//}