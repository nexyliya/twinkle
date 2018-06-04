<?php

/**
 * @file
 * Contains the ContestData class.
 */

/**
 * Build data structures to be rendered.
 */
class ContestData {

  /**
   * Build the data for a contest's admin page.
   *
   * @param object $node
   *   The contest node object.
   *
   * @return array
   *   A one element array of objects.
   */
  public static function adminPage($node = NULL) {
    $node->contest->host = new ContestUser(variable_get('contest_tnc_host_uid', 1));
    $node->contest->host->title = check_plain(variable_get('contest_tnc_host_title', ''));

    $sponsor = user_load_by_mail($node->contest->sponsor_email);
    $node->contest->sponsor = new ContestUser($sponsor->uid);
    $node->contest->sponsor->url = $node->contest->sponsor_url;

    $node->contest->contestants = array();
    $node->contest->entrants = 0;
    $node->contest->entries = 0;
    $node->contest->winners = array_flip(ContestHelper::getWinners($node->nid));

    foreach (ContestStorage::getContestants($node->nid) as $row) {
      if (!$row->mail) {
        continue;
      }
      $node->contest->contestants[$row->uid] = $row;
      $node->contest->entrants++;
      $node->contest->entries += $row->qty;

      if (!empty($node->contest->winners[$row->uid])) {
        $row->winner = $node->contest->winners[$row->uid];
      }
    }
    $data = (object) array(
      'node'        => $node,
      'contest'     => $node->contest,
      'contestants' => $node->contest->contestants,
      'host'        => $node->contest->host,
      'sponsor'     => $node->contest->sponsor,
      'winners'     => $node->contest->winners,
    );
    return array('data' => $data);
  }

  /**
   * Build the data needed to display the block.
   *
   * @param string $view_mode
   *   The view mode.
   *
   * @return array
   *   An array of node objects prepared for render().
   */
  public static function contestList($view_mode = 'teaser') {
    $data = array();
    $limit = 10;

    list($cid, $cache) = ContestCache::get(array(__CLASS__, __FUNCTION__));

    if ($cache !== FALSE) {
      return $cache;
    }
    $nids = ContestStorage::getNids(0, $limit);

    foreach ($nids as $nid) {
      $node = node_load($nid);
      $data[] = node_view($node, $view_mode);
    }
    return ContestCache::set($cid, array('data' => $data));
  }

  /**
   * Build the data needed to display the results.
   *
   * @param object $node
   *   A node object.
   *
   * @return array
   *   A one elemet array of data to build the results.
   */
  public static function results($node = NULL) {
    $states = ContestStorage::getStates(variable_get('site_default_country', ''));
    $winners = array();

    list($cid, $cache) = ContestCache::get(array(__CLASS__, __FUNCTION__, $node->nid));

    if ($cache !== FALSE) {
      return $cache;
    }
    foreach (ContestHelper::getWinners($node->nid) as $place => $uid) {
      $winners[$place] = new ContestUser($uid);

      if (!empty($states[$winners[$place]->state])) {
        $winners[$place]->state = $states[$winners[$place]->state];
      }
    }
    $data = (object) array(
      'node'    => $node,
      'winners' => $winners,
    );
    return ContestCache::set($cid, array('data' => $data));
  }

}
