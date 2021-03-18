*** WORK IN PROGRESS, NOT QUITE COMPLETE! ***
<h1>Escape Room in a case: Starfield</h1>
<h3>Kent L. Smotherman, copyright © 2021</h3>
Released under the Creative Commons Non-commerical use liciense, https://creativecommons.org/licenses/by-nc/3.0/legalcode
In summary, if you want to make this project for friends and family to play for free, go for it! If you want to use this project
as a base and modify it for friends and family to play for free, go for it! If you want to use this project or any modification
of it for a paid experience of any kind, you <strong>CANNOT</strong> go for it! I did this project as a labor of love for free, and want it to
remain that way.

<h2>The Story</h2>
Players are in the role of MI6 operatives who have been given the task of figuring out how to use the contents of the case to destroy
a Russian satellite nuclear weapon.

<h2>Game Design</h2>
The case contains six 3d printed panels:
<p><img src="http://twinfeats.com/tf/starfield/case1.jpg"/></p>
<dl>
  <dt>Message Center</dt>
  <dl>This panel contains the game clock, speakers for audio messages, the 2x16 LCD screen for text messages, volumne and brightness
  controls, and a repeat last audio message button.</dl>
  
  <dt>Control Switch Panel</dt>
  <dl>Each panel other than the message center must be activated in order. The control switch panel itself is first used to power up the
  system, and then the other primary game panels in turn. The switch positions for each panel are contained in the 3d printed puzzle boxes.</dl>
  
  <dt>Satellite Modem</dt>
  <dl>This is a tone-based puzzle with 6 buttons: 1 each for the 5 different tones and one to play the song that the player is trying to
  duplicate. There are 15 notes in the random song.</dl>

  <dt>Firewall</dt>
  <dl>Once the modem is connected, the firewall must be breeched. This is a "Mastermind"-style puzzle of 5 positions of 8 different colors.</dl>
  
  <dt>Nuclear Safeguard Panel</dt>
  <dl>After breeching the firewall, the player(s) must deactive the nuclear safeguard controller. This is game with 8 colored keycards that must
  be inserted into the card reader in the proper order.</dl>
  
  <dt>Reactor Control Rods</dt>
  <dl>This is a "Blackbox"-style game where the players send in gamma rays into a blackbox and observe where the rays exit. They must use this information
  to deduce where the control rods are in order to misalign them, which will engage the Starfield self-destruct.</dl>
</dl>

<h2>Hardware Design</h2>
Each panel contains an Arduino Nano as the controller, and:
<dl>
  <dt>Message Center</dt>
  <dl><ul>
    <li>TM1637 4-digit 7-segment LED display for the clock</li>
    <li>HD44780-based 1602 LCD panel</li>
    <li>3W speaker</li>
    <li>DFRobot DFPlayer mini</li>
    <li>10k potentiometer for audio volume</li>
    <li>10k potentiometer for LCD contrast</li>
    <li>10k potentiometer for the APA-106 LEDs (in other panels) brightness</li>
  </ul></dl>
  
  <dt>Control Switch Panel</dt>
  <dl>Each panel other than the message center must be activated in order. The control switch panel itself is first used to power up the
  system, and then the other primary game panels in turn. The switch positions for each panel are contained in the 3d printed puzzle boxes.</dl>
  
  <dt>Satellite Modem</dt>
  <dl>This is a tone-based puzzle with 6 buttons: 1 each for the 5 different tones and one to play the song that the player is trying to
  duplicate. There are 15 notes in the random song.</dl>

  <dt>Firewall</dt>
  <dl>Once the modem is connected, the firewall must be breeched. This is a "Mastermind"-style puzzle of 5 positions of 8 different colors.</dl>
  
  <dt>Nuclear Safeguard Panel</dt>
  <dl>After breeching the firewall, the player(s) must deactive the nuclear safeguard controller. This is game with 8 colored keycards that must
  be inserted into the card reader in the proper order.</dl>
  
  <dt>Reactor Control Rods</dt>
  <dl>This is a "Blackbox"-style game where the players send in gamma rays into a blackbox and observe where the rays exit. They must use this information
  to deduce where the control rods are in order to misalign them, which will engage the Starfield self-destruct.</dl>
</dl>


