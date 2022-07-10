#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "Tile.h"


struct  ExploreationStrategy {

	virtual void startTile(std::weak_ptr<Tile>) = 0;

	// asks the controller for the next tile to step on
	virtual std::weak_ptr<Tile> chooseTile(std::vector<std::weak_ptr<Tile>> visible_tiles) = 0;

	// virtual destructor for deleting derived objects trough a pointer to base
	virtual ~ExploreationStrategy() {}
};


class RandomWalkStrategy : public ExploreationStrategy {
	std::vector<std::weak_ptr<Tile>> visible;

public:

	void startTile(std::weak_ptr<Tile> start_tile) {}

	std::weak_ptr<Tile> chooseTile(std::vector<std::weak_ptr<Tile>> visible_tiles) {

		std::vector<std::weak_ptr<Tile>> out;
		std::sample(visible_tiles.begin(),
			visible_tiles.end(),
		    std::back_inserter(out),
		    1,
		    std::mt19937{ std::random_device{}() });
		
		return out.front();
	}
};


class DepthFirstSearchStrategy : public ExploreationStrategy {	
	std::unordered_set<std::shared_ptr<Tile>> visited; 
	std::shared_ptr<Tile> current_tile, start_tile;

	struct PathNode {
		std::shared_ptr<Tile> curr;
		std::vector<std::shared_ptr<Tile>> nbrs;
		std::vector<std::shared_ptr<Tile>>::iterator nbrs_it;

		PathNode(std::shared_ptr<Tile> curr, std::vector<std::shared_ptr<Tile>> nbrs) 
			: curr(curr), nbrs(nbrs) {
			nbrs_it = this->nbrs.begin();
		}
		
	};
	std::stack<PathNode> path;

public:

	void startTile(std::weak_ptr<Tile> start_tile) {
		visited.insert(start_tile.lock());
		current_tile = start_tile.lock();
		start_tile = current_tile;
	}

	std::weak_ptr<Tile> chooseTile(std::vector<std::weak_ptr<Tile>> visible_tiles) {

		// filter for not visited nbrs
		std::vector<std::shared_ptr<Tile>> not_visited_nbrs;
		for (auto weak_tile : visible_tiles) {
			std::shared_ptr<Tile> tile = weak_tile.lock();
			if (visited.count(tile) == 0)
				not_visited_nbrs.push_back(tile);
		}


		//if curent node is root or if curent node is not a leaf
		if((path.size() == 0) || current_tile != path.top().curr)
			path.push(PathNode{ current_tile, not_visited_nbrs }); //put a new node on top of the stack


		// if there are no unvisited nbrs. go backwards on the current branch
   		if (not_visited_nbrs.size() == 0) {
			if (path.size() == 1) return std::shared_ptr<Tile>();
			path.pop();
			current_tile = path.top().curr;
			return current_tile;			
		}
		 
		auto& path_node = path.top();
		
		// try to advance forward
		while(path_node.nbrs_it != path_node.nbrs.end()){
			auto curr_nbr = *path_node.nbrs_it;

			if (visited.count(curr_nbr) == 0) { // if curr_nbr is a new tile
				path_node.nbrs_it++;
				visited.insert(curr_nbr);
				current_tile = curr_nbr;
				return curr_nbr;
			}
			path_node.nbrs_it++;
		}


		throw std::runtime_error("something went wrong");
	}
};