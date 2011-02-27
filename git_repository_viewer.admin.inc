<?php
/**
 * @file
 *   Provide the administration interface for the git_repository_viewer.
 */

/**
 * The form definition for the administrative settings form.
 */
function git_repository_viewer_settings_form($form) {
  $form = array();
  return system_settings_form($form);
}
