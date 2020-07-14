#ifndef ENTITIES_COMPONENT_COLLECTION_INTERSECTION_IMPL
#define ENTITIES_COMPONENT_COLLECTION_INTERSECTION_IMPL

#include <tuple>
#include <vector>
#include <cstdint>
#include <algorithm>

#include "ComponentCollection.hpp"

namespace ecs
{
	template <typename... Components>
	class ComponentCollectionIntersection final
	{
	public:
		class Iterator final //: public std::iterator<std::input_iterator_tag, std::uint32_t>
		{
		public:
			// C++17 iterator traits
			using difference_type = unsigned;
			using value_type = unsigned;
			using pointer = const unsigned*;
			using reference = const unsigned&;
			using iterator_category = std::input_iterator_tag;

			Iterator(std::vector<ecs::Collection*> others, ecs::Collection::Iterator begin, ecs::Collection::Iterator end)
				: collections(others)
				, begin(begin)
				, end(end)
			{
				if (begin != end && !intersects())
					++(*this);
			}

			Iterator& operator++() { // ++i
				return (++begin != end && !intersects()) ? ++(*this) : (*this);
			}

			Iterator operator++(int) { // i++
				Iterator it = *this;
				++(*this);
				return it;
			}

			std::uint32_t operator*() const {
				return *begin;
			}

			bool operator==(const Iterator& other) const {
				return other.begin == begin;
			}

			bool operator!=(const Iterator& other) const {
				return other.begin != begin;
			}

		protected:
			bool intersects() const {
				auto index = collections.size();

				// For all the other sets, check whether they have the smallest set's begin iterator value
				for (auto entity = *begin; index && collections[index - 1U]->contains(entity); --index);

				// Will only return true if index ever reaches 0 (both comparables contain it)
				return index == 0U;
			}

		private:
			ecs::Collection::Iterator begin;
			ecs::Collection::Iterator end;
			std::vector<ecs::Collection*> collections;
		};

		ComponentCollectionIntersection(ecs::ComponentCollection<Components>&... collections)
			: others(sizeof...(Components) - 1U)
			, smallest(nullptr)
			, all(collections...)
		{
			auto index = 0U;
			auto size = std::max({ collections.size()... }) + 1U;

			// Assigns the collection with the lowest amount of elements
			auto probe = [this](auto size, auto& collection) {
				return collection.size() < size ? (smallest = &collection, collection.size()) : size;
			};

			// Filters all those collections bigger than the smallest collection
			auto filter = [this](auto index, auto& collection) {
				return (smallest != &collection) ? (others[index++] = &collection, index) : index;
			};

			// Execute lambdas using braced-init-lists technique
			auto probing = { 0U, (size = probe(size, collections), 0U)... };
			auto filtering = { 0U, (index = filter(index, collections), 0U)... };
		}

		Iterator begin() {
			return Iterator(others, smallest->begin(), smallest->end());
		}

		Iterator end() {
			return Iterator(others, smallest->end(), smallest->end());
		}

		template <typename Component>
		ComponentCollection<Component>& get() {
			return std::get<ComponentCollection<Component>&>(all);
		}

	private:
		ecs::Collection* smallest;
		std::vector<ecs::Collection*> others;
		std::tuple<ComponentCollection<Components>&...> all;
	};
}

#endif