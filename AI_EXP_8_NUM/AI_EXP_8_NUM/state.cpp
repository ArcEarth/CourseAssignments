#include "state.h"
using namespace std;
#define swap(a,b) a=a^b,b=a^b,a=a^b
//extern int * Target;
int Target[9]={0,1,2,3,4,5,6,7,8}; 
	 
int SetTarget(const int *p){
	bool check[9]={0,0,0,0,0,0,0,0,0};
	for(int i=0;i<9;i++) {
		if ((p[i]>8)||(p[i]<0)||check[p[i]]) return 0;
		check[p[i]]=true;
		Target[p[i]]=i;
		}
	return 1;
}

/*inline int abs(const int a){
	if (a<0) return -a;
		else return a;
} //*/

void GameBoard::ComputeManhatonDistance(){
	MD=0; 
	for (int k=0;k<9;k++)
		MD+=abs(k%3-Target[D[k]]%3)+abs(k/3-Target[D[k]]/3);
//		MD+=abs(k%3-D[k]%3)+abs(k/3-D[k]/3);
}
  
/// <summary>
/// Prints this instance.
/// </summary>
void GameBoard::Print(){
	int i,j;
	for (i=0;i<9;i+=3)
	{
		for (j=0;j<3;j++) cout<<D[i+j]<<' ';
		cout<<endl;
	}
}

GameBoard::GameBoard(int *p){
	for (int k=0;k<9;k++)
	{
		D[k]=p[k];
		if (!D[k]) ptr=k;
	}
	ComputeManhatonDistance();
}

GameBoard::GameBoard(){
	ptr=MD=0;
	for (int k=0;k<9;k++) D[k]=k;
}

/// <summary>
/// Move the blank by the specified direction.
/// </summary>
/// <param name="direction">The direction.</param>
/// <returns></returns>
bool GameBoard::Move(MoveDirection p){
	switch (p)  {
		case 0 : if (ptr%3==0)	return 0;	swap(D[ptr],D[ptr-1]);	ptr-=1;		break; //0--Left
		case 1 : if (ptr<3)			return 0;	swap(D[ptr],D[ptr-3]);	ptr-=3;		break; //1--Up
		case 2 : if (ptr%3==2)	return 0;	swap(D[ptr],D[ptr+1]);	ptr+=1;	break; //2--Right
		case 3 : if (ptr>5)			return 0;	swap(D[ptr],D[ptr+3]);	ptr+=3;	break; //3--Down
		default : return 0; //Error
	}	
	ComputeManhatonDistance();
	return 1;
}

/// <summary>
/// Random move the blank for some steps.
/// </summary>
/// <param name="steps">The step count.</param>
void GameBoard::RandomMove(int n){
	int k,i,p=2;
	for (k=0;k<n;) {
		i=rand()%4; 
		if (p!=i) {k+=Move((MoveDirection)i); p=(i+2)%4;}  
//		cout<<" i="<<i<<endl;  print();
	}
}

std::shared_ptr<SearchState> CreateNewStateByMove(const std::shared_ptr<SearchState>& p_origin, MoveDirection direction)
{
	bool result = p_origin->Board.Move(direction);

	if (result)
	{
		auto s = std::make_shared<SearchState>(p_origin->Board);
		s->ParentState = p_origin;
		s->LastMove = direction;
		s->Cost = p_origin->Cost + 1;

		p_origin->Board.Move(Inverse(direction));
		return s;
	}

	return std::shared_ptr<SearchState>(nullptr);
}