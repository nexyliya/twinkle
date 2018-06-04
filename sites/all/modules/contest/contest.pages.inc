<?php

/**
 * @file
 * User page callbacks for the token module.
 */

/**
 * Display the administration information about a contest.
 *
 * @param object $node
 *   The contest node.
 *
 * @return string
 *   Fully themed contest admin page.
 */
function contest_admin_page($node = NULL) {
  drupal_add_css(drupal_get_path('module', 'contest') . '/css/contest.css');

  return theme('contest_admin_page', ContestData::adminPage($node));
}

/**
 * Clear an individual winner of a contest.
 *
 * @param int $nid
 *   The node ID.
 * @param int $uid
 *   The user's ID.
 */
function contest_clear_winners($nid, $uid = 0) {
  if (ContestStorage::getPublished($nid)) {
    drupal_set_message(t('You must unpublish contest winners before clearing a winner.'), 'warning');
  }
  elseif ($uid) {
    ContestStorage::clearWinner($nid, $uid);
  }
  else {
    ContestStorage::clearWinners($nid);
  }
  drupal_goto("node/$nid/contest-admin");
}

/**
 * Export a complete list of all contest entrants.
 */
function contest_export_all() {
  $csv = ContestHelper::csvHeader();
  $file = 'contest_users_' . date('Ymdhms', REQUEST_TIME) . '.csv';

  foreach (ContestStorage::getAllEntries() as $uid) {
    $usr = new ContestUser($uid);

    if ($usr->uid) {
      $csv .= ContestHelper::toCsv($usr);
    }
  }
  ContestHelper::downloadFile($file, $csv);
}

/**
 * Export a complete list of contest entries.
 *
 * @param object $node
 *   The contest node.
 */
function contest_export_entries($node) {
  $csv = ContestHelper::csvHeader();
  $file = "contest_entries_{$node->nid}_" . date('Ymdhms', REQUEST_TIME) . '.csv';

  foreach (ContestStorage::getEntries($node->nid) as $uid) {
    $usr = new ContestUser($uid);

    if ($usr->uid) {
      $csv .= ContestHelper::toCsv($usr);
    }
  }
  ContestHelper::downloadFile($file, $csv);
}

/**
 * Export a list of unique entrants.
 *
 * @param object $node
 *   The contest node.
 */
function contest_export_unique($node) {
  $csv = ContestHelper::csvHeader();
  $file = "contest_unique_{$node->nid}_" . date('Ymdhms', REQUEST_TIME) . '.csv';

  foreach (ContestStorage::getUniqueEntries($node->nid) as $uid) {
    $usr = new ContestUser($uid);

    if ($usr->uid) {
      $csv .= ContestHelper::toCsv($usr);
    }
  }
  ContestHelper::downloadFile($file, $csv);
}

/**
 * Callback for the contest list page.
 *
 * @return string
 *   Fully rendered HTML for the contest list page.
 */
function contest_list_page() {
  return theme('contest_list_page', array('data' => ContestData::contestList('teaser')));
}

/**
 * Pick a random winner of a contest.
 *
 * @param int $nid
 *   The node ID.
 * @param int $uid
 *   The user's ID.
 */
function contest_pick_winner($nid, $uid = FALSE) {
  $dq_days = variable_get('contest_dq_days', 90);
  $wids = array(0);

  // We don't allow contest results to be edited while they are published.
  $row = ContestStorage::winnerPreSelect($nid);

  $dq = $row->end - ($dq_days * ContestStorage::DAY);

  if ($row->publish_winners) {
    drupal_set_message(t('You must unpublish a contest before editing results.'), 'warning');
    drupal_goto("node/$nid/contest-admin");
  }
  // We don't allow the winner to be picked when the contest is still open.
  if (REQUEST_TIME < $row->end) {
    drupal_set_message(t("You can't select a contest winner before the contest ends."), 'warning');
    drupal_goto("node/$nid/contest-admin");
  }
  // Check to see if there are any more winners allowed..
  $winners = ContestStorage::getWinnerCount($nid);

  if ($winners >= $row->places) {
    drupal_set_message(format_plural($row->places, 'The contest allows one winner.', 'The contest allows @count places.'), 'warning');
    drupal_set_message(format_plural($row->places, 'Currently there is one.', 'Currently there are @count.'), 'warning');
    drupal_goto("node/$nid/contest-admin");
  }
  // Build a comma separated string of the disqualified users ID's.
  $wids = ContestStorage::getDqs($dq);

  foreach ($wids as $wid) {
    if ($uid && $uid == $wid) {
      drupal_set_message(format_plural($dq_days, 'This person has already won something in the last day.', 'This person has already won something in the last @count days.'), 'warning');
      drupal_goto("node/$nid/contest-admin");
    }
  }
  // Select the winner randomly from the qualifying entrants.
  if ($uid) {
    $row = ContestStorage::getRandomUserEntry($nid, $uid);
  }
  else {
    $row = ContestStorage::getRandomEntry($nid, $wids);
  }
  // If we have a winner get the next place and update the winning entry.
  if ($row->uid) {
    $place = ContestStorage::getMaxPlace($nid) + 1;
    ContestStorage::setWinner($nid, $row->uid, $place, $row->created);
  }
  // Redirect back to the contest entries list page.
  drupal_goto("node/$nid/contest-admin");
}

/**
 * Publish the winners of a contest.
 *
 * @param object $node
 *   The contest node object.
 */
function contest_publish_winners($node) {
  $winners = ContestStorage::getWinnerCount($node->nid);

  if ($winners != $node->contest->places) {
    drupal_set_message(t("You can't publish winners before they've all been selected."), 'warning');
    drupal_goto("node/$node->nid/contest-admin");
  }
  ContestStorage::publishWinners($node->nid);

  drupal_goto("node/$node->nid/contest-admin");
}

/**
 * Unpublish the winners of a contest.
 *
 * @param object $node
 *   The contest node object.
 */
function contest_unpublish_winners($node) {
  ContestStorage::unpublishWinners($node->nid);
  drupal_goto("node/$node->nid/contest-admin");
}

/**
 * Callback for username auto-complete fields.
 *
 * @return json
 *   A username to username hash.
 */
function contest_usr_autocomplete($name = '') {
  return drupal_json_output(ContestStorage::autocomplete(db_like($name) . '%'));
}
