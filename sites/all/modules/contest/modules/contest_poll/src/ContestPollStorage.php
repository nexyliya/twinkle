<?php

/**
 * @file
 * Contains the ContestPollStorage class.
 */

/**
 * Storage class for contest polls.
 */
class ContestPollStorage {

  /**
   * Delete any contests related to this poll.
   *
   * @param string $pid
   *   The poll's ID.
   *
   * @return Drupal\Core\Database\Query\DeleteQuery
   *   A new DeleteQuery object for this connection.
   */
  public static function deletePoll($pid) {
    return db_delete('contest_poll')->condition('pid', $pid)->execute();
  }

  /**
   * Retrieve the block body.
   *
   * @param int $pid
   *   The Poll's node ID.
   *
   * @return int
   *   The contest's node ID.
   */
  public static function getContestId($pid = 0) {
    return db_query("SELECT nid FROM {contest_poll} WHERE pid = :pid", array(':pid' => $pid))->fetchField();
  }

  /**
   * Merge the poll contest IDs.
   *
   * @param int $pid
   *   The poll's node ID.
   * @param int $nid
   *   The contest's node ID.
   *
   * @return object
   *   A Merge object.
   */
  public static function mergePoll($pid, $nid) {
    return db_merge('contest_poll')->key(array('pid' => $pid))->fields(array('nid' => $nid))->execute();
  }

}
