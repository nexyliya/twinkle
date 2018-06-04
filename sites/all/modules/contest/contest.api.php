<?php

/**
 * @file
 * Sample hooks demonstrating usage in Contest.
 */

/**
 * Define callbacks that can be used fine tune contest access.
 *
 * @param bool $access
 *   A boolean to determine if this user can enter a contest.
 */
function hook_contest_entry_access_alter(&$access) {
  global $user;

  $access = ($user->uid >= 0) ? TRUE : FALSE;
}
