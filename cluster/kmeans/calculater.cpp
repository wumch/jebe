
#include "calculater.hpp"
#include <iostream>
#include <clocale>
#include <stdio.h>
#include "misc.hpp"
#include "math.hpp"
#include "aside.hpp"
#include "cluster.hpp"
#include "centerpicker.hpp"
#include "vector.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

void Calculater::calcu()
{
	prepare();
//	while(!reached_top())
//	{
		while(!enough())
		{
			pass();
		}
		dump();
//		upward();
//	}
}

void Calculater::pass()
{
	LOG_IF(INFO, Aside::config->loglevel > 0) << "entering pass: " << iter_times << std::endl;
	deliver();
	rebuild_clses_calcu_centers();
	regen_bound();
	optimize();
	++iter_times;
}

void Calculater::deliver()
{
	any_change = false;
	for (VecList::const_iterator it = vecs.begin(); it != vecs.end(); ++it)
	{
		deliver(*it);
	}
}

// TODO: parallel
void Calculater::deliver(const Vector& vec)
{
	dcur.min_disp = 3.0;
	for (uint32_t i = 0; i < clses.size(); ++i)
	{
		dcur.cur_disp = disparity(vec, clses[i]);
		if (dcur.cur_disp < dcur.min_disp)
		{
			dcur.min_disp = dcur.cur_disp;
			dcur.cls_idx = i;
		}
	}
	deliver(vec, clses[dcur.cls_idx]);
}

bool Calculater::reached_top()
{
	return k <= Aside::config->top_level_min_clses;
}

void Calculater::deliver(const Vector& vec, Cluster& cls)
{
	if (vec.belong_cls != cls.id)
	{
		vec.belong_cls = cls.id;
		if (CS_BUNLIKELY(!any_change))
		{
			any_change = true;
		}
	}
}

bool Calculater::enough()
{
	return !any_change;
}

void Calculater::rebuild_clses_calcu_centers()
{
	for (ClsList::iterator it = clses.begin(); it != clses.end(); ++it)
	{
		it->clear_members();
	}

	for(VecList::const_iterator it = vecs.cbegin(); it != vecs.cend(); ++it)
	{
		get_cls_by_id(it->belong_cls).attach(&*it);
	}
	recalcu_centers();
}

void Calculater::recalcu_centers()
{
	// TODO: parallel
	for (ClsList::iterator it = clses.begin(); it != clses.end(); ++it)
	{
		it->recalc_center();
	}
}

Cluster& Calculater::get_cls_by_id(clsid_t clsid)
{
	return clses[clsid - cls_idx_offset];
}

void Calculater::upward()
{
	++level;
	iter_times = 0;
	cls_idx_offset = cls_id_gen->next();
	supposed_k = k * Aside::config->level_k_rate;
	k = supposed_k * Aside::config->supposed_k_before_decompose;
	max_decompose = k * Aside::config->max_decompose;
	clses.reserve(k);
	// reset vecs
	any_change = false;
	regen_bound();
}

void Calculater::pick_centers()
{
	CenterPicker picker(vecs, k);
	while (picker.more())
	{
		clses.push_back(new Cluster(cls_id_gen->gen(), picker.next()));
	}
}

// 目前只支持分解，不支持分裂。 => TODO
void Calculater::optimize()
{
	CS_RETURN_IF_NORMAL(!should_optimize());
	static clsnum_t decomposed = 0;
	for (ClsList::iterator it = clses.begin(); it != clses.end(); )
	{
		if (it->size() < min_members)
		{
			if (decomposed >= max_decompose)
			{
				break;
			}
			LOG_IF(INFO, Aside::config->loglevel > 0) << "decomposed:" << decomposed << ", max-decompose:" << max_decompose;
			__sync_add_and_fetch(&decomposed, 1);
			it = decompose(it);
		}
		else
		{
			++it;
		}
	}
	recalcu_centers();
}

bool Calculater::should_optimize()
{
	return min_members || max_members;		// min_members > 0 || max_members > 0;
}

Calculater::ClsList::iterator Calculater::optimize(ClsList::iterator cls_iter)
{
	if (cls_iter->size() < min_members)
	{
		return decompose(cls_iter);
	}
	else if (cls_iter->size() > max_members)
	{
		Cluster& cls = *cls_iter;
		ClsList::iterator it = clses.erase(cls_iter);
		separate(cls);
		return it;
	}
	return ++cls_iter;
}

