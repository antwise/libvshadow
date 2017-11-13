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

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libvshadow_block_range_descriptor.h"
#include "libvshadow_debug.h"
#include "libvshadow_definitions.h"
#include "libvshadow_store.h"
#include "libvshadow_store_block.h"
#include "libvshadow_store_descriptor.h"
#include "libvshadow_block_descriptor.h"
#include "libvshadow_store_blocks_iterator.h"

#include "vshadow_store.h"

static int libvshadow_store_blocks_iterator_data_free(
	libvshadow_blocks_iterator_internal_t *iterator,
	libcerror_error_t **error )
{
	static char *function                          = "libvshadow_store_blocks_iterator_data_free";

	if( iterator->block_descriptor != NULL )
	{
		if( libvshadow_block_descriptor_free(
			&iterator->block_descriptor,
			error ) != 1 )
		{
			libcerror_error_set(
				error,
				LIBCERROR_ERROR_DOMAIN_RUNTIME,
				LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				"%s: unable to free block descriptor.",
				function );
		}
	}
	iterator->block_descriptor = NULL;

	if( libvshadow_store_block_free(
		&iterator->store_block,
		error ) != 1 )
	{
		libcerror_error_set(
			error,
			LIBCERROR_ERROR_DOMAIN_RUNTIME,
			LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			"%s: unable to free store block.",
			function );
	}
	iterator->store_block = NULL;

	iterator->next_offset = 0;
	iterator->file_io_handle = 0;
	iterator->store_descriptor = 0;

	memory_free(iterator);

	if(error != NULL)
		return -1;

	return 1;
}

int libvshadow_store_blocks_iterator_free(
	libvshadow_blocks_iterator_internal_t **iterator,
	libcerror_error_t **error )
{
	int res;

	if(iterator == NULL)
		return 1;

	res = libvshadow_store_blocks_iterator_data_free(*iterator, error);
	*iterator = NULL;

	return res;
}

libvshadow_blocks_iterator_internal_t* libvshadow_store_blocks_iterator_create(
	libvshadow_store_t *store,
	libcerror_error_t **error )
{
	static char *function                          = "libvshadow_store_blocks_iterator_create";
	libvshadow_blocks_iterator_internal_t* result  = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	libvshadow_internal_store_t *internal_store     = NULL;

	if( store == NULL )
	{
		libcerror_error_set(
			error,
			LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			"%s: invalid store.",
			function );

		return( NULL );
	}
	internal_store = (libvshadow_internal_store_t *) store;

	if( internal_store->internal_volume == NULL )
	{
		libcerror_error_set(
			error,
			LIBCERROR_ERROR_DOMAIN_RUNTIME,
			LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			"%s: invalid store - missing internal volume.",
			function );

		return( NULL );
	}
	if( libcdata_array_get_entry_by_index(
		internal_store->internal_volume->store_descriptors_array,
		internal_store->store_descriptor_index,
		(intptr_t **) &store_descriptor,
		error ) != 1 )
	{
		libcerror_error_set(
			error,
			LIBCERROR_ERROR_DOMAIN_RUNTIME,
			LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			"%s: unable to retrieve store descriptor: %d.",
			function,
			internal_store->store_descriptor_index );

		return( NULL );
	}

	result = memory_allocate_structure(
		libvshadow_blocks_iterator_internal_t );

	if( result == NULL )
	{
		libcerror_error_set(
			error,
			LIBCERROR_ERROR_DOMAIN_MEMORY,
			LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			"%s: unable to create iterator.",
			function );

		return( NULL );
	}
	if( memory_set(
		result,
		0,
		sizeof( libvshadow_blocks_iterator_internal_t ) ) == NULL )
	{
		libcerror_error_set(
			error,
			LIBCERROR_ERROR_DOMAIN_MEMORY,
			LIBCERROR_MEMORY_ERROR_SET_FAILED,
			"%s: unable to clear iterator.",
			function );

		libvshadow_store_blocks_iterator_data_free( result, error );
		return( NULL );
	}

	if( libvshadow_store_block_initialize(
		&result->store_block,
		0x4000,
		error ) != 1 )
	{
		libcerror_error_set(
			error,
			LIBCERROR_ERROR_DOMAIN_RUNTIME,
			LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			"%s: unable to create store block.",
			function );

		libvshadow_store_blocks_iterator_data_free( result, error );
		return( NULL );
	}

	if( libvshadow_block_descriptor_initialize(
		&result->block_descriptor,
		error ) != 1 )
	{
		libcerror_error_set(
			error,
			LIBCERROR_ERROR_DOMAIN_RUNTIME,
			LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			"%s: unable to create block descriptor.",
			function );

		libvshadow_store_blocks_iterator_data_free( result, error );
		return( NULL );
	}

	result->store_descriptor = store_descriptor;
	result->file_io_handle = internal_store->file_io_handle;
	result->next_offset = store_descriptor->store_block_list_offset;

	return libvshadow_store_blocks_iterator_next(result, error);
}

