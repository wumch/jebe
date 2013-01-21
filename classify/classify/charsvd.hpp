
#pragma once

#include "meta.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <boost/filesystem/path.hpp>
#include <boost/unordered_map.hpp>

#include <Eigen/QR>
#include <Eigen/Householder>
#include <Eigen/Sparse>
#include <Eigen/Jacobi>
#include <Eigen/src/Core/util/DisableStupidWarnings.h>
#include <Eigen/src/misc/Solve.h>
#include <Eigen/src/SVD/JacobiSVD.h>
#include <Eigen/src/SVD/UpperBidiagonalization.h>
#include <Eigen/src/Core/util/ReenableStupidWarnings.h>

#include "mbswcs.hpp"

#define _JEBE_CHINESE_CHARS 6763

namespace classify {

typedef uint16_t charid_t;
typedef uint32_t atimes_t;
typedef int	mindex_t;

static const charid_t invalid_char_id = 65535;

typedef Eigen::SparseMatrix<atimes_t, 1, mindex_t> MatrixType;

class CharMap
{
private:
	typedef std::vector<wchar_t> CharList;
	CharList chars;

	typedef boost::unordered_map<wchar_t, charid_t> IDMap;
	IDMap idmap;

public:
	void build(const boost::filesystem::path& char_file)
	{
		FILE* fp = fopen(char_file.string().c_str(), "r");
		wchar_t* wchars = new wchar_t[_JEBE_CHINESE_CHARS];

		{
			char* chars_ = new char[3 * _JEBE_CHINESE_CHARS + 1];
			int readed = fread(chars_, 3, _JEBE_CHINESE_CHARS, fp);
			assert(readed == _JEBE_CHINESE_CHARS);
			size_t converted = 0;
			staging::mbswcs::mb2wc(chars_, wchars, _JEBE_CHINESE_CHARS * 3, &converted);
			delete chars_;
		}

		chars.reserve(_JEBE_CHINESE_CHARS);
		for (charid_t i = 0; i < _JEBE_CHINESE_CHARS; ++i)
		{
			chars.push_back(wchars[i]);
			idmap.insert(std::make_pair(wchars[i], i));
		}
		delete wchars;
	}

	charid_t getId(wchar_t ch) const
	{
		IDMap::const_iterator it = idmap.find(ch);
		return (it == idmap.end()) ? invalid_char_id : it->second;
	}

	wchar_t getChar(charid_t charid) const
	{
		return chars[charid];
	}
};

class Converter
{
private:
	CharMap chmap;

	MatrixType& matrix;

	mindex_t cursor;

	typedef boost::unordered_map<wchar_t, atimes_t> Counter;
	Counter counter;

public:
	Converter(MatrixType& matrix_)
		: matrix(matrix_)
	{}

	void from_file(const boost::filesystem::path& file)
	{
		matrix.resize(get_lines(file), static_cast<mindex_t>(_JEBE_CHINESE_CHARS));
		std::wfstream ifile(file.string().c_str());
		ifile.imbue(std::locale());
		wchar_t* buf = new wchar_t[256];
		size_t lastpos = 0;
		while (ifile.good() && ifile.peek() != EOF)
		{
			ifile.getline(buf, 256);
			attach_line(buf, static_cast<size_t>(ifile.tellg() - lastpos));
			lastpos = ifile.tellg();
		}
	}

	void attach_line(wchar_t* wstr, size_t len)
	{
		counter.clear();
		Counter::iterator iter = counter.end();
		charid_t id = invalid_char_id;
		for (size_t i = 0; i < len; ++i)
		{
			id = chmap.getId(wstr[i]);
			if (id == invalid_char_id)
			{
				continue;
			}
			iter = counter.find(wstr[i]);
			if (iter == counter.end())
			{
				counter.insert(std::make_pair(id, 1));
			}
			else
			{
				++iter->second;
			}
		}
		for (Counter::const_iterator it = counter.begin(); it != counter.end(); ++it)
		{
			matrix.insert(cursor, static_cast<mindex_t>(it->first)) = it->second;
		}
		++cursor;
	}

	mindex_t get_lines(const boost::filesystem::path& file) const
	{
		FILE* fp = fopen(file.string().c_str(), "rb");
		size_t lines = 0;
		while (!feof(fp))
		{
			fscanf(fp, "%*s\n");
			++lines;
		}
		return lines;
	}
};

class CharSVD
{

};

}
