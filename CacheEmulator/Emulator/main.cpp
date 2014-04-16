#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <fstream>
#include <vector>
using namespace std;

enum CommandTypeEnum
{
	ReadData = 0,
	WriteData = 1,
	ReadInstruction = 2,
	CommandTypeCount,
};

enum WriteMissPolicyEnum
{
	NotAllocation,
	Allocation,
};

enum WriteBackPolicyEnum
{
	WriteBack,
	WriteThrough
};

enum SwitchOutPolicyEnum
{
	FirstInFirstOut,
	LatestRecentUse,
	RandomReplace,
};

//static 
namespace CommandTypeTraits
{
	static bool IsRead(CommandTypeEnum type)
	{
		return type == ReadInstruction || type == ReadData;
	}
	static bool IsWrite(CommandTypeEnum type)
	{
		return type == WriteData;
	}
	static bool IsInstruction(CommandTypeEnum type)
	{
		return type == ReadInstruction;
	}
	static bool IsData(CommandTypeEnum type)
	{
		return type == ReadData || type == WriteData;
	}
};

struct CacheLineMeta
{
	size_t	MappedAddress;
	bool	Dirty;			// For Write-back
	int		TimeStamp;		// For aging algorithm (FIFO/LRU)
	bool	IsEmpty() const
	{
		return MappedAddress == -1;
	}

	void	Reset(int time = -1)
	{
		MappedAddress = -1;
		Dirty = false;
		TimeStamp = time;
	}

	void	Load(size_t	memBlockAddress, int time = -1)
	{
		MappedAddress = memBlockAddress;
		Dirty = false;
		TimeStamp = time;
	}
};

// The interface to operate the cache emulator
class ICache
{
public:
	virtual int		ExcuteCommand(CommandTypeEnum type, size_t memAddr) = 0;
	virtual void	Reset() = 0;

	virtual const int GetCommandCounts(CommandTypeEnum type) const = 0;
	virtual const int GetCommandHitCounts(CommandTypeEnum type) const = 0;
	virtual double GetTimeCost() const = 0;
};

class ComposedCache : public ICache
{
	size_t	DataCacheSize = 32; // Data Cache Size in Byte
	size_t	InstructionCacheSize = 32; // Instruction Cache Size in Byte
	bool EnableIndependentInstructionCache;
};

class PrimaryCache : public ICache
{
public:
	PrimaryCache(size_t	cacheSize = 64U<<10U,
				size_t	lineWidth = 32,
				size_t	setWidth = 1,
				SwitchOutPolicyEnum switchOutPolicy = LatestRecentUse,
				WriteBackPolicyEnum	writeBackPolicy = WriteBack,
				WriteMissPolicyEnum writeMissPolicy = Allocation,
				double	memeryReadTime = 100,
				double	memeryWriteTime = 50,
				double	cacheReadTime = 1,
				double	cacheWriteTime = 1)
				: CacheSize(cacheSize), LineWidth(lineWidth), SetWidth(setWidth), SwitchOutPolicy(switchOutPolicy), WriteBackPolicy(writeBackPolicy), WriteMissPolicy(writeMissPolicy), MemeryReadTime(memeryReadTime), MemeryWriteTime(memeryWriteTime), CacheReadTime(cacheReadTime), CacheWriteTime(cacheWriteTime)
	{
		Reset();
	}
public:
	// Emulator Configurations , it's not safe to modify them inside the processing
	size_t	CacheSize = 64; // Total Cache size in Byte
	size_t	LineWidth = 32;	// Cache line size in Byte
	size_t	LineCount;		// The total count of blocks, = CacheSize / LineWidth
	size_t	SetWidth = 1;	// The parameter N in N-Set association , unit is Cache line
	size_t	SetCount;		// The count of sets, = LineCount / SetWidth

	double	MemeryReadTime = 100;	// For read instruction miss
	double	MemeryWriteTime = 50;		// For case like write throgh or write-back switch out 
	double	CacheReadTime = 1;		// For read instruction hit
	double	CacheWriteTime = 1;		// For write instruction hit


	WriteMissPolicyEnum WriteMissPolicy;

	WriteBackPolicyEnum	WriteBackPolicy;

