#pragma once
#include "RResource.h"

namespace RAPI
{

	struct RBlendStateInfo {
		RBlendStateInfo() {
			SetDefault();
		}

		/** Layed out for D3D11 */
		enum EBlendFunc {
			BF_ZERO = 1,
			BF_ONE = 2,
			BF_SRC_COLOR = 3,
			BF_INV_SRC_COLOR = 4,
			BF_SRC_ALPHA = 5,
			BF_INV_SRC_ALPHA = 6,
			BF_DEST_ALPHA = 7,
			BF_INV_DEST_ALPHA = 8,
			BF_DEST_COLOR = 9,
			BF_INV_DEST_COLOR = 10,
			BF_SRC_ALPHA_SAT = 11,
			BF_BLEND_FACTOR = 14,
			BF_INV_BLEND_FACTOR = 15,
			BF_SRC1_COLOR = 16,
			BF_INV_SRC1_COLOR = 17,
			BF_SRC1_ALPHA = 18,
			BF_INV_SRC1_ALPHA = 19
		};

		/** Layed out for D3D11 */
		enum EBlendOp {
			BO_BLEND_OP_ADD = 1,
			BO_BLEND_OP_SUBTRACT = 2,
			BO_BLEND_OP_REV_SUBTRACT = 3,
			BO_BLEND_OP_MIN = 4,
			BO_BLEND_OP_MAX = 5
		};

		/** Hint for the rendering pipeline to what this comes close to */
		enum EBlendTypeHint {
			BTH_OPAQUE,
			BTH_ADDITIVE,
			BTH_MASKED,
			BTH_BLEND
		};

		/** Sets the default values for this struct */
		RBlendStateInfo &SetDefault() {
			SrcBlend = BF_SRC_ALPHA;
			DestBlend = BF_INV_SRC_ALPHA;
			BlendOp = BO_BLEND_OP_ADD;
			SrcBlendAlpha = BF_ONE;
			DestBlendAlpha = BF_ZERO;
			BlendOpAlpha = BO_BLEND_OP_ADD;
			BlendEnabled = false;
			AlphaToCoverage = false;
			ColorWritesEnabled = true;
			BlendTypeHint = BTH_OPAQUE;
			return *this;
		}

		/** Sets up masked-rendering */
		RBlendStateInfo &SetMasked() {
			SetDefault();

			BlendTypeHint = BTH_MASKED;
			return *this;
		}

		/** Sets up alphablending */
		RBlendStateInfo &SetAlphaBlending() {
			SrcBlend = BF_SRC_ALPHA;
			DestBlend = BF_INV_SRC_ALPHA;
			BlendOp = BO_BLEND_OP_ADD;
			SrcBlendAlpha = BF_ONE;
			DestBlendAlpha = BF_ZERO;
			BlendOpAlpha = BO_BLEND_OP_ADD;
			BlendEnabled = true;
			AlphaToCoverage = false;
			ColorWritesEnabled = true;
			BlendTypeHint = BTH_BLEND;
			return *this;
		}

		/** Sets up additive blending */
		RBlendStateInfo &SetAdditiveBlending() {
			SrcBlend = BF_SRC_ALPHA;
			DestBlend = BF_ONE;
			BlendOp = BO_BLEND_OP_ADD;
			SrcBlendAlpha = BF_ONE;
			DestBlendAlpha = BF_ZERO;
			BlendOpAlpha = BO_BLEND_OP_ADD;
			BlendEnabled = true;
			AlphaToCoverage = false;
			BlendTypeHint = BTH_ADDITIVE;
			return *this;
		}

		/** Sets up modualte blending */
		RBlendStateInfo &SetModulateBlending() {
			SrcBlend = BF_DEST_COLOR;
			DestBlend = BF_ZERO;
			BlendOp = BO_BLEND_OP_ADD;
			SrcBlendAlpha = BF_ONE;
			DestBlendAlpha = BF_ZERO;
			BlendOpAlpha = BO_BLEND_OP_ADD;
			BlendEnabled = true;
			AlphaToCoverage = false;
			BlendTypeHint = BTH_BLEND;

			return *this;
		}

		EBlendFunc SrcBlend;
		EBlendFunc DestBlend;
		EBlendOp BlendOp;
		EBlendFunc SrcBlendAlpha;
		EBlendFunc DestBlendAlpha;
		EBlendOp BlendOpAlpha;
		bool BlendEnabled;
		bool AlphaToCoverage;
		bool ColorWritesEnabled;
		EBlendTypeHint BlendTypeHint;
	};


	class RBaseBlendState :
			public RResource {
	public:
		RBaseBlendState(void);

		~RBaseBlendState(void);

		/**
         * Returns the stateinfo this state was made after
         */
		const RBlendStateInfo &GetStateInfo() const { return StateInfo; }

	protected:
		// General values about the state
		RBlendStateInfo StateInfo;
	};
}
