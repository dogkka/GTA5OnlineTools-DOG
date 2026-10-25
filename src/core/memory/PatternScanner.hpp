#pragma once
#include "Pattern.hpp"
#include "PointerCalculator.hpp"

#include <algorithm>
#include <functional>
#include <vector>

namespace YimMenu
{
	class Module;
	using PatternFunc = std::function<void(PointerCalculator)>;

	class PatternScanner
	{
	private:
		const Module* m_Module;
		std::vector<std::pair<const IPattern*, PatternFunc>> m_Patterns;
		std::vector<const IPattern*> m_OptionalPatterns;

	public:
		PatternScanner(const Module* module);

		template<Signature S>
		void Add(const Pattern<S>& pattern, const PatternFunc& func);

		// Optional patterns gate a single feature only: when they cannot be
		// found the scanner reports success and the feature stays disabled
		// instead of unloading the whole menu. Some game sessions let other
		// components patch the underlying code before we scan it.
		template<Signature S>
		void AddOptional(const Pattern<S>& pattern, const PatternFunc& func);

		// Runtime-pattern overload for lua scripts
		inline void Add(const IPattern& pattern, const PatternFunc& func)
		{
			m_Patterns.push_back(std::make_pair(&pattern, func));
		}

		bool Scan();

		bool IsOptional(const IPattern* pattern) const;

	private:
		bool ScanInternal(const IPattern* pattern, PatternFunc func) const;
	};

	template<Signature S>
	inline void PatternScanner::Add(const Pattern<S>& pattern, const PatternFunc& func)
	{
		m_Patterns.push_back(std::move(std::make_pair(&pattern, func)));
	}

	template<Signature S>
	inline void PatternScanner::AddOptional(const Pattern<S>& pattern, const PatternFunc& func)
	{
		m_OptionalPatterns.push_back(&pattern);
		m_Patterns.push_back(std::move(std::make_pair(&pattern, func)));
	}

	inline bool PatternScanner::IsOptional(const IPattern* pattern) const
	{
		return std::find(m_OptionalPatterns.begin(), m_OptionalPatterns.end(), pattern) != m_OptionalPatterns.end();
	}
}
