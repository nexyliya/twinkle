<?php

/**
 * @file
 * Declares the available audio players for the Audio Field module.
 */

/**
 * Implements hook_audiofield_players().
 */
function audiofield_players() {
  $players = module_invoke_all('audiofield_players');

  $players['html5'] = array(
    'name' => 'HTML5 <audio>',
    'filetypes' => array('mp3', 'wav', 'ogg', 'opus', 'webm'),
    'local' => TRUE,
    'callback' => 'audiofield_html5_audio',
  );

  $players['wpaudioplayer'] = array(
    'path' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/audio-player/player.swf",
    'name' => 'WordPress Audio Player',
    'download_link' => 'http://wpaudioplayer.com/download',
    'filetypes' => array('mp3'),
    'callback' => 'audiofield_wpaudioplayer',
  );

  $players['xspf_slim'] = array(
    'path' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/xspf_player_slim.swf",
    'name' => 'XSPF Slim Player',
    'download_link' => 'http://prdownloads.sourceforge.net/musicplayer/xspf_player_slim-correct-0.2.3.zip?download',
    'filetypes' => array('mp3'),
    'callback' => 'audiofield_xspf_slim',
  );

  $players['xspf_button'] = array(
    'path' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/button/musicplayer.swf",
    'name' => 'XSPF Button Player',
    'download_link' => 'http://prdownloads.sourceforge.net/musicplayer/button_player-0.1.zip?download',
    'filetypes' => array('mp3'),
    'callback' => 'audiofield_xspf_button',
  );

  // #1414398: Premium Beat players no longer available.
  $players['premium_beat_single_track'] = array(
    'path' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/playerSinglePackage/playerSingle.swf",
    'name' => 'Premium Beat Single Player',
    'filetypes' => array('mp3'),
    'callback' => 'audiofield_premium_beat_single_track',
  );

  // #1414398: Premium Beat players no longer available.
  $players['premium_beat_thin'] = array(
    'path' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/OriginalThinMusicPlayer.swf",
    'name' => 'Premium Beat Thin Player',
    'filetypes' => array('mp3'),
    'callback' => 'audiofield_premium_beat_thin',
  );

  // #1414398: Premium Beat players no longer available.
  $players['premium_beat_mini'] = array(
    'path' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/LWMusicPlayer.swf",
    'name' => 'Premium Beat Mini Player',
    'filetypes' => array('mp3'),
    'callback' => 'audiofield_premium_beat_mini',
  );

  $players['flowplayer'] = array(
    'name' => 'Flowplayer',
    'download_link' => 'http://drupal.org/project/flowplayer',
    'filetypes' => array('mp3'),
    'callback' => 'audiofield_flowplayer',
    'module' => 'flowplayer',
  );

  $players['soundmanager2'] = array(
    'path' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/soundmanager2",
    'name' => 'SoundManager2 360',
    'download_link' => 'http://www.schillmania.com/projects/soundmanager2/',
    'filetypes' => array('mp3', 'wav'),
    'callback' => 'audiofield_soundmanager2',
  );

  $players['jplayer'] = array(
    'path' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/jplayer/jquery.jplayer.min.js",
    'css' => variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/jplayer/jplayer.css",
    'name' => 'jPlayer',
    'download_link' => 'https://www.drupal.org/project/jplayer',
    'module' => 'jplayer',
    'filetypes' => array('mp3'),
    'callback' => 'audiofield_jplayer',
  );

  return $players;
}

/**
 * Defining AudioField theme callbacks.
 */
function _audiofield_theme() {
  return array(
    // Themes for the players.
    'audiofield_html5_audio' => array(
      'variables' => array('audio_file' => NULL, 'download_access' => FALSE),
    ),
    'audiofield_players_wpaudioplayer' => array(
      'variables' => array('audio_file' => NULL),
    ),
    'audiofield_players_xspf_slim' => array(
      'variables' => array('player_path' => NULL, 'audio_file' => NULL),
    ),
    'audiofield_players_xspf_button' => array(
      'variables' => array('player_path' => NULL, 'audio_file' => NULL),
    ),
    'audiofield_players_premium_beat_single_track' => array(
      'variables' => array('player_path' => NULL, 'audio_file' => NULL),
    ),
    'audiofield_players_premium_beat_thin' => array(
      'variables' => array('player_path' => NULL, 'audio_file' => NULL),
    ),
    'audiofield_players_premium_beat_mini' => array(
      'variables' => array('player_path' => NULL, 'audio_file' => NULL),
    ),
  );
}

/**
 * Callback function for AudioField HTML5 player.
 */
