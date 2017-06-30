#pragma once

#include "Component.h"

// A component as defined by John Lakos is a pair of header / source files which 
// constitutes a unit of translation

class ComponentLakos
{

	public:
	
	std::vector<const File*> m_files;
	filesystem::path root;

	std::string rootName;


	// deps are the dependencies of your component
	std::unordered_set<ComponentLakos*> pubDeps;
	std::unordered_set<ComponentLakos*> privDeps;

	// links are the components which are using your component
	std::unordered_set<ComponentLakos *> pubLinks;
	std::unordered_set<ComponentLakos *> privLinks;
	std::unordered_set<ComponentLakos *> circulars;
	std::set<std::string> buildAfters;

	size_t index, lowlink;
	bool onStack;

	std::string QuotedName() const;
	std::string NiceName(char sub) const;

	ComponentLakos(const File* headerFile, const File* sourceFile);

	void MergeComponent(ComponentLakos* component);

	size_t loc() const {
		size_t l = 0;
		for (auto f : m_files) { l += f->loc; }
		return l;
	}
};

void ExtractPublicDependencies(std::unordered_map<std::string, ComponentLakos *> &components);

std::vector<std::string> SortedNiceNames(const std::unordered_set<ComponentLakos*>& comps);

size_t NodesWithCycles(std::unordered_map<std::string, ComponentLakos*>& components);