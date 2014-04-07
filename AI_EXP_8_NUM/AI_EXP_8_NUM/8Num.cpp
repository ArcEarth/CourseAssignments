#include "state.h"
//#include <iostream>
//#include <random>
//#include <ctime>
#include <queue>

using namespace std;

// DFS search
int visit(GameBoard sp,int deep,const int DeepLim, int *HistoryOp){
	if (sp.MD==0) 
	{
		HistoryOp[deep]=4;
//		cout<<"Hit! ptr = "<<sp.ptr<<" ; MD = "<<sp.MD<<endl; sp.print();
		return 1;
	} 
	if (deep==DeepLim) return 0;
	int t;
	for (int k=0;k<4;k++)
		if (((k+2)%4!=HistoryOp[deep-1])&&(sp.Move((MoveDirection)k))) {
			HistoryOp[deep]=k;
			t=visit(sp,deep+1,DeepLim,HistoryOp);
			sp.Move((MoveDirection) ((k + 2) % 4));
			if (t) return 1;
		}
	return 0;
}//*/

// A* Search, it's not tested, for showing the idea
shared_ptr<SearchState> AStarSearch(GameBoard initialState)
{
	typedef shared_ptr<SearchState> StatePtr;
	priority_queue<StatePtr, vector<StatePtr>, ptref_less<StatePtr>> stateQueue;

	// put the initial state into the queue
	stateQueue.push(make_shared<SearchState>(initialState));

	do
	{
		// Get the closet state into s
		auto s = stateQueue.top();
		stateQueue.pop();

		// Enumerate all the sub-states
		for (int k = 0; k < 4; k++)
		{
			// if moving the reverse direction of last move, will return to it's parent state, skip this case
			if ((k + 2) % 4 == s->LastMove) 
				continue;
			// try to move "s" by direction k, if failed, it will return a null
			auto newState = CreateNewStateByMove(s,(MoveDirection) k);

			// if move successed
			if (newState) {
				// if got the goal
				if (newState->IsGoal())
				{
					return newState;
					// The state remin within the queue will be destroyed (lost all refrence) , but except the trace of this goal state.
				}

				// to-do , to optimize the algrithm , try to determinate if there is a state overlap inside the queue.

				// add the newly developed state into the queue, you can ignore the "std::move" function, it's for optimizing with "R-Value Reference". 
				stateQueue.push(std::move(newState));
			}
		}
	} while (!stateQueue.empty()); // Or you should setup an time-limit here instead of this unlimited loop
}

// Iterative Depth First A* Search's DFS visit method
int IDAstar_visit(GameBoard sp,int cost,const int Limimt, int *HistoryOp){
	if (sp.MD==0) 
	{
		HistoryOp[cost]=4;
//		cout<<"Hit"<<cost<<endl;
		return 0;
	}
	if (cost+sp.MD>Limimt) return cost+sp.MD;
	int min=100,t;//set it to a MAXINT
	for (int k=0;k<4;k++)
		if (((k + 2) % 4 != HistoryOp[cost - 1]) && (sp.Move((MoveDirection) k))) {
			HistoryOp[cost]=k;
			t=IDAstar_visit(sp,cost+1,Limimt,HistoryOp);
			sp.Move((MoveDirection) ((k + 2) % 4));
			if (t==0) return 0;
			if (t<min) min=t;
		}
	return min;
}//*/

int main() { 
	int n,m,r,ASTAR,k;
	int T[9]={0,1,2,3,4,5,6,7,8};
	cout<<"Please input the goal state(input '-1' For default ,it's {0 1 2 3 4 5 6 7 8}):"<<endl;
	cin>>T[0];
	if (T[0]!=-1) {  
		for (int i=1;i<9;i++) cin>>T[i];
		if(SetTarget(T)) cout<<"Target Accept!"<<endl; 
			else	
			{				
				cout<<"Inlegel Target input,Use the default one."<<endl;
				for (int i=0;i<9;i++) T[i]=i;
			}
	}
	else 	T[0]=0;
	cout<<"Please input the random steps : N = ";
	cin>>n;
	const char Translate[4]={'<','^','>', 'v'};
	int *Opr=new int[50];
	srand(time(NULL));
	GameBoard sp(T);
//	cout<<"The goal state is a default one."<<endl;
//	sp.print();
	sp.RandomMove(n);
	GameBoard st=sp;
	cout<<"This is the start state"<<endl;
	sp.Print();
	cout<<"The total Manhattan distance (A under estimate of solution) of the state is "<<sp.MD<<endl;
	cout<<"please select which Algorithm would you like to use : 0 for ID search / 1 for IDA* search "<<endl;
	cin>>ASTAR;
	m=sp.MD;
	cout<<"Searching..."<<endl;
	if (!ASTAR)	
		while (!visit(sp,0,m,Opr)) m++;//IDsearch
	else				
		while (r=IDAstar_visit(sp,0,m,Opr)) m=r;//IDA*Search

	cout<<"This is the solution operation:"<<endl;
	k=0;
	st.Print();
	while (Opr[k]!=4)
	{
		cout<<"  "<<Translate[Opr[k]]<<endl;
		st.Move((MoveDirection)Opr[k]);
		st.Print();
		k++;
	}
	cout<<"The Solution cost "<<k<<" Step."<<endl;//*/
	delete []Opr;
	system("PAUSE");
//	ids(state sp,
}