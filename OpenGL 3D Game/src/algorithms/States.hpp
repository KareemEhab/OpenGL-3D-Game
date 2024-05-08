#ifndef STATES_HPP
#define STATES_HPP

namespace States
{
	// Check state
	template<typename T>
	bool isIndexActive(T* states, int target)
	{
		return (*states & (1 << target)) == (1 << target);
	}

	// Activate state
	template<typename T>
	void activateIndex(T* states, int target)
	{
		*states |= 1 << target;
	}

	// Uniquely activate state
	template<typename T>
	void uniquelyActivateIndex(T* states, int target)
	{
		activateIndex<T>(states, target);	// Activate state
		*states &= 1 << target;			// Deactivate other states
	}

	// Deactivate states
	template<typename T>
	void deactivateIndex(T* states, int target)
	{
		*states &= ~(1 << target);
	}

	// Toggle state
	template<typename T>
	void toggleIndex(T* states, int target)
	{
		*states ^= 1 << target;
	}

	// Check if state is active
	template<typename T>
	bool isActive(T* states, T state)
	{
		return (*states & state) == state;
	}

	// Activate state
	template<typename T>
	void activate(T* states, T state) {
		*states |= state;
	}

	// Uniquely activate state (no others can be active)
	template<typename T>
	void uniquelyActivate(T* states, T state) {
		*states &= state;
	}

	// Deactivate state
	template<typename T>
	void deactivate(T* states, T state) {
		*states &= ~state;
	}

	// Toggle state
	template<typename T>
	void toggle(T* states, T state) {
		*states ^= state;
	}
}

#endif