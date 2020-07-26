#ifndef ENTITIES_COMPONENT_COLLECTION_IMPL
#define ENTITIES_COMPONENT_COLLECTION_IMPL

#include <vector>

#include "../../Messages/MessageManager.hpp"
#include "../../Entities/Component/Message/ComponentAdded.hpp"
#include "../../Entities/Component/Message/ComponentRemoved.hpp"

namespace ecs
{
	/**
	* @brief Sparse set implementation.
	*/
	class Collection
	{
	public:
		using Item = unsigned;
		using Index = unsigned;
		using Iterator = std::vector<Item>::iterator;

		Collection() = default;
		Collection(const Collection&) = delete; // No copying
		Collection(Collection&&) = default;
		virtual ~Collection() = default;

		virtual bool empty() const {
			return values.empty();
		}
		
		virtual void clear() {
			values.clear();
			indices.clear();
		}

		virtual bool add(Item item) {
			auto exists = contains(item);

			if (!exists) {
				if (item >= indices.size()) {
					indices.resize(item + 1U);
				}
				indices[item] = values.size() | OCCUPIED;
				values.push_back(item);
			}

			return !exists;
		}

		virtual bool remove(Item item) {
			auto exists = contains(item);

			if (exists) {
				auto last = values.back();
				auto index = indices[item] & ~OCCUPIED;

				indices[last] = index | OCCUPIED;
				indices[item] = 0U;

				values[index] = last;
				values.pop_back();
			}

			return exists;
		}

		virtual bool contains(Item item) const {
			return item < indices.size() && (indices[item] & OCCUPIED) != 0U;
		}

		unsigned size() const {
			return values.size();
		}

		Iterator begin() {
			return values.begin();
		}

		Iterator end() {
			return values.end();
		}

	protected:
		static const int OCCUPIED = 0x01000000;
		std::vector<Item> values; // Where the actual values are stored (dense set)
		std::vector<Index> indices; // Where the indices to values are stored (sparse set)
	};

	/**
	* @brief Extended sparse set implementation for components.
	*
	* This specialization of a sparse set associates a component to an entity. The
	* main purpose of this class is to use sparse sets to store components.
	* It guarantees fast access both to the components and to the entities.
	*
	* @note
	* Entities and components have the same order. It's guaranteed both in case of raw
	* access (either to entities or components) and when using input iterators.
	*
	* @note
	* Internal data structures arrange elements to maximize performance. Because of
	* that, there are no guarantees that elements have the expected order when
	* iterate directly the internal packed array (see `raw` and `size` member
	* functions for that). Use `begin` and `end` instead.
	*
	* @tparam Component Type of component assigned to the entities.
	*/
	template <typename Component>
	class ComponentCollection final : public Collection
	{
	public:
		ComponentCollection(const ComponentCollection&) = delete;
		ComponentCollection(ComponentCollection&&) = default;
		explicit ComponentCollection(const std::shared_ptr<mqs::MessageManager>& messages) : messages(messages) {}

		void clear() override {
			components.clear(); // Iterate and send message
			Collection::clear();
		}

		bool reset(Collection::Item item) {
			return contains(item) ? remove(item) : false;
		}

		bool remove(Collection::Item item) override {
			auto removed = Collection::remove(item);

			if (removed) {
				auto index = indices[item] & ~OCCUPIED;
				auto component = components[index];
				components[index] = std::move(components.back()); // Shrink
				components.pop_back(); // Shrink
				messages->publish<ComponentRemoved<Component>>(component, item);
			}

			return removed;
		}

		bool add(Collection::Item item, const Component& component) {
			auto added = Collection::add(item);

			if (added) {
				components.emplace_back(component);
				messages->publish<ComponentAdded<Component>>(component, item);
			}

			return added;
		}

		bool replace(Collection::Item item, const Component& newComponent) {
			auto exists = contains(item);

			if (exists) {
				auto index = indices[item] & ~OCCUPIED;
				auto oldComponent = components[index];
				messages->publish<ComponentRemoved<Component>>(oldComponent, item);
				components[index] = newComponent;
				messages->publish<ComponentAdded<Component>>(newComponent, item);
			}

			return exists;
		}

		bool save(Collection::Item item, const Component& component) {
			return contains(item) ? replace(item, component) : add(item, component);
		}

		Component& get(Collection::Item item) {
			return components[indices[item] & ~OCCUPIED];
		}

	private:
		std::vector<Component> components;
		std::shared_ptr<mqs::MessageManager> messages;
	};
}

#endif