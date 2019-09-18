<?xml version="1.0" encoding="UTF-8"?>
<tileset name="actor_handle" tilewidth="32" tileheight="32" tilecount="2" columns="2">
 <image source="new.png" width="64" height="32"/>
 <tile id="0" type="ACTOR_TEMPLATE">
  <properties>
   <property name="ACTOR_NAME" value="BLOCKY"/>
   <property name="DIRECTION" value="RIGHT"/>
   <property name="ON_ALWAYS" value="Physics"/>
   <property name="ON_COLLISION" value="actor_lava"/>
   <property name="ON_IDLE" value="IdleLast"/>
  </properties>
  <objectgroup draworder="index">
   <object id="1" x="6" y="12" width="20" height="20"/>
  </objectgroup>
 </tile>
 <tile id="1">
  <objectgroup draworder="index"/>
 </tile>
</tileset>
