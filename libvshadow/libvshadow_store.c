/*
 * Store functions
 *
 * Copyright (C) 2011-2013, Joachim Metz <joachim.metz@gmail.com>
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

#include "libvshadow_block.h"
#include "libvshadow_block_descriptor.h"
#include "libvshadow_block_tree.h"
#include "libvshadow_definitions.h"
#include "libvshadow_libbfio.h"
#include "libvshadow_libcdata.h"
#include "libvshadow_libcerror.h"
#include "libvshadow_store.h"
#include "libvshadow_store_descriptor.h"
#include "libvshadow_volume.h"

/* Creates a store
 * Make sure the value store is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_initialize(
     libvshadow_store_t **store,
     libbfio_handle_t *file_io_handle,
     libvshadow_io_handle_t *io_handle,
     libvshadow_internal_volume_t *internal_volume,
     int store_descriptor_index,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_initialize";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
	}
	if( *store != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid store value already set.",
		 function );

		return( -1 );
	}
	if( internal_volume == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid internal volume.",
		 function );

		return( -1 );
	}
	/* Make sure there is a store descriptor for the store
	 */
	if( libcdata_array_get_entry_by_index(
	     internal_volume->store_descriptors_array,
	     store_descriptor_index,
	     (intptr_t **) &store_descriptor,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve store descriptor: %d.",
		 function,
		 store_descriptor_index );

		return( -1 );
	}
	internal_store = memory_allocate_structure(
	                  libvshadow_internal_store_t );

	if( internal_store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create store.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_store,
	     0,
	     sizeof( libvshadow_internal_store_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear store.",
		 function );

		memory_free(
		 internal_store );

		return( -1 );
	}
	internal_store->file_io_handle         = file_io_handle;
	internal_store->io_handle              = io_handle;
	internal_store->internal_volume        = internal_volume;
	internal_store->store_descriptor_index = store_descriptor_index;

	*store = (libvshadow_store_t *) internal_store;

	return( 1 );

on_error:
	if( internal_store != NULL )
	{
		memory_free(
		 internal_store );
	}
	return( -1 );
}

/* Frees a store
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_free(
     libvshadow_store_t **store,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store = NULL;
	static char *function                       = "libvshadow_store_free";
	int result                                  = 1;

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
	}
	if( *store != NULL )
	{
		internal_store = (libvshadow_internal_store_t *) *store;
		*store         = NULL;

		/* The file_io_handle, io_handle and internal_volume references are freed elsewhere
		 */
		memory_free(
		 internal_store );
	}
	return( result );
}

/* Reads (store) data at the current offset into a buffer
 * Returns the number of bytes read or -1 on error
 */
ssize_t libvshadow_store_read_buffer(
         libvshadow_store_t *store,
         void *buffer,
         size_t buffer_size,
         libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store = NULL;
	static char *function                       = "libvshadow_store_read_buffer";
	ssize_t read_count                          = 0;

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
	}
	internal_store = (libvshadow_internal_store_t *) store;

	read_count = libvshadow_store_read_buffer_from_file_io_handle(
		      store,
		      internal_store->file_io_handle,
		      buffer,
		      buffer_size,
		      error );

	if( read_count != (ssize_t) buffer_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read buffer from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( read_count );
}

/* Reads (store) data at the current offset into a buffer using a Basic File IO (bfio) handle
 * Returns the number of bytes read or -1 on error
 */
ssize_t libvshadow_store_read_buffer_from_file_io_handle(
         libvshadow_store_t *store,
         libbfio_handle_t *file_io_handle,
         void *buffer,
         size_t buffer_size,
         libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_read_buffer_from_file_io_handle";
	ssize_t read_count                              = 0;

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
	}
	if( internal_store->current_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid store - current offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( (size64_t) internal_store->current_offset >= internal_store->internal_volume->size )
	{
		return( 0 );
	}
	if( (size64_t) ( internal_store->current_offset + buffer_size ) > internal_store->internal_volume->size )
	{
		buffer_size = (size_t) ( internal_store->internal_volume->size - internal_store->current_offset );
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

		return( -1 );
	}
	read_count = libvshadow_store_descriptor_read_buffer(
		      store_descriptor,
		      file_io_handle,
		      (uint8_t *) buffer,
		      buffer_size,
		      internal_store->current_offset,
		      store_descriptor,
		      error );

	if( read_count != (ssize_t) buffer_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read buffer from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	internal_store->current_offset += read_count;

	return( read_count );
}

/* Reads (store) data at a specific offset
 * Returns the number of bytes read or -1 on error
 */
ssize_t libvshadow_store_read_random(
         libvshadow_store_t *store,
         void *buffer,
         size_t buffer_size,
         off64_t offset,
         libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store = NULL;
	static char *function                       = "libvshadow_store_read_random";
	ssize_t read_count                          = 0;

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
	}
	internal_store = (libvshadow_internal_store_t *) store;

	if( libvshadow_store_seek_offset(
	     store,
	     offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek offset.",
		 function );

		return( -1 );
	}
	read_count = libvshadow_store_read_buffer_from_file_io_handle(
		      store,
		      internal_store->file_io_handle,
	              buffer,
	              buffer_size,
	              error );

	if( read_count <= -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read buffer from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( read_count );
}

/* Seeks a certain offset of the (store) data
 * Returns the offset if seek is successful or -1 on error
 */
off64_t libvshadow_store_seek_offset(
         libvshadow_store_t *store,
         off64_t offset,
         int whence,
         libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store = NULL;
	static char *function                       = "libvshadow_store_seek_offset";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
	}
	if( ( whence != SEEK_CUR )
	 && ( whence != SEEK_END )
	 && ( whence != SEEK_SET ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported whence.",
		 function );

		return( -1 );
	}
	if( whence == SEEK_CUR )
	{
		offset += internal_store->current_offset;
	}
	else if( whence == SEEK_END )
	{
		offset += (off64_t) internal_store->internal_volume->size;
	}
	if( offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid offset value out of bounds.",
		 function );

		return( -1 );
	}
	internal_store->current_offset = offset;

	return( offset );
}