void Calculater::regen_bound()
{
	vnum_t avg = vecs.size() / k;
	min_members = avg * Aside::config->min_members_by_avg;
	max_members = avg * Aside::config->max_members_by_avg;
	LOG_IF(INFO, Aside::config->loglevel > 0) << "required count of members: with k=" << k << ": average=" << avg << ", min=" << min_members << ", max=" << max_members << std::endl;
}

Calculater::ClsList::iterator Calculater::decompose(ClsList::iterator cls_iter)
{
	Cluster::MemberList members = cls_iter->members;
	ClsList::iterator it = clses.erase(cls_iter);
	--k;
	decompose(members);
	regen_bound();
	return it;
}

void Calculater::decompose(Cluster::MemberList& members)
{
	for (Cluster::MemberList::iterator it = members.begin(); it != members.end(); ++it)
	{
		deliver(**it);
	}

	for (Cluster::MemberList::iterator it = members.begin(); it != members.end(); ++it)
	{
		get_cls_by_id((*it)->belong_cls).attach(*it);
	}
	recalcu_centers();
}

Calculater::ClsList::iterator Calculater::separate(ClsList::iterator cls_iter)
{
	return cls_iter;
	Cluster& cls = *cls_iter;
	ClsList::iterator it = clses.erase(cls_iter);
	++k;
	separate(cls);
	regen_bound();
	return it;
}

bool Calculater::validate()
{
	if (k < 2)
	{
		CS_DIE("<Calculater>.k is " << k);
		return false;
	}
	return true;
}

// TODO
void Calculater::separate(Cluster& cls)
{
}

void Calculater::prepare()
{
	validate();
	pick_centers();
}

void Calculater::dump()
{
	LOG_IF(INFO, Aside::config->loglevel > 0) << "dumping cluster centers to " << Aside::config->centers_outputfile
		<< ", with num-of-vectors/k = " << vecs.size() << "/" <<  k << std::endl;
	for (ClsList::const_iterator it = clses.cbegin(); it != clses.cend(); ++it)
	{
		Document doc(it->id, it->center->data());
		msgpack::sbuffer packerBuffer(1 << 10);
		msgpack::packer<msgpack::sbuffer> packer(&packerBuffer);
		packer.pack(doc);
		static_cast<void>(fwrite(packerBuffer.data(), packerBuffer.size(), 1, centers_out));
//		static_cast<void>(fwrite(CS_LINESEP_STR, CS_CONST_STRLEN(CS_LINESEP_STR), 1, centers_out));

		LOG_IF(INFO, Aside::config->loglevel > 1) << "dumping " << it->members.size() << " lines of \"<vector-id>\\t<cluster-id>\\t<level>\" to " << Aside::config->cls_vecs_outputfile << std::endl;
		for (Cluster::MemberList::const_iterator iter = it->members.begin(); iter != it->members.end(); ++iter)
		{
			LOG_IF(INFO, Aside::config->loglevel > 2) << (*iter)->id << Aside::config->output_delimiter << (*iter)->belong_cls << Aside::config->output_delimiter << level << CS_LINESEP;
			fprintf(cls_vecs_out, "%u\t%u\t%u\n", (*iter)->id, (*iter)->belong_cls, level);
		}
	}
}

Calculater::~Calculater()
{
	fclose(centers_out);
	fclose(cls_vecs_out);
}

Calculater::Calculater()
	: cls_id_gen(ClsIdGen::instance()), vecs(Aside::vecs), level(0), supposed_k(Aside::config->supposed_lowest_k),
	  k(supposed_k * Aside::config->supposed_k_before_decompose),
	  max_decompose(k * Aside::config->max_decompose),
	  iter_times(0), cls_idx_offset(cls_id_gen->next()), any_change(true)
{
	centers_out = fopen(Aside::config->centers_outputfile.string().c_str(), "wb");
	cls_vecs_out = fopen(Aside::config->cls_vecs_outputfile.string().c_str(), "w");
}

void Calculater::reset_supposed_k(clsnum_t sk_)
{
	supposed_k = sk_;
	k = supposed_k * Aside::config->supposed_k_before_decompose;
	max_decompose = k * Aside::config->max_decompose;
	clses.reserve(k);
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
