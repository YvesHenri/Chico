#ifndef MESSAGES_MESSAGE_IMPL
#define MESSAGES_MESSAGE_IMPL

#include "../Family.hpp"

namespace mqs
{
	// Messages can have an unique identifier if, and only if, the 'family' value does not repeat.
	// There could be up to 0xffffffff different message representations if using unsigned short 
	// for the 'id' and 'family' values, hence each have a size of 16 bits (0xffff).
	struct Message
	{
		virtual ~Message() = default;

		explicit Message(unsigned short id) : Message(id, 0U) {}
		explicit Message(unsigned short id, unsigned short family) : id(id), family(family) {}

		// Computes the unique identifier when requested
		unsigned int uid() const {
			static int bits = sizeof(unsigned short) * 8;
			return family << bits | id;
		}

		const unsigned short id;
		const unsigned short family;
	};

	// A managed message which uses CRTP to identify their superclass in order to generate an unique 'family' value. 
	// Note that the generated family values may clash with eventual custom Message instances, when using both.
	template <typename M>
	struct ManagedMessage : Message
	{
		explicit ManagedMessage(unsigned short id) : Message(id, MessageFamily::uid<M>()) {}
	};
}

#endif