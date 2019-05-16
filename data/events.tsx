<?xml version="1.0" encoding="UTF-8"?>
<tileset name="events_symbol" tilewidth="60" tileheight="60" tilecount="100" columns="10">
 <image source="tilesets/symbol_numbers.png" width="600" height="600"/>
 <tile id="0" type="AeMoveSustained">
  <properties>
   <property name="DIRECTION" value="UP"/>
   <property name="NAME" value="MoveUp"/>
   <property name="PRIORITY" value="LOW"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="1" type="AeMoveSustained">
  <properties>
   <property name="DIRECTION" value="DOWN"/>
   <property name="NAME" value="MoveDown"/>
   <property name="PRIORITY" value="LOW"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="2" type="AeMoveSustained">
  <properties>
   <property name="DIRECTION" value="RIGHT"/>
   <property name="NAME" value="MoveRight"/>
   <property name="PRIORITY" value="LOW"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="3" type="AeMoveSustained">
  <properties>
   <property name="DIRECTION" value="LEFT"/>
   <property name="NAME" value="MoveLeft"/>
   <property name="PRIORITY" value="LOW"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="4" type="AeMoveSustained">
  <properties>
   <property name="DIRECTION" value="CURRENT"/>
   <property name="NAME" value="MoveLast"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="5" type="AeMoveDirection">
  <properties>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="DIRECTION" value="DOWN"/>
   <property name="DURATION" value="150"/>
   <property name="NAME" value="WalkDown"/>
   <property name="PRIORITY" value="HIGH"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="6" type="AeFall">
  <properties>
   <property name="ACCELERATION" type="float" value="1200"/>
   <property name="ANIMATION_TYPE" value="IDLE"/>
   <property name="NAME" value="Fall"/>
  </properties>
 </tile>
 <tile id="7" type="AeAnimate">
  <properties>
   <property name="DIRECTION" value="CURRENT"/>
   <property name="GAME_FRAMES" type="int" value="1"/>
   <property name="NAME" value="IdleLast"/>
  </properties>
 </tile>
 <tile id="8" type="AeJump">
  <properties>
   <property name="JUMP_DURATION" type="float" value="1.6"/>
   <property name="JUMP_HEIGHT" type="float" value="1000"/>
   <property name="NAME" value="Jump1"/>
   <property name="SLOW_FACTOR" type="float" value="0.05"/>
  </properties>
 </tile>
 <tile id="9" type="AeAnimate">
  <properties>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="CYCLES" type="int" value="5"/>
   <property name="DIRECTION" value="RIGHT"/>
   <property name="NAME" value="Test"/>
  </properties>
 </tile>
 <tile id="10" type="AeMoveDirection">
  <properties>
   <property name="DIRECTION" value="RIGHT"/>
   <property name="NAME" value="WRight"/>
  </properties>
 </tile>
 <tile id="11" type="AeSetVar">
  <properties>
   <property name="NAME" value="Zero_Y"/>
   <property name="VALUE" type="float" value="0"/>
   <property name="VAL_NAME" value="__YSPEED"/>
  </properties>
 </tile>
 <tile id="12" type="AeSound">
  <properties>
   <property name="NAME" value="Jump_Sound"/>
   <property name="PATH" value="Ding2.ogg"/>
  </properties>
 </tile>
 <tile id="13" type="AteOnGround">
  <properties>
   <property name="NAME" value="JumpOnce"/>
   <property name="SUCCESS" value="Jump1"/>
  </properties>
 </tile>
 <tile id="14" type="AeAccelerate">
  <properties>
   <property name="NAME" value="AccLeft"/>
   <property name="XFACTOR" type="float" value="-40"/>
   <property name="XMAX_SPEED" type="float" value="-500"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="15" type="AeAccelerate">
  <properties>
   <property name="NAME" value="AccRight"/>
   <property name="XFACTOR" type="float" value="40"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="16" type="AeMove">
  <properties>
   <property name="NAME" value="Move"/>
  </properties>
 </tile>
 <tile id="17" type="AeDecelerate">
  <properties>
   <property name="NAME" value="Decelerate"/>
   <property name="XFACTOR" type="float" value="30"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="18" type="AeAnimate">
  <properties>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="LEFT"/>
   <property name="GAME_FRAMES" type="int" value="1"/>
   <property name="NAME" value="AnimLeft"/>
  </properties>
 </tile>
 <tile id="19" type="AeAnimate">
  <properties>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="RIGHT"/>
   <property name="GAME_FRAMES" type="int" value="1"/>
   <property name="NAME" value="AnimRight"/>
  </properties>
 </tile>
 <tile id="20" type="AeAccelerate">
  <properties>
   <property name="NAME" value="Jump_Dyn"/>
   <property name="XFACTOR" type="float" value="0"/>
   <property name="YFACTOR" type="float" value="-1250"/>
   <property name="YMAX_SPEED" type="float" value="-1250"/>
  </properties>
 </tile>
 <tile id="21" type="AeAccelerate">
  <properties>
   <property name="NAME" value="Fall_Dyn"/>
   <property name="XFACTOR" type="float" value="0"/>
   <property name="YFACTOR" type="float" value="50"/>
   <property name="YMAX_SPEED" type="float" value="10000"/>
  </properties>
 </tile>
 <tile id="22" type="AeMulti">
  <properties>
   <property name="1" value="Zero_Y"/>
   <property name="2" value="Jump_Dyn"/>
   <property name="3" value="Jump_Sound"/>
   <property name="NAME" value="Jump_Dyn2"/>
  </properties>
 </tile>
 <tile id="23" type="AteOnGround">
  <properties>
   <property name="NAME" value="Jump_Grnd"/>
   <property name="SUCCESS" value="Jump_Dyn2"/>
   <property name="TOLERANCE" type="int" value="15"/>
  </properties>
 </tile>
 <tile id="24" type="AeAccelerate">
  <properties>
   <property name="NAME" value="AccLeftAir"/>
   <property name="XFACTOR" type="float" value="-15"/>
   <property name="XMAX_SPEED" type="float" value="-500"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="25" type="AeAccelerate">
  <properties>
   <property name="NAME" value="AccRightAir"/>
   <property name="XFACTOR" type="float" value="15"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="27" type="AeDecelerate">
  <properties>
   <property name="NAME" value="DecelerateAir"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="79" type="AteOnGround">
  <properties>
   <property name="FAILURE" value="DecelerateAir"/>
   <property name="NAME" value="DecelerateDyn"/>
   <property name="SUCCESS" value="Decelerate"/>
  </properties>
 </tile>
 <tile id="80" type="AeMulti">
  <properties>
   <property name="1" value="DecelerateDyn"/>
   <property name="2" value="Fall_Dyn"/>
   <property name="3" value="Move"/>
   <property name="NAME" value="Physics"/>
  </properties>
 </tile>
 <tile id="81" type="AeMulti">
  <properties>
   <property name="1" value="AccLeft"/>
   <property name="2" value="AnimLeft"/>
   <property name="NAME" value="W-Left"/>
  </properties>
 </tile>
 <tile id="82" type="AeMulti">
  <properties>
   <property name="1" value="AccRight"/>
   <property name="2" value="AnimRight"/>
   <property name="NAME" value="W-Right"/>
  </properties>
 </tile>
 <tile id="84" type="AteOnGround">
  <properties>
   <property name="FAILURE" value="AccLeftAir"/>
   <property name="NAME" value="AccLeftDyn"/>
   <property name="SUCCESS" value="W-Left"/>
  </properties>
 </tile>
 <tile id="85" type="AteOnGround">
  <properties>
   <property name="FAILURE" value="AccRightAir"/>
   <property name="NAME" value="AccRightDyn"/>
   <property name="SUCCESS" value="W-Right"/>
  </properties>
 </tile>
</tileset>
