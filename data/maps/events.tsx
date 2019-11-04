<?xml version="1.0" encoding="UTF-8"?>
<tileset name="events_symbol" tilewidth="60" tileheight="60" tilecount="100" columns="10">
 <image source="../tilesets/symbol_numbers.png" width="600" height="600"/>
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
 <tile id="9" type="AeAnimate">
  <properties>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="CYCLES" type="int" value="5"/>
   <property name="DIRECTION" value="RIGHT"/>
   <property name="NAME" value="Test"/>
  </properties>
 </tile>
 <tile id="10" type="AteOnGround">
  <properties>
   <property name="NAME" value="OnGroundJmp"/>
   <property name="TOLERANCE" type="int" value="15"/>
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
   <property name="PATH" type="file" value="../audio/Ding2.ogg"/>
  </properties>
 </tile>
 <tile id="13" type="AteSwitch">
  <properties>
   <property name="1" value="OnGroundJmp"/>
   <property name="NAME" value="JumpOnce"/>
   <property name="SUCCESS" value="Jump1"/>
  </properties>
 </tile>
 <tile id="14" type="AeAccelerate">
  <properties>
   <property name="NAME" value="AccLeft"/>
   <property name="XFACTOR" type="float" value="-500"/>
   <property name="XMAX_SPEED" type="float" value="-300"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="15" type="AeAccelerate">
  <properties>
   <property name="NAME" value="AccRight"/>
   <property name="XFACTOR" type="float" value="500"/>
   <property name="XMAX_SPEED" type="float" value="300"/>
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
   <property name="XFACTOR" type="float" value="300"/>
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
   <property name="YFACTOR" type="float" value="-999999999"/>
   <property name="YMAX_SPEED" type="float" value="-500"/>
  </properties>
 </tile>
 <tile id="21" type="AeAccelerate">
  <properties>
   <property name="NAME" value="Fall_Dyn"/>
   <property name="XFACTOR" type="float" value="0"/>
   <property name="YFACTOR" type="float" value="1000"/>
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
 <tile id="23" type="AteSwitch">
  <properties>
   <property name="1" value="OnGroundJmp"/>
   <property name="NAME" value="Jump_Grnd"/>
   <property name="SUCCESS" value="Jump_Dyn2"/>
  </properties>
 </tile>
 <tile id="24" type="AeAccelerate">
  <properties>
   <property name="NAME" value="AccLeftAir"/>
   <property name="XFACTOR" type="float" value="-200"/>
   <property name="XMAX_SPEED" type="float" value="-300"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="25" type="AeAccelerate">
  <properties>
   <property name="NAME" value="AccRightAir"/>
   <property name="XFACTOR" type="float" value="200"/>
   <property name="XMAX_SPEED" type="float" value="300"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="27" type="AeDecelerate">
  <properties>
   <property name="NAME" value="DecelerateAir"/>
   <property name="XFACTOR" type="float" value="100"/>
   <property name="YFACTOR" type="float" value="0"/>
  </properties>
 </tile>
 <tile id="40" type="MeTemplateLayer">
  <properties>
   <property name="DESTINATION" value="Actors"/>
   <property name="NAME" value="ShowHandsome"/>
   <property name="SOURCE" value="Test"/>
  </properties>
 </tile>
 <tile id="50" type="AteMouseButtons">
  <properties>
   <property name="NAME" value="LeftClick"/>
  </properties>
 </tile>
 <tile id="51" type="AteSwitch">
  <properties>
   <property name="1" value="LeftClick"/>
   <property name="NAME" value="JumpOnClick"/>
   <property name="SUCCESS" value="Jump_Grnd"/>
  </properties>
 </tile>
 <tile id="58" type="MeSetVar">
  <properties>
   <property name="NAME" value="CoinCountDupe"/>
   <property name="VALUE" value="*CoinCount"/>
   <property name="VAL_NAME" value="CoinCountDupe"/>
  </properties>
 </tile>
 <tile id="59" type="MeSetVar">
  <properties>
   <property name="+=" type="bool" value="true"/>
   <property name="NAME" value="CoinCount"/>
   <property name="VALUE" type="int" value="1"/>
   <property name="VAL_NAME" value="CoinCount"/>
  </properties>
 </tile>
 <tile id="60" type="AeSound">
  <properties>
   <property name="NAME" value="PickupSound"/>
   <property name="PATH" type="file" value="../audio/Collectible.ogg"/>
  </properties>
 </tile>
 <tile id="61" type="AeAnimate">
  <properties>
   <property name="ANIMATION_FRAMES" value="*CoinCountDupe"/>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="NAME" value="Disappear"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="62" type="AeEraseThis">
  <properties>
   <property name="NAME" value="kill"/>
  </properties>
 </tile>
 <tile id="63" type="AeMulti">
  <properties>
   <property name="1" value="PickupSound"/>
   <property name="2" value="Disappear"/>
   <property name="3" value="CoinCount"/>
   <property name="4" value="CoinCountDupe"/>
   <property name="5" value="kill"/>
   <property name="NAME" value="CoinPickup"/>
  </properties>
 </tile>
 <tile id="64" type="AteSingle">
  <properties>
   <property name="EVENT_NAME" value="CoinPickup"/>
   <property name="NAME" value="CoinPickupSafe"/>
  </properties>
 </tile>
 <tile id="70" type="AeSpawnText">
  <properties>
   <property name="BOLD" type="bool" value="true"/>
   <property name="COLOR" type="color" value="#ffcc65d5"/>
   <property name="LAYER_NAME" value="Test"/>
   <property name="NAME" value="StupidText"/>
   <property name="TEXT" value="ASS"/>
   <property name="UNDERLINE" type="bool" value="true"/>
  </properties>
 </tile>
 <tile id="71" type="MeLayerState">
  <properties>
   <property name="HIDE" type="bool" value="true"/>
   <property name="LAYER_NAME" value="Actors"/>
   <property name="NAME" value="Hide"/>
  </properties>
 </tile>
 <tile id="72" type="MeLayerState">
  <properties>
   <property name="LAYER_NAME" value="Actors"/>
   <property name="NAME" value="Unhide"/>
   <property name="UNHIDE" type="bool" value="true"/>
  </properties>
 </tile>
 <tile id="73" type="AeRotate">
  <properties>
   <property name="DEGREE" type="float" value="10"/>
   <property name="NAME" value="Rotate"/>
  </properties>
 </tile>
 <tile id="78" type="AteOnGround">
  <properties>
   <property name="NAME" value="OnGround"/>
  </properties>
 </tile>
 <tile id="79" type="AteSwitch">
  <properties>
   <property name="1" value="OnGround"/>
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
 <tile id="83" type="AteCollision">
  <properties>
   <property name="MY_HITBOX" value="COLLIDE"/>
   <property name="NAME" value="lava_collision"/>
   <property name="OTHER_HITBOX" value="LAVA"/>
  </properties>
 </tile>
 <tile id="84" type="AteSwitch">
  <properties>
   <property name="1" value="OnGround"/>
   <property name="FAILURE" value="AccLeftAir"/>
   <property name="NAME" value="AccLeftDyn"/>
   <property name="SUCCESS" value="W-Left"/>
  </properties>
 </tile>
 <tile id="85" type="AteSwitch">
  <properties>
   <property name="1" value="OnGround"/>
   <property name="FAILURE" value="AccRightAir"/>
   <property name="NAME" value="AccRightDyn"/>
   <property name="SUCCESS" value="W-Right"/>
  </properties>
 </tile>
 <tile id="90" type="AeDecelerate">
  <properties>
   <property name="NAME" value="lava_float"/>
   <property name="XFACTOR" type="float" value="45"/>
   <property name="YFACTOR" type="float" value="300"/>
  </properties>
 </tile>
 <tile id="91" type="AteSwitch">
  <properties>
   <property name="1" value="lava_collision"/>
   <property name="NAME" value="actor_lava"/>
   <property name="SUCCESS" value="lava_float"/>
  </properties>
 </tile>
 <tile id="92" type="AeMulti">
  <properties>
   <property name="1" value="AccRightDyn"/>
   <property name="2" value="JumpOnClick"/>
   <property name="NAME" value="PlayerMouseInteraction"/>
  </properties>
 </tile>
 <tile id="93" type="AeAnimate">
  <properties>
   <property name="ANIMATION_TYPE" value="WALK"/>
   <property name="DIRECTION" value="CURRENT"/>
   <property name="GAME_FRAMES" type="int" value="17"/>
   <property name="NAME" value="Pickup_Anim"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="94" type="GeCloseMap">
  <properties>
   <property name="NAME" value="close_map_internal"/>
  </properties>
 </tile>
 <tile id="95" type="AeGeWrapper">
  <properties>
   <property name="EVENT" value="close_map_internal"/>
   <property name="NAME" value="close_map"/>
  </properties>
 </tile>
 <tile id="96" type="GeChangeMap">
  <properties>
   <property name="NAME" value="next_map_internal"/>
   <property name="PATH" value="main.tmx"/>
   <property name="PRESERVE" type="bool" value="true"/>
  </properties>
 </tile>
 <tile id="97" type="AeGeWrapper">
  <properties>
   <property name="EVENT" value="next_map_internal"/>
   <property name="NAME" value="next_map"/>
  </properties>
 </tile>
 <tile id="98" type="GeQuit">
  <properties>
   <property name="NAME" value="quit_internal"/>
  </properties>
 </tile>
 <tile id="99" type="AeGeWrapper">
  <properties>
   <property name="EVENT" value="quit_internal"/>
   <property name="NAME" value="quit"/>
  </properties>
 </tile>
</tileset>
