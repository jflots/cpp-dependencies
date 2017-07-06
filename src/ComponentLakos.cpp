#include "stdafx.h"

#include "ComponentLakos.h"

#include <boost/graph/graphviz.hpp>

std::string ComponentLakos::QuotedName() const
{
	return std::string("\"") + NiceName('.') + std::string("\"");
}

std::string ComponentLakos::NiceName(char sub) const
{
	if (rootName.empty())
	{
		if (root.string() == ".")
		{
			return std::string("ROOT");
		}

		std::string nicename = root.generic_string().c_str() + 2;
		std::replace(nicename.begin(), nicename.end(), '/', sub);
		return nicename;
	}
	else
	{
		return rootName;
	}
}

ComponentLakos::ComponentLakos(const File* headerFile, const File* sourceFile)
	: index(0)
	, lowlink(0)
	, onStack(false)
{
	if (headerFile)
	{
		m_files.push_back(headerFile);
	}

	if (sourceFile)
	{
		m_files.push_back(sourceFile);
	}

	root = m_files[0]->path;
	rootName = root.stem().string();
}

void ComponentLakos::MergeComponent(ComponentLakos* component)
{
	// add Files

	for (auto& file : component->m_files)
	{
		auto it = std::find(m_files.begin(), m_files.end(), file);

		if (it == m_files.end())
		{
			m_files.push_back(file);
		}
	}

	// add dependencies
	pubDeps.erase(component);

	for (auto aPubDep : component->pubDeps)
	{
		pubDeps.insert(aPubDep);
	}

	privDeps.erase(component);

	for (auto aPrivateDep : component->privDeps)
	{
		privDeps.insert(aPrivateDep);
	}

	pubLinks.erase(component);

	for (auto aPubLink : component->pubLinks)
	{
		pubLinks.insert(aPubLink);
	}

	privLinks.erase(component);

	for (auto aPrivLink : component->privLinks)
	{
		privLinks.insert(aPrivLink);
	}

	rootName += ".";
	rootName += component->rootName;

}

void ExtractPublicDependencies(std::unordered_map<std::string, ComponentLakos*>& components)
{
	for (auto& c : components)
	{
		ComponentLakos* comp = c.second;
		for (auto& fp : comp->m_files)
		{
			if (fp->hasExternalInclude)
			{
				for (auto& dep : fp->dependencies)
				{
					comp->privDeps.erase(dep->lakosComponent);
					comp->pubDeps.insert(dep->lakosComponent);
				}
			}
		}
		comp->pubDeps.erase(comp);
		comp->privDeps.erase(comp);
	}
}

std::vector<std::string> SortedNiceNames(const std::unordered_set<ComponentLakos*>& comps)
{
	std::vector<std::string> ret;
	ret.resize(comps.size());
	std::transform(comps.begin(), comps.end(), ret.begin(), [](const ComponentLakos* comp) -> std::string { return comp->NiceName('.'); });
	std::sort(ret.begin(), ret.end());

	return ret;
}

size_t NodesWithCycles(std::unordered_map<std::string, ComponentLakos*>& components)
{
	size_t count = 0;
	for (auto& c : components)
	{
		if (!c.second->circulars.empty())
		{
			count++;
		}
	}
	return count;
}
