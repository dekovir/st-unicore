#pragma once
#include "unicore/Defs.hpp"

namespace unicore
{
	template<typename ... Args>
	class PublicEvent
	{
	public:
		using ActionType = Action<Args...>;

		UC_NODISCARD bool empty() const { return _actions.empty(); }

		void add(const ActionType& action)
		{
			_actions.push_back(action);
		}

		bool remove(const ActionType& action)
		{
			// TODO: Fix compile error
			//for (auto it = _actions.begin(); it != _actions.end(); ++it)
			//{
			//	if (*it == action)
			//	{
			//		_actions.erase(it);
			//		return true;
			//	}
			//}

			return false;
		}

		PublicEvent& operator+=(const ActionType& action)
		{
			add(action);
			return *this;
		}

		PublicEvent& operator-=(const ActionType& action)
		{
			remove(action);
			return *this;
		}

	protected:
		List<ActionType> _actions;
	};

	template<typename ... Args>
	class Event : public PublicEvent<Args...>
	{
	public:

		void invoke(Args... args)
		{
			for (const auto& action : PublicEvent<Args...>::_actions)
				action(std::forward<Args>(args)...);
		}

		void operator()(Args... args)
		{
			invoke(std::forward<Args>(args)...);
		}
	};

	template<>
	class Event<void> : public PublicEvent<void>
	{
	public:

		void invoke()
		{
			for (const auto& action : _actions)
				action();
		}

		void operator()()
		{
			invoke();
		}
	};

#define UC_OBJECT_EVENT(name, ...) \
	public: using Event_ ## name = PublicEvent<__VA_ARGS__>; \
	public: PublicEvent<__VA_ARGS__>& on_ ## name() { return _event_ ## name; } \
	protected: Event<__VA_ARGS__> _event_ ## name
}