/*
 * Store iterator functions
 *
 * Copyright (C) 2011-2017, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined( _LIBVSHADOW_STORE_BLOCKS_ITERATOR_H )
#define _LIBVSHADOW_STORE_BLOCKS_ITERATOR_H

#include <common.h>
#include <types.h>

#include "libvshadow_extern.h"
#include "libvshadow_block_descriptor.h"
#include "libvshadow_libcerror.h"
#include "libvshadow_libcthreads.h"

#if defined( __cplusplus )
extern "C" {
#endif

struct libvshadow_blocks_iterator_internal
{
	/* Block data
	*/
	 off64_t original_offset;
     off64_t relative_offset;
     off64_t offset;
     uint32_t flags;
     uint32_t bitmap;

	 /*      Private data         */

	 /* The file IO handle
	 */
	libbfio_handle_t *file_io_handle;

	/* The store descriptor
	 */
	libvshadow_store_descriptor_t *store_descriptor;

	/* The store block
	 */
	libvshadow_store_block_t *store_block;

	/* The block
	 */
	libvshadow_block_descriptor_t *block_descriptor;

	/* The offset of reading
	 */
	off64_t next_offset;

	/* The block data
	 */
	uint8_t *block_data;

	/* The block size
	 */
	uint16_t block_size;
};

typedef struct libvshadow_blocks_iterator_internal libvshadow_blocks_iterator_internal_t;

LIBVSHADOW_EXTERN \
libvshadow_blocks_iterator_internal_t* libvshadow_store_blocks_iterator_create(
     libvshadow_store_t *store,
     libcerror_error_t **error );

LIBVSHADOW_EXTERN \
libvshadow_blocks_iterator_internal_t* libvshadow_store_blocks_iterator_next(
     libvshadow_blocks_iterator_internal_t *iterator,
     libcerror_error_t **error );

LIBVSHADOW_EXTERN \
int libvshadow_store_blocks_iterator_free(
     libvshadow_blocks_iterator_internal_t ** iterator,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBVSHADOW_STORE_BLOCKS_ITERATOR_H ) */