	SwitchOutPolicyEnum SwitchOutPolicy;

public:
	// Statistic informations
	int CommandCounts[CommandTypeCount];
	int CommandHitCounts[CommandTypeCount];
	double TimeCost;
public:
	virtual int ExcuteCommand(CommandTypeEnum type, size_t memAddr);
	virtual void Reset();
	virtual const int GetCommandCounts(CommandTypeEnum type) const;
	virtual const int GetCommandHitCounts(CommandTypeEnum type) const;
	virtual double GetTimeCost() const;

protected:
	std::vector<CacheLineMeta> CacheLines;
	//CacheLineMeta	*InstructionCaches;
	//CacheLineMeta	*DataCaches;

protected:
	inline size_t GetCurrentTime() const
	{
		size_t sum = CommandCounts[0];
		for (size_t i = 1; i < CommandTypeCount; i++)
		{
			sum += CommandCounts[i];
		}
		return sum;
	}

	/// <summary>
	/// Map the memery address to in cache set number.
	/// </summary>
	/// <param name="memAddr">The memory address.</param>
	/// <returns></returns>
	inline size_t GetMemeryBlockAddr(size_t memAddr) const
	{
		return memAddr / LineWidth;
	}

	size_t MapToSet(size_t memBlockAddr) const
	{
		//auto lineAddr = memBlockAddr % LineCount;
		//return lineAddr / SetWidth;
		auto lineAddr = (memBlockAddr % SetCount);
		return lineAddr;
	}

	int TryFindInSet(size_t setNo, size_t memBlockAddr) const
	{
		const CacheLineMeta* linesInSet = &CacheLines[SetWidth*setNo];
		for (size_t i = 0; i < SetWidth; i++)
		{
			if (linesInSet[i].MappedAddress == memBlockAddr)
				return SetWidth*setNo + i;
		}
		return -1;
	}

	void SwitchIn(size_t lineAddr, size_t memBlockAddr)
	{
		CacheLines[lineAddr].Load(memBlockAddr, GetCurrentTime());
		TimeCost += MemeryReadTime;
	}
	void SwitchOut(size_t lineAddr)
	{
		if (CacheLines[lineAddr].Dirty)
		{
			TimeCost += MemeryWriteTime; // Write-Back
		}
		CacheLines[lineAddr].Reset();
	}

	void AccessData(CommandTypeEnum type, size_t cacheLineAddr, size_t offset)
	{
		if (SwitchOutPolicy == LatestRecentUse)
			CacheLines[cacheLineAddr].TimeStamp = GetCurrentTime(); // Update the use time inside the time stamp
		if (CommandTypeTraits::IsWrite(type))
			if (WriteBackPolicy == WriteThrough)
				TimeCost += MemeryWriteTime;
			else {
				TimeCost += CacheWriteTime;
				CacheLines[cacheLineAddr].Dirty = true;
			}
		else
			TimeCost += CacheReadTime;
	}

	size_t FindLineToSwitchIn(size_t setNo)
	{
		auto setBegin = setNo*SetWidth;
		auto setEnd = (setNo + 1)*SetWidth;
		auto i = setBegin;
		auto agedLine = setBegin;
		while (i < setEnd && !CacheLines[i].IsEmpty())
		{
			if (CacheLines[i].TimeStamp < CacheLines[agedLine].TimeStamp)
				agedLine = i;
			i++;
		}
		if (i < setEnd) // Empty Line Found
			return i;

		if (SwitchOutPolicy == RandomReplace) // Random Replcae Policy
			return setBegin + rand() % SetWidth;

		return agedLine; // FIFO/LRU Policy , return the most aged line
	}

};

void PrimaryCache::Reset()
{
	LineCount = CacheSize / LineWidth;
	SetCount = LineCount / SetWidth;
	CacheLines.resize(LineCount);

	for (auto& line : CacheLines)
	{
		line.Reset();
	}

	for (size_t i = 0; i < CommandTypeEnum::CommandTypeCount; i++)
	{
		CommandCounts[i] = 0;
		CommandHitCounts[0] = 0;
	}
	TimeCost = 0;
}

