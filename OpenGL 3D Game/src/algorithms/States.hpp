#ifndef STATES_HPP
#define STATES_HPP

namespace States
{
	// Check state
	template<typename T>
	bool isActive(T* states, int target)
	{
		return (*states & (1 << target)) == (1 << target);
	}

	// Activate state
	template<typename T>
	void activate(T* states, int target)
	{
		*states |= 1 << target;
	}

	// Uniquely activate state
	template<typename T>
	void uniquelyActivate(T* states, int target)
	{
		activate<T>(states, target);	// Activate state
		*states &= 1 << target;			// Deactivate other states
	}

	// Deactivate states
	template<typename T>
	void deactivate(T* states, int target)
	{
		*states &= ~(1 << target);
	}

	// Toggle state
	template<typename T>
	void toggle(T* states, int target)
	{
		*states ^= 1 << target;
	}
}

#endif