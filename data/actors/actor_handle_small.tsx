<?xml version="1.0" encoding="UTF-8"?>
<tileset name="actor_handle_small" tilewidth="16" tileheight="16" tilecount="2" columns="2">
 <image source="new_small.png" width="32" height="16"/>
 <tile id="0" type="ACTOR_TEMPLATE">
  <properties>
   <property name="ACTOR_NAME" value="BLOCKY_SMALL"/>
   <property name="DIRECTION" value="RIGHT"/>
   <property name="ON_ALWAYS" value="Physics"/>
   <property name="ON_IDLE" value="IdleLast"/>
  </properties>
  <objectgroup draworder="index">
   <object id="1" x="3" y="6" width="10" height="10"/>
  </objectgroup>
 </tile>
</tileset>
