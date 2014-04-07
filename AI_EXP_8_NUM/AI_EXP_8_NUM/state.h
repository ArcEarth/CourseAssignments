#include <random>
#include <iostream>
#include <ctime>
#include <string>
#include <memory>

enum MoveDirection
{
	None = -1,
	Left = 0,
	Up = 1,
	Right = 2,
	Down = 3,
};

/// <summary>
/// Inverses the specified direction.
/// </summary>
/// <param name="dir">The direction.</param>
/// <returns></returns>
inline MoveDirection Inverse(MoveDirection dir)
{
	return (MoveDirection)((dir + 2) % 4);
}

class GameBoard {
public:
	int MD;
	GameBoard(int *initial_state);
	GameBoard();
	bool Move(MoveDirection direction);
	void Print();
	void RandomMove(int steps);
private:
	int D[9];
	int ptr;
	void ComputeManhatonDistance();
};

int SetTarget(const int *);


class SearchState
{
public:
	// Hold the current game board state , where is the 0, where is other numbers
	GameBoard Board;
	int Cost;

	// Hold a shared pointer to it's parent state , this enables dynamic memery management inside the search state
	std::shared_ptr<SearchState> ParentState;
	MoveDirection LastMove;

public:
	explicit SearchState(const GameBoard &board)
		: Board(board)
	{
		Cost = 0;
		LastMove = None;
	}


	// The evaluation function for A* search,Evaluate(s) = cost(s) + h(s)
	int Evaluate() const
	{
		return Cost + Distance();
	}

	int Distance() const
	{
		return Board.MD;
	}

	bool IsGoal() const
	{
		return Distance() == 0;
	}
};

std::shared_ptr<SearchState> CreateNewStateByMove(const std::shared_ptr<SearchState>& p_origin, MoveDirection direction);


inline bool operator<(const SearchState& lhs, const SearchState& rhs)
{
	lhs.Evaluate() < rhs.Evaluate();
}

template <typename _Ty>
struct ptref_less
	: public binary_function<_Ty, _Ty, bool>
{	// functor for operator<
	bool operator()(const _Ty& _Left, const _Ty& _Right) const
	{	// apply operator< to operands
		return (*_Left < *_Right);
	}
};

//template <typename T>
//class IEnumerable < T >
//{
//public:
//	class iterator
//	{
//		typedef iterator_catalog std::forward_iterator_tag;
//	};
//
//	iterator begin();
//	const iterator& end() const;
//};
//
//class ISearchState
//{
//	virtual IEnumerable<ISearchState> SubStates() const = 0;
//	virtual int Evaluation() const = 0;
//};