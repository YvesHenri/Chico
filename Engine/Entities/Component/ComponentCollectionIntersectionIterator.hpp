#pragma once

#include <vector>

#include "ComponentCollection.hpp"

namespace ecs
{
	class ComponentCollectionIntersectionIterator final
	{
	public:
		// C++17 iterator traits
		using difference_type = unsigned;
		using value_type = unsigned;
		using pointer = const unsigned*;
		using reference = const unsigned&;
		using iterator_category = std::input_iterator_tag;

		explicit ComponentCollectionIntersectionIterator(std::vector<ecs::Collection*> others, ecs::Collection::Iterator begin, ecs::Collection::Iterator end);

		ComponentCollectionIntersectionIterator& operator++(); // ++i
		ComponentCollectionIntersectionIterator operator++(int); // i++

		unsigned operator*() const;

		bool operator==(const ComponentCollectionIntersectionIterator& other) const;
		bool operator!=(const ComponentCollectionIntersectionIterator& other) const;

	protected:
		bool intersects() const;

	private:
		ecs::Collection::Iterator begin;
		ecs::Collection::Iterator end;
		std::vector<ecs::Collection*> collections;
	};
}