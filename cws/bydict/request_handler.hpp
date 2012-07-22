
#pragma once

#include "predef.hpp"
#include "filter.hpp"
#include <boost/array.hpp>
#include <boost/checked_delete.hpp>
#include "config.hpp"
#include "holders.hpp"

namespace jebe {
namespace cws {

enum Action {
	split = 0,
	count,
	compare,

	unknown,
};

class RequestHandler
{
protected:
	static Filter* filter;
	static std::size_t chunkSize;

	typedef boost::array<uint64_t, 3> ActionList;
	static const std::size_t actionBegins = CS_CONST_STRLEN("POST /");
	static const std::size_t actionMaxLen = sizeof(uint64_t);
	static const ActionList actionList;

protected:
	byte_t* const chunk;
	SendBuff& buff;

	Action action;
	SplitHolder* spliter;
	CountHolder* counter;
	CompareHolder* comparer;

public:
    RequestHandler(byte_t* const chunk_, SendBuff& buff_)
    	: chunk(chunk_), buff(buff_), action(unknown)
    {

    }

    static void init(std::size_t chunkSize_)
    {
    	chunkSize = chunkSize_;
    	filter = new Filter(Config::getInstance()->patten_file.string());
    }

    std::size_t handle(byte_t* data, tsize_t dataSize)
    {
    	resolveAction(chunk);
    	if (CS_BLIKELY(action != unknown))
    	{
    		tsize_t offset = chunkSize + 1;
    		switch (action)
    		{
    		case split:
    			if (CS_BLIKELY(!spliter))
				{
    				spliter = new SplitHolder(buff);
				}
    			offset = filter->find<SplitHolder>(data, dataSize, *spliter);
    			break;
    		case count:
    			if (CS_BLIKELY(!counter))
				{
    				counter = new CountHolder(buff);
				}
    			offset = filter->find<CountHolder>(data, dataSize, *counter);
    			break;
    		case compare:
    			if (CS_BLIKELY(!comparer))
				{
    				comparer = new CompareHolder(buff);
				}
    			offset = filter->find<CompareHolder>(data, dataSize, *comparer);
    			break;
    		default:
    			break;
    		}
    		std::size_t remains = chunkSize - offset;
    		if (CS_BLIKELY(remains <= offset))
			{
    			memcpy(chunk, data + offset, remains);
			}
    		else
    		{
    			byte_t* mem = new byte_t[remains];
    			memcpy(mem, data + offset, remains);
    			memcpy(chunk, mem, remains);
    			delete[] mem;
    		}
    		return remains;
    	}
    	return chunkSize + 1;
    }
    
    void genRes()
    {
    	if (CS_BLIKELY(action != unknown))
		{
			switch (action)
			{
			case split:
				return spliter->genRes();
				break;
			case count:
				return counter->genRes();
				break;
			case compare:
				return comparer->genRes();
				break;
			default:
				break;
			}
		}
    }

    ~RequestHandler()
    {
    	boost::checked_delete(spliter);
    	boost::checked_delete(counter);
    	boost::checked_delete(comparer);
    }

protected:
    void resolveAction(const byte_t* data)
    {
    	if (CS_BLIKELY(action == unknown))
    	{
    		byte_t act[actionMaxLen];
    		memset(act, ' ', sizeof(act));
    		const byte_t* bytes = data + actionBegins;
    		for (std::size_t i = 0; i < actionMaxLen; ++i)
    		{
    			if (bytes[i] == ' ')
    			{
    				break;
    			}
    			act[i] = bytes[i];
    		}
    		CS_SAY("[" << act << "]");

    		std::size_t actname = *reinterpret_cast<uint64_t*>(act);
    		for (std::size_t i = 0, end = actionList.size() - 1; i < end; ++i)
    		{
    			CS_SAY(actionList[i] << "," << actname);
    			if (actionList[i] == actname)
    			{
    				action = static_cast<Action>(i);
    			}
    		}
    	}
    }
};

}
}