libvshadow_blocks_iterator_internal_t* libvshadow_store_blocks_iterator_next(
	libvshadow_blocks_iterator_internal_t *iterator,
	libcerror_error_t **error )
{
	static char *function                          = "libvshadow_store_blocks_iterator_next";
	int result                                     = 0;
	while(result == 0) // while read empty records
	{
		while(iterator->block_size < sizeof( vshadow_store_block_list_entry_t )) // read store block
		{
			if(iterator->next_offset == 0) // eof
			{
				libvshadow_store_blocks_iterator_data_free( iterator, error );
				return( NULL );
			}

			if( libvshadow_store_block_read(
				iterator->store_block,
				iterator->file_io_handle,
				iterator->next_offset,
				error ) == -1 )
			{
				libcerror_error_set(
					error,
					LIBCERROR_ERROR_DOMAIN_IO,
					LIBCERROR_IO_ERROR_READ_FAILED,
					"%s: unable to read store block at offset: %" PRIi64 ".",
					function,
					iterator->next_offset );

				libvshadow_store_blocks_iterator_data_free( iterator, error );
				return( NULL );
			}

			if( iterator->store_block->record_type != LIBVSHADOW_RECORD_TYPE_STORE_INDEX )
			{
				libcerror_error_set(
					error,
					LIBCERROR_ERROR_DOMAIN_RUNTIME,
					LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
					"%s: unsupported record type: %" PRIu32 ".",
					function,
					iterator->store_block->record_type );

				libvshadow_store_blocks_iterator_data_free( iterator, error );
				return( NULL );
			}

			iterator->next_offset = iterator->store_block->next_offset;	
			iterator->block_data = &( iterator->store_block->data[ sizeof( vshadow_store_block_header_t ) ] );
			iterator->block_size = (uint16_t) ( iterator->store_block->data_size - sizeof( vshadow_store_block_header_t ) );
		} // while read store block ... 

		while( sizeof( vshadow_store_block_list_entry_t ) <= iterator->block_size && result == 0) // read data blocks in store block
		{
			result = libvshadow_block_descriptor_read_data(
				iterator->block_descriptor,
				iterator->block_data,
				iterator->block_size,
				iterator->store_descriptor->index,
				error );

			if( result == -1 )
			{
				libcerror_error_set(
					error,
					LIBCERROR_ERROR_DOMAIN_IO,
					LIBCERROR_IO_ERROR_READ_FAILED,
					"%s: unable to read block descriptor.",
					function );

				libvshadow_store_blocks_iterator_data_free( iterator, error );
				return( NULL );
			}
			else if( result != 0 ) // we have data
			{
				iterator->original_offset  = iterator->block_descriptor->original_offset;
				iterator->relative_offset  = iterator->block_descriptor->relative_offset;
				iterator->offset  = iterator->block_descriptor->offset;
				iterator->flags  = iterator->block_descriptor->flags;
				iterator->bitmap  = iterator->block_descriptor->bitmap;
			}

			iterator->block_data += sizeof( vshadow_store_block_list_entry_t );
			iterator->block_size -= sizeof( vshadow_store_block_list_entry_t );
		} // while read data block ...
	} // while read empty blocks ...

	return iterator;
}