function audiofield_html5_audio($player_path, $audio_path) {
  return theme('audiofield_html5_audio', array(
    'audio_file' => file_create_url($audio_path),
    'download_access' => user_access('download all audio files'),
  ));
}

/**
 * Theme function for AudioField HTML5 player.
 */
function theme_audiofield_html5_audio($variables) {
  return array(
    '#markup' => '<audio src="' . $variables['audio_file'] . '" controls' . ($variables['download_access'] ? '' : ' controlsList="nodownload"') . '></audio>',
  );
}

/**
 * Callback function for AudioField WordPress standalone player.
 */
function audiofield_wpaudioplayer($player_path, $audio_file) {
  // Add the javascript which renders the player.
  drupal_add_js(variable_get('audiofield_players_dir', 'sites/all/libraries/player') . '/audio-player/audio-player.js', array(
    'type' => 'file',
    'scope' => 'footer',
    'group' => JS_LIBRARY,
    'weight' => 0,
  ));
  drupal_add_js(drupal_get_path('module', 'audiofield') . '/js/wordpress.builder.js', array(
    'type' => 'file',
    'scope' => 'footer',
    'group' => JS_LIBRARY,
    'weight' => 10,
  ));
  return theme('audiofield_players_wpaudioplayer', array(
    'audio_file' => file_create_url($audio_file),
  ));
}

/**
 * Theme function for AudioField Wordpress standalone player.
 */
function theme_audiofield_players_wpaudioplayer($variables) {
  return array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => drupal_html_id('audiofield-wordpress-player'),
      'class' => array(
        'audiofield-wordpress-player',
      ),
      'data-src' => $variables['audio_file'],
    ),
    '#children' => '',
  );
}

/**
 * Callback function for AudioField XSPF Slim player.
 */
function audiofield_xspf_slim($player_path, $audio_file, $options) {
  $song_title = t('XSPF Slim Music Player');
  if (!empty($options)) {
    if (!empty($options['item']['description'])) {
      $song_title = $options['item']['description'];
    }
    elseif (!empty($options['item']['filename'])) {
      $song_title = $options['item']['filename'];
    }
  }
  return theme('audiofield_players_xspf_slim', array(
    'player_path' => $player_path,
    'audio_file' => file_create_url($audio_file),
    'song_title' => $song_title,
  ));
}

/**
 * Theme function for AudioField XSPF Slim player.
 */
function theme_audiofield_players_xspf_slim($variables) {
  return array(
    '#markup' => '<object type="application/x-shockwave-flash" width="175" height="14"
                  data="' . $variables['player_path'] . '?song_url=' . $variables['audio_file'] . '&song_title=' . $variables['song_title'] . '">
                  <param name="movie" value="' . $variables['player_path'] . '?song_url=' . $variables['audio_file'] . '" />
                  <param name="wmode" value="transparent" />
                  </object>',
  );
}

/**
 * Callback function for AudioField XSPF Button player.
 */
function audiofield_xspf_button($player_path, $audio_file) {
  return theme('audiofield_players_xspf_button', array(
    'player_path' => $player_path,
    'audio_file' => file_create_url($audio_file),
  ));
}

/**
 * Theme function for AudioField XSPF Button player.
 */
function theme_audiofield_players_xspf_button($variables) {
  return array(
    '#markup' => '<object type="application/x-shockwave-flash" width="17" height="17"
          data="' . $variables['player_path'] . '?song_url=' . $variables['audio_file'] . '">
          <param name="movie" value="' . $variables['player_path'] . '?song_url=' . $variables['audio_file'] . '" />
          <param name="wmode" value="transparent" />
          </object>',
  );
}

/**
 * Callback function for AudioField Premium Beat single-track player.
 */
function audiofield_premium_beat_single_track($player_path, $audio_file) {
  return theme('audiofield_players_premium_beat_single_track', array(
    'player_path' => $player_path,
    'audio_file' => $audio_file,
  ));
}

/**
 * Theme function for AudioField Premium Beat single-track player.
 */
function theme_audiofield_players_premium_beat_single_track($variables) {
  return array(
    '#markup' => '<object><param name="autoplay" value="true" />
                <param name="controller"value="true" />
                <embed src="' . $variables['player_path'] . '"  width="192" height="80" float="left" wmode="transparent" flashvars="mediaPath=' . $variables['audio_file'] . '"
            autostart="true" loop="false"  controller="true" bgcolor="#FF9900" pluginspage="http://www.macromedia.com/go/getflashplayer" >
                </embed></object>',
  );
}

/**
 * Callback function for AudioField Premium Beat thin player.
 */
