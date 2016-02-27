#pragma once
#include "pch.h"
#include "RResource.h"
#include "Types.h"

namespace RAPI
{
	class RBaseBuffer : public RResource
	{
	public:
		RBaseBuffer();

		virtual ~RBaseBuffer();

		/**
        * Getters
        */
		unsigned int GetSizeInBytes() { return SizeInBytes; }

		// Bindflags
		EBindFlags GetBindFlags() { return BindFlags; }

		// Usage hints
		EUsageFlags GetUsage() { return Usage; }

		// CPU-Access flags
		ECPUAccessFlags GetCpuAccess() { return CpuAccess; }

		// Optional file-name, if this comes from a mesh
		std::string GetFileName() { return FileName; }

		// Size of the structures held by the buffer
		unsigned int GetStructuredByteSize() { return StructuredByteSize; }

		void SetStructuredByteSize(unsigned int structureSize) { StructuredByteSize = structureSize; }

	protected:
		// Size of the whole buffer in bytes
		unsigned int SizeInBytes;

		// Bindflags
		EBindFlags BindFlags;

		// Usage hints
		EUsageFlags Usage;

		// CPU-Access flags
		ECPUAccessFlags CpuAccess;

		// Optional file-name, if this comes from a mesh
		std::string FileName;

		// Optional size of the structures held by the buffer
		unsigned int StructuredByteSize;
	};
}