int PrimaryCache::ExcuteCommand(CommandTypeEnum type, size_t memAddr)
{
	++CommandCounts[type];

	auto blockNo = GetMemeryBlockAddr(memAddr);	// Get the memery address's block(page) address
	auto setNo = MapToSet(blockNo);				// Map this memery-block address to the set 

	int lineNo = TryFindInSet(setNo, blockNo);	// Check if the set has already cached the mem-block

	if (lineNo >= 0) // Finded
	{
		++CommandHitCounts[type];	// Cache hit! good job
	}
	else	// Not hit , Cache miss , we should switch in the data
	{
		lineNo = FindLineToSwitchIn(setNo); // Find the target line to switchIn

		if (!CacheLines[lineNo].IsEmpty())	// Not find empty line
			SwitchOut(lineNo);				// Switch out the existed line

		SwitchIn(lineNo, blockNo);			// Switch in the data we need

	}

	AccessData(type, lineNo, memAddr % LineWidth); // Acess the data
	return lineNo;
}

const int PrimaryCache::GetCommandCounts(CommandTypeEnum type) const
{
	return CommandCounts[type];
}
const int PrimaryCache::GetCommandHitCounts(CommandTypeEnum type) const
{
	return CommandHitCounts[type];
}
double PrimaryCache::GetTimeCost() const
{
	return TimeCost;
}

//int ComposedCache::Reset()
//{
//}
//
//int CacheEmulator::ExcuteCommand(CommandTypeEnum type, size_t memAddr)
//{
//	auto cache = GetCache(type);
//	auto set = MapToSet(type, memAddr);
//
//}

int CacheSize = 64; // Total Cache size in KB
int BlockSize = 32; // Cache line size in Byte
int BlockCount;		// The total count of blocks, = CacheSize / BlockSize

int inst_type;
int map = 1;
int replace = 1;
int write = 0;
int n = 4;

struct chunk{
	int m;
	int t;
};
chunk *ch;

int g_time;

int num_inst = 0;
int c_hit = 0;
double hit_rate = 0.0;

int rd_num = 0;
int ri_num = 0;
int w_num = 0;
int w_hit = 0;
int ri_hit = 0;
int rd_hit = 0;
double ri_rate = 0.0;
double w_rate = 0.0;
double rd_rate = 0.0;

void init(int c_cap, int b_cap){
	BlockCount = c_cap * 1024 / b_cap;
	ch = new chunk[BlockCount];
	for (int i = 0; i < BlockCount; i++){
		ch[i].m = -1;
	}
}

void repl(int s, int c){
	int t = n * s;
	int k = t;
	int min = ch[t].t;
	for (int i = 0; i < n; i++){
		if (ch[t + i].m == -1){
			ch[t + i].m = c;
			ch[t + i].t = g_time;
			return;
		}
	}
	for (int i = 0; i < n; i++){
		if (ch[t + i].t < min){
			min = ch[t + i].t;
			k = t + i;
		}
	}
	ch[k].m = c;
	ch[k].t = g_time;
}

int direct(int memAddr){
	int s;
	s = memAddr%BlockCount;
	if (ch[s].m == memAddr){
		return s;
		c_hit++;
		switch (inst_type) {
		case 0: rd_hit++; break;
		case 1:w_hit++; break;
		case 2:ri_hit++; break;
		}
	}
	ch[s].m = memAddr;
	return s;
}

void SetAssociate(int memAddr){
	int s;
	s = memAddr%(BlockCount / n);
	for (int i = 0; i < n; i++){
		if (ch[n * s + i].m == memAddr)
		{
			c_hit++;
			switch (inst_type) {
			case 0: rd_hit++; break;
			case 1:w_hit++; break;
			case 2:ri_hit++; break;
			}
			if (replace == 1)
				ch[n * s + i].t = g_time;
			return;
		}
	}
	repl(s, memAddr);
}

void play_trace(int b_cap)
{
	ifstream in("instructions\\cc1.din");
	if (!in.is_open())
		return;
	unsigned addr;
	int c;
	in >> hex;
	in >> inst_type;
	in >> addr;

	while (!in.fail()) {
		g_time++;
		num_inst++;
		c = addr / b_cap;
		switch (inst_type) {
		case 0: rd_num++; break;
		case 1:w_num++; break;
		case 2:ri_num++; break;
		}

		switch (map) {
		case 0: direct(c); break;
		case 1: SetAssociate(c); break;
		}
		in >> inst_type >> addr;
	}
}

