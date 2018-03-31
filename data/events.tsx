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
   <property name="DIRECTION" value="DOWN"/>
   <property name="DURATION" value="150"/>
   <property name="NAME" value="WalkDown"/>
   <property name="PRIORITY" value="HIGH"/>
   <property name="SIGNAL" value="STOP"/>
  </properties>
 </tile>
 <tile id="10" type="AeMoveDirection">
  <properties>
   <property name="DIRECTION" value="RIGHT"/>
   <property name="NAME" value="Rights"/>
  </properties>
 </tile>
</tileset>
