<?php

/**
 * @file
 * Template for a contest poll.
 */
?>
<div class="contest-poll poll">
  <div class="vote-form">
    <div class="choices">
      <?php print drupal_render($variables['form']['choice']); ?>
    </div>
    <?php print drupal_render($variables['form']['fieldset']); ?>
    <?php print drupal_render($variables['form']['contest_optin']); ?>
    <?php print drupal_render($variables['form']['vote']); ?>
    <?php print !empty($variables['form']['contest_tnc']) ? drupal_render($variables['form']['contest_tnc']) : ''; ?>
  </div>
  <?php print drupal_render_children($variables['form']); ?>
</div>
