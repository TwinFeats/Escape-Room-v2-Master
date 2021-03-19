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

<br/><br/><img style="height: 50vh" src="http://twinfeats.com/tf/case1.jpg"/><br/>
<em>Starfield case mid-build</em>

<h2>Game Design</h2>
The case contains six 3d printed puzzle boxes, six 3d printed game panels, and eight 3d printed cards for the card reader.
Also included in the case are documents for each panel (in Russian!), a pen and paper and a Samsung tablet with Google Translate installed. The panels are:
<dl>
  <dt>Message Center</dt>
  <dl>This panel contains the game clock, speakers for audio messages, the 2x16 LCD screen for text messages, volumne and brightness
  controls, and a repeat last audio message button.</dl>
  
  <dt>Control Switch Panel</dt>
  <dl>Each panel other than the message center must be activated in order. The control switch panel itself is first used to "power up" the
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
  <dl><ul>
  <li>6 toggle switches</li>
  <li>LED light panel underneath the panel title.</li>
  </ul></dl>
  
  <dt>Satellite Modem</dt>
  <dl><ul>
  <li>6 x push buttons</li>
  <li>LED light panel underneath the panel title.</li>
  </ul></dl>

  <dt>Firewall</dt>
  <dl><ul>
  <li>6 x push buttons</li>
  <li>5 x APA-106 Neopixels</li>
  <li>LED light panel underneath the panel title.</li>
  </ul></dl>
  
  <dt>Nuclear Safeguard Panel</dt>
  <dl><ul>
  <li>TCS34725 color sensor</li>
  <li>TCRT5000 IR reflective sensor</li>
  <li>LED light panel underneath the panel title.</li>
  </ul></dl>
  
  <dt>Reactor Control Rods</dt>
  <dl><ul>
  <li>32 x APA-106 Neopixels</li>
  <li>1 x 64 Neopixel (8x8) panel</li>
  <li>2 Analog joysticks w/integrated push button on the stick</li>
  <li>1 push button</li>
  <li>LED light panel underneath the panel title.</li>
  </ul></dl>
</dl>

In addition, the lid has a magnetic reed switch to determine whether the case is open or closed, as well as
a Wyze Cam v2 to record/broadcast the players in the game. The camera turns on when the case is opened and
shuts down when the case is closed

<h2>Software Design</h2>
Since each panel has its own Arduino Nano to control its operations, each panel also has its own software project. Those projects are:

<ul>
  <li>Escape Room v2 Master</li>
  <li>Escape Case Switches</li>
  <li>Escape Case Modem</li>
  <li>Escape Escape Case Firewall</li>
  <li>Escape Room Control Room</li>
  <li>Escape Room Reactor</li>
</ul>

As you can tell, I'm a model of strict naming conventions. ;)

<p>The code is all written is C and not C++ for one simple reason - I'm a full stack Java developer and C++ (which I used to code in years ago) has a syntax
  just confusingly enough different than Java that I didn't want to confuse myself, so I stuck with straight C (which I also used to code years ago). The
  source code itself is heavily commented so I won't go further into details here, just look through the source for more information.

