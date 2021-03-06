#ifndef __POSITON__H_
#define __POSITON__H_

using namespace std;

#include "Direction.h"

class Position {

	size_t X;
	size_t Y;

public:

	Position(size_t xPos = 0, size_t yPos = 0) : X(xPos), Y(yPos) {};

	// no need for move assignment operator because it is the same as 
	// the copy assignment operator in this case
	Position(Position&& position) noexcept {
		X = position.X;
		Y = position.Y;
	};

	Position(const Position& position) {
		X = position.X;
		Y = position.Y;
	};
	
	Position& operator=(const Position& position) {
		if (this != &position) {
			X = position.X;
			Y = position.Y;
		}
		return *this;
	};

	~Position() = default;

	void setPosition(size_t xPos, size_t yPos) {
		X = xPos;
		Y = yPos;
	};

	Position& moveEast() {
		X++;
		return *this;
	};

	Position& moveWest() {
		if (X > 0) {
			X--;
		}
		return *this;
	};

	Position& moveSouth() {
		Y++;
		return *this;
	};

	Position& moveNorth() {
		if (Y > 0) {
			Y--;
		}
		return *this;
	};

	Position& moveDirection(Direction direction) {
		return
			direction == Direction::North ? moveNorth() :
			direction == Direction::South ? moveSouth() :
			direction == Direction::West ? moveWest() :
			direction == Direction::East ? moveEast() :
			*this;
	};

	size_t getX() const {
		return X;
	};

	size_t getY() const {
		return Y;
	};

	bool operator==(const Position &other) const {
		return this->X == other.X && this->Y == other.Y;
	};

	bool operator!=(const Position &other) const {
		return !(*this == other);
	};

	operator string() const {
		return "(" + to_string(X) + ", " + to_string(Y) + ")";
	};
};

#endif // __POSITON__H_