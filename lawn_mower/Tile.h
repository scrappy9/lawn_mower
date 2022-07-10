#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

class Tile : public sf::Drawable {
    sf::RectangleShape rect;
    sf::Vector2i position;
    unsigned int visits = 0;
    std::vector<std::weak_ptr<Tile>> neighbours;


    void removeNeighbour(Tile* n) {
        for (auto it = neighbours.begin(); it != neighbours.end(); it++) {
            if (it->lock().get() == n) {
                neighbours.erase(it);
                return;
            }
        }
    }

public:

    Tile(sf::Vector2f position, sf::Vector2f size = sf::Vector2f(10, 10)) : position(position) {
        rect.setPosition(position);
        rect.setFillColor(sf::Color::Green);
        rect.setSize(size);
    }

    void cut() {
        ++visits;
        rect.setFillColor(sf::Color::Red);
    }

    void addNeighbour(std::weak_ptr<Tile> n) {
        neighbours.push_back(n);
    }

    void disable() {
        for (auto neighbour : getNeighbours())
            neighbour.lock()->removeNeighbour(this);

        neighbours.clear();
        rect.setFillColor(sf::Color::Black);
        
    }

    std::vector<std::weak_ptr<Tile>> getNeighbours() const {
        return neighbours;
    }

    sf::Vector2i getPosition() {
        return position;
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(rect);
    }

    //~Tile() {
    //    std::cout << "destruct" << std::endl;
    //}
};