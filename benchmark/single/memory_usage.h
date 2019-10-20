#pragma once

//----------------------------------------------------------------------------
/// @file memory_usage.h
/// @brief Code for measuring the memory usage in a given scope.
///
/// @author Copyright (c) 2019 Malte Skarupke\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt )
///
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------

#include <thread>
#include <atomic>

size_t getCurrentRSS();
size_t getPeakRSS();

struct SampleCurrentMemoryUsageInScope
{
	SampleCurrentMemoryUsageInScope()
		: memory_used_before(getCurrentRSS()), max_current_rss(getCurrentRSS())
		, sample_thread([&] { BackgroundThreadFunc(); })
	{
	}
	~SampleCurrentMemoryUsageInScope()
	{
		done = true;
		sample_thread.join();
	}
	size_t GetMaxUsedMemory() const
	{
		return max_current_rss - memory_used_before;
	}

private:
	size_t memory_used_before = 0;
	std::atomic<size_t> max_current_rss{ 0 };
	std::atomic<bool> done{ false };
	std::thread sample_thread;

	void BackgroundThreadFunc()
	{
		while (!done)
		{
			max_current_rss = (std::max)(max_current_rss.load(), getCurrentRSS());
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
};




