#!/usr/bin/env python
#
# Python-bindings read testing program
#
# Copyright (C) 2011-2016, Joachim Metz <joachim.metz@gmail.com>
#
# Refer to AUTHORS for acknowledgements.
#
# This software is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this software.  If not, see <http://www.gnu.org/licenses/>.
#

from __future__ import print_function
import argparse
import os
import sys

import pyvshadow


def get_whence_string(whence):
  """Retrieves a human readable string representation of the whence."""
  if whence == os.SEEK_CUR:
    whence_string = "SEEK_CUR"
  elif whence == os.SEEK_END:
    whence_string = "SEEK_END"
  elif whence == os.SEEK_SET:
    whence_string = "SEEK_SET"
  else:
    whence_string = "UNKNOWN"
  return whence_string


def pyvshadow_test_seek_offset_and_read_buffer(
    vshadow_store, input_offset, input_whence, input_size, expected_offset,
    expected_size):

  print(
      ("Testing reading buffer at offset: {0:d}, whence: {1:s} and "
       "size: {2:d}\t").format(
          input_offset, get_whence_string(input_whence), input_size),
      end="")

  error_string = ""
  result = True
  try:
    vshadow_store.seek(input_offset, input_whence)

    result_offset = vshadow_store.get_offset()
    if expected_offset != result_offset:
      result = False

    if result:
      result_size = 0
      while input_size > 0:
        read_size = 4096
        if input_size < read_size:
          read_size = input_size

        data = vshadow_store.read(size=read_size)
        data_size = len(data)

        input_size -= data_size
        result_size += data_size

        if data_size != read_size:
          break

      if result_size != expected_size:
        error_string = "Unexpected read count: {0:d}".format(result_size)
        result = False

  except Exception as exception:
    print(str(exception))
    if expected_offset != -1:
      result = False

  if not result:
    print("(FAIL)")
  else:
    print("(PASS)")

  if error_string:
    print(error_string)
  return result


def pyvshadow_test_read_buffer_at_offset(
    vshadow_store, input_offset, input_size, expected_offset, expected_size):

  print("Testing reading buffer at offset: {0:d} and size: {1:d}\t".format(
      input_offset, input_size), end="")

  error_string = ""
  result = True
  try:
    result_size = 0
    while input_size > 0:
      read_size = 4096
      if input_size < read_size:
        read_size = input_size

      data = vshadow_store.read_buffer_at_offset(read_size, input_offset)
      data_size = len(data)

      input_offset += data_size
      input_size -= data_size
      result_size += data_size

      if data_size != read_size:
        break

    if input_offset != expected_offset:
      error_string = "Unexpected offset: {0:d}".format(input_offset)
      result = False

    elif result_size != expected_size:
      error_string = "Unexpected read count: {0:d}".format(result_size)
      result = False

  except Exception as exception:
    print(str(exception))
    if expected_offset != -1:
      result = False

  if not result:
    print("(FAIL)")
  else:
    print("(PASS)")

  if error_string:
    print(error_string)
  return result