void ExcuteFromFile(ICache& cache, const std::string& fileName)
{
	size_t i_type, address;

	ifstream in(fileName);
	if (!in.is_open())	return;
	in >> hex; // switch to hex mode

	cache.Reset(); //Reset the cache to start the new command sequence

	in >> i_type >> address;
	while (!in.fail()) {
		cache.ExcuteCommand((CommandTypeEnum)i_type, address);
		in >> i_type >> address;
	}
}

void PrintAccuracy(const std::string& InstructionName, int CommandCount, int HitCount)
{
	auto MissCount = CommandCount - HitCount;
	cout << (InstructionName + "次数：") << CommandCount;
	cout << "\t不命中次数：" << MissCount;
	cout << "\t不命中率：" << setprecision(4) << (float) MissCount / (float) CommandCount *100.0f << endl << endl;
}

void main(int argc, char** argv){

	PrimaryCache cache(64*1024,32,2,FirstInFirstOut,WriteBack,Allocation);

	ExcuteFromFile(cache, "instructions\\cc1.din");

	auto CommandCount	= cache.GetCommandCounts(ReadInstruction) + cache.GetCommandCounts(ReadData) + cache.GetCommandCounts(WriteData);
	auto HitCount = cache.GetCommandHitCounts(ReadInstruction) + cache.GetCommandHitCounts(ReadData) + cache.GetCommandHitCounts(WriteData);

	PrintAccuracy("全部指令", CommandCount, HitCount);
	PrintAccuracy("指令读指令", cache.GetCommandCounts(ReadInstruction), cache.GetCommandHitCounts(ReadInstruction));
	PrintAccuracy("数据读指令", cache.GetCommandCounts(ReadData), cache.GetCommandHitCounts(ReadData));
	PrintAccuracy("数据写指令", cache.GetCommandCounts(WriteData), cache.GetCommandHitCounts(WriteData));
	system("pause");

	map = 1;
	replace = 1;
	write = 0;
	n = 2;

	//cout << "choose the strategies of cache replacement by input 0 or 1:" << endl;
	//cout << "0:直接映射 1、n路组相连：";
	//cin >> map;
	//if (map == 1){
	//	cout << "input n:";
	//	cin >> n;
	//}
	//cout << endl << "0、FIFO 1、LRU :";
	//cin >> replace;
	//cout << endl << "0、写回法 1、写直达法:";
	//cin >> write;
	//cout << "input the storage of cache(KB):";
	//cin >> c_cap;
	//cout << "\ninput the storage of block(B):";
	//cin >> b_cap;



	//init(CacheSize, BlockSize);
	//time = 0;

	//play_trace(BlockSize);

	//delete ch;

	//hit_rate = (double) (num_inst - c_hit) / (double) num_inst;
	//rd_rate = (double) (rd_num - rd_hit) / (double) rd_num;
	//ri_rate = (double) (ri_num - ri_hit) / (double) ri_num;
	//w_rate = (double) (w_num - w_hit) / (double) w_num;

	//cout << endl << endl;
	//cout << "\t\t\t  模拟结果" << endl << endl;
	//cout << "访问总次数：" << num_inst;
	//cout << "\t不命中次数：" << (num_inst - c_hit);
	//cout << "\t不命中率：" << hit_rate << endl << endl;

	//cout << "其中：" << endl << endl;

	//cout << "读指令次数：" << ri_num;
	//cout << "\t不命中次数：" << (ri_num - ri_hit);
	//cout << "\t不命中率：" << ri_rate << endl << endl;

	//cout << "读数据次数：" << rd_num;
	//cout << "\t不命中次数：" << (rd_num - rd_hit);
	//cout << "\t不命中率：" << rd_rate << endl << endl;

	//cout << "写数据次数：" << w_num;
	//cout << "\t不命中次数：" << (w_num - w_hit);
	//cout << "\t不命中率：" << w_rate << endl << endl;
	//system("pause");
}