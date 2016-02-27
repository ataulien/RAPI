#pragma once

namespace RAPI
{
	class RResource
	{
	public:
		RResource(void);

		virtual ~RResource(void);

		/**
		 * Getters/Setters
		 */
		void SetID(unsigned int id)
		{ ID = id; }

		unsigned int GetID()
		{ return ID; }

	protected:
		// ID of this resource. If we know that we haven't created a lot of these
		// resources, we can use smaller datatypes to find this objects with this.
		// For example, it's safe to assume that we won't create more than 65k textures
		// and thus only use a 16-bit ID in the draw-call data.
		unsigned int ID;
	};
}