def pyvshadow_test_read(vshadow_store):
  file_size = vshadow_store.size

  # Case 0: test full read

  # Test: offset: 0 size: <file_size>
  # Expected result: offset: 0 size: <file_size>
  if not pyvshadow_test_seek_offset_and_read_buffer(
      vshadow_store, 0, os.SEEK_SET, file_size, 0, file_size):
    return False

  # Test: offset: 0 size: <file_size>
  # Expected result: offset: 0 size: <file_size>
  if not pyvshadow_test_seek_offset_and_read_buffer(
      vshadow_store, 0, os.SEEK_SET, file_size, 0, file_size):
    return False

  # Case 1: test buffer at offset read

  # Test: offset: <file_size / 7> size: <file_size / 2>
  # Expected result: offset: <file_size / 7> size: <file_size / 2>
  if not pyvshadow_test_seek_offset_and_read_buffer(
      vshadow_store, file_size / 7, os.SEEK_SET, file_size / 2,
      file_size / 7, file_size / 2):
    return False

  # Test: offset: <file_size / 7> size: <file_size / 2>
  # Expected result: offset: <file_size / 7> size: <file_size / 2>
  if not pyvshadow_test_seek_offset_and_read_buffer(
      vshadow_store, file_size / 7, os.SEEK_SET, file_size / 2,
      file_size / 7, file_size / 2):
    return False

  # Case 2: test read beyond media size

  if file_size < 1024:
    # Test: offset: <file_size - 1024> size: 4096
    # Expected result: offset: -1 size: <undetermined>
    if not pyvshadow_test_seek_offset_and_read_buffer(
        vshadow_store, file_size - 1024, os.SEEK_SET, 4096, -1, -1):
      return False

    # Test: offset: <file_size - 1024> size: 4096
    # Expected result: offset: -1 size: <undetermined>
    if not pyvshadow_test_seek_offset_and_read_buffer(
        vshadow_store, file_size - 1024, os.SEEK_SET, 4096, -1, -1):
      return False

  else:
    # Test: offset: <file_size - 1024> size: 4096
    # Expected result: offset: <file_size - 1024> size: 1024
    if not pyvshadow_test_seek_offset_and_read_buffer(
        vshadow_store, file_size - 1024, os.SEEK_SET, 4096,
        file_size - 1024, 1024):
      return False

    # Test: offset: <file_size - 1024> size: 4096
    # Expected result: offset: <file_size - 1024> size: 1024
    if not pyvshadow_test_seek_offset_and_read_buffer(
        vshadow_store, file_size - 1024, os.SEEK_SET, 4096,
        file_size - 1024, 1024):
      return False

  # Case 3: test buffer at offset read

  # Test: offset: <file_size / 7> size: <file_size / 2>
  # Expected result: offset: < ( file_size / 7 ) + ( file_size / 2 ) > size: <file_size / 2>
  if not pyvshadow_test_read_buffer_at_offset(
      vshadow_store, file_size / 7, file_size / 2,
      (file_size / 7) + (file_size / 2), file_size / 2):
    return False

  # Test: offset: <file_size / 7> size: <file_size / 2>
  # Expected result: offset: < ( file_size / 7 ) + ( file_size / 2 ) > size: <file_size / 2>
  if not pyvshadow_test_read_buffer_at_offset(
      vshadow_store, file_size / 7, file_size / 2,
      (file_size / 7) + (file_size / 2), file_size / 2):
    return False

  return True


def pyvshadow_test_read_file(filename):
  vshadow_volume = pyvshadow.volume()

  vshadow_volume.open(filename, "r")

  result = True
  for vshadow_store in vshadow_volume.stores:
    result = pyvshadow_test_seek(vshadow_store)
    if not result:
      break

  vshadow_volume.close()

  return result


def pyvshadow_test_read_file_object(filename):
  file_object = open(filename, "rb")
  vshadow_volume = pyvshadow.volume()

  vshadow_volume.open_file_object(file_object, "r")

  result = True
  for vshadow_store in vshadow_volume.stores:
    result = pyvshadow_test_seek(vshadow_store)
    if not result:
      break

  vshadow_volume.close()

  return result


def main():
  args_parser = argparse.ArgumentParser(description=(
      "Tests read."))

  args_parser.add_argument(
      "source", nargs="?", action="store", metavar="FILENAME",
      default=None, help="The source filename.")

  options = args_parser.parse_args()

  if not options.source:
    print("Source value is missing.")
    print("")
    args_parser.print_help()
    print("")
    return False

  if not pyvshadow_test_read_file(options.source):
    return False

  if not pyvshadow_test_read_file_object(options.source):
    return False

  return True


if __name__ == "__main__":
  if not main():
    sys.exit(1)
  else:
    sys.exit(0)
