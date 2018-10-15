/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Authors: Ruchika Gupta <ruchika.gupta@nxp.com>
 *
 */

#include <platform_def.h>
#include <bl_common.h>
#include <desc_image_load.h>
#include <platform.h>
#include <tbbr_img_def.h>
#include <debug.h>
#ifdef CSF_HEADER_PREPENDED
#include <csf_hdr.h>
#endif

/*******************************************************************************
 * Following descriptor provides BL image/ep information that gets used
 * by BL2 to load the images and also subset of this information is
 * passed to next BL image. The image loading sequence is managed by
 * populating the images in required loading order. The image execution
 * sequence is managed by populating the `next_handoff_image_id` with
 * the next executable image id.
 ******************************************************************************/
static bl_mem_params_node_t bl2_mem_params_descs[] = {
#ifdef POLICY_FUSE_PROVISION
	/* Fill SCP_BL2 related information if it exists */
    {
	    .image_id = SCP_BL2_IMAGE_ID,

		SET_STATIC_PARAM_HEAD(ep_info, PARAM_IMAGE_BINARY,
			VERSION_2, entry_point_info_t, SECURE | NON_EXECUTABLE),

		SET_STATIC_PARAM_HEAD(image_info, PARAM_IMAGE_BINARY,
			VERSION_2, image_info_t, 0),

	/*
	 * SCP Image is Fuse file which is loaded temporarily in the same
	 * location as BL31. It is later overridden by BL31 image.
	 * The order needs to be maintained in this file, SCP entry
	 * should be before BL31 to avoid overwrite of BL31
	 */
#ifdef CSF_HEADER_PREPENDED
		.image_info.image_base = BL31_BASE - CSF_HDR_SZ,
		.image_info.image_max_size = (BL31_LIMIT - BL31_BASE) +
								CSF_HDR_SZ,
#else
		.image_info.image_base = BL31_BASE,
		.image_info.image_max_size = (BL31_LIMIT - BL31_BASE),
#endif
		.ep_info.pc = BL31_BASE,

	    .next_handoff_image_id = INVALID_IMAGE_ID,
    },
#endif /* POLICY_FUSE_PROVISION */

	/* Fill BL31 related information */
	{
		.image_id = BL31_IMAGE_ID,

		SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
				VERSION_2, entry_point_info_t,
				SECURE | EXECUTABLE | EP_FIRST_EXE),
		.ep_info.pc = BL31_BASE,
		.ep_info.spsr = SPSR_64(MODE_EL3, MODE_SP_ELX,
				DISABLE_ALL_EXCEPTIONS),
#if DEBUG
		.ep_info.args.arg1 = LS_BL31_PLAT_PARAM_VAL,
#endif

		SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
			VERSION_2, image_info_t, IMAGE_ATTRIB_PLAT_SETUP),
#ifdef CSF_HEADER_PREPENDED
		.image_info.image_base = BL31_BASE - CSF_HDR_SZ,
		.image_info.image_max_size = (BL31_LIMIT - BL31_BASE) +
								CSF_HDR_SZ,
#else
		.image_info.image_base = BL31_BASE,
		.image_info.image_max_size = (BL31_LIMIT - BL31_BASE),
#endif

# ifdef LS_LOAD_BL32
		.next_handoff_image_id = BL32_IMAGE_ID,
# else
		.next_handoff_image_id = BL33_IMAGE_ID,
# endif
	},
# ifdef LS_LOAD_BL32
	/* Fill BL32 related information */
	{
		.image_id = BL32_IMAGE_ID,

		SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
			VERSION_2, entry_point_info_t, SECURE | EXECUTABLE),
		.ep_info.pc = BL32_BASE,

		SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
				VERSION_2, image_info_t, 0),
#ifdef CSF_HEADER_PREPENDED
		.image_info.image_base = BL32_BASE - CSF_HDR_SZ,
		.image_info.image_max_size = (BL32_LIMIT - BL32_BASE) +
								CSF_HDR_SZ,
#else
		.image_info.image_base = BL32_BASE,
		.image_info.image_max_size = (BL32_LIMIT - BL32_BASE),
#endif
		.next_handoff_image_id = BL33_IMAGE_ID,
	},
# endif /* BL32_BASE */

	/* Fill BL33 related information */
	{
		.image_id = BL33_IMAGE_ID,
		SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
			VERSION_2, entry_point_info_t, NON_SECURE | EXECUTABLE),
		.ep_info.pc = BL33_BASE,

		SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
				VERSION_2, image_info_t, 0),
#ifdef CSF_HEADER_PREPENDED
		.image_info.image_base = BL33_BASE - CSF_HDR_SZ,
		.image_info.image_max_size = (BL33_LIMIT - BL33_BASE) +
								 CSF_HDR_SZ,
#else
		.image_info.image_base = BL33_BASE,
		.image_info.image_max_size = BL33_LIMIT - BL33_BASE,
#endif
		.ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX,
					DISABLE_ALL_EXCEPTIONS),

		.next_handoff_image_id = INVALID_IMAGE_ID,
	}
};

REGISTER_BL_IMAGE_DESCS(bl2_mem_params_descs)