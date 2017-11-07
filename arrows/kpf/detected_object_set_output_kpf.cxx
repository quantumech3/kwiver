/*ckwg +29
 * Copyright 2016-2017 by Kitware, Inc.
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

#include "detected_object_set_output_kpf.h"

#include <vital/util/tokenize.h>

#include <memory>
#include <vector>
#include <fstream>
#include <time.h>

#if ( __GNUC__ == 4 && __GNUC_MINOR__ < 5 && !defined(__clang__) )
  #include <cstdatomic>
#else
  #include <atomic>
#endif


namespace kwiver {
namespace arrows {
namespace kpf {

// ------------------------------------------------------------------
class detected_object_set_output_kpf::priv
{
public:
  priv( detected_object_set_output_kpf* parent)
    : m_parent( parent )
    , m_logger( kwiver::vital::get_logger( "detected_object_set_output_kpf" ) )
    , m_first( true )
    , m_frame_number( 1 )
  {}

  ~priv() {}

  void read_all();

  detected_object_set_output_kpf* m_parent;
  kwiver::vital::logger_handle_t m_logger;
  bool m_first;
  int m_frame_number;
};


// ==================================================================
detected_object_set_output_kpf::
detected_object_set_output_kpf()
  : d( new detected_object_set_output_kpf::priv( this ) )
{
}


detected_object_set_output_kpf::
~detected_object_set_output_kpf()
{
  
}


// ------------------------------------------------------------------
void
detected_object_set_output_kpf::
set_configuration( vital::config_block_sptr config_in )
{
  vital::config_block_sptr config = this->get_configuration();
  config->merge_config( config_in );
}


// ------------------------------------------------------------------
vital::config_block_sptr
detected_object_set_output_kpf::
get_configuration() const
{
  // get base config from base class
  kwiver::vital::config_block_sptr config = algorithm::get_configuration();

  return config;
}


// ------------------------------------------------------------------
bool
detected_object_set_output_kpf::
check_configuration( vital::config_block_sptr config ) const
{
  return true;
}


// ------------------------------------------------------------------
void
detected_object_set_output_kpf::
write_set( const kwiver::vital::detected_object_set_sptr set, std::string const& image_name )
{

  if (d->m_first)
  {
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char* cp =  asctime( timeinfo );
    cp[ strlen( cp )-1 ] = 0; // remove trailing newline
    const std::string atime( cp );

    // Write file header(s)
    stream() << "# 1:Track-id "
             << "2:Track-length "
             << "3:Frame-number "
             << "4:Tracking-plane-loc(x) "
             << "5:Tracking-plane-loc(y) "
             << "6:velocity(x) "
             << "7:velocity(y) "

             << "8:Image-loc(x)"
             << " 9:Image-loc(y)"
             << " 10:Img-bbox(TL_x)"
             << " 11:Img-bbox(TL_y)"
             << " 12:Img-bbox(BR_x)"
             << " 13:Img-bbox(BR_y)"
             << " 14:Area"

             << " 15:World-loc(x)"
             << " 16:World-loc(y)"
             << " 17:World-loc(z)"
             << " 18:timestamp"
             << " 19:track-confidence"
             << std::endl

      // Provide some provenience to the file. Could have a config
      // parameter that is copied to the file as a configurable
      // comment or marker.

             << "# Written on: " << atime
             << "   by: detected_object_set_output_kpf"
             << std::endl;

    d->m_first = false;

   
  } // end first

  // process all detections
  auto ie =  set->cend();
  for ( auto det = set->cbegin(); det != ie; ++det )
  {
    const kwiver::vital::bounding_box_d bbox( (*det)->bounding_box() );
    double ilx = ( bbox.min_x() + bbox.max_x() ) / 2.0;
    double ily = ( bbox.min_y() + bbox.max_y() ) / 2.0;

    static std::atomic<unsigned> id_counter( 0 );
    const unsigned id = id_counter++;

    stream() << id                  // 1: track id
             << " 1 "               // 2: track length
             << d->m_frame_number-1 // 3: frame number / set number
             << " 0 "               // 4: tracking plane x
             << " 0 "               // 5: tracking plane y
             << "0 "                // 6: velocity x
             << "0 "                // 7: velocity y
             << ilx << " "          // 8: image location x
             << ily << " "          // 9: image location y
             << bbox.min_x() << " " // 10: TL-x
             << bbox.min_y() << " " // 11: TL-y
             << bbox.max_x() << " " // 12: BR-x
             << bbox.max_y() << " " // 13: BR-y
             << bbox.area() << " "  // 14: area
             << "0 "                // 15: world-loc x
             << "0 "                // 16: world-loc y
             << "0 "                // 17: world-loc z
             << "-1 "                // 18: timestamp
             << (*det)->confidence()   // 19: confidence
             << std::endl;

  } // end foreach

  // Put each set on a new frame
  ++d->m_frame_number;
}

} } } // end namespace
