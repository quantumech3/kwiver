/*ckwg +29
 * Copyright 2016 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 * \brief Implementation for detected_object_set_input_kpf
 */

#include "detected_object_set_input_kpf.h"

#include <vital/util/tokenize.h>
#include <vital/util/data_stream_reader.h>
#include <vital/logger/logger.h>
#include <vital/exceptions.h>

#include <map>
#include <sstream>
#include <cstdlib>

namespace kwiver {
namespace arrows {
namespace kpf {

// ------------------------------------------------------------------
class detected_object_set_input_kpf::priv
{
public:
  priv( detected_object_set_input_kpf* parent)
    : m_parent( parent )
    , m_logger( kwiver::vital::get_logger( "detected_object_set_input_kpf" ) )
    , m_first( true )
  { }

  ~priv() { }

  void read_all();

  detected_object_set_input_kpf* m_parent;
  kwiver::vital::logger_handle_t m_logger;
  bool m_first;

  int m_current_idx;
  int m_last_idx;

  // Map of detected objects indexed by frame number. Each set
  // contains all detections for a single frame.
  std::map< int, kwiver::vital::detected_object_set_sptr > m_detected_sets;
};


// ==================================================================
detected_object_set_input_kpf::
detected_object_set_input_kpf()
  : d( new detected_object_set_input_kpf::priv( this ) )
{
}


detected_object_set_input_kpf::
~detected_object_set_input_kpf()
{
}


// ------------------------------------------------------------------
void
detected_object_set_input_kpf::
set_configuration(vital::config_block_sptr config)
{ }


// ------------------------------------------------------------------
bool
detected_object_set_input_kpf::
check_configuration(vital::config_block_sptr config) const
{
  return true;
}


// ------------------------------------------------------------------
bool
detected_object_set_input_kpf::
read_set( kwiver::vital::detected_object_set_sptr & set, std::string& image_name )
{
  if ( d->m_first )
  {
    // Read in all detections
    d->read_all();
    d->m_first = false;

    // set up iterators for returning sets.
    d->m_current_idx = d->m_detected_sets.begin()->first;
    d->m_last_idx = d->m_detected_sets.rbegin()->first;
  } // end first

  // we do not return image name
  image_name.clear();

  // test for end of stream
  if (this->at_eof())
  {
    return false;
  }

  // return detection set at current index if there is one
  if ( 0 == d->m_detected_sets.count( d->m_current_idx ) )
  {
    // return empty set
    set = std::make_shared< kwiver::vital::detected_object_set>();
  }
  else
  {
    // Return detections for this frame.
    set = d->m_detected_sets[d->m_current_idx];
  }

  ++d->m_current_idx;

  return true;
}


// ------------------------------------------------------------------
void
detected_object_set_input_kpf::
new_stream()
{
  d->m_first = true;
}


// ==================================================================
void
detected_object_set_input_kpf::priv::
read_all()
{
  std::string line;
  kwiver::vital::data_stream_reader stream_reader( m_parent->stream() );

  m_detected_sets.clear();

  while ( stream_reader.getline( line ) )
  {
    std::vector< std::string > col;
    kwiver::vital::tokenize( line, col, " ", true );

    if ( ( col.size() < 18 ) || ( col.size() > 20 ) )
    {
      std::stringstream str;
      str << "This is not a kpf file; found " << col.size()
          << " columns in\n\"" << line << "\"";
      throw kwiver::vital::invalid_data( str.str() );
    }

    /*
     * Check to see if we have seen this frame before. If we have,
     * then retrieve the frame's index into our output map. If not
     * seen before, add frame -> detection set index to our map and
     * press on.
     *
     * This allows for track states to be written in a non-contiguous
     * manner as may be done by streaming writers.
     */
    int index = 0;
    if ( 0 == m_detected_sets.count( index ) )
    {
      // create a new detection set entry
      m_detected_sets[ index ] = std::make_shared<kwiver::vital::detected_object_set>();
    }

    kwiver::vital::bounding_box_d bbox(
      atof( col[0].c_str() ),
      atof( col[0].c_str() ),
      atof( col[0].c_str() ),
      atof( col[0].c_str() ) );

    double conf(1.0);
    if ( col.size() == 19 )
    {
      conf = atof( col[0].c_str() );
    }

    // Create detection
    kwiver::vital::detected_object_sptr dob = std::make_shared< kwiver::vital::detected_object>( bbox, conf );

    // Add detection to set for the frame
    m_detected_sets[index]->add( dob );
  } // ...while !eof
} // read_all

} } } // end namespace
