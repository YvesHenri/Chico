#ifndef ECS_ENTITY_DEF
#define ECS_ENTITY_DEF

namespace ecs
{
	class EntityManager;

	class Entity final
	{
	public:
		static const int ID_MASK = 0xFFFFFF;
		static const int VERSION_MASK = 0xFF;
		static const int VERSION_SHIFT = 24;

		Entity() = delete;
		Entity(const Entity&) = default;
		Entity(unsigned id, EntityManager* manager);

		unsigned id() const;
		unsigned version() const;

		template <typename Component>
		Component& component() const;

		template <typename Component, typename... Args>
		Component assign(Args&&... args) const;

		template <typename Component, typename... Args>
		Component replace(Args&&... args) const;

		template <typename Component, typename... Args>
		Component save(Args&&... args) const;

		template <typename Component, typename... Components>
		void assign(const Component& component, const Components&... components) const;

		template <typename Component, typename... Components>
		void replace(const Component& component, const Components&... components) const;

		template <typename Component, typename... Components>
		void save(const Component& component, const Components&... components) const;

		template <typename Component, typename... Components>
		void reset(const Component* unused, const Components*... unuseds);

		template <typename Component, typename... Components>
		void reset();

		template <typename Component, typename... Components>
		void remove(const Component* unused, const Components*... unuseds);

		template <typename Component, typename... Components>
		void remove();

		template <typename Component, typename... Components>
		bool has() const;

		template <typename Component, typename... Components>
		bool has(const Component* unused, const Components*... unuseds) const;

		void destroy();
		bool valid() const;

		bool operator==(const Entity& other) const;
		bool operator!=(const Entity& other) const;

		operator bool() const;

	private:
		unsigned identifier;
		EntityManager* manager;
	};
}

#endif