function audiofield_premium_beat_thin($player_path, $audio_file) {
  return theme('audiofield_players_premium_beat_thin', array(
    'player_path' => $player_path,
    'audio_file' => $audio_file,
  ));
}

/**
 * Theme function for AudioField Premium Beat thin player.
 */
function theme_audiofield_players_premium_beat_thin($variables) {
  return array(
    '#markup' => '<object><param name="autoplay" value="true" />
                  <param name="controller"value="true" />
                  <embed src="' . $variables['player_path'] . '"  width="220" height="21" float="left" wmode="transparent" flashvars="mediaPath=' . $variables['audio_file'] . '&defaultVolume=100" autostart="true" loop="false"  controller="true" bgcolor="#FF9900" pluginspage="http://www.macromedia.com/go/getflashplayer" >
                  </embed></object>',
  );
}

/**
 * Callback function for AudioField Premium Beat mini player.
 */
function audiofield_premium_beat_mini($player_path, $audio_file) {
  return theme('audiofield_players_premium_beat_mini', array(
    'player_path' => $player_path,
    'audio_file' => $audio_file,
  ));
}

/**
 * Theme function for AudioField Premium Beat mini player.
 */
function theme_audiofield_players_premium_beat_mini($variables) {
  return array(
    '#markup' => '<object><param name="autoplay" value="true" />
                  <param name="controller"value="true" />
                  <embed src="' . $variables['player_path'] . '"  width="65" height="21" float="left" wmode="transparent" flashvars="mediaPath=' . $variables['audio_file'] . '&defaultVolume=100" autostart="true" loop="false"  controller="true" bgcolor="#FF9900" pluginspage="http://www.macromedia.com/go/getflashplayer" >
                  </embed></object>',
  );
}

/**
 * Callback function for AudioField SoundManager2 player.
 */
function audiofield_soundmanager2($player_path, $audio_file) {
  static $instanceIndex = 0;
  if ($audio_file == '') {
    return t('HTML5 media player for soundmanager2 (skin 360-player)');
  }
  else {
    $instanceIndex += 1;
    $player_base_path = variable_get('audiofield_players_dir', 'sites/all/libraries/player') . "/soundmanager2";
    $ie_js = array(
      '#type' => 'markup',
      '#markup' => '<!--[if IE]><script type="text/javascript" src="' . $player_path . '/demo/360-player/script/excanvas.js"></script><![endif]-->',
    );
    drupal_add_html_head($ie_js, 'audiofield');

    drupal_add_js($player_base_path . '/script/soundmanager2.js', array('weight' => 0));

    // 360 viewer.
    drupal_add_js($player_base_path . '/demo/360-player/script/berniecode-animator.js', array('weight' => 1));
    drupal_add_js($player_base_path . '/demo/360-player/script/360player.js', array('weight' => 1));
    drupal_add_css($player_base_path . '/demo/360-player/360player.css');
    drupal_add_js('
      soundManager.url = "' . $player_path . '/swf";
      soundManager.useFastPolling = true;
      soundManager.waitForWindowLoad = true;
      soundManager.preferFlash = true;

      soundManager.onready(function () {
        soundManager.stopAll();
        threeSixtyPlayer.init();
      });
    ', array(
      'type' => 'inline',
      'scope' => 'footer',
      'weight' => 10,
    ));

    return array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => drupal_html_id('sm2-container-real'),
        'class' => array(
          'ui360',
        ),
      ),
      '#children' => '<a href="' . file_create_url($audio_file) . '"></a>',
    );
  }
}

/**
 * Callback function for AudioField Flowplayer player.
 */
function audiofield_flowplayer($player_path, $audio_file) {
  static $seq = 1;
  return array(
    '#markup' => theme('flowplayer', array(
      'config' => array(
        'clip' => array(
          'url' => $audio_file,
          'autoPlay' => FALSE,
        ),
      ),
      'id' => 'audiofield-' . $seq++,
      'attributes' => array('style' => 'height: 24px'),
    )),
  );
}

/**
 * Callback function for AudioField jPlayer player.
 */
function audiofield_jplayer($player_path, $audio_file, $options) {
  return array(
    '#markup' => theme(
      'jplayer',
      array(
        'entity_type' => $options['entity_type'],
        'entity' => $options['entity'],
        'field_name' => $options['instance']['field_name'],
        'items' => array($options['item']),
        'settings' => array(
          'autoplay' => 0,
          'solution' => 'html, flash',
          'preload' => 'metadata',
          'volume' => 80,
          'muted' => FALSE,
          'repeat' => 'none',
          'backgroundColor' => '000000',
          'mode' => 'playlist',
          'continuous' => FALSE,
        ),
      )
    ),
  );
}
