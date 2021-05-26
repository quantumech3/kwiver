// This file is part of TeleSculptor, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/TeleSculptor/blob/master/LICENSE for details.

/**
 * \file
 * \brief Header defining DBoW2 implementation of match_descriptor_sets
 */

#ifndef VITAL_DBOW2_MATCH_DESCRIPTOR_SETS_H_
#define VITAL_DBOW2_MATCH_DESCRIPTOR_SETS_H_

#include <vital/vital_config.h>

#include <arrows/dbow2/kwiver_algo_dbow2_export.h>

#include <vital/algo/match_descriptor_sets.h>

namespace kwiver {
namespace arrows {
namespace dbow2 {

/// class for bag of words image matching
/**
 * This class implements bag of words image matching with DBoW2
 */
class KWIVER_ALGO_DBOW2_EXPORT match_descriptor_sets
  : public vital::algo::match_descriptor_sets
{
public:
  match_descriptor_sets();
  virtual ~match_descriptor_sets() = default;

  PLUGIN_INFO( "dbow2",
               "Use DBoW2 for bag of words matching of descriptor sets. "
               "This is currently limited to OpenCV ORB descriptors." )

  /// Add an image to the inverted file system.
  /**
   * Add the image to the inverted file system.  Future matching results may
   * include this image in their results.
   * \param[in] desc set of descriptors for the image
   * \param[in] frame_number frame of the associated image
   * \returns None
   */
  void append_to_index( const vital::descriptor_set_sptr desc,
                        vital::frame_id_t frame_number ) override;

  /// Query the inverted file system for similar images.
  /**
   * Query the inverted file system and return the most similar images.
   * \param[in] desc set of descriptors for the image
   * \returns vector of possibly matching frames found by the query
   */
  std::vector< vital::frame_id_t >
  query( const vital::descriptor_set_sptr desc ) override;

  /// Query the inverted file system for similar images and append the querying
  /// image.
  /**
   * Query the inverted file system and return the most similar
   * images.  This method may be faster than first querying and then
   * appending if both operations are required.
   * \param[in] desc set of descriptors for the image
   * \param[in] frame id of the query image
   * \returns vector of possibly matching frames found by the query
   */
  std::vector< vital::frame_id_t >
  query_and_append( const vital::descriptor_set_sptr desc,
                    vital::frame_id_t frame ) override;

  /// Get this algorithm's \link vital::config_block configuration block
  /// \endlink
  /**
   * This base virtual function implementation returns an empty configuration
   * block whose name is set to \c this->type_name.
   *
   * \returns \c config_block containing the configuration for this algorithm
   *          and any nested components.
   */
  vital::config_block_sptr get_configuration() const override;

  /// Set this algorithm's properties via a config block
  /**
   * \throws no_such_configuration_value_exception
   *    Thrown if an expected configuration value is not present.
   * \throws algorithm_configuration_exception
   *    Thrown when the algorithm is given an invalid \c config_block or is'
   *    otherwise unable to configure itself.
   *
   * \param config  The \c config_block instance containing the configuration
   *                parameters for this algorithm
   */
  void set_configuration( vital::config_block_sptr config ) override;

  /// Check that the algorithm's currently configuration is valid
  /**
   * This checks solely within the provided \c config_block and not against
   * the current state of the instance. This isn't static for inheritence
   * reasons.
   *
   * \param config  The config block to check configuration of.
   *
   * \returns true if the configuration check passed and false if it didn't.
   */
  bool check_configuration( vital::config_block_sptr config ) const override;

protected:
  /// the feature m_detector algorithm
  class priv;

  std::shared_ptr< priv > d_;
};

/// Shared pointer type for generic image_io definition type.
typedef std::shared_ptr<match_descriptor_sets> match_descriptor_sets_sptr;

} } } // end namespace

#endif // VITAL_DBOW2_MATCH_DESCRIPTOR_SETS_H_
