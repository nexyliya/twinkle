<?php

/**
 * @file
 * Support functions for installation and updates.
 */

/**
 * Build some content blocks using the block system for storage.
 *
 * @param string $info
 *   The label used on the backend.
 * @param string $body
 *   The content.
 * @param string $format
 *   The input format.
 *
 * @return int
 *   The block ID.
 */
function _contest_tnc_install_block_add($info, $body, $format = 'full_html') {
  $bid = db_query_range("SELECT bid FROM {block_custom} WHERE TRIM(LOWER(info)) = TRIM(LOWER(:info))", 0, 1, array(':info' => $info))->fetchField();
  $formats = filter_formats();

  if ($bid) {
    return $bid;
  }
  $fields = array(
    'body'   => $body,
    'info'   => $info,
    'format' => !empty($formats[$format]) ? $format : key($formats),
  );
  $bid = db_insert('block_custom')->fields($fields)->execute();

  _contest_install_feedback("Created Block: $bid - $info");

  return $bid;
}

/**
 * The contest's terms and conditions.
 *
 * @return string
 *   The contest's terms and conditions.
 */
function _contest_tnc_install_tnc() {
  return "
    <ol>
      <li>
        <strong>No purchase necessary to enter or win. A purchase or payment of any kind will not increase your chances of winning.</strong>
      </li>
      <li>
        <strong>How to Enter Via the Internet:</strong> Beginning [contest:date-start] through midnight [contest:date-end] ([contest:timezone]), visit [contest:host-link] and follow the on-screen instructions to be entered into the sweepstakes. You must already have access to the Internet, as it is not required that you obtain Internet access solely to participate in this Sweepstakes. The computer used to enter the Sweepstakes must accept cookies, or any successor or similar technology (now know or hereafter devised or discovered), which may be used by [contest:host-link] for the purposes of entry tracking. All entries must be made manually. Limit one entry per email address, per household per day. Entries submitted through the use of scripts, programs or other electro-mechanical processes (other than manual entry) are prohibited. No responsibility is assumed for any computer, telephone, cable, network, satellite, electronic or Internet hardware or software malfunctions, failures, connections, availability or garbled or jumbled transmissions, or service provider/Internet/Web site/use net accessibility or availability, traffic congestions, or unauthorized human intervention.
      </li>
      <li>
        <strong>How to Enter Via Mail:</strong> Look for an Official Entry Form in [contest:host-business]. To enter the sweepstakes, check the appropriate box on your Official Entry Form and hand print your name, address, zip code plus (optional) email address. Affix first-class postage and mail.
      </li>
      <li>
        <strong>Eligibility:</strong> This sweepstakes (&ldquo;Contest&rdquo;) is hosted by [contest:host-title], Inc. D.B.A. [contest:host-full-name] and prizes provided by  [contest:sponsor] (&ldquo;Sponsor&rdquo;) to [contest:country] residents only (excluding [contest:country] territories), who are at least 18 years old. Employees and directors of host or sponsor and all of its subsidiary and related companies and the immediate family (spouse, mother, father, sister, brother, daughter or son, regardless of where they live) or members of their same households (whether related or not) of such employees and directors are not eligible. By entering this sweepstakes, you agree to these Official Rules and the decisions of host, which shall be final and binding in all respects. No purchase is necessary to win, and purchasing any product will not improve your chances to win. Winner must accept all terms and conditions of prize to qualify. Sweepstakes open to general public.
      </li>
      <li>
        <strong>Promotional Period:</strong> The &ldquo;Promotional Period&rdquo; for this Sweepstakes begins on [contest:date-start] and ends on [contest:date-end] at midnight ([contest:timezone]).
      </li>
      <li>
        <strong>Drawing:</strong> Winner will be selected by random drawing from all eligible entries received during the Promotional Period. All non-winning entries will become the property of host and sponsor and may be used as seen fit for marketing publicity or sale.
      </li>
      <li>
        <strong>Prize/Odds:</strong> No warranties or representations of any kind are made about the prizes. host reserves the right to exchange any prize for any reason with another prize. No assignment or transfer of a prize is permitted prior to delivery of the prize to the winner. Winner is responsible for all federal, state and local taxes and shipping and handling charges. The prize(s), of which [contest:places] will be awarded, (described above).
      </li>
      <li>
        <strong>General Conditions:</strong> Void where prohibited. Winner agrees to release and hold harmless host, its affiliates, subsidiaries, advertising and promotion agencies and their respective directors, officers, employees, representatives and agents from any and all liability for any injuries, loss or damage of any kind to person, including death, and property, arising in whole or in part, directly or indirectly, from acceptance, possession, use or misuse of a prize, participation in any sweepstakes related activity, or participation in this sweepstakes. To accomplish this, winner must execute and return an Affidavit of Eligibility, Publicity Release and Release from Liability within 10 days of notification. Failure to timely return this affidavit, or if prize notification or prize is returned as non-deliverable, may result in disqualification with an alternate winner selected. Where permitted by law, winner (and in the instance when interim drawing has taken place, potential winner who has been selected in the interim drawing) agree to grant to host, and its licensees, affiliates and assigns, the right to print, publish, broadcast and use, worldwide in any media now known or hereafter developed, including but not limited to the world wide web, at any time or times, the winner&rsquo;s name, portrait, picture, voice, likeness and biographical information as news or information and for advertising and promotional purposes without additional consideration; and further without such additional compensation, appear for, or provide biographical information for use in any presentation or other activity which may include filming/audio/video/electronic or other recordings and/or interviews, as may be determined from time to time by host. Failure to make such appearances or grant such rights may result in disqualification with an alternate winner or potential winner selected; and while not obligated to do so, host may in its sole discretion, bear such reasonable costs and expenses which host, in its sole discretion, deems appropriate for winners or potential winners to appear for a presentation or other activity. host reserved the right, at its sole discretions, to disqualify any individual it finds, in its sole discretion, to be tampering with the entry process or the operation of the Sweepstakes or Web site; to be in violation of the Terms of Service of the Web site, to be acting in violation of these Sweepstakes Rules; or to be acting in a non-sportsmanlike or disruptive manner, or with intent to annoy, abuse, threaten or harass any other person. Any use of robotic, automatic, macros, programmed or like entry method will void all such entries by such method. In the event of a dispute as to entries submitted by multiple users having the same email account, the authorized subscriber of the account used to enter the Sweepstakes at the actual time of entry will be deemed to be the participant and must comply with these rules. Authorized account subscriber is deemed to be the natural person who is assigned an email address by an Internet access provider, online service provider or other organization, which is responsible for assigning email addresses or the domain associated with the submitted email address. host will prosecute any fraudulent activities to the full extent of the law.
      </li>
      <li>
        <strong>Limitations of Liability:</strong> host is not responsible for any incorrect or inaccurate information, whether caused by Web site users, or tampering or hacking, or by any of the equipment or programming associated with or utilized in the Sweepstakes and assumes no responsibility for any error, omission, interruption, deletion, defect, delay in operation or transmission, communications line failures, theft or destruction or unauthorized access to the Web site. host is not responsible for injury or damage to participants&rsquo; or to any other person&rsquo;s computer related to or resulting from participating in this Sweepstakes or downloading material from or use of the Web site. If, for any reason, the Sweepstakes is not capable of running as planned by reason of infection by computer virus, worms, bugs, tampering, unauthorized interventions, fraud, technical failures, or any other causes which, in the sole opinion of host, could corrupt or affect the administrations, security, fairness, integrity or proper conduct of this Sweepstakes, host reserved the right at its sole discretion to cancel, terminate, modify or suspend the Internet portion of this sweepstakes for any drawing(s) and select the winner from Internet entries received for that drawing prior to the action taken. IN NO VENT WILL HOST, ITS LICENSEES, AND OR AFFILIATES, SUBSIDIARIES AND RELATED COMPANIES, THEIR ADVERTISING, LEGAL OR PROMOTION AGENCIES OR THEIR RESPECTIVE OFFICERS, DIRECTORS, EMPLOYEES, REPRESENTATIVE AND AGENTS, BE RESPONSIBLE OR LIABLE FOR ANY DAMAGES OR LOSSES OF ANY KIND, INCLUDING DIRECT, INDIRECT, INCIDENTAL, CONSEQUENTIAL OR PUNITIVE DAMAGES ARISING OUT OF YOUR ACCESS TO AND USE OF INTERNET SITE [contest:host-link] OR THE DOWNLOADING FROM AND/OR PRINTING MATERIAL DOWNLOADED FROM SAID SITE. WITHOUT LIMITING THE FOREGOING, EVERYTHING ON THIS SITE IS PROVIDED &ldquo;AS IS&rdquo; WITHOUT WARRANTY OF ANY KIND. EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT. SOME JURISDICTIONS MAY NOT ALLOW THE LIMITATIONS OR EXCLUSION OF LIABILITY FOR INCIDENTAL OR CONSEQUENTIAL DAMAGES OR EXCLUSION OF IMPLIED WARRANTIES SO SOME OF THE ABOVE LIMITATIONS OR EXCLUSIONS MAY NOT APLY TO YOU. CHECKK YOUR LOCAL LAWS FOR ANY RESTRICTIONS OR LIMITATION REGARDING THESE LIMITATIONS OR EXCLUSIONS.
      </li>
      <li>
        <strong>Disputes:</strong> As a condition of participating in this Sweepstakes, participant agrees that any and all disputes which cannot be resolved between the parties, and causes of action arising out of or connected with this Sweepstakes, shall be resolved individually, without resort to any form of class action, exclusively, before a court located in [contest:country], [contest:host-state] having competent jurisdictions, which Court shall apply the laws of the State of [contest:host-state] without regard for the doctrines of Conflict of Law. Further, in any such dispute, under no circumstances will participant be permitted to obtain awards for, and hereby waives all rights to claim punitive, incidental or consequential damages, or any other damages, including attorney&rsquo;s fees, other than participant&rsquo;s actual out-of-pocket expense (i.e. costs associated with entering this Promotion), and participant further waives all rights to have damages multiplied or increased.
      </li>
      <li>
        <strong>Winner Notification:</strong> Winner will be notified via phone, e-mail or postal mail, at host&rsquo;s discretion, on or about [contest:date-notify]. For the winner&rsquo;s name, send a stamped, self-addressed envelope to: [contest:host-full-name] Attn: Sweepstakes Winners List Please, [contest:host-address], [contest:host-city], [contest:host-state] [contest:host-zip]
      </li>
      <li>
        <strong>COPPA Policy:</strong> In accordance with the Children&rsquo;s Online Protection Policy, we cannot accept registration from anyone under the age of eighteen. COPPA provides protection for children while online and prohibits Web sites from accepting any identifiable data or information from anyone thirteen and under.
      </li>
    </ol>
  ";
}