/* Retrieves the current offset of the (store) data
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_offset(
     libvshadow_store_t *store,
     off64_t *offset,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store = NULL;
	static char *function                       = "libvshadow_store_get_offset";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
	}
	if( offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid offset.",
		 function );

		return( -1 );
	}
	*offset = internal_store->current_offset;

	return( 1 );
}

/* Retrieves the size
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_size(
     libvshadow_store_t *store,
     size64_t *size,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store = NULL;
	static char *function                       = "libvshadow_store_get_size";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
	}
	if( libvshadow_volume_get_size(
	     (libvshadow_volume_t *) internal_store->internal_volume,
	     size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve size from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the volume size as stored in the store information
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_volume_size(
     libvshadow_store_t *store,
     size64_t *volume_size,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_get_volume_size";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
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

		return( -1 );
	}
	if( libvshadow_store_descriptor_get_volume_size(
	     store_descriptor,
	     volume_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve volume size from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the identifier
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_identifier(
     libvshadow_store_t *store,
     uint8_t *guid,
     size_t size,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_get_identifier";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
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

		return( -1 );
	}
	if( libvshadow_store_descriptor_get_identifier(
	     store_descriptor,
	     guid,
	     size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve identifier from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the creation date and time
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_creation_time(
     libvshadow_store_t *store,
     uint64_t *filetime,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_get_creation_time";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
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

		return( -1 );
	}
	if( libvshadow_store_descriptor_get_creation_time(
	     store_descriptor,
	     filetime,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve creation time from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the copy identifier
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_copy_identifier(
     libvshadow_store_t *store,
     uint8_t *guid,
     size_t size,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_get_copy_identifier";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
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

		return( -1 );
	}
	if( libvshadow_store_descriptor_get_copy_identifier(
	     store_descriptor,
	     guid,
	     size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve copy identifier from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the copy set identifier
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_copy_set_identifier(
     libvshadow_store_t *store,
     uint8_t *guid,
     size_t size,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_get_copy_set_identifier";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
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

		return( -1 );
	}
	if( libvshadow_store_descriptor_get_copy_set_identifier(
	     store_descriptor,
	     guid,
	     size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve copy set identifier from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the attribute flags
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_attribute_flags(
     libvshadow_store_t *store,
     uint32_t *attribute_flags,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_get_attribute_flags";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
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

		return( -1 );
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

		return( -1 );
	}
	if( libvshadow_store_descriptor_get_attribute_flags(
	     store_descriptor,
	     attribute_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve attribute flags from store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of blocks
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_number_of_blocks(
     libvshadow_store_t *store,
     int *number_of_blocks,
     libcerror_error_t **error )
{
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_get_number_of_blocks";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
	}
	internal_store = (libvshadow_internal_store_t *) store;

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

		return( -1 );
	}
	if( store_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing store descriptor: %d.",
		 function,
		 internal_store->store_descriptor_index );

		return( -1 );
	}
	if( libcdata_list_get_number_of_elements(
	     store_descriptor->block_descriptors_list,
	     number_of_blocks,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of block descriptors.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a specific block
 * Returns 1 if successful or -1 on error
 */
int libvshadow_store_get_block_by_index(
     libvshadow_store_t *store,
     int block_index,
     libvshadow_block_t **block,
     libcerror_error_t **error )
{
	libvshadow_block_descriptor_t *block_descriptor = NULL;
	libvshadow_internal_store_t *internal_store     = NULL;
	libvshadow_store_descriptor_t *store_descriptor = NULL;
	static char *function                           = "libvshadow_store_get_block_by_index";

	if( store == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid store.",
		 function );

		return( -1 );
	}
	internal_store = (libvshadow_internal_store_t *) store;

	if( block == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid block.",
		 function );

		return( -1 );
	}
	if( *block != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid block value already set.",
		 function );

		return( -1 );
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

		return( -1 );
	}
	if( libcdata_list_get_value_by_index(
	     store_descriptor->block_descriptors_list,
	     block_index,
	     (intptr_t **) &block_descriptor,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve block descriptor: %d.",
		 function,
		 block_index );

		return( -1 );
	}
	if( libvshadow_block_initialize(
	     block,
	     block_descriptor,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create block: %d.",
		 function,
		 block_index );

		return( -1 );
	}
	return( 1 );
}
