// This file is part of TeleSculptor, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/TeleSculptor/blob/master/LICENSE for details.

/**
 * \file
 * \brief OpenCV algorithm registration implementation
 */

#include <arrows/dbow2/kwiver_algo_dbow2_plugin_export.h>
#include <vital/algo/algorithm_factory.h>

#include <opencv2/opencv_modules.hpp>
#ifdef HAVE_OPENCV_NONFREE
#include <opencv2/nonfree/nonfree.hpp>
#endif

#include <arrows/dbow2/match_descriptor_sets.h>

namespace kwiver {
namespace arrows {
namespace dbow2 {

extern "C"
KWIVER_ALGO_DBOW2_PLUGIN_EXPORT
void
register_factories( ::kwiver::vital::plugin_loader& vpm )
{
  ::kwiver::vital::algorithm_registrar reg( vpm, "arrows.dbow2" );

  if (reg.is_module_loaded())
  {
    return;
  }

#if defined(HAVE_OPENCV_NONFREE)
  cv::initModule_nonfree();
#endif

  reg.register_algorithm< ::kwiver::arrows::dbow2::match_descriptor_sets >();

    reg.mark_module_as_loaded();
}

} // end namespace dbow2
} // end namespace arrows
} // end namespace kwiver
