
#pragma once

#include "predef.hpp"
#include "filter.hpp"
#include <string.h>
#include <boost/array.hpp>
#include <boost/checked_delete.hpp>
#include "config.hpp"
#include "holders.hpp"

namespace jebe {
namespace cws {

class RequestHandler
{
protected:
	static const Filter* filter;
	static std::size_t chunkSize;

	typedef boost::array<uint64_t, unknown> ActionList;
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
	MarveHolder* marver;

public:
    RequestHandler(byte_t* const chunk_, SendBuff& buff_)
    	: chunk(chunk_), buff(buff_), action(unknown),
    	  spliter(NULL), counter(NULL), comparer(NULL), marver(NULL)
    {
    	CS_SAY("address of buff in RequestHandler: [" << &buff << "]");
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
    		bool solved_flag = false;
    		switch (action)
    		{
    		case split:
    			if (CS_BUNLIKELY(!spliter))
				{
    				spliter = new SplitHolder(buff);
				}
    			offset = filter->find<SplitHolder>(data, dataSize, *spliter);
    			solved_flag = true;
    			break;
    		case count:
    			if (CS_BUNLIKELY(!counter))
				{
    				counter = new CountHolder(buff);
				}
    			offset = filter->find<CountHolder>(data, dataSize, *counter);
    			solved_flag = true;
    			break;
    		case compare:
    			if (CS_BUNLIKELY(!comparer))
				{
    				comparer = new CompareHolder(buff);
				}
    			offset = filter->find<CompareHolder>(data, dataSize, *comparer);
    			solved_flag = true;
    			break;
    		case marve:
    			if (CS_BUNLIKELY(!marver))
				{
    				marver = new MarveHolder(buff);
				}
    			offset = filter->find<MarveHolder>(data, dataSize, *marver);
    			solved_flag = true;
    			break;
    		default:
    			break;
    		}
    		if (CS_BLIKELY(solved_flag))
    		{
				std::size_t remains = dataSize - offset;
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
    			if (CS_BUNLIKELY(!spliter))
				{
    				spliter = new SplitHolder(buff);
				}
				return spliter->genRes();
				break;
			case count:
    			if (CS_BUNLIKELY(!counter))
				{
    				counter = new CountHolder(buff);
				}
				return counter->genRes();
				break;
			case compare:
    			if (CS_BUNLIKELY(!comparer))
				{
    				comparer = new CompareHolder(buff);
				}
				return comparer->genRes();
				break;
			case marve:
    			if (CS_BUNLIKELY(!marver))
				{
    				marver = new MarveHolder(buff);
				}
				return marver->genRes();
				break;
			default:
				break;
			}
		}
    }

    ~RequestHandler()
    {
    	CS_SAY("request-handler destroyed");
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
    			if (bytes[i] == ' ' || bytes[i] == '/' || bytes[i] == '?')
    			{
    				break;
    			}
    			act[i] = bytes[i];
    		}
    		CS_SAY("[" << act << "]");

    		std::size_t actname = *reinterpret_cast<uint64_t*>(act);
    		for (std::size_t i = 0; i < actionList.size(); ++i)
    		{
    			CS_SAY(actionList[i] << "," << actname);
    			if (actionList[i] == actname)
    			{
    				action = static_cast<Action>(i);
    				break;
    			}
    		}
    	}
    }
};

}
}
